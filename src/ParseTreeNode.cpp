#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

ParseTree::Node::Node(size_t _node_id, NodeType type) {
    node_id = _node_id;
    node_type = type;
}

const string ParseTree::Node::get_key_name(NodeKey type) {
    switch (type) {
        case SYMBOL:
            return "symbol";
    }
}

void ParseTree::Node::set_symbol(const string &symbol) {
    auto iter = keys.find(SYMBOL);
    if (iter != keys.end()) {
        delete (string *) iter->second;
        printf("警告: 对节点%zu 的symbol值进行了多次设置\n", node_id);
    }
    keys[SYMBOL] = ((size_t) new string(symbol));
}

const string &ParseTree::Node::get_symbol() const {
    const auto iter = keys.find(SYMBOL);
    return *(string *) iter->second;
}

ParseTree::Node::~Node() {
    for (auto each: keys) {
        switch ((NodeKey) each.first) {
            case SYMBOL:
                delete (string *) each.second;
                break;
        }
    }
}

#pragma clang diagnostic pop