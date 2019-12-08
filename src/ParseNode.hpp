#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_PARSE_NODE_HPP
#define NKU_PRACTICE_PARSE_NODE_HPP

#include "ParseDef.hpp"
#include "ParseType.hpp"
#include <string>
#include <map>

using namespace std;

class ParseNode {
    friend class ParseTree;

    size_t node_id;
    NodeType type;
    // 字段到字段值的映射, 值是一个指针需要经过转换
    map<int, size_t> keys;

public:

    explicit ParseNode(size_t _node_id, NodeType type);

    ParseNode(const ParseNode &other);

    /**
     * 获取节点的K_SYMBOL键对应的值
     * @param tree 如果节点的类型没有此键值,如果有可能, 会自动向下寻找其子节点中是否有符合语境的唯一的相应键值
     *                          如果有, 返回之, 但是节点并没有存树的信息,所以这种情况下需要传一个树的指针,
     *                          以下的get_node_key系列的tree参数都是如此
     */
    string get_symbol(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的K_CONST_TYPE键对应的值
     * 获取该节点的常量类型,
     * 如果不是常量类型节点,则返回-1
     * 否则返回ConstValueType
     * @param tree
     * @return
     */
    int get_const_type(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的K_CONST_VALUE键对应的有符号整数值
     * 如果类型不正确, 会报警告, 并返回默认值
     * @param tree
     * @return
     */
    long long get_const_signed_value(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的K_CONST_VALUE键对应的无符号整数值
     * 如果类型不正确, 会报警告, 并返回默认值
     * @param tree
     * @return
     */
    unsigned long long get_const_unsigned_value(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的K_CONST_VALUE键对应的字符串值
     * 如果类型不正确, 会报警告, 并返回默认值
     * @param tree
     * @return
     */
    string get_const_string_value(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的K_CONST_VALUE键对应的浮点数值
     * 如果类型不正确, 会报警告, 并返回默认值
     * @param tree
     * @return
     */
    long double get_const_float_value(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的K_CONST_VALUE键对应的布尔值
     * 如果类型不正确, 会报警告, 并返回默认值
     * @param tree
     * @return
     */
    bool get_const_bool_value(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的K_VAR_TYPE键对应的类型
     * 如果类型不正确, 会报警告, 并返回未知类型
     * @param tree
     * @return
     */
//    ParseType get_variable_type(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的K_VAR_ADDRESS键对应的类型
     * 如果类型不正确, 会报警告, 并返回0
     * @param tree
     * @return
     */
//    size_t get_variable_address(ParseTree* tree = nullptr) const;

    /**
    * 获取节点的K_TYPE键对应的类型
    * 如果类型不正确, 会报警告, 并返回未知类型
     * @param tree
    * @return
    */
    ParseType get_type(ParseTree* tree = nullptr) const;

    /**
     * 获取节点的类型
     * @return
     */
    NodeType get_node_type() const;

    /**
     * 设置节点的symbol键对应的值
     * @param symbol
     */
    void set_symbol(const string &symbol);

    /**
     * 设置常量值: 有符号整数
     * @return
     */
    void set_const(long long value);

    /**
     * 设置常量值: 无符号整数
     * @return
     */
    void set_const(unsigned long long value);

    /**
     * 设置常量值: 浮点数
     * @return
     */
    void set_const(long double value);

    /**
     * 设置常量值: 布尔
     * @return
     */
    void set_const(bool value);

    /**
     * 设置常量值: 字符串
     * @return
     */
    void set_const(const string &value);

    /**
     * 设置变量值
     * @param type
     * @param symbol
     * @param address
     */
//    void set_variable(const ParseType &type, const string &symbol, size_t address);

    /**
     * 设置类型修饰声明
     * @param type
     */
    void set_type_specifier(const ParseType &p_type);

    /**
     * 获取节点信息
     * @return
     */
    string get_node_info() const;

    /**
     * 获取节点键的名字
     * @param type
     * @return
     */
    static string get_key_name(NodeKey type);

    /**
     * 获取节点类型的名字
     * @param type
     * @return
     */
    static string get_node_type_name(NodeType type);

    /**
     * 获取类型名字
     * @param type
     * @return
     */
    static string get_const_type_name(ConstValueType type);

private:

    /**
     * 当常量值更新时需要执行的函数:负责检查键值是否存在,
     * 如果存在需要删除之,再更新其值
     * @param const_type_address
     * @param const_value_address
     */
    void update_const(size_t const_type_address, size_t const_value_address);

    /**
     *  删除常量所涉及的字段
     */
    void delete_const();

    /**
     * 当变量更新时需要执行的函数:
     * 负责检查键值是否存在,如果存在需要删除之
     * @param type_address 指向变量类型的地址
     * @param symbol_address 指向变量符号的地址
     * @param var_add_address 指向变量地址(size_t)的地址
     */
//    void update_variable(size_t type_address, size_t symbol_address, size_t var_add_address);

    /**
     * 输出时用, 获取表示常量的值的字符串
     * @return
     */
    string get_const_value_str() const;

    /**
     * 删除所有keys所指向的具体对象, 这本来是析构函数所做的事
     * 但是因为vector会构造一个临时对象, 而本人觉得没有必要为每个
     * 节点的每个key都重新new再delete一遍,所以放到整个语法树析构时执行此动作
     * 该动作delete keys所指向的各个具体对象
     * @return
     */
    void delete_all_keys();
};

#endif //NKU_PRACTICE_PARSE_NODE_HPP

#pragma clang diagnostic pop