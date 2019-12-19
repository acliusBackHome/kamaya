#include "IR.hpp"
#include "ParseExpression.hpp"
#include "ParseNode.hpp"
#include "ParseTree.hpp"
#include "ParseType.hpp"

void IR::print() {
  cout << "quas" << endl;
  for (int i = 0; i < quas.size(); i++) {
    cout << to_string(i) << "\t(" << get<0>(quas[i]) << "," <<
                                     get<1>(quas[i]) << "," <<
                                     get<2>(quas[i]) << "," <<
                                     get<3>(quas[i]) << ")" << endl;
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

void IR::recordBegin() {
  stkpush(offset);
  offset = 0;
}

void IR::recordEnd() { offset = stkpop(); }

void IR::relopEmit(ParseTree &tree, size_t p0, size_t p1, size_t p3,
                   string relop) {
  ParseNode &B = tree.node(p0);
  const ParseNode &E1 = tree.node(p1);
  const ParseNode &E2 = tree.node(p3);
  B.set_true_list(makelist(getNextinstr()));
  B.set_false_list(makelist(getNextinstr() + 1));
  gen(relop, to_string(E1.get_expression().get_address()),
      to_string(E2.get_expression().get_address()), "_");
  gen("jmp", "_", "_", "_");
}

// 也许没用
void IR::exprEmit(const ParseExpression &init_expr, const ParseType &this_type,
                  const string &symbol, ParseTree &tree) {
  addOffset(this_type.get_size());
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
      cout << "other constant type" << endl;
      break;
    }
    gen(":=", arg1, "_", symbol);
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
      arg2 = to_string(
          tree.node(init_expr.get_child(1)).get_expression().get_address());
    }
    switch (exp_type) {
    case ExpressionType::E_VAR:
      op = ":=";
      break;
    case ExpressionType::E_NOT:
      op = "!";
      break;
    case ExpressionType::E_ASSIGN:
      op = ":=";
      break;
    default:
      cout << "other exp type" << endl;
      break;
    }
    if (op != "_") {
      arg1 = to_string(
          tree.node(init_expr.get_child(0)).get_expression().get_address());
    }
    // result = to_string(init_expr.get_id());
    result = symbol;
    if (op != "_") {
      gen(op, arg1, arg2, result);
    }
  }
}