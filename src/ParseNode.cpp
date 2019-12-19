#include <iostream>
#include <cstdarg>
#include "ParseNode.hpp"
#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace std;

ParseNode::ParseNode(ParseTree &_tree, size_t _node_id, NodeType _type) : tree(_tree) {
    node_id = _node_id;
    type = _type;
    keys = 0;
}

ParseNode::ParseNode(const ParseNode &other) : tree(other.tree) {
    node_id = other.node_id;
    type = other.type;
    keys = other.keys;
}

string ParseNode::get_key_name(NodeKey type) noexcept {
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
        case K_SCOPE_ID:
            return "scope_id";
        case K_PARAM_LIST_NODE:
            return "param_list_node";
        case K_NEXT:
            return "next";
        case K_BEGIN:
            return "begin";
        case K_CODE:
            return "code";
        case K_INSTR:
            return "instr";
        case K_TRUE_LIST:
            return "true_list";
        case K_FALSE_LIST:
            return "false_list";
        case K_TRUE_JUMP:
            return "true_jump";
        case K_FALSE_JUMP:
            return "false_jump";
        case K_NEXT_LIST:
            return "next_list";
        case K_PARAM_LIST:
            return "parameters_list";
        case K_INIT_DEC:
            return "init_declarator";
    }
    return "unknown";
}

string ParseNode::get_node_type_name(NodeType type) noexcept {
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
            return "parameters_list";
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
        case N_BLOCK_ITEM_LIST:
            return "block_item_list";
        case N_COMP_STMT:
            return "compound_statement";
        case N_FOR_STMT:
            return "for statement";
    }
    return "unknown";
}

string ParseNode::get_node_info() const noexcept {
    if (type == N_NORMAL || type == N_UNKNOWN) {
        return tree.node_msg[node_id];
    }
    string info = get_node_type_name(type) + " { ";
    size_t each_key = 1;
    while (each_key < MAX_NODE_KEY_TYPE) {
        if (keys & each_key) {
            // 有该键值再输出
            switch ((NodeKey) each_key) {
                case K_SYMBOL:
                    info += "symbol: " + get_symbol() + ", ";
                    break;
                case K_CONST:
                    info += "const: " + get_const().get_info() + ", ";
                    break;
                case K_VARIABLE:
                    info += "variable: " + get_variable().get_info() + ", ";
                    break;
                case K_TYPE:
                    info += "type: " + get_type().get_info() + ", ";
                    break;
                case K_FUNCTION:
                    info += "function: " + get_function().get_info() + ", ";
                    break;
                case K_IS_PTR:
                    info += "is_pointer: ";
                    if (get_is_pointer()) {
                        info += "true";
                    } else {
                        info += "false";
                    }
                    info += ", ";
                    break;
                case K_PARAM_LIST_NODE: {
                    info += "param_list_node: " + to_string(get_param_list_node());
                    info += ", ";
                    break;
                }
                case K_EXPRESSION:
                    info += "expression: " + get_expression().get_info() + ", ";
                    break;
                case K_IS_ARRAY:
                    info += "is_array: ";
                    if (get_is_array()) {
                        info += "true";
                    } else {
                        info += "false";
                    }
                    info += ", ";
                    break;
                case K_INIT_DECLARATORS: {
                    info += "init_declarators: [";
                    const auto &list = get_init_declarators();
                    for (const auto &dec: list) {
                        info += "{ symbol: " + get<0>(dec) +
                                ", initializer: " + get<1>(dec).get_info() +
                                ", is_pointer: " + ((get<2>(dec)) ? "true" : "false") +
                                ", array_size: " + to_string(get<3>(dec)) +
                                ", param_list_node: " + to_string(get<4>(dec)),
                                " }, ";
                    }
                    info += "], ";
                    break;
                }
                case K_SCOPE_ID: {
                    info += "scope_id: " + to_string(get_scope_id()) + ", ";
                    break;
                }
                case K_NEXT: {
                    info += "next: " + to_string(get_next()) + ", ";
                    break;
                }
                case K_BEGIN: {
                    info += "begin: " + to_string(get_begin()) + ", ";
                    break;
                }
                case K_CODE: {
                    info += "code: " + to_string(get_code()) + ", ";
                    break;
                }
                case K_INSTR: {
                    info += "instr: " + to_string(get_instr()) + ", ";
                    break;
                }
                case K_TRUE_LIST: {
                    info += "true_list: [";
                    const auto &list = get_true_list();
                    for (const auto &t: list) {
                        info += to_string(t) + ", ";
                    }
                    info += "], ";
                    break;
                }
                case K_FALSE_LIST: {
                    info += "false_list: [";
                    const auto &list = get_false_list();
                    for (const auto &t: list) {
                        info += to_string(t) + ", ";
                    }
                    info += "], ";
                    break;
                }
                case K_TRUE_JUMP: {
                    info += "true_jump: " + to_string(get_true_jump()) + ", ";
                    break;
                }
                case K_FALSE_JUMP: {
                    info += "false_jump: " + to_string(get_false_jump()) + ", ";
                    break;
                }
                case K_NEXT_LIST: {
                    info += "next_list: [";
                    const auto &list = get_next_list();
                    for (const auto &t: list) {
                        info += to_string(t) + ", ";
                    }
                    info += "], ";
                    break;
                }
                case K_PARAM_LIST: {
                    info += "parameters_list: [";
                    const auto &list = get_parameters_list();
                    for (const auto &p: list) {
                        info += p.get_info() + ", ";
                    }
                    info += "], ";
                    break;
                }
                case K_INIT_DEC: {
                    const auto &init_dec = get_init_declarator();
                    info += "{ symbol: " + get<0>(init_dec) +
                            ", initializer: " + get<1>(init_dec).get_info() +
                            ", is_pointer: " + ((get<2>(init_dec)) ? "true" : "false") +
                            ", array_size: " + to_string(get<3>(init_dec)) +
                            ", param_list_node: " + to_string(get<4>(init_dec)),
                            " }, ";
                    break;
                }
            }
        }
        each_key <<= 1;
    }
    return info + "}";
}

