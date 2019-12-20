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
  static map<size_t, string> mp = {
    {1, "db"},
    {2, "dw"},
    {4, "dd"},
    {8, "dq"}
  };
  if (mp.find(size) == mp.end()) {
    log("other size: " + to_string(size));
    return "unknown";
  } else {
    return mp[size];
  }
}

void IR::dataEmit(const string &name, size_t size, const string &value, size_t node_id) {
  string type = size2type(size);
  sectionData.push_back(SData{name, type, value});
  gen("section .data", type, value, name, node_id);
}

void IR::dataUndefinedEmit(const string &name, size_t size, size_t node_id) {
  static const string OUTZERO = "0";
  dataEmit(name, size, OUTZERO, node_id);
}

// 也许没用
void IR::exprEmit(const ParseExpression &init_expr, const ParseType &this_type,
                  const string &symbol, ParseTree &tree, size_t node_id,
                  size_t scope_id) {
  if (init_expr.is_const()) {
    const ParseConstant &E = init_expr.get_const();
    ConstValueType con_type = E.get_type();
    string arg1;
    switch (con_type) {
      case ConstValueType::C_SIGNED:
        arg1 = to_string(E.get_signed());
        break;
      case ConstValueType::C_UNSIGNED:
        arg1 = to_string(E.get_unsigned());
        break;
      case ConstValueType::C_FLOAT:
        arg1 = to_string(E.get_float());
        break;
      case ConstValueType::C_BOOL:
        arg1 = to_string((int)E.get_bool());
        break;
      default:
        log("other constant type");
        break;
    }
    if (scope_id == 0) {
      // 在静态数据段生成，字符串(TODO)也在此生成
      dataEmit(symbol, this_type.get_size(), arg1, node_id);
    } else {
      // 在栈中分配
      gen(":=", arg1, "_", formKey(scope_id, symbol), node_id);
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
      default:
        break;
    }
    if (op != "_") {
      arg2 = to_string(ParseExpression::get_expression(init_expr.get_child(1)).get_address());
          //tree.node(init_expr.get_child(1)).get_expression().get_address());
    }
    switch (exp_type) {
      case ExpressionType::E_VAR: {
          // 变量直接赋值
          const ParseVariable &var = init_expr.get_variable();
          gen(":=", var.get_symbol(), "_", formKey(scope_id, symbol), node_id);
          return;
        }
        break;
      case ExpressionType::E_NOT:
        op = "!";
        break;
      case ExpressionType::E_ASSIGN:
        op = ":=";
        break;
      case ExpressionType::E_UNDEFINED:
        if (scope_id == 0) {
          dataUndefinedEmit(symbol, this_type.get_size(), node_id);
        }
        return;
        break;
      default:
        log("other exp type");
        break;
    }
    if (op != "_") {
      arg1 = to_string(ParseExpression::get_expression(init_expr.get_child(0)).get_address());
      // arg1 = to_string(
      //     tree.node(init_expr.get_child(0)).get_expression().get_address());
    }
    result = formKey(scope_id, symbol);
    if (op != "_") {
      gen(op, arg1, arg2, result, node_id);
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
  gen(relop, to_string(E1.get_expression().get_address()),
      to_string(E2.get_expression().get_address()), "_", node_id);
  gen("jmp", "_", "_", "_", node_id);
}

void IR::allocEmit(const size_t &scope, const string &symbol,
                   const size_t &size, const size_t &node_id) {
  // 变量内存分配
  const string key = formKey(scope, symbol);
  if (allocMap.find(key) != allocMap.end()) {
    log("重复的变量内存分配");
    return;
  }
  allocMap[key] = Alloc{offset, size};
  gen("alloc", to_string(offset), to_string(size), key, node_id);
  addOffset(size);
}

void IR::varDecEmit(const string &symbol, const ParseExpression &init_expr, size_t node_id, size_t scope_id) {
  if (init_expr.get_expr_type() != ExpressionType::E_UNDEFINED) {
    gen(":=", to_string(init_expr.get_address()), "_", formKey(scope_id, symbol), node_id);
  }
}