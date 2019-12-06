#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace std;

ParseTree::Node::Node(size_t _node_id, NodeType _type) {
    node_id = _node_id;
    type = _type;
}

string ParseTree::Node::get_key_name(NodeKey type) {
    switch (type) {
        case K_SYMBOL:
            return "symbol";
        case K_CONST_TYPE:
            return "constant_type";
        case K_CONST_VALUE:
            return "constant_value";
    }
    return "unknown";
}

string ParseTree::Node::get_node_type_name(NodeType type) {
    switch (type) {
        case N_NORMAL:
            return "normal";
        case N_IDENTIFIER:
            return "identifier";
        case N_CONST:
            return "constant";
        case N_UNKNOWN:
            break;
    }
    return "unknown";
}

void ParseTree::Node::set_symbol(const string &symbol) {
    auto iter = keys.find(K_SYMBOL);
    if (iter != keys.end()) {
        delete (string *) iter->second;
        printf("警告: 对节点%zu 的symbol值进行了多次设置\n", node_id);
    }
    keys[K_SYMBOL] = ((size_t) new string(symbol));
}

string ParseTree::Node::get_symbol() const {
    const auto &iter = keys.find(K_SYMBOL);
    if (iter == keys.end()) {
        printf("警告:节点%zu未定义字段%s\n", node_id, get_key_name(K_SYMBOL).c_str());
        return "";
    }
    return *(string *) iter->second;
}

string ParseTree::Node::get_node_info() const {
    if (type == N_NORMAL) {
        return "";
    }
    string info = get_node_type_name(type) + " {";
    char buff[256];
    for (const auto each: keys) {
        switch ((NodeKey) each.first) {
            case K_SYMBOL:
                sprintf(buff, " symbol: %s,", ((string *) each.second)->c_str());
                info += buff;
                break;
            case K_CONST_TYPE:
                sprintf(buff, " const_type : %s,", get_const_type_name((ConstValueType) get_const_type()).c_str());
                info += buff;
                break;
            case K_CONST_VALUE:
                sprintf(buff, " const_value : %s,", get_const_value_str().c_str());
                info += buff;
                break;
        }
    }
    return info + "}";
}

ParseTree::Node::Node(const ParseTree::Node &other) : keys(other.keys) {
    node_id = other.node_id;
    type = other.type;
}

NodeType ParseTree::Node::get_type() const {
    return type;
}

void ParseTree::Node::update_const() {
    if (type != N_CONST) {
        printf("ParseTree::Node::update_const: 警告:试图设置非常量类型%s的节点%zu的常量值\n",
               get_node_type_name(type).c_str(), node_id);
        return;
    }
    auto t = keys.find(K_CONST_TYPE);
    if (t != keys.end()) {
        printf("ParseTree::Node::update_const: 警告:尝试重定义节点%zu的常量值", node_id);
        delete_const();
    }
}

void ParseTree::Node::set_const(long long value) {
    update_const();
    keys[K_CONST_TYPE] = (size_t) new ConstValueType(C_SIGNED);
    keys[K_CONST_VALUE] = (size_t) new long long(value);
}

void ParseTree::Node::set_const(unsigned long long value) {
    update_const();
    keys[K_CONST_TYPE] = (size_t) new ConstValueType(C_UNSIGNED);
    keys[K_CONST_VALUE] = (size_t) new unsigned long long(value);
}

void ParseTree::Node::set_const(const string &value) {
    update_const();
    keys[K_CONST_TYPE] = (size_t) new ConstValueType(C_STRING);
    keys[K_CONST_VALUE] = (size_t) new string(value);
}

void ParseTree::Node::set_const(bool value) {
    update_const();
    keys[K_CONST_TYPE] = (size_t) new ConstValueType(C_BOOL);
    keys[K_CONST_VALUE] = (size_t) new bool(value);
}

void ParseTree::Node::set_const(long double value) {
    update_const();
    keys[K_CONST_TYPE] = (size_t) new ConstValueType(C_FLOAT);
    keys[K_CONST_VALUE] = (size_t) new long double(value);
}

string ParseTree::Node::get_const_type_name(ConstValueType _type) {
    switch (_type) {
        case C_STRING:
            return "const_string";
        case C_SIGNED:
            return "const_signed_int";
        case C_UNSIGNED:
            return "const_unsigned_int";
        case C_FLOAT:
            return "const_float";
        case C_BOOL:
            return "const_bool";
        case C_UNDEFINED:
            break;
    }
    return "undefined";
}

void ParseTree::Node::delete_const() {
    auto t = keys.find(K_CONST_TYPE),
            v = keys.find(K_CONST_VALUE);
    if (t == keys.end() || v == keys.end()) {
        printf("ParseTree::Node::delete_const():警告:试图释放节点%zu的未定义常量值空间\n", node_id);
        return;
    }
    switch (*(ConstValueType *) t->second) {
        case C_STRING:
            delete (string *) v->second;
            v->second = 0;
            break;
        case C_SIGNED:
            delete (long long *) v->second;
            v->second = 0;
            break;
        case C_UNSIGNED:
            delete (unsigned long long *) v->second;
            v->second = 0;
            break;
        case C_FLOAT:
            delete (long double *) v->second;
            v->second = 0;
            break;
        case C_BOOL:
            delete (bool *) v->second;
            break;
        case C_UNDEFINED:
            break;
    }
    delete (ConstValueType *) t->second;
    t->second = 0;
    keys.erase(t);
    keys.erase(v);
}

void ParseTree::Node::delete_all_keys() {
    switch (type) {
        case N_NORMAL:
        case N_UNKNOWN:
            break;
        case N_IDENTIFIER: {
            const auto &iter = keys.find(K_SYMBOL);
            delete (string *) iter->second;
            break;
        }
        case N_CONST: {
            delete_const();
            break;
        }
    }

}

int ParseTree::Node::get_const_type() const {
    if (type != N_CONST) {
        return -1;
    }
    const auto &iter = keys.find(K_CONST_TYPE);
    return *(ConstValueType *) iter->second;
}

string ParseTree::Node::get_const_value_str() const {
    int c_type = get_const_type();
    if (c_type < 0) {
        return "unknown";
    }
    auto iter = keys.find(K_CONST_VALUE);
    if (iter == keys.end()) {
        printf("ParseTree::Node::get_const_value_str(): 警告, 节点%zu类型为常量但是没有设置值\n", node_id);
        return "warning";
    }
    char buff[32];
    switch ((ConstValueType) c_type) {
        case C_STRING:
            return *(string *) iter->second;
        case C_SIGNED:
            sprintf(buff, "%lld", *(long long *) iter->second);
            return string(buff);
        case C_UNSIGNED:
            sprintf(buff, "%llu", *(unsigned long long *) iter->second);
            return string(buff);
        case C_FLOAT:
            sprintf(buff, "%Lf", *(long double *) iter->second);
            return string(buff);
        case C_BOOL:
            return *(bool *) iter->second ? "true" : "false";
        case C_UNDEFINED:
            break;
    }
    return "undefined";
}

#pragma clang diagnostic pop