NodeType ParseNode::get_node_type() const {
    return type;
}

//=========================set_key系列===========================

void ParseNode::set_symbol(const string &symbol) {
    set_field<string>(K_SYMBOL, symbol);
}

void ParseNode::set_const(long long value) {
    set_field<ParseConstant>(K_CONST, ParseConstant(value));
}

void ParseNode::set_const(unsigned long long value) {
    set_field<ParseConstant>(K_CONST, ParseConstant(value));
}

void ParseNode::set_const(const string &value) {
    set_field<ParseConstant>(K_CONST, ParseConstant(value));
}

void ParseNode::set_const(bool value) {
    set_field<ParseConstant>(K_CONST, ParseConstant(value));
}

void ParseNode::set_const(long double value) {
    set_field<ParseConstant>(K_CONST, ParseConstant(value));
}

void ParseNode::set_type(const ParseType &p_type) {
    set_field<ParseType>(K_TYPE, p_type);
}

void ParseNode::set_variable(const ParseType &p_type, const string &symbol, size_t address) {
    set_field<ParseVariable>(K_VARIABLE, ParseVariable(p_type, symbol, address));
}

void ParseNode::set_variable(const ParseVariable &variable) {
    set_field<ParseVariable>(K_VARIABLE, variable);
}

void ParseNode::set_is_pointer(bool is_pointer) {
    set_field<bool>(K_IS_PTR, is_pointer);
}

void ParseNode::set_param_list_node(size_t param_list_node) {
    set_field<size_t>(K_PARAM_LIST_NODE, param_list_node);
}

void ParseNode::set_function(const ParseFunction &function) {
    set_field<ParseFunction>(K_FUNCTION, function);
}

void ParseNode::set_expression(const ParseExpression &expression) {
    set_field<ParseExpression>(K_EXPRESSION, expression);
}

void ParseNode::set_is_array(bool is_array) {
    set_field<bool>(K_IS_ARRAY, is_array);
}

void ParseNode::set_scope_id(size_t scope_id) {
    set_field<size_t>(K_SCOPE_ID, scope_id);
}

void ParseNode::set_next(size_t next) {
    set_field<size_t>(K_NEXT, next);
}

void ParseNode::set_begin(size_t begin) {
    set_field<size_t>(K_BEGIN, begin);
}

void ParseNode::set_code(size_t code) {
    set_field<size_t>(K_BEGIN, code);
}

void ParseNode::set_instr(size_t instr) {
    set_field<size_t>(K_INSTR, instr);
}

void ParseNode::set_true_list(const vector<size_t> &true_list) {
    set_field<vector<size_t> >(K_TRUE_LIST, true_list);
}

