#include "IR.hpp"
#include "ParseExpression.hpp"
#include "ParseNode.hpp"
#include "ParseTree.hpp"
#include "ParseType.hpp"

void IR::gen(const string &op, const string &arg1, const string &arg2,
             const string &result, size_t node_id) {
  quas.push_back(make_tuple(op, arg1, arg2, result));
  if (node_id) {
    // 设置该节点的第一条指令编号
    set_node_begen_code(node_id, nextinstr);
  }
  nextinstr++;
}

void IR::print() {
  log("IR QUAS OUTPUT");
  for (int i = 0; i < quas.size(); i++) {
    cout << to_string(i) << "\t(" << get<0>(quas[i]) << "," << get<1>(quas[i])
         << "," << get<2>(quas[i]) << "," << get<3>(quas[i]) << ")" << endl;
  }
}

vector<size_t> IR::merge(const vector<size_t> &p1, const vector<size_t> &p2) {
  vector<size_t> ret;
  std::merge(p1.begin(), p1.end(), p2.begin(), p2.end(), back_inserter(ret));
  return ret;
}

void IR::backpatch(const vector<size_t> &p, size_t pp) {
  // 将 i 填入 p 指向四元式的所有待填域
  for (size_t i : p) {
    get<3>(quas[i]) = to_string(pp);
  }
}

string IR::size2type(size_t size) {
  static map<size_t, string> mp = {{1, "db"}, {2, "dw"}, {4, "dd"}, {8, "dq"}};
  if (mp.find(size) == mp.end()) {
    log("other size: " + to_string(size));
    return "unknown";
  } else {
    return mp[size];
  }
}

string IR::getConstValueStr(const ParseExpression &init_expr) {
  const ParseConstant &E = init_expr.get_const();
  ConstValueType con_type = E.get_type();
  string arg1;
  switch (con_type) {
  case ConstValueType::C_SIGNED:
    return to_string((int)E.get_signed());
  case ConstValueType::C_UNSIGNED:
    return to_string((int)E.get_unsigned());
  case ConstValueType::C_FLOAT:
    return to_string((int)E.get_float());
  case ConstValueType::C_BOOL:
    return to_string((int)E.get_bool());
  case ConstValueType::C_STRING:
    return E.get_string();
  default:
    log("other constant type");
    break;
  }
  return "BadValue";
}

size_t IR::dataEmit(const string &name, size_t size, const string &value,
                    size_t node_id) {
  string type = size2type(size);
  sectionData.push_back(SData{name, type, value});
  gen(".data", type, value, name, node_id);
  return -sectionData.size();
}

size_t IR::dataUndefinedEmit(const string &name, size_t size, size_t node_id) {
  static const string OUTZERO = "0";
  return dataEmit(name, size, OUTZERO, node_id);
}

size_t IR::dfsArrayGetItem(const ParseExpression &expr, vector<size_t> &psize,
                           vector<size_t> &pid) {
  const ParseExpression &left =
      ParseExpression::get_expression(expr.get_child(0));
  const ParseExpression &right =
      ParseExpression::get_expression(expr.get_child(1));
  if (right.is_const() &&
      right.get_const().get_type() == ConstValueType::C_SIGNED) {
    pid.push_back(right.get_const().get_signed());
  } else {
    throw ParseException(ExceptionCode::EX_NOT_IMPLEMENTED,
                         "IR::dfsArrayGetItem: implamented right value type");
  }
  if (left.is_variable()) {
    if (right.is_const() &&
        right.get_const().get_type() == ConstValueType::C_SIGNED) {
      psize.push_back(left.get_ret_type().get_array_size());
      return left.get_variable().get_address();
    } else {
      throw ParseException(ExceptionCode::EX_NOT_IMPLEMENTED,
                           "IR::dfsArrayGetItem: the the bottom of arrary get "
                           "item exp, implamented right value type");
    }
  } else {
    psize.push_back(left.get_ret_type().get_array_size());
    return dfsArrayGetItem(left, psize, pid);
  }
}

size_t IR::getItemEmit(const ParseExpression &init_expr,
                       const ParseType &this_type, const string &symbol,
                       ParseTree &tree, size_t node_id, size_t scope_id) {
  vector<size_t> psize;
  vector<size_t> pid;
  psize.push_back(init_expr.get_ret_type().get_size());
  size_t esp = dfsArrayGetItem(init_expr, psize, pid);
  for (int i = 1; i < psize.size(); i++) {
    psize[i] *= psize[i - 1];
  }
  size_t pos = 0;
  if (psize.size() == pid.size() + 1) {
    for (int i = 0; i <= pid.size() - 1; i++) {
      pos += pid[i] * psize[i];
    }
  } else {
    throw ParseException(ExceptionCode::EX_UNKNOWN,
                         "IR::getItemEmit: Bad Array Type " +
                             init_expr.get_info());
  }
  init_expr.set_address(esp + pos);
  return 0;
}

