#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_PARSE_NODE_HPP
#define NKU_PRACTICE_PARSE_NODE_HPP

#include "ParseDef.hpp"
#include "ParseType.hpp"
#include "ParseExpression.hpp"
#include "IR.hpp"
#include "ParseException.hpp"
#include <string>
#include <map>

using namespace std;

class ParseNode {
    friend class ParseTree;

    size_t node_id, keys;
    ParseTree &tree;

    NodeType type;

public:
    explicit ParseNode(ParseTree &_tree, size_t _node_id, NodeType type);

    ParseNode(const ParseNode &other);

    /**
     * 判断节点是否有某个键的记录
     * @param key
     * @return
     */
    bool has_key(NodeKey key);

    /**
     * 获取节点的K_SYMBOL键对应的值
     * get_系列函数都会抛出ParseException异常
     */
    const string &get_symbol() const;

    /**
     * 获取节点的K_CONST键对应的常量对象引用
     * get_系列函数都会抛出ParseException异常
     * @return
     */
    const ParseConstant &get_const() const;

    /**
     * 获取节点的K_VARIABLE键对应的变量
     * get_系列函数都会抛出ParseException异常
     * @return
     */
    const ParseVariable &get_variable() const;

    /**
    * 获取节点的K_TYPE键对应的类型
    * get_系列函数都会抛出ParseException异常
    * @return
    */
    const ParseType &get_type() const;

    /**
    * 获取节点的K_PTR_LV键对应的级数
    * get_系列函数都会抛出ParseException异常
    * @return
    */
    size_t get_ptr_lv() const;

    /**
    * 获取节点的K_PARAM_LIST_NODE键对应的值
    * get_系列函数都会抛出ParseException异常
    * @return
    */
    size_t get_param_list_node() const;

    /**
    * 获取节点的K_FUNCTION键对应的函数记录
    * get_系列函数都会抛出ParseException异常
    * @return
    */
    ParseFunction &get_function() const;

    /**
     * 获取节点的参数列表, 用于函数获取其参数列表时,
     * get_系列函数都会抛出ParseException异常
     * @return
     */
    const vector<ParseVariable> &get_parameters_list() const;

    /**
     * 获取初始化声明器
     * get_系列函数都会抛出ParseException异常
     * @return
     */
    const InitDeclarator &get_init_declarator() const;

    /**
     * 获取节点的K_ARRAY_SIZE值
     * get_系列函数都会抛出ParseException异常
     * @return
     */
    const vector<size_t> &get_array_size() const;

    /**
     * 获取节点的K_INIT_DECLARATOR值
     * get_系列函数都会抛出ParseException异常
     * @return
     */
    const vector<InitDeclarator> &get_init_declarators() const;

    /**
     * 获取节点的K_EXPRESSION值
     * get_系列函数都会抛出ParseException异常
     * @return
     */
    const ParseExpression &get_expression() const;

    /**
     * 获取节点所属的语句块id
     * get_系列函数都会抛出ParseException异常
     * @return
     */
    size_t get_scope_id() const;

    /**
     * 获取节点的K_NEXT键值
     * get_系列函数都会抛出ParseException异常
     */
    size_t get_next() const;

    /**
     * 获取节点的K_BEGIN键值
     * get_系列函数都会抛出ParseException异常
     */
    size_t get_begin() const;

    /**
     * 获取节点的K_CODE键值
     * get_系列函数都会抛出ParseException异常
     */
    size_t get_code() const;

    /**
     * 获取节点的K_INSTR键值
     * get_系列函数都会抛出ParseException异常
     */
    size_t get_instr() const;

    /**
     * 获取节点的K_TRUE_LIST键值
     * get_系列函数都会抛出ParseException异常
     */
    const vector<size_t> &get_true_list() const;

    /**
     * 获取节点的K_FALSE_LIST键值
     * get_系列函数都会抛出ParseException异常
     */
    const vector<size_t> &get_false_list() const;

    /**
     * 获取节点的K_TRUE_JUMP键值
     * get_系列函数都会抛出ParseException异常
     */
    size_t get_true_jump() const;

    /**
     * 获取节点的K_FALSE_JUMP键值
     * get_系列函数都会抛出ParseException异常
     */
    size_t get_false_jump() const;

    /**
     * 获取节点的K_BEGIN_CODE键值
     * get_系列函数都会抛出ParseException异常
     */
    size_t get_begin_code() const;

    /**
     * 获取节点的K_NEXT_LIST键值
     * @param true_list
     */
    const vector<size_t> &get_next_list() const;