void ParseNode::set_false_list(const vector<size_t> &false_list) {
    set_field<vector<size_t> >(K_FALSE_LIST, false_list);
}

void ParseNode::set_true_jump(size_t true_jump) {
    set_field<size_t>(K_TRUE_JUMP, true_jump);
}

void ParseNode::set_false_jump(size_t false_jump) {
    set_field<size_t>(K_FALSE_JUMP, false_jump);
}

void ParseNode::set_next_list(const vector<size_t> &next_list) {
    set_field<vector<size_t> >(K_NEXT_LIST, next_list);
}

void ParseNode::update_is_array(bool is_array) {
    set_field<bool>(K_IS_ARRAY, is_array, true);
}

void ParseNode::set_param_list(const vector<ParseVariable> &param_list) {
    set_field<vector<ParseVariable> >(K_PARAM_LIST, param_list);
}

void ParseNode::set_init_dec(const InitDeclarator &init_dec) {
    set_field<InitDeclarator>(K_INIT_DEC, init_dec);
}

void ParseNode::add_init_declarator(const InitDeclarator &init_declarator) {
    try {
        // 暂时转变为非常量引用
        auto &list = (vector<InitDeclarator> &) get_init_declarators();
        list.emplace_back(init_declarator);
    } catch (ParseException &exc) {
        switch (exc.get_code()) {
            // 捕获异常: 找不到键值, 那就新建一个
            case EX_NODE_NO_SUCH_KEY: {
                vector<InitDeclarator> list;
                list.emplace_back(init_declarator);
                set_field<vector<InitDeclarator> >(K_INIT_DECLARATORS, list);
                break;
            }
            default:
                exc.push_trace("in ParseNode::add_init_declarator(const InitDeclarator &init_declarator)");
                throw exc;
        }
    }
}

//=========================get_key系列===========================

const string &ParseNode::get_symbol() const {
    return get_field<string>(K_SYMBOL);
}

const ParseType &ParseNode::get_type() const {
    return get_field<ParseType>(K_TYPE);
}

bool ParseNode::get_is_pointer() const {
    return get_field<bool>(K_IS_PTR);
}

size_t ParseNode::get_param_list_node() const {
    return get_field<size_t>(K_PARAM_LIST_NODE);
}

const ParseVariable &ParseNode::get_variable() const {
    return get_field<ParseVariable>(K_VARIABLE);
}

const ParseFunction &ParseNode::get_function() const {
    return get_field<ParseFunction>(K_FUNCTION);
}

const vector<ParseVariable> &ParseNode::get_parameters_list() const {
    return get_field<vector<ParseVariable> >(K_PARAM_LIST);
}

bool ParseNode::get_is_array() const {
    return get_field<bool>(K_IS_ARRAY);
}

const ParseConstant &ParseNode::get_const() const {
    return get_field<ParseConstant>(K_CONST);
}

const vector<InitDeclarator> &ParseNode::get_init_declarators() const {
    return get_field<vector<InitDeclarator> >(K_INIT_DECLARATORS);
}

const InitDeclarator &ParseNode::get_init_declarator() const {
    return get_field<InitDeclarator>(K_INIT_DEC);
}

const ParseExpression &ParseNode::get_expression() const {
    return get_field<ParseExpression>(K_EXPRESSION);
}

size_t ParseNode::get_scope_id() const {
    return get_field<size_t>(K_SCOPE_ID);
}

size_t ParseNode::get_next() const {
    return get_field<size_t>(K_NEXT);
}

size_t ParseNode::get_begin() const {
    return get_field<size_t>(K_BEGIN);
}

size_t ParseNode::get_code() const {
    return get_field<size_t>(K_CODE);
}

size_t ParseNode::get_instr() const {
    return get_field<size_t>(K_INSTR);
}

const vector<size_t> &ParseNode::get_true_list() const {
    // 需求特殊性: 如果没有则先set为空, 再返回之
    if(!(keys & K_TRUE_LIST)) {
        ((ParseNode*)this)->set_true_list(vector<size_t>());
    }
    return get_field<vector<size_t>>(K_TRUE_LIST);
}

const vector<size_t> &ParseNode::get_false_list() const {
    // 需求特殊性: 如果没有则先set为空, 再返回之
    if(!(keys & K_FALSE_LIST)) {
        ((ParseNode*)this)->set_false_list(vector<size_t>());
    }
    return get_field<vector<size_t>>(K_FALSE_LIST);
}

