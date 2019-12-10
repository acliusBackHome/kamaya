#include <iostream>
#include <cstdarg>
#include "ParseNode.hpp"
#include "ParseTree.hpp"

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
        case K_VARIABLE:
            return "variable";
        case K_TYPE:
            return "type";
        case K_IS_PTR:
            return "is_pointer";
        case K_FUNCTION:
            return "function";
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
        case N_UNKNOWN:
            return "unknown";
        case N_TYPE_SPE:
            return "type_specifier";
        case N_DECLARATION_SPE:
            return "declaration_specifier";
        case N_DECLARATOR:
            return "declarator";
        case N_DIRECT_DEC:
            return "direct_declarator";
        case N_PARAM_LIST:
            return "parameter_list";
        case N_PARAM_DECLARATION:
            return "parameter_declaration";
        case N_FUNCTION_DEFINITION:
            return "function_definition";
    }
    return "unknown";
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
                info += "symbol: " + *((string *) each.second) + ", ";
                break;
            case K_CONST_TYPE:
                info += "const_type: " + get_const_type_name(*(ConstValueType *) each.second) + ", ";
                break;
            case K_CONST_VALUE:
                info += "const_value: " + get_const_value_str() + ", ";
                break;
            case K_VARIABLE:
                info += "variable: " + ((ParseVariable *) each.second)->get_info();
                break;
            case K_TYPE:
                info += "type: " + ((ParseType *) each.second)->get_info() + ", ";
                break;
            case K_FUNCTION:
                info += "function: " + ((ParseFunction *) each.second)->get_info() + ", ";
                break;
            case K_IS_PTR:
                info += "is_pointer: ";
                if (*(bool *) each.second) {
                    info += "true";
                } else {
                    info += "false";
                }
                info += ", ";
                break;
        }
    }
    return info + "}";
}

ParseNode::ParseNode(const ParseNode &other) : keys(other.keys) {
    node_id = other.node_id;
    type = other.type;
}

