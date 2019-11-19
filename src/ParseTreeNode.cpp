#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace std;

ParseTree::Node::Node(size_t _node_id, NodeType type) {
    node_id = _node_id;
    node_type = type;
}

string ParseTree::Node::get_key_name(NodeKey type) {
    switch (type) {
        case K_SYMBOL:
            return "symbol";
        case K_CONST_VALUE:
            return "constant value";
        case K_TYPE:
            return "type";
    }
    return "unknown";
}

string ParseTree::Node::get_type_name(VariableType type) {
    switch (type) {
        case V_SHORT:
            return "short";
        case V_INT:
            return "int";
        case V_LONG:
            return "long";
        case V_FLOAT:
            return "float";
        case V_DOUBLE:
            return "double";
        case V_BOOL:
            return "bool";
        case V_CHAR:
            return "char";
        case V_ENUM:
            return "enum";
        case V_STRUCT:
            return "long";
        case V_UNION:
            return "union";
        case V_VOID:
            return "void";
        case V_POINTER:
            return "pointer";
        case V_CONST_STRING:
            return "const string";
        case V_UNDEFINED:
            return "undefined";
    }
}

string ParseTree::Node::get_node_type_name(NodeType type) {
    switch (type) {
        case N_NORMAL:
            return "normal";
        case N_IDENTIFIER:
            return "identifier";
        case N_CONST:
            return "constant";
    }
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
    if (node_type == N_NORMAL) {
        return "";
    }
    string info = get_node_type_name(node_type) + " {";
    char buff[64];
    for (const auto each: keys) {
        switch ((NodeKey) each.first) {
            case K_SYMBOL:
                sprintf(buff, " symbol: %s,", ((string *) each.second)->c_str());
                info += buff;
                break;
            case K_TYPE:
                sprintf(buff, " type: %s,", Node::get_type_name((VariableType)*(int*)each.second).c_str());
                info += buff;
                break;
            case K_CONST_VALUE:
                info += ((ConstValueType*)each.second)->get_info();
                break;
        }
    }
    return info + "}";
}

ParseTree::Node::Node(const ParseTree::Node &other) : keys(other.keys) {
    node_id = other.node_id;
    node_type = other.node_type;
}

void ParseTree::Node::set_type(VariableType type) {
    auto iter = keys.find(K_TYPE);
    if (iter != keys.end()) {
        printf("警告:对节点%zu的type值进行了多次设置\n", node_id);
        delete (int *) iter->second;
    }
    keys[K_TYPE] = ((size_t) new int(type));
}

VariableType ParseTree::Node::get_type() const {
    auto iter = keys.find(K_TYPE);
    if (iter == keys.end()) {
        printf("警告:试图获取节点%zu的未定义的type值\n", node_id);
        return V_UNDEFINED;
    }
    return (VariableType) *(int *) iter->second;
}

void ParseTree::Node::set_const_signed_value(VariableType type, long long value) {
    if (node_type != N_CONST) {
        printf("警告, 试图给类型为%s的节点%zu设置不支持的常量字段\n",
               get_node_type_name(node_type).c_str(), node_id);
        return;
    }
    set_type(type);
    switch (type) {
        case V_SHORT:
        case V_INT:
        case V_LONG:
            keys[K_CONST_VALUE] = ((size_t) new ConstValueType(type, value));
            break;
        default:
            printf("警告, 试图给变量类型为%s的节点%zu设置不支持的常量有符号整数\n", get_type_name(type).c_str(), node_id);
            break;
    }
}

long long ParseTree::Node::get_const_signed_value() const {
    if (node_type != N_CONST) {
        printf("警告:试图获取类型为%s的节点%zu的常量值\n", get_node_type_name(node_type).c_str(), node_id);
        return 0;
    }
    switch (get_type()) {
        case V_SHORT:
        case V_INT:
        case V_LONG: {
            auto iter = keys.find(K_CONST_VALUE);
            if (iter == keys.end()) {
                printf("警告:试图获取节点%zu的未定义的const value值\n", node_id);
                return 0;
            }
            return *(long long *) iter->second;
        }
        default:
            printf("警告:试图获取节点%zu的不支持的有符号整数常量值\n", node_id);
            return 0;
    }
}

