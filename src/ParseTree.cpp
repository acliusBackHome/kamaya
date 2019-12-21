#include "ParseTree.hpp"
#include "ParseDeclaration.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

ParseTree::ParseTree(const string &msg) {
    nodes.emplace_back(*this, 0, N_NORMAL);
    node_parent.push_back(0);
    node_msg.push_back(msg);
    node_children.emplace_back();
    // 初始化节点键值到指定map的映射
    node_key2map[K_SYMBOL] = (size_t) &node_symbol;
    node_key2map[K_CONST] = (size_t) &node_const;
    node_key2map[K_VARIABLE] = (size_t) &node_variable;
    node_key2map[K_TYPE] = (size_t) &node_type;
    node_key2map[K_PTR_LV] = (size_t) &node_ptr_lv;
    node_key2map[K_FUNCTION] = (size_t) &node_function;
    node_key2map[K_EXPRESSION] = (size_t) &node_expression;
    node_key2map[K_ARRAY_SIZE] = (size_t) &node_array_size;
    node_key2map[K_INIT_DECLARATORS] = (size_t) &node_init_declarators;
    node_key2map[K_SCOPE_ID] = (size_t) &node_scope_id;
    node_key2map[K_PARAM_LIST_NODE] = (size_t) &node_param_list_node;
    node_key2map[K_NEXT] = (size_t) &node_next;
    node_key2map[K_BEGIN] = (size_t) &node_begin;
    node_key2map[K_CODE] = (size_t) &node_code;
    node_key2map[K_INSTR] = (size_t) &node_instr;
    node_key2map[K_TRUE_LIST] = (size_t) &node_true_list;
    node_key2map[K_FALSE_LIST] = (size_t) &node_false_list;
    node_key2map[K_TRUE_JUMP] = (size_t) &node_true_jump;
    node_key2map[K_FALSE_JUMP] = (size_t) &node_false_jump;
    node_key2map[K_NEXT_LIST] = (size_t) &node_next_list;
    node_key2map[K_PARAM_LIST] = (size_t) &node_param_list;
    node_key2map[K_INIT_DEC] = (size_t) &node_init_dec;
    node_key2map[K_BEGIN_CODE] = (size_t) &node_begin_code;
    node_key2map[K_EXPRESSION_LIST] = (size_t) &node_expression_list;
    // 初始化可搜索节点
    search_able.insert(HasNodeKey(K_SYMBOL, N_DIRECT_DEC, N_DIRECT_DEC));
    search_able.insert(HasNodeKey(K_SYMBOL, N_DIRECT_DEC, N_IDENTIFIER));
    search_able.insert(HasNodeKey(K_SYMBOL, N_DECLARATOR, N_DIRECT_DEC));
    search_able.insert(HasNodeKey(K_SYMBOL, N_INIT_DECLARATOR, N_DECLARATOR));
    search_able.insert(HasNodeKey(K_TYPE, N_DECLARATION_SPE, N_TYPE_SPE));
    search_able.insert(HasNodeKey(K_PTR_LV, N_INIT_DECLARATOR, N_DECLARATOR));
    search_able.insert(HasNodeKey(K_PTR_LV, N_DECLARATOR, N_DIRECT_DEC));
    search_able.insert(HasNodeKey(K_PARAM_LIST_NODE, N_DIRECT_DEC, N_PARAM_LIST));
    search_able.insert(HasNodeKey(K_PARAM_LIST_NODE, N_DECLARATOR, N_DIRECT_DEC));
    search_able.insert(HasNodeKey(K_PARAM_LIST_NODE, N_INIT_DECLARATOR, N_DECLARATOR));
    search_able.insert(HasNodeKey(K_PARAM_LIST, N_DIRECT_DEC, N_PARAM_LIST));
    search_able.insert(HasNodeKey(K_PARAM_LIST, N_DECLARATOR, N_DIRECT_DEC));
    search_able.insert(HasNodeKey(K_ARRAY_SIZE, N_DECLARATOR, N_DIRECT_DEC));
    search_able.insert(HasNodeKey(K_CONST, N_INITIALIZER, N_CONST));
    search_able.insert(HasNodeKey(K_EXPRESSION, N_DECLARATOR, N_DIRECT_DEC));
    search_able.insert(HasNodeKey(K_EXPRESSION, N_INITIALIZER, N_EXPRESSION));
    search_able.insert(HasNodeKey(K_EXPRESSION, N_INITIALIZER, N_CONST));
}

