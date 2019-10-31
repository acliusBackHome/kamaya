#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_MESSAGETREE_H
#define NKU_PRACTICE_MESSAGETREE_H

#define MESSAGE_TREE_DEBUG

#include <vector>
#include <string>
#include <algorithm>
#include <memory.h>

using namespace std;

class MessageTree {
public:
    /**
     * 构造函数, 树初始时必须有一个根, 根编号0
     * @param msg 根的信息
     */
    explicit MessageTree(const string &msg) {
        node_parent.push_back(0);
        node_msg.push_back(msg);
        node_children.emplace_back();
    }

    /**
     * 获得指定节点的信息
     * @param node_id
     * @return
     */
    inline string get_msg(size_t node_id) const {
        if (node_id >= node_msg.size()) {
            return "该树并没有此节点";
        }
        return node_msg[node_id];
    }

    /**
     * 往树中添加新节点
     * @param parent 父节点编号
     * @param msg 新节点的信息
     * @return 新节点的编号
     */
    size_t new_node(const string &msg) {
        node_msg.push_back(msg);
        node_parent.push_back((size_t) (-1));
        node_children.emplace_back();
        size_t this_node = node_msg.size() - 1;
        last_nodes.insert(this_node);
        return this_node;
    }

    /**
     *  设置节点的父节点
     * @param node_id 被设置的节点id
     * @param parent 父节点id
     */
    inline void set_parent(size_t node_id, size_t parent) {
        if (node_id >= node_msg.size() || parent >= node_msg.size()) {
            printf("set_parent:无效的节点id(%zu, %zu)\n", node_id, parent);
            return;
        }
        if (node_parent[node_id] == (size_t) (-1)) {
            node_parent[node_id] = parent;
            node_children[parent].push_back(node_id);
        } else {
            printf("该节点%zu已经有了父节点, 不能再设父节点为%zu\n", node_id, parent);
        }
        if (last_nodes.find(node_id) != last_nodes.end()) {
            last_nodes.erase(node_id);
        }
    }

    inline size_t get_parent(size_t node_id) {
      return node_parent[node_id];
    }

    /**
     *  打印树
     */
    inline void print() const {
        vector<size_t> has_next_children;
        auto *vis = new bool[get_node_num()];
        memset(vis, 0, sizeof(bool) * (get_node_num()));
        print_node(0, has_next_children, 1, true, vis);
        has_next_children.clear();
        for (size_t i = 1; i < get_node_num(); ++i) {
            if(!vis[i] && node_parent[i] == (size_t) (-1)) {
                printf("异常子树:\n");
                print_node(i, has_next_children, 2, true, vis);
                has_next_children.clear();
            }
        }

    }

    /**
     *  获取树的节点数
     */
    inline size_t get_node_num() const {
        return node_msg.size();
    }

    /**
     *  记录错误节点
     */
    inline void error_push(size_t error_id) {
        error_nodes.push_back(error_id);
    }

    /**
     *  获取错误节点数
     */
    inline int error_cnt() {
        return error_nodes.size();
    }

    /**
     *  获取错误节点
     */
    inline size_t get_error(int idx) {
        if (idx < 0 || idx >= error_nodes.size()) {
            return -1;
        }
        return error_nodes[idx];
    }

    /**
     *  组织错误节点及剩余节点，错误节点是那些由于发生错误而未被组织进树里的节点
     */
    inline void last_combine() {
        int idx = 0;
        for (auto i : last_nodes) {
            #ifdef MESSAGE_TREE_DEBUG
            if (idx >= error_nodes.size()) {
                puts("COMPILER ERROR: OUT RANGE OF ERROR NODES.");
                exit(1);
            }
            if (i == error_nodes[idx]) {
                puts("COMPILER ERROR: SAME TAG ERROR AND LAST OF NODE.");
                exit(1);
            }
            #endif
            if (i > error_nodes[idx]) { // 必须在逻辑上保证错误节点不是剩余节点
                idx ++;
            }
            set_parent(i, error_nodes[idx]); // 必须保证在程序正确的情况下错误节点能容纳所有剩余节点
        }
    }
private:
    vector<string> node_msg;
    vector<size_t> node_parent, error_nodes;
    vector<vector<size_t> > node_children;
    set<size_t> last_nodes; // 剩下的节点

    void print_node(size_t node_id, vector<size_t> &has_next_children, size_t depth, bool last_child,
                    bool *vis) const {
        if (node_id != 0) {
            if (last_child) {
                printf(" |_");
            } else {
                printf(" |-");
            }
        }
        printf("%zu:%s\n", node_id, node_msg[node_id].c_str());
        vis[node_id] = true;
        string pre_fix;
        for (size_t i = 1; i < depth; ++i) {
            pre_fix.append("  ");
        }
        for (auto each : has_next_children) {
            pre_fix[1 + 2 * (each - 1)] = '|';
        }
        for (size_t i = 0; i < node_children[node_id].size(); ++i) {
            size_t next_node = node_children[node_id][i];
            printf("%s", pre_fix.c_str());
            if (has_next_children.empty() || has_next_children[has_next_children.size() - 1] < depth) {
                has_next_children.push_back(depth);
            }
            if (i == node_children[node_id].size() - 1) {
                auto it = find(has_next_children.begin(), has_next_children.end(), depth);
                if (it != has_next_children.end()) {
                    has_next_children.erase(it);
                }
                print_node(next_node, has_next_children, depth + 1, true, vis);
            } else {
                print_node(next_node, has_next_children, depth + 1, false, vis);
            }
        }
    }
};

#endif //NKU_PRACTICE_MESSAGETREE_H

#pragma clang diagnostic pop