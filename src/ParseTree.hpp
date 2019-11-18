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
        NodeType node_type;
        // 字段到字段值的映射, 值是一个指针需要经过转换
        map<int, size_t> keys;
    public:

        explicit Node(size_t _node_id, NodeType type);

        Node(const Node &other);

        string get_key_name(NodeKey type) const;

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

        string get_node_info() const;

    private:

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
     * @param parent 父节点编号
     * @param msg 新节点的信息
     * @return 新节点的编号
     */
    size_t new_node(const string &msg, NodeType node_type = NORMAL);

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