string ParseTree::get_msg(size_t node_id) const {
    if (node_id >= node_msg.size()) {
        return "该树并没有此节点";
    }
    return node_msg[node_id];
}

size_t ParseTree::new_node(const string &msg, NodeType node_type) {
    node_msg.push_back(msg);
    node_parent.push_back((size_t) (-1));
    node_children.emplace_back();
    size_t this_node = node_msg.size() - 1;
    nodes.emplace_back(*this, this_node, node_type);
    last_nodes.insert(this_node);
    return this_node;
}

size_t ParseTree::new_node(NodeType node_type) {
    return new_node("", node_type);
}

void ParseTree::set_parent(size_t node_id, size_t parent) {
    if (node_id >= node_msg.size() || parent >= node_msg.size()) {
        printf("set_parent:无效的节点id(%zu, %zu)\n", node_id, parent);
        return;
    }
    if (node_parent[node_id] == (size_t) (-1)) {
        node_parent[node_id] = parent;
        if (!nodes[parent].has_key(K_BEGIN_CODE) && nodes[node_id].has_key(K_BEGIN_CODE)) {
            // 如果父节点没有K_BEGIN_CODE键值且孩子
            // 拥有键值K_BEGIN_CODE, 那么其K_BEGIN_CODE将
            // 传到其父节点也就是该节点上
            nodes[parent].set_begin_code(nodes[node_id].get_begin_code());
        }
        node_children[parent].push_back(node_id);
    } else {
        printf("该节点%zu已经有了父节点%zu, 不能再设父节点为%zu\n", node_id, node_parent[node_id], parent);
    }
    if (last_nodes.find(node_id) != last_nodes.end()) {
        last_nodes.erase(node_id);
    }
}

size_t ParseTree::get_parent(size_t node_id) {
    return node_parent[node_id];
}

void ParseTree::print() const {
    vector<size_t> has_next_children;
    auto *vis = new bool[get_node_num()];
    memset(vis, 0, sizeof(bool) * (get_node_num()));
    print_node(0, has_next_children, 1, true, vis);
    has_next_children.clear();
    for (size_t i = 1; i < get_node_num(); ++i) {
        if (!vis[i] && node_parent[i] == (size_t) (-1)) {
            printf("异常子树:\n");
            print_node(i, has_next_children, 2, true, vis);
            has_next_children.clear();
        }
    }
}

size_t ParseTree::get_node_num() const {
    return node_msg.size();
}

void ParseTree::print_node(size_t node_id, vector<size_t> &has_next_children,
                           size_t depth, bool last_child, bool *vis) const {
    if (node_id != 0) {
        if (last_child) {
            printf(" |_");
        } else {
            printf(" |-");
        }
    }
    try {
        printf("%zu:%s\n", node_id, nodes[node_id].get_node_info().c_str());
    } catch (ParseException &ext) {
        printf("!ExceptionNode %zu:%s\n", node_id, ParseNode::get_node_type_name(nodes[node_id].type).c_str());
    }
    vis[node_id] = true;
    string pre_fix;
    for (size_t i = 1; i < depth; ++i) {
        pre_fix.append("  ");
    }
    for (auto each : has_next_children) {
        pre_fix[1 + 2 * (each - 1)] = '|';
    }
    for (size_t i = 0; i < node_children[node_id].size(); ++i) {
        size_t next_node = node_children[node_id][i];
        printf("%s", pre_fix.c_str());
        if (has_next_children.empty() || has_next_children[has_next_children.size() - 1] < depth) {
            has_next_children.push_back(depth);
        }
        if (i == node_children[node_id].size() - 1) {
            auto it = find(has_next_children.begin(), has_next_children.end(), depth);
            if (it != has_next_children.end()) {
                has_next_children.erase(it);
            }
            print_node(next_node, has_next_children, depth + 1, true, vis);
        } else {
            print_node(next_node, has_next_children, depth + 1, false, vis);
        }
    }
}

bool ParseTree::check_node(size_t node_id) const {
    if (node_id >= node_msg.size()) {
        printf("警告:树中没有节点%zu\n", node_id);
        return false;
    }
    return true;
}

ParseNode &ParseTree::node(size_t node_id) {
    if (check_node(node_id)) {
        return nodes[node_id];
    }
    string info = "ParseTree::node(size_t node_id) node_id=";
    info += to_string(node_id);
    throw ParseException(EX_TREE_NO_SUCH_NODE, info);
}

//===========================make_系列============================

