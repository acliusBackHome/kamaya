#include <cmath>
#include "ParseExpression.hpp"
#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
using namespace std;

map<ParseExpression, size_t> ParseExpression::expr2id;
vector<ParseExpression> ParseExpression::id2expr;
vector<size_t> ParseExpression::id2address;

string ParseExpression::get_info() const {
    char buff[64];
    string res = "ParseExpression-";
    if (!expr_id) {
        update(*this);
    }
    sprintf(buff, "%zu", expr_id);
    res += buff;
    res += " {";
    switch (expr_type) {
        case E_UNDEFINED:
            return res + "undefined }";
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_MOD:
        case E_POW:
        case E_LOGIC_OR:
        case E_LOGIC_AND:
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE:
        case E_ASSIGN:
        case E_DIV: {
            const ParseExpression &expr1 = get_expression(child[0]),
                    &expr2 = get_expression(child[1]);
            sprintf(buff, "{ Expr-%zu ", child[0]);
            res += buff;
            res += get_expr_type_name(expr_type);
            sprintf(buff, " Expr-%zu }, ", child[1]);
            res += buff;
            break;
        }
        case E_NOT: {
            sprintf(buff, "! { Expr-%zu }", child[0]);
            res += buff;
        }
        case E_VAR: {
            res += "variable:{ " + ((ParseVariable *) child[0])->get_info() + " }, ";
            break;
        }
        case E_CONST: {
            res += "constant: " + ((ParseConstant *) child[0])->get_info() + ", ";
            break;
        }
    }
    if (expr_type != E_CONST && is_const()) {
        res += "const_value: " + ((ParseConstant *) const_value)->get_info() + ", ";
    }
    res += "return_type: " + get_ret_type().get_info() + ", ";
    res += "address: " + to_string(get_address()) + "}";
    return res;
}

string ParseExpression::get_expr_type_name(ExpressionType type) {
    switch (type) {
        case E_ADD:
            return "+";
        case E_SUB:
            return "-";
        case E_MUL:
            return "*";
        case E_DIV:
            return "/";
        case E_POW:
            return "^";
        case E_LOGIC_OR:
            return "||";
        case E_LOGIC_AND:
            return "&&";
        case E_G:
            return ">";
        case E_GE:
            return ">=";
        case E_EQUAL:
            return "==";
        case E_NE:
            return "!=";
        case E_L:
            return "<";
        case E_LE:
            return "<=";
        case E_VAR:
            return "variable";
        case E_CONST:
            return "const";
        case E_MOD:
            return "%";
        case E_NOT:
            return "!";
        case E_ASSIGN:
            return "=";
        case E_UNDEFINED:
            break;
    }
    return "undefined";
}

ParseExpression::ParseExpression() {
    ret_type_id = (size_t) -1;
    child[0] = child[1] = expr_id = 0;
    const_value = 0;
    expr_type = E_UNDEFINED;
}

ParseExpression::ParseExpression(const ParseConstant &constant) {
    expr_type = E_CONST;
    child[0] = (size_t)
            new ParseConstant(constant);
    const_value = child[0];
    ret_type_id = ParseType(constant).get_id();
    update(*this);
}

ParseExpression::ParseExpression(const ParseExpression &expr) {
    assign(*this, expr);
}

ParseExpression::ParseExpression(const ParseVariable &variable) {
    const_value = 0;
    ret_type_id = variable.get_type().get_id();
    expr_type = E_VAR;
    child[0] = (size_t)
            new ParseVariable(variable);
    update(*this);
}

void ParseExpression::init() {
    id2expr.emplace_back(ParseExpression());
    id2address.emplace_back(0);
    expr2id[ParseExpression()] = 0;
}