    /**
     * 获取节点的K_EXPRESSION键值
     * @param true_list
     */
    const vector<size_t> &get_expression_list() const;

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
     * 设置节点的K_PTR_LV键对应的值
     * 设置声明指针的级数
     * @param ptr_lv
     */
    void set_ptr_lv(size_t ptr_lv);

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
     * @param false_list
     */
    void set_false_list(const vector<size_t> &false_list);

    /**
     * 设置节点的K_TRUE_JUMP键值
     * @param true_jump
     */
    void set_true_jump(size_t true_jump);

    /**
     * 设置节点的K_FALSE_JUMP键值
     * @param false_jump
     */
    void set_false_jump(size_t);

    /**
     * 设置节点的K_NEXT_LIST键值
     * @param next_list
     */
    void set_next_list(const vector<size_t> &next_list);

    /**
     * 设置节点的K_PARAM_LIST键值
     * @param param_list
     */
    void set_param_list(const vector<ParseVariable> &param_list);

    /**
     * 设置节点的K_INIT_DEC键值
     * @param init_dec
     */
    void set_init_dec(const InitDeclarator &init_dec);

    /**
     * 设置节点的K_BEGIN_CODE
     * @param code
     */
    void set_begin_code(size_t begin_code);

    /**
     * 设置节点的K_ARRAY_SIZE
     * @param code
     */
    void set_array_size(const vector<size_t> &array_size);

    /**
     * 往节点的K_EXPRESSION_LIST里加入一个ParseExpression的id
     * 如果没有K_EXPRESSION_LIST,则new一个
     * @param expr
     */
    void add_expression_list(const ParseExpression &expr);

    /**
     * 增加节点的K_ARRAY_SIZE键对应的值列表
     * @param array_size 这一层的数组大小
     */
    void add_array_size(size_t array_size);

    /**
     * 往节点的K_INIT_DECLARATOR里加入一个InitDeclarator
     * 如果没有K_INIT_DECLARATOR,则new一个
     */
    void add_init_declarator(const InitDeclarator &init_declarator);

    /**
     * 没有警告地更新K_ARRAY_SIZE键值
     * 更新是否声明为数组
     * @param array_size
     */
    void update_array_size(size_t array_size);

    /**
     * 没有警告地更新K_PTR_LV键值
     * 更新指针级数
     * @param ptr_lv
     */
    void update_ptr_lv(size_t ptr_lv);

    /**
     * 节点动作: 注册声明, 根据节点和子节点的信息声明变量等
     * @param scope_id
     * @param tree
     */
    void action_declaration(size_t scope_id, IR &ir) const;

    /**
     * 产生变量声明时的代码
     */
    size_t action_variable_declaration_code_generate(
            IR &ir, const ParseExpression &init_expr, const ParseType &this_type,
            const string &symbol, const size_t &scope_id) const;

    /**
     * 获取节点信息
     * @return
     */
    string get_node_info() const noexcept;

    /**
     * 获取节点键的名字
     * @param type
     * @return
     */
    static string get_key_name(NodeKey type) noexcept;

    /**
     * 获取节点类型的名字
     * @param type
     * @return
     */
    static string get_node_type_name(NodeType type) noexcept;

private:

    /**
     * 收集K_PARAM_LIST的键值信息并存到节点的K_PARAM_LIST中
     * collect_函数用于收集子节点分散的信息存到该节点的key中
     * @return 返回其常量引用
     */
    void collect_parameters_list() const;

    /**
     * 收集K_INIT_DEC的键值信息并存到节点的K_INIT_DEC中
     * collect_函数用于收集子节点分散的信息存到该节点的key中
     * @return 返回其常量引用
     */
    void collect_init_declarator() const;

    /**
     * 获取指定键值, 如果找不到则抛出异常, 会根据该节点类型和所有孩子节点类型进行向下搜索
     * @tparam MapType
     * @param key
     * @return
     */
    template<typename MapType>
    MapType &get_field(NodeKey key) const noexcept(false);

    /**
     * 设置节点的键对应的值
     * @tparam MapType
     * @param key 需要设置的键
     * @param object 需要设置的对象值
     * @param is_update 通过update调用时可以防止输出警告
     * @return
     */
    template<typename MapType>
    void set_field(NodeKey key, const MapType &object, bool is_update = false)
    noexcept(false);
};

#endif //NKU_PRACTICE_PARSE_NODE_HPP

#pragma clang diagnostic pop