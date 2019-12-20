#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

void ParseTree::error_push(size_t error_id) {
    error_nodes.push_back(error_id);
}

size_t ParseTree::get_error(int idx) {
    if (idx < 0 || idx >= error_nodes.size()) {
        return (size_t) -1;
    }
    return error_nodes[idx];
}

int ParseTree::error_cnt() {
    return error_nodes.size();
}

void ParseTree::last_combine() {
    int idx = 0;
    set<size_t> tmp_nodes(last_nodes);
    for (auto i : tmp_nodes) {
        if (idx >= error_nodes.size()) {
#ifdef PARSE_TREE_DEBUG
            puts("ParseTree::last_combine log: OUT RANGE OF ERROR NODES.");
#endif
            break;
        }
        if (i == error_nodes[idx]) {
#ifdef PARSE_TREE_DEBUG
            puts("ParseTree::last_combine log: SAME TAG ERROR AND LAST OF NODE.");
#endif
            continue;
        }
        if (i > error_nodes[idx]) { // 必须在逻辑上保证错误节点不是剩余节点
            idx++;
        }
        if (native_root(error_nodes[idx]) == i) { // 错误结点位于以 i 为根的树
            continue;
        }
        set_parent(i, error_nodes[idx]); // 必须保证在程序正确的情况下错误节点能容纳所有剩余节点
    }
    for (auto i : last_nodes) {
        set_parent(i, 0);
    }
}

size_t ParseTree::native_root(size_t idx) {
    size_t p = idx;
    while (get_parent(p) != (size_t) (-1)) {
        p = get_parent(p);
    }
    return p;
}

#ifdef PARSE_TREE_DEBUG

void ParseTree::watch_children_nodes() const {
    printf("node_children[%zu]\n", node_children.size());
    for (int i = 0; i < node_children.size(); i++) {
        printf("node_children[%i] size %zu: ", i, node_children[i].size());
        for (auto j : node_children[i]) {
            printf("%zu ", j);
        }
        printf("\n");
    }
    printf("\n");
}

void ParseTree::watch_last_nodes() const {
    printf("last_nodes[%zu]\n", last_nodes.size());
    for (auto i : last_nodes) {
        printf("%zu %s\n", i, get_msg(i).c_str());
    }
    printf("\n");
}

void ParseTree::watch_error_nodes() const {
    printf("error_nodes[%zu]\n", error_nodes.size());
    for (auto i : error_nodes) {
        printf("%zu %s\n", i, get_msg(i).c_str());
    }
    printf("\n");
}

#endif

#pragma clang diagnostic pop