bool ParseExpression::operator<(const ParseExpression &other) const {
    if (expr_type < other.expr_type) {
        return true;
    } else if (other.expr_type < expr_type) {
        return false;
    }
    if (ret_type_id < other.ret_type_id) {
        return true;
    } else if (ret_type_id < other.ret_type_id) {
        return false;
    }
    switch (expr_type) {
        case E_UNDEFINED:
            break;
        case E_CONST: {
            const ParseConstant &value1 = *(ParseConstant *) child[0],
                    &value2 = *(ParseConstant *) other.child[0];
            if (value1 < value2) {
                return true;
            } else if (value2 < value1) {
                return false;
            }
            break;
        }
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_POW:
        case E_MOD:
        case E_LOGIC_OR:
        case E_LOGIC_AND:
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE:
        case E_ASSIGN:
        case E_DIV: {
            for (size_t i = 0; i < 2; ++i) {
                if (child[i] < other.child[i]) {
                    return true;
                } else if (child[i] > other.child[i]) {
                    return false;
                }
            }
            break;
        }
        case E_NOT: {
            if (child[0] < other.child[0]) {
                return true;
            } else if (child[0] > other.child[0]) {
                return false;
            }
            break;
        }
        case E_VAR: {
            ParseVariable &this_var = *(ParseVariable *) child[0],
                    &that_var = *(ParseVariable *) (other.child[0]);
            if (this_var < that_var) {
                return true;
            } else if (that_var < this_var) {
                return false;
            }
            break;
        }
    }
    return false;
}

ParseExpression::~ParseExpression() {
    if (is_const()) {
        delete (ParseConstant *) const_value;
    }
    switch (expr_type) {
        case E_VAR:
            delete (ParseVariable *) child[0];
            break;
        case E_CONST:
            if (child[0] != const_value) {
                delete (ParseConstant *) child[0];
            }
            break;
        case E_UNDEFINED:
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_MOD:
        case E_POW:
        case E_NOT:
        case E_LOGIC_OR:
        case E_LOGIC_AND:
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE:
        case E_ASSIGN:
            return;
    }
}

const ParseExpression &ParseExpression::get_expression(size_t expr_id) {
    if (id2expr.empty()) {
        init();
    }
    if (expr_id >= id2expr.size()) {
        printf("ParseExpression::expression(size_t expr_id): 警告:试图获取未定义的表达式ID:%zu\n",
               expr_id);
        return id2expr[0];
    }
    return id2expr[expr_id];
}

ParseExpression &ParseExpression::operator=(const ParseVariable &variable) {
    if (id2expr.empty()) {
        init();
    }
    expr_type = E_VAR;
    child[0] = (size_t)
            new ParseVariable(variable);
    update(*this);
    return *this;
}

ParseExpression &ParseExpression::operator=(const ParseExpression &expr) {
    assign(*this, expr);
    return *this;
}

void ParseExpression::update(const ParseExpression &expr) {
    if (id2expr.empty()) {
        init();
    }
    auto it = expr2id.find(expr);
    if (it == expr2id.end()) {
        expr2id[expr] = id2expr.size();
        ((ParseExpression &) expr).expr_id = id2expr.size();
        id2expr.emplace_back(expr);
        id2address.emplace_back(0);
        expr_call_back(id2expr[id2expr.size() - 1]);
    } else {
        ((ParseExpression &) expr).expr_id = it->second;
    }
}


const ParseExpression &ParseExpression::operator+(const ParseExpression &expr) const {
    return generate_expression(E_ADD, *this, expr);
}

const ParseExpression &ParseExpression::operator-(const ParseExpression &expr) const {
    return generate_expression(E_SUB, *this, expr);
}

const ParseExpression &ParseExpression::operator*(const ParseExpression &expr) const {
    return generate_expression(E_MUL, *this, expr);
}

const ParseExpression &ParseExpression::operator/(const ParseExpression &expr) const {
    return generate_expression(E_DIV, *this, expr);
}

const ParseExpression &ParseExpression::operator%(const ParseExpression &expr) const {
    return generate_expression(E_MOD, *this, expr);
}

const ParseExpression &ParseExpression::operator^(const ParseExpression &expr) const {
    return generate_expression(E_POW, *this, expr);
}

const ParseExpression &ParseExpression::operator!() const {
    ParseExpression res;
    res.expr_type = E_NOT;
    res.child[0] = get_expr_id(*this);
    res.ret_type_id = T_BOOL;
    if (!const_value) {
        res.calculate_const();
    }
    update(res);
    return get_expression(res.get_id());
}

const ParseExpression &
ParseExpression::get_logic_expression(ExpressionType type,
                                      const ParseExpression &expr1, const ParseExpression &expr2) {
    switch (type) {
        case E_LOGIC_OR:
        case E_LOGIC_AND:
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE:
            return generate_expression(type, expr1, expr2);
        default:
            break;
    }
    // 错误调用
    string info = "ParseExpression::get_logic_expression(ExpressionType type,"
                  "const ParseExpression &expr1, const ParseExpression &expr2)";
    info += " expr1=" + to_string(expr1.get_id());
    info += " expr2=" + to_string(expr2.get_id());
    throw ParseException(EX_EXPRESSION_CAN_NOT_GENERATE, info);
}

