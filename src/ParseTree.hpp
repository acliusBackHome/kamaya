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

using namespace std;

class ParseTree {
public:
    class Node {
        friend class ParseTree;

        size_t node_id;
        NodeType type;
        // 字段到字段值的映射, 值是一个指针需要经过转换
        map<int, size_t> keys;

    public:

        explicit Node(size_t _node_id, NodeType type);

        Node(const Node &other);

        /**
         * 获取节点的类型
         * @return
         */
        NodeType get_type() const;

        /**
         * 设置节点的symbol键对应的值
         * @param symbol
         */
        void set_symbol(const string &symbol);

        /**
         * 获取节点的symbol键对应的值
         * @param symbol
         */
        string get_symbol() const;

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
         * 当常量值更新时需要执行的函数:
         * 负责检查键值是否存在,如果存在需要删除之
         */
        void update_const();

        /**
         *  删除常量所涉及的字段
         */
        void delete_const();

        /**
         * 获取该节点的常量类型,
         * 如果不是常量类型节点,则返回-1
         * 否则返回ConstValueType
         * @return
         */
        int get_const_type() const;

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

    friend class Node;

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
    Node *node(size_t node_id);

    ~ParseTree();

private:
    vector<Node> nodes;
    vector<string> node_msg;
    vector<size_t> node_parent, error_nodes;
    vector<vector<size_t> > node_children;
    set<size_t> last_nodes; // 剩下的节点

    void print_node(size_t node_id, vector<size_t> &has_next_children,
                    size_t depth, bool last_child, bool *vis) const;

    bool check_node(size_t node_id) const;

};

#endif //NKU_PRACTICE_PARSE_TREE_H

#pragma clang diagnostic pop