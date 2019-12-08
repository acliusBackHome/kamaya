#include <iostream>
#include "ParseNode.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace std;

ParseNode::ParseNode(size_t _node_id, NodeType _type) {
    node_id = _node_id;
    type = _type;
}

string ParseNode::get_key_name(NodeKey type) {
    switch (type) {
        case K_SYMBOL:
            return "symbol";
        case K_CONST_TYPE:
            return "constant_type";
        case K_CONST_VALUE:
            return "constant_value";
        case K_VAR_TYPE:
            return "variable_type";
        case K_VAR_ADDRESS:
            return "variable_address";
    }
    return "unknown";
}

string ParseNode::get_node_type_name(NodeType type) {
    switch (type) {
        case N_NORMAL:
            return "normal";
        case N_IDENTIFIER:
            return "identifier";
        case N_CONST:
            return "constant";
        case N_VARIABLE:
            return "variable";
        case N_UNKNOWN:
            break;
    }
    return "unknown";
}

void ParseNode::set_symbol(const string &symbol) {
    auto iter = keys.find(K_SYMBOL);
    if (iter != keys.end()) {
        delete (string *) iter->second;
        printf("警告: 对节点%zu 的symbol值进行了多次设置\n", node_id);
    }
    keys[K_SYMBOL] = ((size_t) new string(symbol));
}

string ParseNode::get_symbol() const {
    const auto &iter = keys.find(K_SYMBOL);
    if (iter == keys.end()) {
        printf("警告:节点%zu未定义字段%s\n", node_id, get_key_name(K_SYMBOL).c_str());
        return "";
    }
    return *(string *) iter->second;
}

string ParseNode::get_node_info() const {
    if (type == N_NORMAL) {
        return "";
    }
    string info = get_node_type_name(type) + " { ";
    for (const auto each: keys) {
        if (!(each.second)) {
            printf("ParseNode::get_node_info(): 警告: 节点%zu的类型%s应有字段%s没有定义\n",
                   node_id, get_node_type_name(type).c_str(), get_key_name((NodeKey) each.first).c_str());
            continue;
        }
        switch ((NodeKey) each.first) {
            case K_SYMBOL:
                info += "symbol: " +  *((string *) each.second) + ", ";
                break;
            case K_CONST_TYPE:
                info += "const_type: " + get_const_type_name(*(ConstValueType *) each.second) + ", ";
                break;
            case K_CONST_VALUE:
                info += "const_value: " + get_const_value_str() + ", ";
                break;
            case K_VAR_TYPE:
                info += "variable_type: " + ((ParseType *) each.second)->get_info() + ", ";
                break;
            case K_VAR_ADDRESS: {
                char buff[64];
                sprintf(buff, "variable_address: %zu, ", *(size_t *) each.second);
                info += buff;
                break;
            }

        }
    }
    return info + "}";
}

ParseNode::ParseNode(const ParseNode &other) : keys(other.keys) {
    node_id = other.node_id;
    type = other.type;
}

NodeType ParseNode::get_type() const {
    return type;
}

void ParseNode::update_const(size_t const_type_address, size_t const_value_address) {
    if (type != N_CONST) {
        printf("ParseNode::update_const: 警告:试图设置非常量类型%s的节点%zu的常量值\n",
               get_node_type_name(type).c_str(), node_id);
        return;
    }
    auto t = keys.find(K_CONST_TYPE);
    if (t != keys.end()) {
        printf("ParseNode::update_const: 警告:尝试重定义节点%zu的常量值\n", node_id);
        delete_const();
    }
    keys[K_CONST_TYPE] = const_type_address;
    keys[K_CONST_VALUE] = const_value_address;
}

void ParseNode::set_const(long long value) {
    update_const((size_t) new ConstValueType(C_SIGNED), (size_t) new long long(value));
}

void ParseNode::set_const(unsigned long long value) {
    update_const((size_t) new ConstValueType(C_UNSIGNED), (size_t) new unsigned long long(value));
}

void ParseNode::set_const(const string &value) {
    update_const((size_t) new ConstValueType(C_STRING), (size_t) new string(value));
}

void ParseNode::set_const(bool value) {
    update_const((size_t) new ConstValueType(C_BOOL), (size_t) new bool(value));
}

void ParseNode::set_const(long double value) {
    update_const((size_t) new ConstValueType(C_BOOL), (size_t) new long double(value));
}

string ParseNode::get_const_type_name(ConstValueType _type) {
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

void ParseNode::delete_const() {
    auto t = keys.find(K_CONST_TYPE),
            v = keys.find(K_CONST_VALUE);
    if (t == keys.end() || v == keys.end()) {
        printf("ParseNode::delete_const():警告:试图释放节点%zu的未定义常量值空间\n", node_id);
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
    keys.erase(t);
    keys.erase(v);
}

void ParseNode::delete_all_keys() {
    switch (type) {
        case N_NORMAL:
        case N_UNKNOWN:
            break;
        case N_IDENTIFIER: {
            auto iter = keys.find(K_SYMBOL);
            if (iter == keys.end() || iter->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s,的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (string *) iter->second;
            break;
        }
        case N_CONST:
            delete_const();
            break;
        case N_VARIABLE: {
            auto s = keys.find(K_SYMBOL), t = keys.find(K_VAR_TYPE), a = keys.find(K_VAR_ADDRESS);
            if (s == keys.end() || s->second == 0 ||
                t == keys.end() || t->second == 0 ||
                a == keys.end() || a->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s,的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (ParseType *) t->second;
            delete (string *) s->second;
            delete (size_t *) a->second;
            break;
        }
    }

}

int ParseNode::get_const_type() const {
    if (type != N_CONST) {
        return -1;
    }
    const auto &iter = keys.find(K_CONST_TYPE);
    return *(ConstValueType *) iter->second;
}

string ParseNode::get_const_value_str() const {
    int c_type = get_const_type();
    if (c_type < 0) {
        return "unknown";
    }
    auto iter = keys.find(K_CONST_VALUE);
    if (iter == keys.end() || iter->second == 0) {
        printf("ParseNode::get_const_value_str(): 警告, 节点%zu类型为常量但是没有设置值\n", node_id);
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

void ParseNode::set_variable(const ParseType &type, const string &symbol, size_t address) {
    update_variable((size_t) new ParseType(type), (size_t) new string(symbol), (size_t) new size_t(address));
}

void ParseNode::update_variable(size_t type_address, size_t symbol_address, size_t var_add_address) {
    if (type != N_VARIABLE) {
        printf("ParseNode::update_variable: 警告:试图设置非变量类型%s的节点%zu的变量值\n",
               get_node_type_name(type).c_str(), node_id);
        return;
    }
    auto t = keys.find(K_VAR_TYPE);
    if (t != keys.end()) {
        printf("ParseNode::update_variable: 警告:尝试重定义节点%zu的变量值\n", node_id);
        auto s = keys.find(K_SYMBOL), a = keys.find(K_VAR_ADDRESS);
        if (t->second == 0 || s == keys.end() || s->second == 0 || a == keys.end() || a->second == 0) {
            printf("ParseNode::update_variable: 警告:变量节点%zu的字段不完整,请检查调用或者实现\n", node_id);
            return;
        }
        delete (ParseType *) t->second;
        delete (string *) s->second;
        delete (size_t *) a->second;
    }
    keys[K_VAR_TYPE] = type_address;
    keys[K_VAR_ADDRESS] = var_add_address;
    keys[K_SYMBOL] =  symbol_address;
}

#pragma clang diagnostic pop