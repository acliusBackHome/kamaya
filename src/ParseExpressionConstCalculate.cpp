// 此文件是用于ParseExpression的常量计算, 减小ParseExpression.cpp文件长度
#include "ParseExpression.hpp"
#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

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

        case E_ADR:
        case E_PTR: {
            // 还未实现的调用
            string info = "ParseExpression::calculate_const() not implemented calling address operator";
            throw ParseException(EX_NOT_IMPLEMENTED, info);
        }
        case E_NEG:
        case E_POS:
        case E_BIT_NOT:
        case E_NOT: {
            if (!get_expression(child[0]).is_const()) {
                // 不是常量退出
                break;
            }
            const ParseConstant &constant = get_expression(child[0]).get_const();
            switch (expr_type) {
                case E_NEG: {
                    calculate_const_neg(constant.get_type(), constant);
                    return;
                }
                case E_POS: {
                    const_value = (size_t)
                            new ParseConstant(constant);
                    return;
                }
                case E_BIT_NOT: {
                    calculate_const_bit_not(constant.get_type(), constant);
                    return;
                }
                case E_NOT: {
                    calculate_const_not(constant.get_type(), constant);
                    return;
                }
                default:
                    break;
            }
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
        case E_GET_ITEM: // 能进行后缀表达式运算的都不会是常量
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

void ParseExpression::calculate_const_neg(ConstValueType cal_type, const ParseConstant &constant) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(-constant.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(-constant.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(-constant.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::calculate_const_bit_not(ConstValueType cal_type, const ParseConstant &constant) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(~constant.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(~constant.get_unsigned());
            return;
        }
        case C_FLOAT:
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}

void ParseExpression::calculate_const_not(ConstValueType cal_type, const ParseConstant &constant) {
    switch (cal_type) {
        case C_BOOL:
        case C_SIGNED: {
            const_value = (size_t)
                    new ParseConstant(!constant.get_signed());
            return;
        }
        case C_UNSIGNED: {
            const_value = (size_t)
                    new ParseConstant(!constant.get_unsigned());
            return;
        }
        case C_FLOAT: {
            const_value = (size_t)
                    new ParseConstant(!constant.get_float());
            return;
        }
        case C_STRING:
        default:
            break;
    }
    const_value = (size_t) -1;
}


#pragma clang diagnostic pop