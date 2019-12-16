#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_PARSE_NODE_HPP
#define NKU_PRACTICE_PARSE_NODE_HPP

#include "ParseDef.hpp"
#include "ParseType.hpp"
#include "ParseExpression.hpp"
#include <string>
#include <map>

using namespace std;

class ParseVariable;

class ParseFunction;

class ParseConstant;

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
    string get_symbol(ParseTree *tree = nullptr) const;

    /**
     * 获取节点的K_CONST键对应的常量对象引用
     * 如果类型不正确, 会报警告, 并返回默认值
     * @param tree
     * @return
     */
    const ParseConstant get_const(ParseTree *tree = nullptr) const;

    /**
     * 获取节点的K_VARIABLE键对应的变量
     * 如果类型不正确, 会报警告, 并返回默认值
     * @param tree
     * @return
     */
    ParseVariable get_variable(ParseTree *tree = nullptr) const;

    /**
    * 获取节点的K_TYPE键对应的类型
    * 如果类型不正确, 会报警告, 并返回未知类型
     * @param tree
    * @return
    */
    ParseType get_type(ParseTree *tree = nullptr) const;

    /**
    * 获取节点的K_IS_PTR键对应的布尔值
    * 如果类型不正确, 会报警告, 并返回未知类型
     * @param tree
    * @return
    */
    bool get_is_pointer(ParseTree *tree = nullptr) const;

    /**
    * 获取节点的K_PARAM_LIST_NODE键对应的布尔值
    * 如果类型不正确, 会报警告, 并返回未知类型
     * @param tree
    * @return
    */
    size_t get_param_list_node(ParseTree *tree = nullptr) const;

    /**
    * 获取节点的K_FUNCTION键对应的函数记录
    * 如果类型不正确, 会报警告, 并返回默认
     * @param tree
    * @return
    */
    ParseFunction get_function(ParseTree *tree = nullptr) const;

    /**
     * 获取节点的参数列表, 用于函数获取其参数列表时,
    * 目前只有N_PARAM_LIST类型的节点有效
     * 因为此函数必定会往下搜索节点, 所以所需参数是常量引用
     * @param tree
     * @return
     */
    vector<ParseVariable> get_parameters_list(ParseTree &tree) const;

    /**
     * 获取初始化声明器
     * 因为此函数必定会往下搜索节点, 所以所需参数是常量引用
     * @param tree
     * @return
     */
    InitDeclarator get_init_declarator(ParseTree &tree) const;

    /**
     * 获取节点的K_IS_ARRAY值
     * @param tree
     * @return
     */
    bool get_is_array(ParseTree *tree = nullptr) const;

    /**
     * 获取节点的K_INIT_DECLARATOR
     * 因为复制构造比较浪费资源
     * 所以用指针代替, 如果没有定义,返回空指针并警告
     * @return
     */
    const vector<InitDeclarator> *get_init_declarator_list(ParseTree *tree = nullptr) const;

    /**
     * 获取节点的K_EXPRESSION
     * 因为复制构造比较浪费资源
     * 所以用指针代替, 如果没有定义,返回空指针并警告
     * @return
     */
    ParseExpression get_expression(ParseTree *tree = nullptr) const;

    /**
     * 获取节点所属的语句块
     * @return
     */
    size_t get_scope_id(ParseTree *tree = nullptr) const;

    /**
     * 设置节点的K_NEXT键值
     * @param next
     */
    size_t get_next(ParseTree *tree = nullptr) const;

    /**
     * 设置节点的K_BEGIN键值
     * @param begin
     */
    size_t get_begin(ParseTree *tree = nullptr) const;

    /**
     * 设置节点的K_CODE键值
     * @param code
     */
    size_t get_code(ParseTree *tree = nullptr) const;

    /**
     * 设置节点的K_INSTR键值
     * @param instr
     */
    size_t get_instr(ParseTree *tree = nullptr) const;

    /**
     * 设置节点的K_TRUE_LIST键值
     * @param true_list
     */
    vector<size_t> *get_true_list(ParseTree *tree = nullptr);

    /**
     * 设置节点的K_TRUE_LIST键值
     * @param true_list
     */
    vector<size_t> *get_false_list(ParseTree *tree = nullptr);

    /**
     * 获取节点的类型
     * @return
     */
    NodeType get_node_type() const;

    /**
     * 设置节点的K_SYMBOL键对应的值
     * @param symbol
     */
    void set_symbol(const string &symbol);

    /**
     * 设置节点的K_CONST_TYPE和K_CONST_VALUE键对应的值
     * 设置常量值: 有符号整数
     * @return
     */
    void set_const(long long value);

    /**
     * 设置节点的K_CONST_TYPE和K_CONST_VALUE键对应的值
     * 设置常量值: 无符号整数
     * @return
     */
    void set_const(unsigned long long value);

    /**
     * 设置节点的K_CONST_TYPE和K_CONST_VALUE键对应的值
     * 设置常量值: 浮点数
     * @return
     */
    void set_const(long double value);

    /**
     * 设置节点的K_CONST_TYPE和K_CONST_VALUE键对应的值
     * 设置常量值: 布尔
     * @return
     */
    void set_const(bool value);

    /**
     * 设置节点的K_CONST_TYPE和K_CONST_VALUE键对应的值
     * 设置常量值: 字符串
     * @return
     */
    void set_const(const string &value);

    /**
     * 设置节点的K_VARIABLE键对应的值
     * @param type
     * @param symbol
     * @param address
     */
    void set_variable(const ParseType &p_type, const string &symbol, size_t address = (size_t) -1);

    /**
     * 设置节点的K_VARIABLE键对应的值
     * @param variable
     */
    void set_variable(const ParseVariable &variable);

    /**
     * 设置节点的K_TYPE键对应的值
     * 设置类型修饰声明
     * @param type
     */
    void set_type(const ParseType &p_type);

    /**
     * 设置节点的K_IS_PTR键对应的值
     * 设置是否声明为指针
     * @param is_pointer
     */
    void set_is_pointer(bool is_pointer = false);

    /**
     * 设置节点的K_PARAM_LIST_NODE键值
     * 设置是否声明为函数, 如果声明为函数, 则该值不为0,
     * 其值为存有该函数
     * @param param_list_node
     */
    void set_param_list_node(size_t param_list_node = 0);

    /**
     * 设置节点的K_FUNCTION键对应的值
     * 设置函数记录
     * @param function
     */
    void set_function(const ParseFunction &function);

    /**
     * 设置节点的K_EXPRESSION键对应的值
     * 设置表达式记录
     * @param expression
     */
    void set_expression(const ParseExpression &expression);

    /**
     * 设置节点的K_IS_ARRAY键对应的值
     * 设置是否声明为数组
     * @param is_array
     */
    void set_is_array(bool is_array);

    /**
     * 设置节点的K_SCOPE_ID键值
     * 设置其属于的语句块空间id
     * @param scope_id
     */
    void set_scope_id(size_t scope_id);

    /**
     * 设置节点的K_NEXT键值
     * @param next
     */
    void set_next(size_t next);

    /**
     * 设置节点的K_BEGIN键值
     * @param begin
     */
    void set_begin(size_t begin);

    /**
     * 设置节点的K_CODE键值
     * @param code
     */
    void set_code(size_t code);

    /**
     * 设置节点的K_INSTR键值
     * @param instr
     */
    void set_instr(size_t instr);

    /**
     * 设置节点的K_TRUE_LIST键值
     * @param true_list
     */
    void set_true_list(const vector<size_t> &true_list);

    /**
     * 设置节点的K_FALSE_LIST键值
     * @param true_list
     */
    void set_false_list(const vector<size_t> &false_list);

    /**
     * 往节点的K_INIT_DECLARATOR里加入一个InitDeclarator
     * 如果没有K_INIT_DECLARATOR,则new一个
     */
    void add_init_declarator(const InitDeclarator &init_declarator);

    /**
     * 没有警告地更新K_IS_ARRAY键值
     * 更新是否声明为数组
     * @param is_array
     */
    void update_is_array(bool is_array);

    /**
     * 节点动作: 注册声明, 根据节点和子节点的信息声明变量等
     * @param scope_id
     * @param tree
     */
    void action_declaration(size_t scope_id, const ParseTree &tree) const;

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

private:

    /**
     * 当常量值更新时需要执行的函数:负责检查键值是否存在,
     * 如果存在需要删除之,再更新其值
     * @param const_address
     */
    void update_const(size_t const_address);

    /**
     *  删除常量所涉及的字段
     */
    void delete_const();

    /**
     * 删除所有keys所指向的具体对象, 这本来是析构函数所做的事
     * 但是因为vector会构造一个临时对象, 而本人觉得没有必要为每个
     * 节点的每个key都重新new再delete一遍,所以放到整个语法树析构时执行此动作
     * 该动作delete keys所指向的各个具体对象
     * @return
     */
    void delete_all_keys();

    /**
     * 更新节点键值之前动作:
     * 判断节点的类型是否能更新该键值, 并自动释放即将被覆盖的对象,
     * @tparam OpType 传入delete的类参数
     * @param msg 警告信息前缀
     * @param key_type 更新的键值
     * @param ... NodeType 能够进行此操作的节点类型 最后一个要写-1
     */
    template<class OpType>
    void before_update_key(const string &msg, NodeKey key_type, ...);
};

#endif //NKU_PRACTICE_PARSE_NODE_HPP

#pragma clang diagnostic pop