NodeType ParseNode::get_node_type() const {
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
        case N_DECLARATION_SPE:
        case N_DIRECT_DEC:
        case N_PARAM_LIST:
            break;
        case N_IDENTIFIER: {
            auto iter = keys.find(K_SYMBOL);
            if (iter == keys.end() || iter->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (string *) iter->second;
            break;
        }
        case N_DECLARATOR: {
            auto iter = keys.find(K_IS_PTR);
            if (iter == keys.end() || iter->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (bool *) iter->second;
            break;
        }
        case N_CONST:
            delete_const();
            break;
        case N_TYPE_SPE: {
            auto t = keys.find(K_TYPE);
            if (t == keys.end() || t->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s,的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (ParseType *) t->second;
            break;
        }
        case N_FUNCTION_DEFINITION: {
            auto v = keys.find(K_FUNCTION);
            if (v == keys.end() || v->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (ParseFunction *) v->second;
            break;
        }
        case N_PARAM_DECLARATION: {
            auto v = keys.find(K_VARIABLE);
            if (v == keys.end() || v->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (ParseVariable *) v->second;
            break;
        }
    }

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

template<class OpType>
void ParseNode::before_update_key(const string &msg, NodeKey key_type, ...) {
    // 读取可变长参数
    va_list ap;
    //将ap指向参数key_type后面的参数，也就是说第二个参数会被跳过。
    va_start(ap, key_type);
    vector<NodeType> allows;
    while (true) {
        //获取当前参数的值，同时将ap指向下一个参数
        int read = va_arg(ap, int);
        if (read == -1) {
            break;
        }
        allows.emplace_back((NodeType) read);
    }
    // 记得end
    va_end(ap);
    // 可变长列表读取结束
    for (const auto &each : allows) {
        if (type == each) {
            auto t = keys.find(key_type);
            if (t != keys.end()) {
                printf("%s: 警告:尝试重定义节点%zu的类型值\n", msg.c_str(), node_id);
                if (t->second) {
                    printf("%s: 警告:变量节点%zu的字段不完整,请检查调用或者实现\n",
                           msg.c_str(), node_id);
                    return;
                }
                delete (OpType *) t->second;
            }
            return;
        }
    }
    printf("%s: 警告:试图设置%s类型的节点%zu不允许的%s键值\n", msg.c_str(),
           get_node_type_name(type).c_str(), node_id,
           get_key_name(key_type).c_str());
}

void ParseNode::set_symbol(const string &symbol) {
    before_update_key<string>("ParseNode::set_symbol(const string &symbol)",
                              K_SYMBOL, N_IDENTIFIER, -1);
    keys[K_SYMBOL] = ((size_t) new string(symbol));
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

void ParseNode::set_type(const ParseType &p_type) {
    before_update_key<ParseType>("ParseNode::set_type(const ParseType &p_type)",
                                 K_TYPE, N_TYPE_SPE, -1);
    keys[K_TYPE] = (size_t) new ParseType(p_type);
}

void ParseNode::set_variable(const ParseType &p_type, const string &symbol, size_t address) {
    before_update_key<ParseVariable>(
            "ParseNode::set_variable(const ParseType &p_type, const string &symbol, size_t address)",
            K_VARIABLE, N_PARAM_DECLARATION, -1
    );
    keys[K_VARIABLE] = (size_t) new ParseVariable(p_type, symbol, address);
}

void ParseNode::set_variable(const ParseVariable &variable) {
    set_variable(variable.get_type(), variable.get_symbol(), variable.get_address());
}

void ParseNode::set_is_pointer(bool is_pointer) {
    before_update_key<bool>("ParseNode::set_is_pointer(bool is_pointer)",
                            K_IS_PTR, N_DECLARATOR, -1);
    keys[K_IS_PTR] = (size_t) new bool(is_pointer);
}

void ParseNode::set_function(const ParseFunction &function) {
    before_update_key<ParseFunction>(
            "ParseNode::set_function(const ParseFunction &function)",
            K_FUNCTION, N_FUNCTION_DEFINITION, -1
    );
    keys[K_FUNCTION] = (size_t) new ParseFunction(function);
}

string ParseNode::get_symbol(ParseTree *_tree) const {
    const auto &iter = keys.find(K_SYMBOL);
    if (iter != keys.end()) {
        return *(string *) iter->second;
    }
    if (_tree) {
        ParseTree &tree = *_tree;
        switch (type) {
            case N_DIRECT_DEC: {
                for (const auto &each : tree.node_children[node_id]) {
                    ParseNode &child = tree.nodes[each];
                    if (child.type == N_IDENTIFIER || child.type == N_DIRECT_DEC) {
                        return child.get_symbol(_tree);
                    }
                }
                break;
            }
            case N_DECLARATOR: {
                for (const auto &each : tree.node_children[node_id]) {
                    ParseNode &child = tree.nodes[each];
                    if (child.type == N_DIRECT_DEC) {
                        return child.get_symbol(_tree);
                    }
                }
                break;
            }
            default:
                printf("ParseNode::get_symbol(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("警告:节点%zu未定义字段%s\n", node_id, get_key_name(K_SYMBOL).c_str());
    return "";
}

int ParseNode::get_const_type(ParseTree *tree) const {
    if (type == N_CONST) {
        const auto &iter = keys.find(K_CONST_TYPE);
        return *(ConstValueType *) iter->second;
    }
    if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_const_type(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    return -1;
}

long long ParseNode::get_const_signed_value(ParseTree *tree) const {
    if (type == N_CONST) {
        auto t = keys.find(K_CONST_TYPE);
        if (t == keys.end() || t->second == 0) {
            printf("ParseNode::get_const_signed_value(): 警告: 试图获取节点%zu未定义的%s值\n", node_id,
                   get_key_name(K_CONST_TYPE).c_str());
            return 0;
        }
        if (*((ConstValueType *) t->second) != C_SIGNED) {
            printf("ParseNode::get_const_signed_value(): 警告: 试图获取类型为%s的节点%zu的%s值\n",
                   get_const_type_name(*((ConstValueType *) t->second)).c_str(),
                   node_id,
                   get_const_type_name(C_SIGNED).c_str());
            return 0;
        }
        auto v = keys.find((K_CONST_VALUE));
        if (v == keys.end() || v->second == 0) {
            printf("ParseNode::get_const_signed_value(): 警告: 试图获取常量节点%zu的字段定义不完整, 请检查调用和实现\n", node_id);
            return 0;
        }
        return *(long long *) (v->second);
    } else if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_const_signed_value(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    return 0;
}

unsigned long long ParseNode::get_const_unsigned_value(ParseTree *tree) const {
    if (type == N_CONST) {
        auto t = keys.find(K_CONST_TYPE);
        if (t == keys.end() || t->second == 0) {
            printf("ParseNode::get_const_unsigned_value(): 警告: 试图获取节点%zu未定义的%s值\n", node_id,
                   get_key_name(K_CONST_TYPE).c_str());
            return 0;
        }
        if (*((ConstValueType *) t->second) != C_UNSIGNED) {
            printf("ParseNode::get_const_unsigned_value(): 警告: 试图获取类型为%s的节点%zu的%s值\n",
                   get_const_type_name(*((ConstValueType *) t->second)).c_str(),
                   node_id,
                   get_const_type_name(C_UNSIGNED).c_str());
            return 0;
        }
        auto v = keys.find((K_CONST_VALUE));
        if (v == keys.end() || v->second == 0) {
            printf("ParseNode::get_const_unsigned_value(): 警告: 试图获取常量节点%zu的字段定义不完整, 请检查调用和实现\n", node_id);
            return 0;
        }
        return *(unsigned long long *) (v->second);
    } else if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_const_unsigned_value(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    return 0;
}

string ParseNode::get_const_string_value(ParseTree *tree) const {
    if (type == N_CONST) {
        auto t = keys.find(K_CONST_TYPE);
        if (t == keys.end() || t->second == 0) {
            printf("ParseNode::get_const_string_value(): 警告: 试图获取节点%zu未定义的%s值\n", node_id,
                   get_key_name(K_CONST_TYPE).c_str());
            return "";
        }
        if (*((ConstValueType *) t->second) != C_STRING) {
            printf("ParseNode::get_const_string_value(): 警告: 试图获取类型为%s的节点%zu的%s值\n",
                   get_const_type_name(*((ConstValueType *) t->second)).c_str(),
                   node_id,
                   get_const_type_name(C_STRING).c_str());
            return "";
        }
        auto v = keys.find((K_CONST_VALUE));
        if (v == keys.end() || v->second == 0) {
            printf("ParseNode::get_const_string_value(): 警告: 试图获取常量节点%zu的字段定义不完整, 请检查调用和实现\n", node_id);
            return "";
        }
        return *(string *) (v->second);
    } else if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_const_string_value(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    return "";
}

long double ParseNode::get_const_float_value(ParseTree *tree) const {
    if (type == N_CONST) {
        auto t = keys.find(K_CONST_TYPE);
        if (t == keys.end() || t->second == 0) {
            printf("ParseNode::get_const_float_value(): 警告: 试图获取节点%zu未定义的%s值\n", node_id,
                   get_key_name(K_CONST_TYPE).c_str());
            return 0;
        }
        if (*((ConstValueType *) t->second) != C_FLOAT) {
            printf("ParseNode::get_const_float_value(): 警告: 试图获取类型为%s的节点%zu的%s值\n",
                   get_const_type_name(*((ConstValueType *) t->second)).c_str(),
                   node_id,
                   get_const_type_name(C_FLOAT).c_str());
            return 0;
        }
        auto v = keys.find((K_CONST_VALUE));
        if (v == keys.end() || v->second == 0) {
            printf("ParseNode::get_const_float_value(): 警告: 试图获取常量节点%zu的字段定义不完整, 请检查调用和实现\n", node_id);
            return 0;
        }
        return *(long double *) (v->second);
    } else if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_const_float_value(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    return 0;
}

bool ParseNode::get_const_bool_value(ParseTree *tree) const {
    if (type == N_CONST) {
        auto t = keys.find(K_CONST_TYPE);
        if (t == keys.end() || t->second == 0) {
            printf("ParseNode::get_const_bool_value(): 警告: 试图获取节点%zu未定义的%s值\n", node_id,
                   get_key_name(K_CONST_TYPE).c_str());
            return false;
        }
        if (*((ConstValueType *) t->second) != C_BOOL) {
            printf("ParseNode::get_const_bool_value(): 警告: 试图获取类型为%s的节点%zu的%s值\n",
                   get_const_type_name(*((ConstValueType *) t->second)).c_str(),
                   node_id,
                   get_const_type_name(C_BOOL).c_str());
            return false;
        }
        auto v = keys.find((K_CONST_VALUE));
        if (v == keys.end() || v->second == 0) {
            printf("ParseNode::get_const_bool_value(): 警告: 试图获取常量节点%zu的字段定义不完整, 请检查调用和实现\n", node_id);
            return false;
        }
        return *(bool *) (v->second);
    } else if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_const_bool_value(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    return false;
}

ParseType ParseNode::get_type(ParseTree *_tree) const {
    auto t = keys.find(K_TYPE);
    if (t != keys.end() && t->second != 0) {
        return *(ParseType *) (t->second);
    }
    if (_tree) {
        ParseTree &tree = *_tree;
        switch (type) {
            case N_DECLARATION_SPE: {
                for (const auto &each : tree.node_children[node_id]) {
                    if (tree.nodes[each].type == N_TYPE_SPE) {
                        return tree.nodes[each].get_type();
                    }
                }
                break;
            }
            default:
                printf("ParseNode::get_type(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("ParseNode::get_const_signed_value(): 警告: 试图获取节点%zu未定义的%s值\n", node_id,
           get_key_name(K_TYPE).c_str());
    return ParseType(T_UNKNOWN);
}

bool ParseNode::get_is_pointer(ParseTree *_tree) const {
    const auto &iter = keys.find(K_IS_PTR);
    if (iter != keys.end()) {
        return *(bool *) iter->second;
    }
    if (_tree) {
        switch (type) {
            default:
                printf("ParseNode::get_is_pointer(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("警告:节点%zu未定义字段%s\n", node_id, get_key_name(K_IS_PTR).c_str());
    return false;
}

ParseVariable ParseNode::get_variable(ParseTree *tree) const {
    const auto &iter = keys.find(K_VARIABLE);
    if (iter != keys.end()) {
        return *(ParseVariable *) iter->second;
    }
    if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_is_pointer(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("警告:节点%zu未定义字段%s\n", node_id, get_key_name(K_VARIABLE).c_str());
    return ParseVariable();
}

ParseFunction ParseNode::get_function(ParseTree *tree) const {
    const auto &iter = keys.find(K_FUNCTION);
    if (iter != keys.end()) {
        return *(ParseFunction *) iter->second;
    }
    if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_function(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("警告:节点%zu未定义字段%s\n", node_id, get_key_name(K_FUNCTION).c_str());
    return ParseFunction();
}

vector<ParseVariable> ParseNode::get_parameters_list(const ParseTree &tree) const {
    switch (type) {
        case N_PARAM_LIST: {
            vector<ParseVariable> res;
            for (const auto &each : tree.node_children[node_id]) {
                const ParseNode &node = tree.nodes[each];
                if (node.type == N_PARAM_DECLARATION) {
                    res.emplace_back(node.get_variable());
                }
            }
            return res;
        }
        case N_DIRECT_DEC: {
            for (const auto &each : tree.node_children[node_id]) {
                const ParseNode &node = tree.nodes[each];
                if (node.type == N_PARAM_LIST) {
                    return node.get_parameters_list(tree);
                }
            }
            return vector<ParseVariable>();
        }
        case N_DECLARATOR: {
            for (const auto &each : tree.node_children[node_id]) {
                const ParseNode &node = tree.nodes[each];
                if (node.type == N_DIRECT_DEC) {
                    return node.get_parameters_list(tree);
                }
            }
            break;
        }
        default:
            break;
    }
    printf("ParseNode::get_parameters_list(const ParseTree &tree): 警告: 节点%zu不支持此操作\n", node_id);
    return vector<ParseVariable>();
}

#pragma clang diagnostic pop