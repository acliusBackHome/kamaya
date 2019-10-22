#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_MESSAGETREE_H
#define NKU_PRACTICE_MESSAGETREE_H

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class MessageTree {
public:
    /**
     * 构造函数, 树初始时必须有一个根, 根编号0
     * @param msg 根的信息
     */
    explicit MessageTree(const string& msg) {
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
     * 往树中添加新节点, 指定父节点
     * @param parent 父节点编号
     * @param msg 新节点的信息
     * @return 新节点的编号
     */
    size_t add_new_node(size_t parent, const string& msg) {
        node_msg.push_back(msg);
        node_parent.push_back(parent);
        node_children.emplace_back();
        size_t this_node = node_msg.size() - 1;
        node_children[parent].push_back(this_node);
        return this_node;
    }

    inline void print() const {
        vector<size_t> has_next_children;
        print_node(0, has_next_children, 1, true);
    }

private:
    vector<string> node_msg;
    vector<size_t > node_parent;
    vector<vector<size_t> > node_children;

    void print_node(size_t node_id, vector<size_t> & has_next_children, size_t depth, bool last_child) const {
        if(node_id != 0) {
            if(last_child) {
                printf(" |__");
            } else {
                printf(" |--");
            }
        }
        printf("%s\n", node_msg[node_id].c_str());
        string pre_fix;
        for(size_t i = 1; i < depth; ++i) {
            pre_fix.append("  ");
        }
        for(auto each : has_next_children) {
            pre_fix[1 + 2 * (each - 1)] = '|';
        }
        if(has_next_children.empty() || has_next_children[has_next_children.size() - 1] < depth) {
            has_next_children.push_back(depth);
        }
        for(size_t i = 0; i < node_children[node_id].size(); ++i) {
            printf(pre_fix.c_str());
            size_t next_node = node_children[node_id][i];
            if (i == node_children[node_id].size() - 1) {
                //printf("pop_back(%u)\n", has_next_children[has_next_children.size() - 1]);
                auto it = find(has_next_children.begin(), has_next_children.end(), depth);
                if(it != has_next_children.end()) {
                    has_next_children.erase(it);
                }
                /*for(auto each : has_next_children) {
                    printf("%u ", each);
                }*/
                //putchar('\n');
                print_node(next_node, has_next_children, depth + 1, true);
            } else {
                print_node(next_node, has_next_children, depth + 1, false);
            }
        }
    }
};

#endif //NKU_PRACTICE_MESSAGETREE_H

#pragma clang diagnostic pop