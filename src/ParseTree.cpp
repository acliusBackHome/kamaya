#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

ParseTree::ParseTree(const string &msg) {
    nodes.emplace_back(0, NORMAL);
    node_parent.push_back(0);
    node_msg.push_back(msg);
    node_children.emplace_back();
}

string ParseTree::get_msg(size_t node_id) const {
    if (node_id >= node_msg.size()) {
        return "该树并没有此节点";
    }
    return node_msg[node_id];
}

size_t ParseTree::new_node(const string &msg, NodeType node_type) {
    node_msg.push_back(msg);
    node_parent.push_back((size_t) (-1));
    node_children.emplace_back();
    size_t this_node = node_msg.size() - 1;
    nodes.emplace_back(this_node, node_type);
    last_nodes.insert(this_node);
    return this_node;
}

void ParseTree::set_parent(size_t node_id, size_t parent) {
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

size_t ParseTree::get_parent(size_t node_id) {
    return node_parent[node_id];
}

void ParseTree::print() const {
    vector<size_t> has_next_children;
    auto *vis = new bool[get_node_num()];
    memset(vis, 0, sizeof(bool) * (get_node_num()));
    print_node(0, has_next_children, 1, true, vis);
    has_next_children.clear();
    for (size_t i = 1; i < get_node_num(); ++i) {
        if (!vis[i] && node_parent[i] == (size_t) (-1)) {
            printf("异常子树:\n");
            print_node(i, has_next_children, 2, true, vis);
            has_next_children.clear();
        }
    }
}

size_t ParseTree::get_node_num() const {
    return node_msg.size();
}

void ParseTree::set_node_symbol(size_t node_id, const string &symbol) {
    if (!check_node(node_id)) {
        return;
    }
    nodes[node_id].set_symbol(symbol);
}

string ParseTree::get_node_symbol(size_t node_id) const {
    if (!check_node(node_id)) {
        return "";
    }
    return nodes[node_id].get_symbol();
}

void ParseTree::print_node(size_t node_id, vector<size_t> &has_next_children,
                             size_t depth, bool last_child, bool *vis) const {
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

bool ParseTree::check_node(size_t node_id) const {
    if (node_id >= node_msg.size()) {
        printf("错误,树中没有节点%zu\n", node_id);
        return false;
    }
    return true;
}

#pragma clang diagnostic pop