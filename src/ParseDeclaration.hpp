#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef NKU_PRACTICE_PARSE_DECLARATION_HPP
#define NKU_PRACTICE_PARSE_DECLARATION_HPP

#include "ParseDef.hpp"
#include "ParseType.hpp"

/**
 * 用于表示变量记录
 */
class ParseVariable {
public:
    /**
     * 默认构造函数, 初始化
     * type = T_UNKNOWN
     * symbol = ""
     * address = (size_t)-1
     */
    ParseVariable();

    /**
     * 简易地赋值
     * @param _type
     * @param _symbol
     * @param _address
     */
    ParseVariable(const ParseType &_type, const string &_symbol, size_t _address = (size_t) -1);

    /**
     * 简单地复制构造
     * @param other
     */
    ParseVariable(const ParseVariable &other);

    /**
     * 获得变量的类型
     * @return
     */
    const ParseType &get_type() const;

    /**
     * 获得变量的符号
     * @return
     */
    string get_symbol() const;

    /**
     * 获得变量的地址,如果没有赋值,则返回
     * (size_t) -1
     * @return
     */
    size_t get_address() const;

    /**
     * 在构造后再赋予地址
     * @param _address
     */
    void set_address(size_t _address);

    /**
     * 获得变量的信息
     * @return
     */
    string get_info() const;

    ParseVariable &operator=(const ParseVariable &other);

    bool operator<(const ParseVariable &other) const;

private:
    ParseType type;
    string symbol;
    size_t address;
};

/**
 * 用于表示函数记录
 */
class ParseFunction {
public:
    ParseFunction();

    ParseFunction(
            const ParseType &_ret_type, const string &_symbol, const vector<ParseVariable> &_args,
            size_t _address = (size_t) -1);

    ParseFunction(const ParseFunction &other) = default;

    /**
     * 获得返回值类型
     * @return
     */
    ParseType get_ret_type() const;

    /**
     * 获得参数列表
     * @return
     */
    const vector<ParseVariable> &get_args() const;

    /**
     * 获得函数的符号
     * @return
     */
    string get_symbol() const;

    /**
     * 获得函数的地址,如果没有赋值,则返回
     * (size_t) -1
     * @return
     */
    size_t get_address() const;

    /**
     * 在构造后再赋予地址
     * @param _address
     */
    void set_address(size_t _address);

    /**
     * 获得函数的信息
     * @return
     */
    string get_info() const;


    ParseFunction &operator=(const ParseFunction &other) = default;

private:
    ParseType ret_type;
    string symbol;
    vector<ParseVariable> args;
    size_t address;
};

/**
 * 用于表示常量记录
 */
class ParseConstant {

public:
    // 默认构造:
    // type: C_UNDEFINED
    // value: 0
    ParseConstant();

    ParseConstant(const ParseConstant &constant);

    /**
     * 构造有符号整数常量
     * @param v
     */
    explicit ParseConstant(long long v);

    /**
     * 构造无符号整数常量
     * @param v
     */
    explicit ParseConstant(unsigned long long v);

    /**
     * 构造浮点数常量
     * @param v
     */
    explicit ParseConstant(long double v);

    /**
     * 构造布尔常量
     * @param v
     */
    explicit ParseConstant(bool v);

    /**
     * 构造字符串常量
     * @param v
     */
    explicit ParseConstant(const string &v);

    /**
     * 获取有符号整数常量, type不对应会发出警告
     * @return
     */
    long long get_signed() const;

    /**
     * 获取无符号整数常量, type不对应会发出警告
     * @return
     */
    unsigned long long get_unsigned() const;

    /**
     * 获取浮点数常量值, type不对应会发出警告
     * @return
     */
    long double get_float() const;

    /**
     * 获取布尔值常量, type不对应会发出警告
     * @return
     */
    bool get_bool() const;

    /**
     * 获取字符串常量, type不对应会发出警告
     * @return
     */
    string get_string() const;

    /**
     * 获取常量信息
     * @return
     */
    string get_info() const;

    /**
     * 重载小于号, 用于map比较
     * @param other
     * @return
     */
    bool operator<(const ParseConstant &other) const;

    /**
     * 重载等于号: 赋值
     * @param other
     * @return
     */
    ParseConstant &operator=(const ParseConstant &other);

    /**
     * 获取常量类型
     * @return
     */
    ConstValueType get_type() const;

    ~ParseConstant();

    /**
     * 获取每个常量类型的名字
     * @param _type
     * @return
     */
    static string get_const_type_name(ConstValueType _type);

    /**
     * 获取常量自动类型转化后的常量类型
     * @param type1
     * @param type2
     */
    static ConstValueType wider_const_type(ConstValueType type1, ConstValueType type2);

private:
    ConstValueType type;
    // 指针: 根据type不同指向不同的对象
    // C_SIGNED: longlong*
    // C_UNSIGNED: unsigned long long*
    // C_FLOAT: long double*
    // C_STRING: string*
    // C_BOOL: bool*
    size_t value;

    /**
     * 赋值函数
     * @param constant
     * @param from_constant
     */
    static void assign(ParseConstant &constant, const ParseConstant &from_constant);
};

/**
 * 用于记录和表示已经声明符号和对应的函数,常量,变量等声明
 */
class ParseDeclaration {
public:
    /**
     * 进入一个新的空间:
     * 符号表中可以重复声明之前声明过的变量符号
     * init()函数初始化一个空间,这个空间就是静态空间
     */
    static void push_scope();

    /**
     * 离开之前的空间, 释放这个空间内压栈的所有局部变量声明
     */
    static void pop_scope();

    /**
     * 声明符号为变量
     * @param symbol
     * @param variable
     */
    static void declaration(const string &symbol, const ParseVariable &variable);

    /**
     * 声明符号为函数
     * @param symbol
     * @param variable
     */
    static void declaration(const string &symbol, const ParseFunction &function);

    /**
     * 获取声明的函数, 如果没有对应记录, 警告并返回无效声明
     * @param symbol
     * @return
     */
    static vector<ParseFunction> get_function_declaration(const string &symbol);

    /**
     *获取声明的变量, 如果没有对应记录, 警告并返回无效声明
     * @param symbol
     * @return
     */
    static const ParseVariable &get_variable_declaration(const string &symbol);

    /**
     * 打印所有已有的声明
     */
    static void print_all_declaration();

private:
    struct Scope {
        //由于有函数重载, 所以映射到的对象是id的集合
        map<string, pair<DeclarationType , vector<size_t> > > symbol2dec;
        // 这里有些冗余, 但是这个类没有对象, 所以没有析构函数
        // 为了自动清除这些记录, 所以用了有些冗余的处理方式
        // 如果该id不是变量,则存默认值, 否则存有效值
        vector<ParseVariable> id2var;
        // 如果该id不是函数,则存默认值, 否则存有效值
        vector<ParseFunction> id2func;
    };

    static vector<Scope> scope_stack;

    /**
     * 不允许生成对象
     */
    ParseDeclaration() = default;

    /**
     * 初始化
     */
    static void init();
};


#endif //NKU_PRACTICE_PARSE_DECLARATION_HPP

#pragma clang diagnostic pop