void ParseTree::Node::set_const_float_value(VariableType type, double value) {
    if (node_type != N_CONST) {
        printf("警告, 试图给类型为%s的节点%zu设置不支持的常量字段\n",
               get_node_type_name(node_type).c_str(), node_id);
        return;
    }
    set_type(type);
    switch (type) {
        case V_FLOAT:
        case V_DOUBLE:
            keys[K_CONST_VALUE] = ((size_t) new ConstValueType(type, value));
            break;
        default:
            printf("警告, 试图给类型为%s的节点%zu设置不支持的常量有符号整数\n", get_type_name(type).c_str(), node_id);
            break;
    }
}

double ParseTree::Node::get_const_float_value() const {
    if (node_type != N_CONST) {
        printf("警告:试图获取类型为%s的节点%zu的常量值\n", get_node_type_name(node_type).c_str(), node_id);
        return 0;
    }
    switch (get_type()) {
        case V_FLOAT:
        case V_DOUBLE: {
            auto iter = keys.find(K_CONST_VALUE);
            if (iter == keys.end()) {
                printf("警告:试图获取节点%zu的未定义的const value值\n", node_id);
                return 0;
            }
            return *(double *) iter->second;
        }
        default:
            printf("警告:试图获取节点%zu的不支持的浮点数常量值\n", node_id);
            return 0;
    }
}

void ParseTree::Node::set_const_bool_value(VariableType type, bool value) {
    if (node_type != N_CONST) {
        printf("警告, 试图给类型为%s的节点%zu设置不支持的常量字段\n",
               get_node_type_name(node_type).c_str(), node_id);
        return;
    }
    set_type(type);
    if (type != V_BOOL) {
        keys[K_CONST_VALUE] = ((size_t) new ConstValueType(type, value));
    } else {
        printf("警告, 试图给类型为%s的节点%zu设置不支持的布尔常量类型\n", get_type_name(type).c_str(), node_id);
    }
}

bool ParseTree::Node::get_const_bool_value() const {
    if (node_type != N_CONST) {
        printf("警告:试图获取类型为%s的节点%zu的常量值\n", get_node_type_name(node_type).c_str(), node_id);
        return false;
    }
    if (get_type() != V_BOOL) {
        auto iter = keys.find(K_CONST_VALUE);
        if (iter == keys.end()) {
            printf("警告:试图获取节点%zu的未定义的const value值\n", node_id);
            return false;
        }
        return *(bool *) iter->second;
    } else {
        printf("警告:试图获取节点%zu的不支持的布尔常量值\n", node_id);
        return false;
    }
}

void ParseTree::Node::set_const_string_value(VariableType type, const string &value) {
    if (node_type != N_CONST) {
        printf("警告, 试图给类型为%s的节点%zu设置不支持的常量字段\n",
               get_node_type_name(node_type).c_str(), node_id);
        return;
    }
    set_type(type);
    switch (type) {
        case V_CHAR:
        case V_CONST_STRING:
            keys[K_CONST_VALUE] = ((size_t) new ConstValueType(type, value));
            break;
        default:
            printf("警告, 试图给类型为%s的节点%zu设置不支持的常量字符串\n", get_type_name(type).c_str(), node_id);
            break;
    }
}

string ParseTree::Node::get_const_string_value() const {
    if (node_type != N_CONST) {
        printf("警告:试图获取类型为%s的节点%zu的常量值\n", get_node_type_name(node_type).c_str(), node_id);
        return "";
    }
    switch (get_type()) {
        case V_CHAR:
        case V_CONST_STRING: {
            auto iter = keys.find(K_CONST_VALUE);
            if (iter == keys.end()) {
                printf("警告:试图获取节点%zu的未定义的const value值\n", node_id);
                return "";
            }
            return *(string *) iter->second;
        }
        default:
            printf("警告:试图获取节点%zu的不支持的字符串常量值\n", node_id);
            return "";
    }
}

#pragma clang diagnostic pop