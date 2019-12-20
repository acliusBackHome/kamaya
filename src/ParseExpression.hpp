#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef NKU_PRACTICE__PARSE_EXPRESSION_H
#define NKU_PRACTICE__PARSE_EXPRESSION_H

#include <string>
#include <map>
#include <vector>
#include "ParseType.hpp"
#include <cmath>

using namespace std;

/**
 * 用于表示表达式, 并利用全局变量进行DAG优化
 */
class ParseExpression {
public:
    ParseExpression();

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
     * 获取表达式类型的名字
     * @param type
     * @return
     */
    static string get_expr_type_name(ExpressionType type);

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
    const ParseExpression &operator+(const ParseExpression &expr) const;


    /**
     * 获得减法表达式
     * @param expr
     * @return
     */
    const ParseExpression &operator-(const ParseExpression &expr) const;

    /**
     * 获得乘法表达式
     * @param expr
     * @return
     */
    const ParseExpression &operator*(const ParseExpression &expr) const;

    /**
     * 获得除法表达式
     * @param expr
     * @return
     */
    const ParseExpression &operator/(const ParseExpression &expr) const;

    /**
     * 获得取模表达式
     * @param expr
     * @return
     */
    const ParseExpression &operator%(const ParseExpression &expr) const;

    /**
     * 获得求幂表达式
     * @param expr
     * @return
     */
    const ParseExpression &operator^(const ParseExpression &expr) const;

    /**
     * 获得非表达式, 即!expression
     * @param expr
     * @return
     */
    const ParseExpression &operator!() const;

    /**
     * 获取表达式的常量, 如果不存在常量值,则会抛出异常
     * @return
     */
    const ParseConstant &get_const() const;

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

    /**
     * 判断该表达式是否是常量
     * @return
     */
    bool is_const() const;

    /**
     * 后缀中括号表达式
     * @param expr
     * @return
     */
    const ParseExpression &get_item(const ParseExpression &expr) const;

    /**
     * 输出所有的表达式
     */
    static void print_all_expression();

    /**
     * 获得逻辑表达式, 因为operator<被map用了,所以不能用运算符重载来简单调用
     * 产生逻辑表达式了
     * @param expr
     * @return
     */
    static const ParseExpression &get_logic_expression(ExpressionType type,
                                                       const ParseExpression &expr1, const ParseExpression &expr2);

    /**
     * 获取赋值语句
     * @param variable
     * @param expr
     * @return
     */
    static const ParseExpression &get_assign_expression(
            const ParseExpression &unary_expr, const ParseExpression &expr
    );

    /**
     * 获取表达式的地址,
     * 如果地址未被赋值过, 会返回0
     * @return
     */
    size_t get_address() const;

    /**
     * 设置表达式的地址, 如果两个表达式对象结构一样,
     * 设置其中一个的地址会导致另一个的地址也随之改变, 因为表达式
     * 会在全局有一份唯一不重复的备份
     * @param expr_address
     */
    void set_address(size_t expr_address) const;

    /**
    * 生成双目表达式统一处理函数
    * @param expr1
    * @param expr2
    * @return
    */
    static const ParseExpression &
    generate_expression(ExpressionType type,
                        const ParseExpression &expr1, const ParseExpression &expr2);

    /**
    * 生成单目表达式统一处理函数
    * @param expr
    * @return
    */
    static const ParseExpression &
    generate_expression(ExpressionType type, const ParseExpression &expr);


private:
    size_t child[2], // 子表达式id, 有时候是ParseVariable*所以用size_t代替地址,目前只用两个
            expr_id, //缓存当前类型的id
            const_value,// 能否在编译期间被计算出来, 如果能,则是一个非0值,是指向ParseConstant的指针,否则是0
            ret_type_id; // 返回值类型的全局id
    ExpressionType expr_type;

    static map<ParseExpression, size_t> expr2id;
    static vector<ParseExpression> id2expr;
    static vector<size_t> id2address;

    /**
     * 计算当前表达式的常数值, 如果不能计算常数值, 则返回默认值
     * @return
     */
    void calculate_const();

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_add(ConstValueType cal_type,
                             const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_sub(ConstValueType cal_type,
                             const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_mul(ConstValueType cal_type,
                             const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_div(ConstValueType cal_type,
                             const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_mod(ConstValueType cal_type,
                             const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_g(ConstValueType cal_type,
                           const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_ge(ConstValueType cal_type,
                            const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_eq(ConstValueType cal_type,
                            const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_ne(ConstValueType cal_type,
                            const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_l(ConstValueType cal_type,
                           const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_le(ConstValueType cal_type,
                            const ParseConstant &const1, const ParseConstant &const2
    );

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_neg(ConstValueType cal_type, const ParseConstant &constant);

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_bit_not(ConstValueType cal_type, const ParseConstant &constant);

    /**
     * calculate_const太长而拆分出的子函数
     * @param cal_type 进行运算时的常量类型
     * @param const1
     * @param const2
     */
    void calculate_const_not(ConstValueType cal_type, const ParseConstant &constant);

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