size_t ParseExpression::get_expr_id(const ParseExpression &expr) {
    if (expr.expr_id) {
        return expr.expr_id;
    }
    update(expr);
    return expr.expr_id;
}

const ParseExpression &ParseExpression::generate_expression(
        ExpressionType type,
        const ParseExpression &expr1,
        const ParseExpression &expr2) {
    ParseExpression res;
    res.expr_type = type;
    res.child[0] = get_expr_id(expr1);
    res.child[1] = get_expr_id(expr2);
    switch (type) {
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_MOD:
        case E_POW: {
            // 如果其中一个是不可在编译期计算的表达式, 另一个是常量表达式, 则表达式返回值为不可计算表达式的类型
            if ((!expr1.is_const()) && expr2.is_const()) {
                res.ret_type_id = expr1.get_ret_type().get_id();
            } else if (!(expr2.is_const()) && expr1.is_const()) {
                res.ret_type_id = expr2.get_ret_type().get_id();
            } else {
                // 两者都是变量或者两者都是常量
                try {
                    res.ret_type_id = ParseType::convert(type, expr1.get_ret_type(), expr2.get_ret_type());
                } catch (ParseException &exc) {
                    string info = "in ParseExpression::generate_expression("
                                  "ExpressionType type, "
                                  "const ParseExpression &expr1, "
                                  "const ParseExpression &expr2)";
                    info += " expr1=" + to_string(expr1.get_id());
                    info += " expr2=" + to_string(expr2.get_id());
                    exc.push_trace(info);
                    throw exc;
                }
                if (expr1.is_const() && expr2.is_const()) {
                    res.calculate_const();
                }
            }
            // 求幂运算比较特殊(想出来C语言支持求幂运算符的要求的真是天才),
            // 返回值是long double
            if (type == E_POW) {
                ParseType ret_type(T_DOUBLE, S_LONG);
                res.ret_type_id = ret_type.get_id();
            }
            break;
        }
        case E_LOGIC_OR:
        case E_LOGIC_AND:
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE:
            res.ret_type_id = T_BOOL;
            if (expr1.is_const() && expr2.is_const()) {
                res.calculate_const();
            }
            break;
        case E_NOT:
        case E_UNDEFINED:
        case E_VAR:
        case E_CONST:
        case E_ASSIGN: {
            // 错误调用该函数
            string info = "ParseExpression::generate_expression("
                          "ExpressionType type, "
                          "const ParseExpression &expr1, "
                          "const ParseExpression &expr2)";
            info += " expr1=" + to_string(expr1.get_id());
            info += " expr2=" + to_string(expr2.get_id());
            throw ParseException(EX_EXPRESSION_CAN_NOT_GENERATE, info);
        }
    }
    update(res);
    return get_expression(res.get_id());
}

const ParseExpression &ParseExpression::get_assign_expression(
        const ParseExpression &unary_expr, const ParseExpression &expr) {
    ParseExpression res;
    try {
        if (ParseType::convert(unary_expr.get_ret_type(), expr.get_ret_type())) {
            res.expr_type = E_ASSIGN;
            res.child[0] = get_expr_id(unary_expr);
            res.child[1] = get_expr_id(expr);
            res.ret_type_id = unary_expr.get_ret_type().get_id();
        }
    } catch (ParseException &exc) {
        string info = "in ParseExpression::get_assign_expression("
                      "const ParseExpression &unary_expr, const ParseExpression &expr)";
        info += " unary_expr: " + to_string(unary_expr.get_id());
        info += " expr: " + to_string(expr.get_id());
        exc.push_trace(info);
        throw exc;
    }
    update(res);
    return get_expression(res.get_id());
}


