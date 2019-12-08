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

using namespace std;


class ParseTree {
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
     * 生成一个变量节点
     * @param type 变量类型
     * @param symbol 变量符号
     * @param address 变量地址
     * @return
     */
    size_t make_variable_node(const ParseType &type, const string &symbol, size_t address);

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

    //对节点操作的接口
    ParseNode *node(size_t node_id);

    ~ParseTree();

private:
    vector<ParseNode> nodes;
    vector<string> node_msg;
    // 每个节点的父亲
    vector<size_t> node_parent, error_nodes;
    // 每个节点的孩子
    vector<vector<size_t> > node_children;
    set<size_t> last_nodes; // 剩下的节点

    void print_node(size_t node_id, vector<size_t> &has_next_children,
                    size_t depth, bool last_child, bool *vis) const;

    bool check_node(size_t node_id) const;

};

#endif //NKU_PRACTICE_PARSE_TREE_H

#pragma clang diagnostic pop