size_t ParseNode::get_true_jump() const {
    return get_field<size_t>(K_TRUE_JUMP);
}

size_t ParseNode::get_false_jump() const {
    return get_field<size_t>(K_FALSE_JUMP);
}

const vector<size_t> &ParseNode::get_next_list() const {
    // 需求特殊性: 如果没有则先set为空, 再返回之
    if(!(keys & K_NEXT_LIST)) {
        ((ParseNode*)this)->set_next_list(vector<size_t>());
    }
    return get_field<vector<size_t> >(K_NEXT_LIST);
}

void ParseNode::action_declaration(size_t scope_id, IR &ir) const {
    if (type != N_DECLARATION) {
        // 目前只允许声明节点做这个声明动作
        string info = "ParseNode::action_declaration(size_t scope_id): node_type=";
        info += get_node_type_name(type);
        info += "node_id=" + to_string(node_id);
        throw ParseException(EX_NODE_NOT_ALLOW_OP, info);
    }
    // 寻找初始化器节点和类型声明节点
    size_t init_declarator_list_node = 0, declaration_specifiers_node = 0;
    for (const auto &each :tree.node_children[node_id]) {
        if (tree.nodes[each].type == N_INIT_DECLARATOR_LIST) {
            init_declarator_list_node = each;
        } else if (tree.nodes[each].type == N_DECLARATION_SPE) {
            declaration_specifiers_node = each;
        }
    }
    if (!init_declarator_list_node || !declaration_specifiers_node) {
        // 找不到所需信息, 树结构不完整
        string info = "action_declaration(size_t scope_id) scope_id=";
        info += to_string(scope_id);
        info += " node_id=";
        info += to_string(node_id);
        throw ParseException(EX_TREE_NOT_INCOMPLETE, info);
    }
    // 初始化所需所有信息列表
    const auto &init_dec_list = tree.nodes[init_declarator_list_node].get_init_declarators();
    // 声明的类型
    const auto &dec_type = tree.nodes[declaration_specifiers_node].get_type();
    auto &which_scope = ParseScope::get_scope(scope_id);
    for (const auto &each : init_dec_list) {
        const string &symbol = get<0>(each);// 变量符号
        const ParseExpression &init_expr = get<1>(each);// 初始化表达式
        bool is_ptr = get<2>(each);// 是否声明为指针
        const size_t &array_size = get<3>(each),// 数组大小: 如果为0, 则表明不是数组
                &param_list_node = get<4>(each)// 声明为函数时的参数列表节点, 如果不声明为函数该值为0
        ;
        ParseType this_type(dec_type);
        if (is_ptr) {
            this_type = ParseType::get_pointer(this_type);
        }
        if (array_size) {
            this_type = ParseType::get_array(this_type, array_size);
        }
        if (param_list_node) {
            // 这里是函数声明
            const auto &args = tree.nodes[param_list_node].get_parameters_list();
            which_scope.declaration(symbol, ParseFunction(this_type, symbol, args)
            );
        } else {
            // 这里是变量声明
            ParseScope::get_scope(scope_id).declaration(symbol, ParseVariable(this_type, symbol, ir.getOffset()));
            IR_EMIT {
                ir.addOffset(this_type.get_size());
                if (init_expr.is_const()) {
                    const ParseConstant &E  = init_expr.get_const();
                    if (E.get_type() == ConstValueType::C_SIGNED) {
                        ir.gen(":=", symbol, "_", to_string(E.get_signed()));
                    } // TODO: MORE TYPE
                } else {
                    ExpressionType exp_type = init_expr.get_expr_type();
                    string op = "_", arg1 = "_", arg2 = "_", result = "_";
                    if (exp_type == ExpressionType::E_ADD) {
                        op = "+",
                        arg1 = to_string(tree.node(init_expr.get_child_expression(0)).get_expression().get_address());
                        arg2 = to_string(tree.node(init_expr.get_child_expression(1)).get_expression().get_address());
                        result = to_string(init_expr.get_id());
                    } 
                    else if (exp_type == ExpressionType::E_SUB) {
                        op = "-",
                        arg1 = to_string(tree.node(init_expr.get_child(0)).get_expression().get_address());
                        arg2 = to_string(tree.node(init_expr.get_child(1)).get_expression().get_address());
                        result = to_string(init_expr.get_id());
                    } 
                    else if (exp_type == ExpressionType::E_MUL) {
                        op = "*",
                        arg1 = to_string(tree.node(init_expr.get_child(0)).get_expression().get_address());
                        arg2 = to_string(tree.node(init_expr.get_child(1)).get_expression().get_address());
                        result = to_string(init_expr.get_id());
                    } 
                    else if (exp_type == ExpressionType::E_DIV) {
                        op = "/",
                        arg1 = to_string(tree.node(init_expr.get_child(0)).get_expression().get_address());
                        arg2 = to_string(tree.node(init_expr.get_child(1)).get_expression().get_address());
                        result = to_string(init_expr.get_id());
                    } 
                    else if (exp_type == ExpressionType::E_MOD) {
                        op = "%",
                        arg1 = to_string(tree.node(init_expr.get_child(0)).get_expression().get_address());
                        arg2 = to_string(tree.node(init_expr.get_child(1)).get_expression().get_address());
                        result = to_string(init_expr.get_id());
                    }
                    ir.gen(op, arg1, arg2, result);
                }
            }
        }
    }
}