void ParseExpression::assign(ParseExpression &expr, const ParseExpression &from_expr) {
    expr.expr_id = from_expr.expr_id;
    expr.ret_type_id = from_expr.ret_type_id;
    if (from_expr.is_const()) {
        expr.const_value = (size_t)
                new ParseConstant(*(ParseConstant *) from_expr.const_value);
    } else {
        expr.const_value = (size_t) -1;
    }
    switch (expr.expr_type = from_expr.expr_type) {
        case E_UNDEFINED:
            break;
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_MOD:
        case E_POW:
        case E_LOGIC_OR:
        case E_LOGIC_AND:
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE:
            expr.child[0] = from_expr.child[0];
            expr.child[1] = from_expr.child[1];
            break;
        case E_NOT:
            expr.child[0] = from_expr.child[0];
            break;
        case E_VAR:
            expr.child[0] = (size_t)
                    new ParseVariable(*(ParseVariable *) from_expr.child[0]);
            break;
        case E_CONST:
            expr.child[0] = (size_t)
                    new ParseConstant(*(ParseConstant *) from_expr.child[0]);
            break;
        case E_ASSIGN:
            expr.child[0] = from_expr.child[0];
            expr.child[1] = from_expr.child[1];
            break;
    }
}

const ParseConstant &ParseExpression::get_const() const {
    ((ParseExpression *) this)->calculate_const();
    if (is_const()) {
        return *(ParseConstant *) const_value;
    }
    string info = "ParseExpression::get_const() expr_id=";
    info += to_string(expr_id);
    throw ParseException(EX_EXPRESSION_NOT_CONST, info);
}

const ParseType &ParseExpression::get_ret_type() const {
    if (ret_type_id != (size_t) -1) {
        return ParseType::get_type(ret_type_id);
    }
    // -1表示未经过计算
    switch (expr_type) {
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_MOD:
        case E_POW: {
            size_t &ret_cache = (((ParseExpression *) this)->ret_type_id);
            try {
                ret_cache = ParseType::convert(expr_type,
                                               get_expression(child[0]).get_ret_type(),
                                               get_expression(child[1]).get_ret_type()
                );
            } catch (ParseException &exc) {
                string info = "in ParseExpression::get_ret_type()";
                info += " expr=" + to_string(get_id());
                exc.push_trace(info);
                throw exc;
            }
            return ParseType::get_type(ret_cache);
        }
        case E_CONST: {
            size_t &ret_cache = (((ParseExpression *) this)->ret_type_id);
            ret_cache = ParseType(*((ParseConstant *) child[0])).get_id();
            return ParseType::get_type(ret_cache);
        }
        case E_VAR: {
            size_t &ret_cache = (((ParseExpression *) this)->ret_type_id);
            ret_cache = ((ParseVariable *) child[0])->get_type().get_id();
            return ParseType::get_type(ret_cache);
        }
        case E_NOT: {
            size_t &ret_cache = (((ParseExpression *) this)->ret_type_id);
            ret_cache = get_expression(child[0]).get_ret_type().get_id();
            return ParseType::get_type(ret_cache);
        }
        case E_LOGIC_OR:
        case E_LOGIC_AND:
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE: {
            size_t &ret_cache = (((ParseExpression *) this)->ret_type_id);
            ret_cache = T_BOOL;
            return ParseType::get_type(ret_cache);
        }
        case E_ASSIGN: {
            size_t &ret_cache = (((ParseExpression *) this)->ret_type_id);
            ret_cache = ParseType::convert(E_ASSIGN,
                                           get_expression(child[0]).get_ret_type(),
                                           get_expression(child[1]).get_ret_type());
            return ParseType::get_type(ret_cache);
        }
        case E_UNDEFINED:
            break;
    }
    ((ParseExpression *) this)->ret_type_id = 0;
    return ParseType::get_type(T_UNKNOWN);
}

size_t ParseExpression::get_child(size_t _child) const {
    if (_child >= 2) {
        string info = "ParseExpression::get_child(size_t _child) expr_id=";
        info += to_string(expr_id);
        info += " _child=" + to_string(_child);
        throw ParseException(EX_EXPRESSION_CAN_NOT_ACCESS, info);
    }
    switch (expr_type) {
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_MOD:
        case E_POW:
        case E_LOGIC_OR:
        case E_LOGIC_AND:
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE:
        case E_ASSIGN:
            return child[_child];
        case E_NOT:
            // 只有一个子表达式
            if (_child >= 1) {
                string info = "ParseExpression::get_child_expression(size_t _child) expr_id=";
                info += to_string(expr_id);
                info += " _child=" + to_string(_child);
                throw ParseException(EX_EXPRESSION_CAN_NOT_ACCESS, info);
            }
            return child[_child];
        case E_VAR:
        case E_CONST:
        case E_UNDEFINED: {
            string info = "ParseExpression::get_child_expression(size_t _child) expr_id=";
            info += to_string(expr_id);
            info += " _child=" + to_string(_child);
            throw ParseException(EX_EXPRESSION_CAN_NOT_ACCESS, info);
        }
    }
    // 应该不可能有控制流到达此
    string info = "ParseExpression::get_child_expression(size_t _child) expr_id=";
    info += to_string(expr_id);
    info += " _child=" + to_string(_child);
    throw ParseException(EX_UNKNOWN, info);
}

