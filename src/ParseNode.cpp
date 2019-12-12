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
        case K_CONST:
            return "constant";
        case K_VARIABLE:
            return "variable";
        case K_TYPE:
            return "type";
        case K_IS_PTR:
            return "is_pointer";
        case K_FUNCTION:
            return "function";
        case K_EXPRESSION:
            return "expression";
        case K_IS_ARRAY:
            return "is_array";
        case K_INIT_DECLARATORS:
            return "init_declarators";
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
        case N_INIT_DECLARATOR:
            return "initialize_declarator";
        case N_INIT_DECLARATOR_LIST:
            return "initialize_declarator_list";
        case N_INITIALIZER:
            return "initializer";
        case N_DECLARATION:
            return "declaration";
        case N_EXPRESSION:
            return "expression";
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
            case K_CONST:
                info += "const: " + ((ParseConstant *) each.second)->get_info() + ", ";
                break;
            case K_VARIABLE:
                info += "variable: " + ((ParseVariable *) each.second)->get_info() + ", ";
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
            case K_EXPRESSION:
                info += "expression: " + ((ParseExpression *) each.second)->get_info() + ", ";
                break;
            case K_IS_ARRAY:
                info += "is_array: ";
                if (*(bool *) each.second) {
                    info += "true";
                } else {
                    info += "false";
                }
                info += ", ";
                break;
            case K_INIT_DECLARATORS:
                info += "init_declarators: [";
                vector<InitDeclarator> &list = *(vector<InitDeclarator> *) (each.second);
                char buff[32];
                for (const auto &dec: list) {
                    info += "{ symbol: " + get<0>(dec) +
                            ", initializer: " + get<1>(dec).get_info() +
                            ", is_pointer: " + ((get<2>(dec)) ? "true" : "false") +
                            ", array_size: ";
                    sprintf(buff, "%zu", get<3>(dec));
                    info += buff;
                    info += " }, ";
                }
                info += "]";
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

void ParseNode::update_const(size_t const_address) {
    if (type != N_CONST) {
        printf("ParseNode::update_const: 警告:试图设置非常量类型%s的节点%zu的常量值\n",
               get_node_type_name(type).c_str(), node_id);
        return;
    }
    auto t = keys.find(K_CONST);
    if (t != keys.end()) {
        printf("ParseNode::update_const: 警告:尝试重定义节点%zu的常量值\n", node_id);
        delete_const();
    }
    keys[K_CONST] = const_address;
}

void ParseNode::delete_const() {
    auto it = keys.find(K_CONST);
    if (it == keys.end()) {
        printf("ParseNode::delete_const():警告:试图释放节点%zu的未定义常量值空间\n", node_id);
        return;
    }
    delete (ParseConstant *) it->second;
    keys.erase(it);
}

void ParseNode::delete_all_keys() {
    switch (type) {
        case N_NORMAL:
        case N_UNKNOWN:
        case N_DECLARATION_SPE:
        case N_PARAM_LIST:
        case N_DECLARATION:
        case N_INITIALIZER:
        case N_INIT_DECLARATOR:
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
        case N_DIRECT_DEC: {
            auto b = keys.find(K_IS_ARRAY);
            if (b == keys.end() || b->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            auto is_array = *(bool *) (b->second);
            if (is_array) {
                auto e = keys.find(K_EXPRESSION);
                if (e == keys.end() || e->second == 0) {
                    printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s的字段定义不完全\n",
                           node_id, get_node_type_name(type).c_str());
                    break;
                }
                delete (ParseExpression *) (e->second);
            }
            delete (bool *) (b->second);
            break;
        }
        case N_INIT_DECLARATOR_LIST: {
            auto l = keys.find(K_INIT_DECLARATORS);
            if (l == keys.end() || l->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (vector<InitDeclarator> *) (l->second);
            break;
        }
        case N_EXPRESSION: {
            auto e = keys.find(K_EXPRESSION);
            if (e == keys.end() || e->second == 0) {
                printf("ParseNode::delete_all_keys(): 警告:节点%zu,类型%s的字段定义不完全\n",
                       node_id, get_node_type_name(type).c_str());
                break;
            }
            delete (ParseExpression *) (e->second);
            break;
        }
    }
}

//=========================set_key系列===========================

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
                              K_SYMBOL, N_IDENTIFIER, N_INIT_DECLARATOR, -1);
    keys[K_SYMBOL] = ((size_t) new string(symbol));
}

