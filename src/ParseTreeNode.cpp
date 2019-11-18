#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace std;

ParseTree::Node::Node(size_t _node_id, NodeType type) {
    node_id = _node_id;
    node_type = type;
}

string ParseTree::Node::get_key_name(NodeKey type) const {
    switch (type) {
        case SYMBOL:
            return "symbol";
        default:
            return "unknown";
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

string ParseTree::Node::get_symbol() const {
    const auto &iter = keys.find(SYMBOL);
    if (iter == keys.end()) {
        printf("警告:节点%zu未定义字段%s\n", node_id, get_key_name(SYMBOL).c_str());
        return "";
    }
    return *(string *) iter->second;
}

string ParseTree::Node::get_node_info() const {
    string info;
    char buff[64];
    switch (node_type) {
        case IDENTIFIER:
            info = "identifier {";
            break;
        case NORMAL:
            break;
    }
    for (const auto each: keys) {
        switch ((NodeKey) each.first) {
            case SYMBOL:
                sprintf(buff, " symbol: %s,", ((string *) each.second)->c_str());
                info += buff;
                break;
        }
    }
    return info + "}";
}

ParseTree::Node::Node(const ParseTree::Node &other) : keys(other.keys) {
    node_id = other.node_id;
    node_type = other.node_type;
}

#pragma clang diagnostic pop