size_t ParseTree::make_const_node(unsigned long long unsigned_num) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(unsigned_num);
    return new_one;
}

size_t ParseTree::make_const_node(long long signed_num) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(signed_num);
    return new_one;
}

size_t ParseTree::make_const_node(long double float_num) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(float_num);
    return new_one;
}

size_t ParseTree::make_const_node(bool b) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(b);
    return new_one;
}

size_t ParseTree::make_const_node(const string &str) {
    size_t new_one = new_node(N_CONST);
    nodes[new_one].set_const(str);
    return new_one;
}

size_t ParseTree::make_identifier_node(const string &symbol) {
    size_t new_one = new_node(N_IDENTIFIER);
    nodes[new_one].set_symbol(symbol);
    return new_one;
}


size_t ParseTree::make_type_specifier_node(const ParseType &type) {
    size_t new_one = new_node(N_TYPE_SPE);
    nodes[new_one].set_type(type);
    return new_one;
}

ParseType ParseTree::get_type(const string &type_name) {
    auto iter = type_dic.find(type_name);
    if (iter == type_dic.end()) {
        return ParseType(T_UNKNOWN);
    }
    return ParseType::get_type(iter->second);
}

void ParseTree::type_def(const string &type_name, const ParseType &type) {
    auto iter = type_dic.find(type_name);
    if (iter != type_dic.end()) {
        printf("ParseTree::type_def(const string &type_name, const ParseType &type): "
               "警告: 试图给已声明的类型名字%s赋予新的类型%s, 无视该操作\n", type_name.c_str(),
               type.get_info().c_str());
        return;
    }
    size_t type_id = ParseType::get_type_id(type);
    type_dic[type_name] = type_id;
}

size_t ParseTree::make_declaration_specifier_node() {
    return new_node(N_DECLARATION_SPE);
}

size_t ParseTree::make_declarator_node(size_t ptr_lv) {
    size_t new_one = new_node(N_DECLARATOR);
    nodes[new_one].set_ptr_lv(ptr_lv);
    return new_one;
}

size_t ParseTree::make_direct_declarator_node(const vector<size_t> &array_size, const ParseExpression *expression) {
    size_t new_one = new_node(N_DIRECT_DEC);
    nodes[new_one].set_array_size(array_size);
    if (!array_size.empty()) {
        if (expression) {
            nodes[new_one].set_expression(*expression);
        } else {
            nodes[new_one].set_expression(ParseExpression::get_expression(0));
        }
    }
    return new_one;
}

size_t ParseTree::make_parameter_list_node() {
    return new_node(N_PARAM_LIST);
}

size_t ParseTree::make_parameter_declaration(const ParseVariable &variable) {
    size_t new_one = new_node(N_PARAM_DECLARATION);
    nodes[new_one].set_variable(variable);
    return new_one;
}

size_t ParseTree::make_function_definition_node() {
    return new_node(N_FUNCTION_DEFINITION);
}

size_t ParseTree::make_init_declarator_node() {
    return new_node(N_INIT_DECLARATOR);
}

size_t ParseTree::make_initializer_node() {
    return new_node(N_INITIALIZER);
}

size_t ParseTree::make_declaration_node() {
    return new_node(N_DECLARATION);
}

size_t ParseTree::make_init_declarator_list_node() {
    return new_node(N_INIT_DECLARATOR_LIST);
}

size_t ParseTree::make_expression_node(const ParseExpression &expression) {
    size_t new_one = new_node(N_EXPRESSION);
    nodes[new_one].set_expression(expression);
    return new_one;
}

size_t ParseTree::make_assign_expression_node(
        const ParseExpression &l_expr, const ParseExpression &r_expr) {
    const ParseExpression &assign_expr = ParseExpression::get_assign_expression(l_expr, r_expr);
    size_t new_one = new_node(N_EXPRESSION);
    nodes[new_one].set_expression(assign_expr);
    return new_one;
}

size_t ParseTree::make_block_item_list_node() {
    return new_node(N_BLOCK_ITEM_LIST);
}

size_t ParseTree::make_compound_statement_node() {
    return new_node(N_COMP_STMT);
}

size_t ParseTree::make_for_statement_node() {
    return new_node(N_FOR_STMT);
}

size_t ParseTree::make_pointer_node(size_t ptr_lv) {
    size_t new_one = new_node(N_POINTER);
    nodes[new_one].set_ptr_lv(ptr_lv);
    return new_one;
}

#pragma clang diagnostic pop