void ParseExpression::print_all_expression() {
    for (size_t i = 0; i < id2expr.size(); ++i) {
        printf("%zu: %s\n", i, id2expr[i].get_info().c_str());
    }
}

bool ParseExpression::is_defined() const {
    return get_expr_id(*this) != E_UNDEFINED;
}

ExpressionType ParseExpression::get_expr_type() const {
    return expr_type;
}

size_t ParseExpression::get_id() const {
    return get_expr_id(*this);
}

void ParseExpression::calculate_const() {
    if (const_value) {
        return;
    }
    // 只有const_value等于0时才计算常量
    switch (expr_type) {
        //双目运算符
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_POW:
        case E_MOD:
            // 比较运算符
        case E_G:
        case E_GE:
        case E_EQUAL:
        case E_NE:
        case E_L:
        case E_LE: {
            if (!get_expression(child[0]).is_const() || !get_expression(child[1]).is_const()) {
                // 不是常量退出
                break;
            }
            const ParseConstant &const1 = get_expression(child[0]).get_const(),
                    &const2 = get_expression(child[1]).get_const();
            if (expr_type == E_POW) {
                // 求幂运算比较特殊 直接返回long double
                const_value = (size_t)
                        new ParseConstant(powl(const1.get_float(), const2.get_float()));
                return;
            }
            ConstValueType cal_type =
                    ParseConstant::wider_const_type(const1.get_type(), const2.get_type());
            if (cal_type == C_UNDEFINED) {
                break;
            }
            switch (expr_type) {
                case E_ADD: {
                    calculate_const_add(cal_type, const1, const2);
                    return;
                }
                case E_SUB: {
                    calculate_const_sub(cal_type, const1, const2);
                    return;
                }
                case E_MUL: {
                    calculate_const_mul(cal_type, const1, const2);
                    return;
                }
                case E_DIV: {
                    calculate_const_div(cal_type, const1, const2);
                    return;
                }
                case E_MOD: {
                    calculate_const_mod(cal_type, const1, const2);
                    return;
                }
                    // 比较运算符
                case E_G: {
                    calculate_const_g(cal_type, const1, const2);
                    return;
                }
                case E_GE: {
                    calculate_const_ge(cal_type, const1, const2);
                    return;
                }
                case E_EQUAL: {
                    calculate_const_eq(cal_type, const1, const2);
                    return;
                }
                case E_NE: {
                    calculate_const_ne(cal_type, const1, const2);
                    return;
                }
                case E_L: {
                    calculate_const_l(cal_type, const1, const2);
                    return;
                }
                case E_LE: {
                    calculate_const_le(cal_type, const1, const2);
                    return;
                }
                default:
                    break;
            }
            break;
        }
            // 单目运算符
        case E_NOT: {
            if (!get_expression(child[0]).is_const()) {
                // 不是常量退出
                break;
            }
            const ParseConstant &const1 = get_expression(child[0]).get_const();
            if (const1.get_type() == C_UNDEFINED) {
                break;
            }
            const_value = (size_t)
                    new ParseConstant(!(const1.get_bool()));
            return;
        }
            // 逻辑运算符
        case E_LOGIC_OR:
        case E_LOGIC_AND: {
            if (!get_expression(child[0]).is_const() || !get_expression(child[1]).is_const()) {
                // 不是常量退出
                break;
            }
            const ParseConstant &const1 = get_expression(child[0]).get_const(),
                    &const2 = get_expression(child[1]).get_const();
            if (const1.get_type() == C_UNDEFINED || const2.get_type() == C_UNDEFINED) {
                break;
            }
            switch (expr_type) {
                case E_LOGIC_OR:
                    const_value = (size_t)
                            new ParseConstant(const1.get_bool() || const2.get_bool());
                    return;
                case E_LOGIC_AND:
                    const_value = (size_t)
                            new ParseConstant(const1.get_bool() && const2.get_bool());
                    return;
                default:
                    break;
            }
            break;
        }
        case E_ASSIGN:// 赋值语句肯定不是常量
        case E_CONST:
        case E_UNDEFINED:
        case E_VAR:
            // 变量或者常量, 未定义不会计算常量
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_add(ConstValueType cal_type,
                    const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() + const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() + const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() + const2.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_sub(ConstValueType cal_type,
                    const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() - const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() - const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() - const2.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_mul(ConstValueType cal_type,
                    const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() * const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() * const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() * const2.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_div(ConstValueType cal_type,
                    const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            auto divisor = const2.get_signed();
            if (divisor == 0) {
                string info = "ParseExpression::calculate_const() expr_id=" + to_string(get_id());
                throw ParseException(EX_EXPRESSION_DIVIDE_ZERO, info);
            }
            const_value = (size_t)
                    new ParseConstant(ParseConstant(const1.get_signed() / divisor));
            return;
        }
        case C_UNSIGNED: {
            auto divisor = const2.get_unsigned();
            if (divisor == 0) {
                string info = "ParseExpression::calculate_const() expr_id=" + to_string(get_id());
                throw ParseException(EX_EXPRESSION_DIVIDE_ZERO, info);
            }
            const_value = (size_t)
                    new ParseConstant(ParseConstant(const1.get_unsigned() / divisor));
            return;
        }
        case C_FLOAT: {
            auto divisor = const2.get_float();
            if (divisor == 0) {
                string info = "ParseExpression::calculate_const() expr_id=" + to_string(get_id());
                throw ParseException(EX_EXPRESSION_DIVIDE_ZERO, info);
            }
            const_value = (size_t)
                    new ParseConstant(ParseConstant(const1.get_float() / divisor));
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_mod(ConstValueType cal_type,
                    const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            auto divisor = const2.get_signed();
            if (divisor == 0) {
                string info = "ParseExpression::calculate_const() expr_id=" + to_string(get_id());
                throw ParseException(EX_EXPRESSION_DIVIDE_ZERO, info);
            }
            const_value = (size_t)
                    new ParseConstant(ParseConstant(const1.get_signed() % divisor));
            return;
        }
        case C_UNSIGNED: {
            auto divisor = const2.get_unsigned();
            if (divisor == 0) {
                string info = "ParseExpression::calculate_const() expr_id=" + to_string(get_id());
                throw ParseException(EX_EXPRESSION_DIVIDE_ZERO, info);
            }
            const_value = (size_t)
                    new ParseConstant(ParseConstant(const1.get_unsigned() % divisor));
            return;
        }
        case C_FLOAT:
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_g(ConstValueType cal_type,
                  const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() > const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() > const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() > const2.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_ge(ConstValueType cal_type,
                   const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() >= const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() >= const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() >= const2.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_eq(ConstValueType cal_type,
                   const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() == const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() == const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() == const2.get_float());
            return;
        }
        case C_STRING:
            const_value = (size_t)
                    new ParseConstant(const1.get_string() == const2.get_string());
            return;
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_ne(ConstValueType cal_type,
                   const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() != const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() != const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() != const2.get_float());
            return;
        }
        case C_STRING:
            const_value = (size_t)
                    new ParseConstant(const1.get_string() != const2.get_string());
            return;
        default:
            break;
    }
    const_value = (size_t) -1;
}


void ParseExpression::
calculate_const_l(ConstValueType cal_type,
                  const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() < const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() < const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() < const2.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::
calculate_const_le(ConstValueType cal_type,
                   const ParseConstant &const1, const ParseConstant &const2) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_signed() <= const2.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(const1.get_unsigned() <= const2.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(const1.get_float() <= const2.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}


bool ParseExpression::is_const() const {
    if (const_value == (size_t) -1) {
        return false;
    }
    if (!const_value) {
        ((ParseExpression *) this)->calculate_const();
    }
    return const_value != (size_t) -1;
}

size_t ParseExpression::get_address() const {
    return id2address[get_id()];
}

void ParseExpression::set_address(size_t expr_address) const {
    id2address[get_id()] = expr_address;
}

#pragma clang diagnostic pop