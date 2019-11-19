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
    struct ConstValueType {
        VariableType type;
        void *value_pointer;

        /**
         * 有符号整数构造函数
         * @param type
         * @param value
         */
        ConstValueType(VariableType _type, long long value);

        /**
         * 浮点数
         * @param type
         * @param value
         */
        ConstValueType(VariableType _type, double value);

        /**
         * 布尔值
         * @param type
         * @param value
         */
        ConstValueType(VariableType _type, bool value);

        /**
         * 字符串
         * @param type
         * @param value
         */
        ConstValueType(VariableType _type, const string &value);

        string get_info() const;

        ~ConstValueType();
    };

public:
    class Node {
        friend class ParseTree;

        size_t node_id;
        NodeType node_type;
        // 字段到字段值的映射, 值是一个指针需要经过转换
        map<int, size_t> keys;

    public:

        explicit Node(size_t _node_id, NodeType type);

        Node(const Node &other);

        static string get_key_name(NodeKey type);

        static string get_type_name(VariableType type);

        static string get_node_type_name(NodeType type);

        VariableType get_type() const;

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
         * 设置该节点为有符号整数常量
         * @param type
         * @param value
         */
        void set_const_signed_value(VariableType type, long long value);

        /**
         * 获取节点的有符号整数常量的值
         * @return
         */
        long long get_const_signed_value() const;

        /**
         * 设置该节点为浮点数常量
         * @param type
         * @param value
         */
        void set_const_float_value(VariableType type, double value);

        /**
         * 获取节点的浮点数常量的值
         * @return
         */
        double get_const_float_value() const;

        /**
         * 设置节点为布尔值常量
         * @param type
         * @param value
         */
        void set_const_bool_value(VariableType type, bool value);

        /**
         * 获取节点的布尔值常量
         * @return
         */
        bool get_const_bool_value() const;

        /**
         * 设置节点为字符串常量
         * @param type
         * @param value
         */
        void set_const_string_value(VariableType type, const string& value);

        /**
         * 获取节点的字符串常量值
         */
        string get_const_string_value() const;

        /**
         * 获取节点信息
         * @return
         */
        string get_node_info() const;

    private:
        /**
         * 设置节点的变量类型
         * @param type
         */
        void set_type(VariableType type);

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