void ParseNode::collect_parameters_list() const {
    auto &the_map = tree.node_param_list;
    // 结果
    vector<ParseVariable> res;
    switch (type) {
        case N_PARAM_LIST: {
            // 允许此操作的节点
            // 收集所有对应子节点类型的信息
            for (const auto &each : tree.node_children[node_id]) {
                const ParseNode &node = tree.nodes[each];
                if (node.type == N_PARAM_DECLARATION) {
                    res.emplace_back(node.get_variable());
                }
            }
            break;
        }
        default: {
            // 不允许此操作的节点
            string info = "ParseNode::collect_parameters_list(): node_type=";
            info += get_node_type_name(type);
            info += " node_id=" + to_string(node_id);
            throw ParseException(EX_NODE_NOT_ALLOW_OP, info);
        }
    }
    // 这里强制转换成非常量指针来改变keys表示该节点拥有这个键值
    ((ParseNode *) this)->keys |= K_PARAM_LIST;
    the_map[node_id] = res;
}

void ParseNode::collect_init_declarator() const {
    auto &the_map = tree.node_init_dec;
    InitDeclarator res(get_symbol(), ParseExpression(), get_is_pointer(), 0, get_param_list_node());
    // 获取元祖引用以便修改其值
    // 初始化表达式可能会有变动
    ParseExpression &init_expression = get<1>(res);
    // 数组大小声明可能会有变动
    size_t &arr_size = get<3>(res);
    switch (type) {
        case N_DECLARATOR: {
            // 没有初始化表达式, 所以可以直接get_expression()来获取表达式
            if (get_is_array()) {
                // 数组大小声明表达式
                const ParseExpression &arr_dec_expr = get_expression();
                if (arr_dec_expr.is_defined()) {
                    // 数组声明大小必须是可计算的无符号整数
                    try {
                        arr_size = arr_dec_expr.get_const().get_unsigned();
                    } catch (ParseException &ext) {
                        // 发现异常不是本层能够处理的
                        string info = "ParseNode::collect_init_declarator() node_id=" + to_string(node_id);
                        ext.push_trace(info);
                        throw ext;
                    }
                }
            }
            break;
        }
        case N_INIT_DECLARATOR: {
            // 因为有两个表达式, 一个是初始化表达式, 一个是数组大小声明表达式
            // 所以get_expression在该节点有歧义, 必须分开
            for (const auto &each : tree.node_children[node_id]) {
                const ParseNode &child = tree.nodes[each];
                switch (child.type) {
                    case N_DECLARATOR: {
                        if (child.get_is_array()) {
                            const ParseExpression &arr_dec_expr = child.get_expression();
                            if (arr_dec_expr.is_defined()) {
                                // 数组声明大小必须是可计算的无符号整数
                                try {
                                    arr_size = arr_dec_expr.get_const().get_unsigned();
                                } catch (ParseException &ext) {
                                    // 发现异常不是本层能够处理的
                                    string info = "ParseNode::collect_init_declarator() node_id=" + to_string(node_id);
                                    ext.push_trace(info);
                                    throw ext;
                                }
                            }
                        }
                        break;
                    }
                    case N_INITIALIZER : {
                        // TODO:其实可以只记录指针
                        init_expression = ParseExpression(child.get_expression());
                        break;
                    }
                    default:
                        break;
                }
            }
            break;
        }
        default: {
            string info = "ParseNode::collect_init_declarator(): node_type=";
            info += get_node_type_name(type);
            info += "node_id=" + to_string(node_id);
            throw ParseException(EX_NODE_NOT_ALLOW_OP, info);
        }
    }
    // 到此res就是抽取出的InitDeclarator
    // 这里强制转换成非常量指针来改变keys表示该节点拥有这个键值
    ((ParseNode *) this)->keys |= K_INIT_DEC;
    the_map[node_id] = res;
}