// 也许没用
void IR::exprEmit(const ParseExpression &init_expr, const ParseType &this_type,
                  const string &symbol, ParseTree &tree, size_t node_id,
                  size_t scope_id) {
  if (init_expr.is_const()) {
    string arg1 = getConstValueStr(init_expr);
    string result = address2pointer(init_expr.get_address());
    if (scope_id == 0) {
      // 在静态数据段生成，字符串(TODO)也在此生成
      dataEmit(symbol, this_type.get_size(), arg1, node_id);
    } else {
      // 在栈中分配
      gen(":=", arg1, "_", result, node_id);
    }
  } else {
    ExpressionType exp_type = init_expr.get_expr_type();
    string op = "_", arg1 = "_", arg2 = "_", result = "_";
    switch (exp_type) {
    // 双目运算符
    case ExpressionType::E_ADD:
      op = "+";
      break;
    case ExpressionType::E_SUB:
      op = "-";
      break;
    case ExpressionType::E_MUL:
      op = "*";
      break;
    case ExpressionType::E_DIV:
      op = "/";
      break;
    case ExpressionType::E_MOD:
      op = "%";
      break;
    case ExpressionType::E_POW:
      op = "^";
      break;
    case ExpressionType::E_LOGIC_OR:
      op = "||";
      break;
    case ExpressionType::E_LOGIC_AND:
      op = "&&";
      break;
    case ExpressionType::E_G:
      op = ">";
      break;
    case ExpressionType::E_GE:
      op = ">=";
      break;
    case ExpressionType::E_EQUAL:
      op = "==";
      break;
    case ExpressionType::E_NE:
      op = "!=";
      break;
    case ExpressionType::E_L:
      op = "<";
      break;
    case ExpressionType::E_LE:
      op = "<=";
      break;
    case ExpressionType::E_GET_ITEM: {
      getItemEmit(init_expr, this_type, symbol, tree, node_id, scope_id);
      return;
    }
    default:
      break;
    }
    if (op != "_") {
      const ParseExpression &right =
          ParseExpression::get_expression(init_expr.get_child(1));
      if (right.is_const()) {
        arg2 = getConstValueStr(right);
      } else {
        arg2 = address2pointer(right.get_address());
      }
    }
    switch (exp_type) {
    case ExpressionType::E_VAR: {
      return;
    }
    case ExpressionType::E_FUN: {
      gen("call", "_", "_", (init_expr.get_functions()[0])->get_symbol(), node_id);
      gen(":=", "eax", "_", address2pointer(init_expr.get_address()), node_id);
      return;
    }
    case ExpressionType::E_NOT:
      op = "!";
      break;
    case ExpressionType::E_ASSIGN: {
      // 赋值表达式
      assignEmit(init_expr.get_child(0), init_expr.get_child(1));
      const ParseExpression &left =
          ParseExpression::get_expression(init_expr.get_child(0));
      const ParseExpression &right =
          ParseExpression::get_expression(init_expr.get_child(1));
      string arg1, result;
      result = address2pointer(left.get_address());
      if (right.is_const()) {
        arg1 = getConstValueStr(right);
      } else {
        arg1 = address2pointer(right.get_address());
      }
      gen(":=", arg1, "_", result, node_id);
      return;
    }
    case ExpressionType::E_UNDEFINED: // 应该不会跑到这里
      if (scope_id == 0) {
        dataUndefinedEmit(symbol, this_type.get_size(), node_id);
      }
      // 这个表达式非常特别
      return;
    default:

      break;
    }
    if (op != "_") {
      const ParseExpression &left =
          ParseExpression::get_expression(init_expr.get_child(0));
      if (left.is_const()) {
        arg1 = getConstValueStr(left);
      } else {
        arg1 = address2pointer(left.get_address());
      }
    }
    result = address2pointer(init_expr.get_address());
    if (op != "_") {
      gen(op, arg1, arg2, result, node_id);
    } else {
      throw ParseException(ExceptionCode::EX_NOT_IMPLEMENTED,
                           "other exp type " + to_string(exp_type));
    }
  }
}

void IR::relopEmit(ParseTree &tree, size_t p0, size_t p1, size_t p3,
                   const string &relop, size_t node_id) {
  ParseNode &B = tree.node(p0);
  const ParseNode &E1 = tree.node(p1);
  const ParseNode &E2 = tree.node(p3);
  B.set_true_list(makelist(getNextinstr()));
  B.set_false_list(makelist(getNextinstr() + 1));
  string arg1, arg2;
  const ParseExpression &left = E1.get_expression();
  const ParseExpression &right = E2.get_expression();
  if (left.is_const()) {
    arg1 = getConstValueStr(left);
  } else {
    arg1 = address2pointer(left.get_address());
  }
  if (right.is_const()) {
    arg2 = getConstValueStr(right);
  } else {
    arg2 = address2pointer(right.get_address());
  }
  gen(relop, arg1, arg2, "_", node_id);
  gen("jmp", "_", "_", "_", node_id);
}

size_t IR::allocEmit(const size_t &scope, const string &symbol,
                     const size_t &size, const size_t &node_id) {
  // 变量内存分配, 返回地址
  const string key = formKey(scope, symbol);
  if (allocMap.find(key) != allocMap.end()) {
    throw ParseException(ExceptionCode::EX_UNKNOWN, "bad alloc for same var");
    return (size_t)-1;
  }
  allocMap[key] = Alloc{offset, size};
  if (size != (size_t)-1) {
    gen("alloc", to_string(offset), to_string(size), key, node_id);
  } else {
    gen("alloc", to_string(offset), "BadWide", key, node_id);
  }
  size_t ret = offset;
  addOffset(size);
  return ret;
}

void IR::varDecEmit(size_t addr, const ParseExpression &init_expr,
                    size_t node_id, size_t scope_id) {
  if (init_expr.get_expr_type() != ExpressionType::E_UNDEFINED) {
    if (init_expr.is_const()) {
      gen(":=", getConstValueStr(init_expr), "_", address2pointer(addr),
          node_id);
    } else {
      gen(":=", address2pointer(init_expr.get_address()), "_",
          address2pointer(addr), node_id);
    }
  }
}

string IR::getVarPointer(size_t id) {
  return address2pointer(
      ParseExpression::get_expression(id).get_variable().get_address());
}

string IR::getVarPointer(const ParseVariable &var) {
  return address2pointer(var.get_address());
}
