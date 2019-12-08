#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

ParseTree::ParseTree(const string &msg) {
    nodes.emplace_back(0, N_NORMAL);
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

size_t ParseTree::new_node(NodeType node_type) {
    return new_node("", node_type);
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

void ParseTree::print_node(size_t node_id, vector<size_t> &has_next_children,
                           size_t depth, bool last_child, bool *vis) const {
    if (node_id != 0) {
        if (last_child) {
            printf(" |_");
        } else {
            printf(" |-");
        }
    }
    switch (nodes[node_id].type) {
        case N_IDENTIFIER:
        case N_CONST:
        case N_VARIABLE:
        case N_TYPE_SPE:
            printf("%zu:%s\n", node_id, nodes[node_id].get_node_info().c_str());
            break;
        case N_NORMAL:
        case N_UNKNOWN:
            printf("%zu:%s\n", node_id, node_msg[node_id].c_str());
            break;
    }
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
        printf("警告:树中没有节点%zu\n", node_id);
        return false;
    }
    return true;
}

ParseNode *ParseTree::node(size_t node_id) {
    if (check_node(node_id)) {
        return &nodes[node_id];
    }
    return nullptr;
}

size_t ParseTree::make_const_node(unsigned long long unsigned_num) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(unsigned_num);
    return new_one;
}

size_t ParseTree::make_const_node(long long signed_num) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(signed_num);
    return new_one;
}

size_t ParseTree::make_const_node(long double float_num) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(float_num);
    return new_one;
}

size_t ParseTree::make_const_node(bool b) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(b);
    return new_one;
}

size_t ParseTree::make_const_node(const string &str) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(str);
    return new_one;
}

size_t ParseTree::make_identifier_node(const string &symbol) {
    size_t new_one = new_node(N_IDENTIFIER);
    nodes[new_one].set_symbol(symbol);
    return new_one;
}

ParseTree::~ParseTree() {
    for (auto &each : nodes) {
        each.delete_all_keys();
    }
}

size_t ParseTree::make_variable_node(const ParseType &type, const string &symbol, size_t address) {
    size_t new_one = new_node(N_VARIABLE);
    nodes[new_one].set_variable(type, symbol, address);
    return new_one;
}

size_t ParseTree::make_type_specifiers(const ParseType &type) {
    size_t new_one = new_node(N_TYPE_SPE);
    nodes[new_one].set_type_specifier(type);
    return new_one;
}

ParseType ParseTree::get_type(const string &type_name) {
    auto iter = type_dic.find(type_name);
    if (iter == type_dic.end()) {
        return ParseType(T_UNKNOWN);
    }
    return ParseType::get_type(iter->second);
}

void ParseTree::type_def(const string &type_name, const ParseType &type) {
    auto iter = type_dic.find(type_name);
    if (iter != type_dic.end()) {
        printf("ParseTree::type_def(const string &type_name, const ParseType &type): "
               "警告: 试图给已声明的类型名字%s赋予新的类型%s, 无视该操作\n", type_name.c_str(),
               type.get_info().c_str());
        return;
    }
    size_t type_id = ParseType::get_type_id(type);
    type_dic[type_name] = type_id;
}

#pragma clang diagnostic pop