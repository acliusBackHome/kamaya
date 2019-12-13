#include <cmath>
#include "ParseExpression.hpp"
#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
using namespace std;

map<ParseExpression, size_t> ParseExpression::expr2id;
vector<ParseExpression> ParseExpression::id2expr;

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
        case E_DIV: {
            const ParseExpression &expr1 = id2expr[child[0]],
                    &expr2 = id2expr[child[1]];
            sprintf(buff, "{ Expr-%zu ", child[0]);
            res += buff;
            switch (expr_type) {
                case E_ADD:
                    res += "+";
                    break;
                case E_SUB:
                    res += "-";
                    break;
                case E_MUL:
                    res += "*";
                    break;
                case E_DIV:
                    res += "/";
                    break;
                default:
                    break;
            }
            sprintf(buff, " Expr-%zu }, ", child[1]);
            res += buff;
            break;
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
    res += "return_type: " + get_ret_type().get_info() + "}";
    return res;
}

ParseExpression::ParseExpression() {
    ret_type_id = T_UNKNOWN;
    child[0] = child[1] = expr_id = 0;
    const_value = 0;
    expr_type = E_UNDEFINED;
}

ParseExpression::ParseExpression(const ParseConstant &constant) {
    expr_type = E_CONST;
    child[0] = (size_t) new ParseConstant(constant);
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
    child[0] = (size_t) new ParseVariable(variable);
    update(*this);
}

void ParseExpression::init() {
    id2expr.emplace_back(ParseExpression());
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
    if (const_value) {
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
        default:
            break;
    }
}

