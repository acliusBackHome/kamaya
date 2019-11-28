#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_PARSE_TREE_H
#define NKU_PRACTICE_PARSE_TREE_H

#define PARSE_TREE_DEBUG

#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <memory.h>
#include "ParseDef.hpp"
#include "ParseType.hpp"
#include "ParseNode.hpp"
#include "ParseExpression.hpp"
#include "ParseDeclaration.hpp"

using namespace std;

class ParseVariable;

class ParseTree {
    friend class ParseNode;

public:
    /**
     * 构造函数, 树初始时必须有一个根, 根编号0
     * @param msg 根的信息
     */
    explicit ParseTree(const string &msg);

    /**
     * 获得指定节点的信息
     * @param node_id
     * @return
     */
    string get_msg(size_t node_id) const;

    /**
     * 往树中添加新节点
     * @param msg 新节点的信息
     * @return 新节点的编号
     */
    size_t new_node(const string &msg, NodeType node_type = N_NORMAL);

    /**
     * 往树中添加新节点
     * @param node_type 新节点的类型
     * @return
     */
    size_t new_node(NodeType node_type = N_NORMAL);


    /**
     * 生成一个新的常量无符号整数的节点
     * @param unsigned_num
     * @return 节点编号
     */
    size_t make_const_node(unsigned long long unsigned_num);

    /**
     * 生成一个新的常量有符号整数的节点
     * @param signed_num
     * @return 节点编号
     */
    size_t make_const_node(long long signed_num);

    /**
     * 生成一个新的常量浮点数的节点
     * @param float_num
     * @return 节点编号
     */
    size_t make_const_node(long double float_num);

    /**
     * 生成一个新的常量布尔值的节点
     * @param bool
     * @return 节点编号
     */
    size_t make_const_node(bool b);

    /**
     * 生成一个新的常量字符串的节点, 传入参数是字符串,
     * 要求该指针在该树的声明周期内有效
     * @param str
     * @return 节点编号
     */
    size_t make_const_node(const string &str);

    /**
     * 生成一个标识符节点
     * @param symbol id的符号
     * @return 节点编号
     */
    size_t make_identifier_node(const string &symbol);

    /**
     * 生成一个类型声明节点
     * @param type 这个节点所声明的类型
     * @return
     */
    size_t make_type_specifier_node(const ParseType &type);

    /**
     * 生成一个修饰类型声明节点
     * @return
     */
    size_t make_declaration_specifier_node();

    /**
     * 生成一个声明器节点
     * @return
     */
    size_t make_declarator_node(bool is_pointer = false);

    /**
     * 生成一个直接声明器节点
     * @param is_array 是否声明为数组
     * @param expression 数组大小表达式
     * @return
     */
    size_t make_direct_declarator_node(bool is_array = false, const ParseExpression *expression = nullptr);

    /**
     * 生成一个参数列表节点
     * @return
     */
    size_t make_parameter_list_node();

    /**
     * 生成一个参数声明节点
     * @return
     */
    size_t make_parameter_declaration(const ParseVariable &variable);

    /**
     * 生成一个函数声明节点
     * @param ret_type 返回值
     * @param symbol 函数名称
     * @param args 参数列表
     * @param address 初始地址
     * @return
     */
    size_t make_function_definition_node(
            const ParseType &ret_type,
            const string &symbol,
            const vector<ParseVariable> &args,
            size_t address = (size_t) -1);

    /**
     * 生成一个初始化声明器节点
     * @return
     */
    size_t make_init_declarator_node();

    /**
     * 生成一个初始化器节点
     * @return
     */
    size_t make_initializer_node();

    /**
     * 生成一个声明节点
     * @return
     */
    size_t make_declaration_node();

    /**
     * 生成一个带初始化声明器列表节点
     * @return
     */
    size_t make_init_declarator_list_node();

    /**
     * 生成一个表达式节点
     * @return
     */
    size_t make_expression_node(const ParseExpression &expression);

    /**
     * 生成一个语句块列表节点
     * @return
     */
    size_t make_block_item_list_node();

    /**
     * 生成一个语句块列表节点
     * @return
     */
    size_t make_compound_statement_node();

    /**
     * 生成一个for语句节点
     * @return
     */
    size_t make_for_statement_node();

    /**
     * 自定义类型声明或者给类型别名, 建立字符串到ParseType的映射
     * @param type_name
     * @param type
     */
    void type_def(const string &type_name, const ParseType &type);

    /**
     * 通过类型名获取对应的类型, 如果不存在,则返回
     * 无效类型
     * @param str
     * @return
     */
    ParseType get_type(const string &type_name);

    /**
     *  设置节点的父节点
     * @param node_id 被设置的节点id
     * @param parent 父节点id
     */
    void set_parent(size_t node_id, size_t parent);

    /**
     * 获取节点的父节点
     * @param node_id
     * @return
     */
    size_t get_parent(size_t node_id);

    /**
     *  打印树
     */
    void print() const;

    /**
     *  获取树的节点数
     */
    size_t get_node_num() const;

    /**
     *  记录错误节点
     */
    void error_push(size_t error_id);

    /**
     *  获取错误节点数
     */
    int error_cnt();

    /**
     *  获取错误节点
     */
    size_t get_error(int idx);

    /**
     *  组织错误节点及剩余节点，错误节点是那些由于发生错误而未被组织进树里的节点
     */
    void last_combine();

    /**
     * 对节点操作的接口, 如果节点不存在则返回nullptr
     * @param node_id
     * @return
     */
    ParseNode *node(size_t node_id);

    ~ParseTree();

#ifdef PARSE_TREE_DEBUG
    /**
        * 检查子节点生成情况
        */
    void watch_children_nodes() const;

    /**
        * 检查剩余结点情况
        */
    void watch_last_nodes() const;
#endif

private:
    vector<ParseNode> nodes;
    vector<string> node_msg;
    // 每个节点的父亲
    vector<size_t> node_parent, error_nodes;
    // 每个节点的孩子
    vector<vector<size_t> > node_children;
    set<size_t> last_nodes; // 剩下的节点
    //类型字典: 从自定义id映射到ParseType
    map<string, size_t> type_dic;

    void print_node(size_t node_id, vector<size_t> &has_next_children,
                    size_t depth, bool last_child, bool *vis) const;

    bool check_node(size_t node_id) const;

};

#endif //NKU_PRACTICE_PARSE_TREE_H

#pragma clang diagnostic pop