template<typename MapType>
MapType &ParseNode::get_field(NodeKey key) const {
    const auto key_map_iter = tree.node_key2map.find(key);
    map<size_t, MapType> &the_map = *((map<size_t, MapType> *) (key_map_iter->second));
    typedef typename map<size_t, MapType>::iterator MapIter;
    if (keys & key) {
        // 当前节点标记中有该key记录
        // 找该key对应的map
        if (key_map_iter == tree.node_key2map.end()) {
            // 但是没有找到key对应的记录
            // 说明没有定义key对应的map, 有bug
            string info = "NodeKey:";
            info += get_key_name(key) + " not defined.";
            throw ParseException(EX_NODE_KEY_NOT_DEFINED, info);
        }
        const MapIter &iter = the_map.find(node_id);
        if (iter == the_map.end()) {
            // 该节点标记有该记录, 但是对应的map中找不到该节点id的记录, 数据一致性被破坏 有bug
            string info = "ParseNode::get_field(NodeKey key): key=";
            info += get_key_name(key) + ", node_id=";
            info += to_string(node_id);
            throw ParseException(EX_NODE_KEY_DEFINED_NOT_FOUND, info);
        }
        return iter->second;
    } else {
        // 当前节点没有该记录
        // 有些节点是需要收集子树的信息来生成自身节点的信息, 所以需要先做一些动作进行收集信息
        switch (type) {
            case N_PARAM_LIST: {
                if (key == K_PARAM_LIST) {
                    collect_parameters_list();
                    return the_map.find(node_id)->second;
                }
                break;
            }
            case N_DECLARATOR:
            case N_INIT_DECLARATOR: {
                if (key == K_INIT_DEC) {
                    collect_init_declarator();
                    return the_map.find(node_id)->second;
                }
                break;
            }
            default:
                break;
        }
        // 试图向下搜索节点
        for (const auto &child_id: tree.node_children[node_id]) {
            const auto &child = tree.nodes[child_id];
            auto iter = tree.search_able.find(HasNodeKey(key, type, child.type));
            if (iter != tree.search_able.end()) {
                // 找到了记录, 说明该子节点存在唯一的key记录, 返回之
                return child.get_field<MapType>(key);
            }
        }
        // 到这仍未返回, 表示该节点没有该key唯一信息,
        // 说明存在异常, 因为只有当节点一定有该key唯一信息时才会调用此函数
        string info = "ParseNode::get_field(NodeKey key): key=";
        info += get_key_name(key) + ", node_id=";
        info += to_string(node_id);
        throw ParseException(EX_NODE_NO_SUCH_KEY, info);
    }
}

template<typename MapType>
void ParseNode::set_field(NodeKey key, const MapType &object, bool is_update) {
    const auto key_iter = tree.node_key2map.find(key);
    if (key_iter == tree.node_key2map.end()) {
        string info = "ParseNode::set_field(NodeKey key, const MapType &object, bool is_update): key=";
        info += get_key_name(key) + ", node_id=";
        info += to_string(node_id);
        throw ParseException(EX_NODE_KEY_NOT_DEFINED, info);
    }
    keys |= key;
    map<size_t, MapType> &the_map = *((map<size_t, MapType> *) (key_iter->second));
    typedef typename map<size_t, MapType>::iterator MapIter;
    const MapIter &iter = the_map.find(node_id);
    if (iter != the_map.end() && !is_update) {
        printf("ParseNode::set_field(NodeKey key, const MapType &object):"
               "警告:试图覆盖节点%zu的%s键\n", node_id, get_key_name(key).c_str());
    }
    the_map.insert(pair<size_t, MapType>(node_id, object));
}

#pragma clang diagnostic pop