const ParseExpression &ParseExpression::get_expression(size_t expr_id) {
    if (id2expr.empty()) {
        init();
    }
    if (expr_id >= id2expr.size()) {
        printf("ParseExpression::expression(size_t expr_id): 警告:试图获取未定义的类型ID:%zu\n",
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
    child[0] = (size_t) new ParseVariable(variable);
    update(*this);
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
    } else {
        ((ParseExpression &) expr).expr_id = it->second;
    }
}

ParseExpression &ParseExpression::operator=(const ParseExpression &expr) {
    assign(*this, expr);
    return *this;
}

ParseExpression ParseExpression::operator+(const ParseExpression &expr) {
    return generate_expression(E_ADD, *this, expr);
}

ParseExpression ParseExpression::operator-(const ParseExpression &expr) {
    return generate_expression(E_SUB, *this, expr);
}

ParseExpression ParseExpression::operator*(const ParseExpression &expr) {
    return generate_expression(E_MUL, *this, expr);
}

ParseExpression ParseExpression::operator/(const ParseExpression &expr) {
    return generate_expression(E_DIV, *this, expr);
}

ParseExpression ParseExpression::operator%(const ParseExpression &expr) {
    return generate_expression(E_MOD, *this, expr);
}

size_t ParseExpression::get_expr_id(const ParseExpression &expr) {
    if (expr.expr_id) {
        return expr.expr_id;
    }
    update(expr);
    return expr.expr_id;
}

ParseExpression ParseExpression::generate_expression(
        ExpressionType type,
        const ParseExpression &expr1,
        const ParseExpression &expr2) {
    ParseExpression res;
    res.expr_type = type;
    res.child[0] = get_expr_id(expr1);
    res.child[1] = get_expr_id(expr2);
    res.ret_type_id = ParseType::wider_type(expr1.get_ret_type(), expr2.get_ret_type()).get_id();
    update(res);
    return res;
}

void ParseExpression::assign(ParseExpression &expr, const ParseExpression &from_expr) {
    expr.expr_id = from_expr.expr_id;
    expr.ret_type_id = from_expr.ret_type_id;
    if (from_expr.const_value) {
        expr.const_value = (size_t) new ParseConstant(*(ParseConstant *) from_expr.const_value);
    } else {
        expr.const_value = 0;
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
            expr.child[0] = from_expr.child[0];
            expr.child[1] = from_expr.child[1];
            break;
        case E_VAR:
            expr.child[0] = (size_t) new ParseVariable(*(ParseVariable *) from_expr.child[0]);
            break;
        case E_CONST:
            expr.child[0] = (size_t) new ParseConstant(*(ParseConstant *) from_expr.child[0]);
            break;
    }
}

ParseConstant ParseExpression::get_const() const {
    if (const_value) {
        return *(ParseConstant *) const_value;
    }
    switch (expr_type) {
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_MOD:
        case E_POW: {
            ParseConstant const1 = id2expr[child[0]].get_const(),
                    const2 = id2expr[child[1]].get_const();
            ConstValueType ret_const_type = ParseConstant::wider_const_type(const1.get_type(), const2.get_type());
            if (ret_const_type == C_UNDEFINED) {
                return ParseConstant();
            }
            switch (expr_type) {
                case E_ADD: {
                    switch (ret_const_type) {
                        case C_BOOL:
                        case C_SIGNED: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_signed() + const2.get_signed());

                            return *(ParseConstant *) const_value;
                        }
                        case C_UNSIGNED: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_unsigned() + const2.get_unsigned());
                            return *(ParseConstant *) const_value;
                        }
                        case C_FLOAT: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_float() + const2.get_float());
                            return *(ParseConstant *) const_value;
                        }
                        case C_STRING:
                        default:
                            break;
                    }
                    printf("ParseExpression::get_const():警告:不支持的加法操作\n");
                    return ParseConstant();
                }
                case E_SUB: {
                    switch (ret_const_type) {
                        case C_BOOL:
                        case C_SIGNED: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_signed() - const2.get_signed());
                            return *(ParseConstant *) const_value;
                        }
                        case C_UNSIGNED: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_unsigned() - const2.get_unsigned());
                            return *(ParseConstant *) const_value;
                        }
                        case C_FLOAT: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_float() - const2.get_float());
                            return *(ParseConstant *) const_value;
                        }
                        case C_STRING:
                        default:
                            break;
                    }
                    printf("ParseExpression::get_const():警告:不支持的减法操作\n");
                    return ParseConstant();
                }
                case E_MUL: {
                    switch (ret_const_type) {
                        case C_BOOL:
                        case C_SIGNED: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_signed() * const2.get_signed());
                            return *(ParseConstant *) const_value;
                        }
                        case C_UNSIGNED: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_unsigned() * const2.get_unsigned());
                            return *(ParseConstant *) const_value;
                        }
                        case C_FLOAT: {
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(const1.get_float() * const2.get_float());
                            return *(ParseConstant *) const_value;
                        }
                        case C_STRING:
                        default:
                            break;
                    }
                    printf("ParseExpression::get_const():警告:不支持的乘法操作\n");
                    return ParseConstant();
                }
                case E_DIV: {
                    switch (ret_const_type) {
                        case C_BOOL:
                        case C_SIGNED: {
                            auto divisor = const2.get_signed();
                            if (divisor == 0) {
                                printf("ParseExpression::get_const(): 警告: 常量计算过程中试图除以0\n");
                                return ParseConstant();
                            }
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(ParseConstant(const1.get_signed() / divisor));
                            return *(ParseConstant *) const_value;
                        }
                        case C_UNSIGNED: {
                            auto divisor = const2.get_unsigned();
                            if (divisor == 0) {
                                printf("ParseExpression::get_const(): 警告: 常量计算过程中试图除以0\n");
                                return ParseConstant();
                            }
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(ParseConstant(const1.get_unsigned() / divisor));
                            return *(ParseConstant *) const_value;
                        }
                        case C_FLOAT: {
                            auto divisor = const2.get_float();
                            if (divisor == 0) {
                                printf("ParseExpression::get_const(): 警告: 常量计算过程中试图除以0\n");
                                return ParseConstant();
                            }
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(ParseConstant(const1.get_float() / divisor));
                            return *(ParseConstant *) const_value;
                        }
                        case C_STRING:
                        default:
                            break;
                    }
                    printf("ParseExpression::get_const():警告:不支持的除法操作\n");
                    return ParseConstant();
                }
                case E_MOD: {
                    switch (ret_const_type) {
                        case C_BOOL:
                        case C_SIGNED: {
                            auto divisor = const2.get_signed();
                            if (divisor == 0) {
                                printf("ParseExpression::get_const(): 警告: 常量计算过程中试图除以0\n");
                                return ParseConstant();
                            }
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(ParseConstant(const1.get_signed() % divisor));
                            return *(ParseConstant *) const_value;
                        }
                        case C_UNSIGNED: {
                            auto divisor = const2.get_unsigned();
                            if (divisor == 0) {
                                printf("ParseExpression::get_const(): 警告: 常量计算过程中试图除以0\n");
                                return ParseConstant();
                            }
                            ((ParseExpression *) this)->const_value =
                                    (size_t) new ParseConstant(ParseConstant(const1.get_unsigned() % divisor));
                            return *(ParseConstant *) const_value;
                        }
                        case C_FLOAT:
                        case C_STRING:
                        default:
                            break;
                    }
                    printf("ParseExpression::get_const():警告:不支持的取模操作\n");
                    return ParseConstant();
                }
                case E_POW: {
                    printf("ParseExpression::get_const():警告:不支持的求幂操作\n");
                    return ParseConstant();
                }
                default:
                    break;
            }
            break;
        }
        case E_CONST: {
            return *(ParseConstant *) child[0];
        }
        case E_UNDEFINED:
        case E_VAR:
            break;
    }
    printf("ParseExpression::get_const(): 警告: 试图获取非常量表达式-%zu的常量值\n",
           get_expr_id(*this)
    );
    return ParseConstant();
}

const ParseType &ParseExpression::get_ret_type() const {
    if (ret_type_id) {
        return ParseType::get_type(ret_type_id);
    }
    switch (expr_type) {
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_MOD:
        case E_POW: {
            size_t &ret_cache = (((ParseExpression *) this)->ret_type_id);
            ret_cache = ParseType::wider_type(
                    id2expr[child[0]].get_ret_type(), id2expr[child[0]].get_ret_type()
            ).get_id();
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
        case E_UNDEFINED:
            break;
    }
    return ParseType::get_type(T_UNKNOWN);
}

size_t ParseExpression::get_child(size_t _child) const {
    if (_child >= 2) {
        printf("ParseExpression::get_child(size_t _child): 警告: 试图获取表达式%zu的异常子表达式 child_id %zu\n",
               get_expr_id(*this), _child);
        return (size_t) -1;
    }
    switch (expr_type) {
        case E_ADD:
        case E_SUB:
        case E_MUL:
        case E_DIV:
        case E_MOD:
        case E_POW:
            return child[_child];
        case E_VAR:
        case E_CONST:
        case E_UNDEFINED:
            printf("ParseExpression::get_child(size_t _child): 警告:  试图获取表达式%zu的未定义子表达式\n",
                   get_expr_id(*this));
            return (size_t) -1;
    }
    return (size_t) -1;
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


#pragma clang diagnostic pop