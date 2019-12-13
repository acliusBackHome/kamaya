#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef NKU_PRACTICE__PARSE_EXPRESSION_H
#define NKU_PRACTICE__PARSE_EXPRESSION_H

#include <string>
#include <map>
#include <vector>
#include "ParseType.hpp"

using namespace std;

class ParseVariable;

class ParseConstant;

/**
 * 用于表示表达式, 并利用全局变量进行DAG优化
 */
class ParseExpression {
public:
    explicit ParseExpression(const ParseVariable &variable);

    ParseExpression(const ParseExpression &exp);

    /**
     * 从常量构造表达式
     * @param constant
     */
    explicit ParseExpression(const ParseConstant &constant);

    ~ParseExpression();

    string get_info() const;

    /**
     * 获取表达式的类型
     * @return
     */
    ExpressionType get_expr_type() const;

    /**
     * 获取表达式的全局注册id
     * @return
     */
    size_t get_id() const;

    /**
     * 获取表达式的返回值
     * @return
     */
    const ParseType &get_ret_type() const;

    /**
     * 重载小于符, 用于全局map寻找相同结构的表达式用于DAG优化
     * @param other
     * @return
     */
    bool operator<(const ParseExpression &other) const;

    /**
     * 全局获取表达式
     * @param expr_id
     * @return
     */
    static const ParseExpression &get_expression(size_t expr_id);

    /**
     * 生成一个只有变量的表达式, 自动注册其id
     * @param variable
     * @return
     */
    ParseExpression &operator=(const ParseVariable &variable);

    /**
     * 复制
     * @param expr
     * @return
     */
    ParseExpression &operator=(const ParseExpression &expr);

    /**
     * 获得加法表达式
     * @param expr
     * @return
     */
    ParseExpression operator+(const ParseExpression &expr);


    /**
     * 获得减法表达式
     * @param expr
     * @return
     */
    ParseExpression operator-(const ParseExpression &expr);

    /**
     * 获得乘法表达式
     * @param expr
     * @return
     */
    ParseExpression operator*(const ParseExpression &expr);

    /**
     * 获得除法表达式
     * @param expr
     * @return
     */
    ParseExpression operator/(const ParseExpression &expr);

    /**
     * 获得取模表达式
     * @param expr
     * @return
     */
    ParseExpression operator%(const ParseExpression &expr);

    /**
     * 获取表达式的常量, 如果不存在常量值,则会发出警告,可以在未来成熟后改成异常
     * @return
     */
    ParseConstant get_const() const;

    /**
     * 获取表达式的子表达式id, child参数目前小于2
     * 如果该表达式的类型没有子表达式, 则返回(size_t) -1
     * @param child
     * @return
     */
    size_t get_child(size_t child) const;

    /**
     * 判断该表达式是否定义
     * @return
     */
    bool is_defined() const;

    ParseExpression();

    /**
     * 输出所有的表达式
     */
    static void print_all_expression();

private:
    size_t child[2], // 子表达式id, 有时候是ParseVariable*所以用size_t代替地址,目前只用两个
            expr_id, //缓存当前类型的id
            const_value,// 能否在编译期间被计算出来, 如果能,则是一个非0值,是指向ParseConstant的指针,否则是0
            ret_type_id; // 返回值类型的全局id
    ExpressionType expr_type;

    static map<ParseExpression, size_t> expr2id;
    static vector<ParseExpression> id2expr;

    /**
    * 生成表达式统一处理函数
    * @param expr1
    * @param expr2
    * @return
    */
    static ParseExpression generate_expression(ExpressionType type,
                                               const ParseExpression &expr1, const ParseExpression &expr2);

    /**
     * 初始化
     */
    static void init();

    /**
     * 获得一个表达式的全局id
     * @param expr
     * @return
     */
    static size_t get_expr_id(const ParseExpression &expr);

    /**
     * 赋值
     * @param expr
     * @param from_expr
     */
    static void assign(ParseExpression &expr, const ParseExpression &from_expr);

    /**
     * 更新全局注册表达式信息
     * @param expr
     */
    static void update(const ParseExpression &expr);

};


#endif //NKU_PRACTICE__PARSE_EXPRESSION_H

#pragma clang diagnostic pop