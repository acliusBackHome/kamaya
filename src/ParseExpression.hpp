#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef NKU_PRACTICE__PARSE_EXPRESSION_H
#define NKU_PRACTICE__PARSE_EXPRESSION_H

#include <string>

/**
 * 用于表示表达式, 并利用全局变量进行DAG优化
 */
class ParseExpression {
public:
    ParseExpression();

    ParseExpression(const ParseExpression &expression) = default;

    std::string get_info() const;

    bool is_defined() const;

private:
    bool defined;
};


#endif //NKU_PRACTICE__PARSE_EXPRESSION_H

#pragma clang diagnostic pop