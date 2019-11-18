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
    for (auto i : last_nodes) {
#ifdef PARSE_TREE_DEBUG
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
            idx++;
        }
        set_parent(i, error_nodes[idx]); // 必须保证在程序正确的情况下错误节点能容纳所有剩余节点
    }
}

#pragma clang diagnostic pop