void ParseNode::set_const(long long value) {
    update_const((size_t) new ParseConstant(value));
}

void ParseNode::set_const(unsigned long long value) {
    update_const((size_t) new ParseConstant(value));
}

void ParseNode::set_const(const string &value) {
    update_const((size_t) new ParseConstant(value));
}

void ParseNode::set_const(bool value) {
    update_const((size_t) new ParseConstant(value));
}

void ParseNode::set_const(long double value) {
    update_const((size_t) new ParseConstant(value));
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

void ParseNode::set_expression(const ParseExpression &expression) {
    before_update_key<ParseExpression>(
            "ParseNode::set_expression(const ParseFunction &expression)",
            K_EXPRESSION, N_INIT_DECLARATOR, N_DIRECT_DEC, N_EXPRESSION, -1
    );
    keys[K_EXPRESSION] = (size_t) new ParseExpression(expression);
}

void ParseNode::set_is_array(bool is_array) {
    before_update_key<bool>(
            "ParseNode::set_is_array(bool is_array)",
            K_IS_ARRAY, N_DIRECT_DEC, -1
    );
    keys[K_IS_ARRAY] = (size_t) new bool(is_array);
}

void ParseNode::update_is_array(bool is_array) {
    auto i = keys.find(K_IS_ARRAY);
    if (i == keys.end() || i->second == 0) {
        printf("ParseNode::update_is_array(bool is_array): 警告: 试图更新节点%zu未定义的%s键值\n",
               node_id, get_key_name(K_IS_ARRAY).c_str());
        return;
    }
    *((bool *) (i->second)) = is_array;
}

void ParseNode::add_init_declarator(const InitDeclarator &init_declarator) {
    if (type != N_INIT_DECLARATOR_LIST) {
        printf("ParseNode::add_init_declarator(const pair<string, ParseExpression>): 警告: "
               "试图设置节点%zu不允许的%s键值\n", node_id,
               get_key_name(K_INIT_DECLARATORS).c_str());
        return;
    }
    auto l = keys.find(K_INIT_DECLARATORS);
    vector<InitDeclarator> *list;
    if (l == keys.end()) {
        list = new vector<InitDeclarator>();
        keys[K_INIT_DECLARATORS] = (size_t) list;
    } else {
        list = (vector<InitDeclarator> *) (l->second);
    }
    list->emplace_back(init_declarator);
}

//=========================get_key系列===========================

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
            case N_INIT_DECLARATOR: {
                for (const auto &each : tree.node_children[node_id]) {
                    ParseNode &child = tree.nodes[each];
                    if (child.type == N_DECLARATOR) {
                        return child.get_symbol(_tree);
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    printf("ParseNode::get_symbol(ParseTree *tree): 警告: 节点%zu不支持此操作\n", node_id);
    return "";
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
        ParseTree &tree = *_tree;
        switch (type) {
            case N_INIT_DECLARATOR:
                for (const auto &each : tree.node_children[node_id]) {
                    if (tree.nodes[each].type == N_DECLARATOR) {
                        return tree.nodes[each].get_is_pointer();
                    }
                }
                break;
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

vector<ParseVariable> ParseNode::get_parameters_list(ParseTree &tree) const {
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

bool ParseNode::get_is_array(ParseTree *_tree) const {
    const auto &iter = keys.find(K_IS_ARRAY);
    if (iter != keys.end()) {
        return *(bool *) iter->second;
    }
    if (_tree) {
        ParseTree &tree = *_tree;
        switch (type) {
            case N_DECLARATOR: {
                for (const auto &each : tree.node_children[node_id]) {
                    const ParseNode &node = tree.nodes[each];
                    if (node.type == N_DIRECT_DEC) {
                        return node.get_is_array(_tree);
                    }
                }
                break;
            }
            default:
                printf("ParseNode::get_is_array(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("ParseNode::get_is_array(ParseTree *tree): 警告:节点%zu未定义字段%s\n", node_id, get_key_name(K_IS_ARRAY).c_str());
    return false;
}

const ParseConstant ParseNode::get_const(ParseTree *_tree) const {
    const auto &iter = keys.find(K_CONST);
    if (iter != keys.end()) {
        return *(ParseConstant *) iter->second;
    }
    if (_tree) {
        ParseTree &tree = *_tree;
        switch (type) {
            case N_INITIALIZER: {
                for (const auto &each : tree.node_children[node_id]) {
                    const ParseNode &node = tree.nodes[each];
                    if (node.type == N_CONST) {
                        return node.get_const();
                    }
                }
                break;
            }
            default:
                printf("ParseNode::get_const(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("ParseNode::get_const(ParseTree *tree): 警告:节点%zu未定义字段%s\n",
           node_id, get_key_name(K_CONST).c_str());
    return ParseConstant();
}

const vector<InitDeclarator> *ParseNode::get_init_declarator_list(ParseTree *tree) const {
    const auto &iter = keys.find(K_INIT_DECLARATORS);
    if (iter != keys.end()) {
        return (vector<InitDeclarator> *) iter->second;
    }
    if (tree) {
        switch (type) {
            default:
                printf("ParseNode::get_init_declarator_list(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("ParseNode::get_init_declarator_list(ParseTree *tree): 警告:节点%zu未定义字段%s\n",
           node_id, get_key_name(K_INIT_DECLARATORS).c_str());
    return nullptr;
}

InitDeclarator ParseNode::get_init_declarator(ParseTree &tree) const {
    switch (type) {
        case N_DECLARATOR: {
            for (const auto &each : tree.node_children[node_id]) {
                const ParseNode &node = tree.nodes[each];
                switch (node.type) {
                    case N_DIRECT_DEC: {
                        ParseExpression arr_dec_expr;
                        if (node.get_is_array(&tree)) {
                            arr_dec_expr = node.get_expression();
                        }
                        if (arr_dec_expr.is_defined()) {
                            return InitDeclarator(
                                    node.get_symbol(&tree), ParseExpression(), get_is_pointer(),
                                    arr_dec_expr.get_const().get_unsigned()
                            );
                        }
                        return InitDeclarator(node.get_symbol(&tree), ParseExpression(), get_is_pointer(), 0);
                    }
                    default:
                        break;
                }
            }
            break;
        }
        case N_INIT_DECLARATOR: {
            bool is_ptr = get_is_pointer(&tree);
            string symbol = get_symbol(&tree);
            ParseExpression init_expr,
                    arr_dec_expr;
            for (const auto &each : tree.node_children[node_id]) {
                const ParseNode &node = tree.nodes[each];
                switch (node.type) {
                    case N_DECLARATOR: {
                        if (node.get_is_array(&tree)) {
                            arr_dec_expr = node.get_expression(&tree);
                        }
                        break;
                    }
                    case N_INITIALIZER : {
                        init_expr = ParseExpression(node.get_const(&tree));
                        break;
                    }
                    default:
                        break;
                }
            }
            if (arr_dec_expr.is_defined()) {
                return InitDeclarator(symbol, init_expr, is_ptr, arr_dec_expr.get_const().get_unsigned());
            }
            return InitDeclarator(symbol, init_expr, is_ptr, 0);
        }
        default:
            break;
    }
    printf("ParseNode::get_init_declarator(const ParseTree &tree): 警告: 节点%zu不支持此操作\n", node_id);
    return InitDeclarator("", ParseExpression(), false, 0);
}

ParseExpression ParseNode::get_expression(ParseTree *_tree) const {
    auto iter = keys.find(K_EXPRESSION);
    if (iter != keys.end()) {
        return *(ParseExpression *) iter->second;
    }
    iter = keys.find(K_CONST);
    if (iter != keys.end()) {
        return ParseExpression(*(ParseConstant *) iter->second);
    }
    if (_tree) {
        ParseTree &tree = *_tree;
        switch (type) {
            case N_DECLARATOR: {
                for (const auto &each : tree.node_children[node_id]) {
                    const ParseNode &node = tree.nodes[each];
                    if (node.type == N_DIRECT_DEC) {
                        return node.get_expression();
                    }
                }
                break;
            }
            default:
                printf("ParseNode::get_expression(ParseTree *tree): 警告: 节点%zu不支持此操作", node_id);
                break;
        }
    }
    printf("ParseNode::get_expression(ParseTree *tree): 警告:节点%zu未定义字段%s\n",
           node_id, get_key_name(K_EXPRESSION).c_str());
    return ParseExpression();
}

#pragma clang diagnostic pop