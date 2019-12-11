#include "ParseTree.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

ParseTree::ParseTree(const string &msg) {
    nodes.emplace_back(0, N_NORMAL);
    node_parent.push_back(0);
    node_msg.push_back(msg);
    node_children.emplace_back();
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
    nodes.emplace_back(this_node, node_type);
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
        node_children[parent].push_back(node_id);
    } else {
        printf("该节点%zu已经有了父节点, 不能再设父节点为%zu\n", node_id, parent);
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
    switch (nodes[node_id].type) {
        case N_IDENTIFIER:
        case N_CONST:
        case N_TYPE_SPE:
        case N_DECLARATION_SPE:
        case N_DECLARATOR:
        case N_DIRECT_DEC:
        case N_PARAM_LIST:
        case N_PARAM_DECLARATION:
        case N_FUNCTION_DEFINITION:
        case N_INIT_DECLARATOR:
        case N_INIT_DECLARATOR_LIST:
        case N_INITIALIZER:
        case N_DECLARATION:
            printf("%zu:%s\n", node_id, nodes[node_id].get_node_info().c_str());
            break;
        case N_NORMAL:
        case N_UNKNOWN:
            printf("%zu:%s\n", node_id, node_msg[node_id].c_str());
            break;

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

ParseNode *ParseTree::node(size_t node_id) {
    if (check_node(node_id)) {
        return &nodes[node_id];
    }
    return nullptr;
}

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

ParseTree::~ParseTree() {
    for (auto &each : nodes) {
        each.delete_all_keys();
    }
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

size_t ParseTree::make_declarator_node(bool is_pointer) {
    size_t new_one = new_node(N_DECLARATOR);
    nodes[new_one].set_is_pointer(is_pointer);
    return new_one;
}

size_t ParseTree::make_direct_declarator_node(bool is_array, const ParseExpression *expression) {
    size_t new_one = new_node(N_DIRECT_DEC);
    nodes[new_one].set_is_array(is_array);
    if (is_array) {
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

size_t ParseTree::make_function_definition_node(
        const ParseType &ret_type, const string &symbol,
        const vector<ParseVariable> &args,
        size_t address) {
    size_t new_one = new_node(N_FUNCTION_DEFINITION);
    nodes[new_one].set_function(ParseFunction(
            ret_type, symbol, args, address
    ));
    return new_one;
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


ParseVariable::ParseVariable() : type(T_UNKNOWN), symbol(), address((size_t) -1) {}

ParseVariable::ParseVariable(const ParseType &_type, const string &_symbol, size_t _address) :
        type(_type), symbol(_symbol), address(_address) {}

string ParseVariable::get_info() const {
    string res = "ParseVariable { type:" + type.get_info() +
                 ", symbol:" + symbol + ", address:";
    char buff[32];
    sprintf(buff, "%zu }", address);
    return res + buff;
}

const ParseType &ParseVariable::get_type() const {
    return type;
}

string ParseVariable::get_symbol() const {
    return symbol;
}

size_t ParseVariable::get_address() const {
    return address;
}

void ParseVariable::set_address(size_t _address) {
    address = _address;
}

bool ParseVariable::operator<(const ParseVariable &other) const {
    if (type < other.type) {
        return true;
    } else if (other.type < type) {
        return false;
    }
    if (symbol.length() < other.symbol.length()) {
        return true;
    } else if (other.symbol.length() < symbol.length()) {
        return false;
    }
    for (size_t i = 0; i < symbol.length(); ++i) {
        if (symbol[i] < other.symbol[i]) {
            return true;
        } else if (other.symbol[i] < symbol[i]) {
            return false;
        }
    }
    return false;
}

ParseVariable &ParseVariable::operator=(const ParseVariable &other) = default;

ParseVariable::ParseVariable(const ParseVariable &other) = default;

ParseType ParseFunction::get_ret_type() const {
    return ret_type;
}

ParseFunction::ParseFunction(const ParseType &_ret_type, const string &_symbol, const vector<ParseVariable> &_args,
                             size_t _address) : ret_type(_ret_type), symbol(_symbol), args(_args), address(_address) {}

const vector<ParseVariable> &ParseFunction::get_args() const {
    return args;
}

string ParseFunction::get_symbol() const {
    return symbol;
}

size_t ParseFunction::get_address() const {
    return address;
}

void ParseFunction::set_address(size_t _address) {
    address = _address;
}

string ParseFunction::get_info() const {
    string res = "ParseFunction { return_type:" + ret_type.get_info() +
                 ", symbol:" + symbol + ", args[";
    for (const auto &each : args) {
        res += each.get_info() + ", ";
    }
    res += "], address:";
    char buff[32];
    sprintf(buff, "%zu }", address);
    return res + buff;
}

ParseFunction::ParseFunction() : ret_type(T_UNKNOWN), symbol("undefined"), args(), address((size_t) -1) {}

ParseConstant::ParseConstant() : type(C_UNDEFINED), value(0) {}

ParseConstant::ParseConstant(const ParseConstant &constant) {
    assign(*this, constant);
}


ParseConstant::ParseConstant(long long v) {
    type = C_SIGNED;
    value = (size_t) new long long(v);
}

ParseConstant::ParseConstant(unsigned long long v) {
    type = C_UNSIGNED;
    value = (size_t) new unsigned long long(v);
}

ParseConstant::ParseConstant(bool v) {
    type = C_BOOL;
    value = (size_t) new bool(v);
}

ParseConstant::ParseConstant(long double v) {
    type = C_FLOAT;
    value = (size_t) new long double(v);
}

ParseConstant::ParseConstant(const string &v) {
    type = C_STRING;
    value = (size_t) new string(v);
}

long long ParseConstant::get_signed() const {
    switch (type) {
        case C_SIGNED:
            return *(long long *) value;
        case C_UNSIGNED:
            return *(unsigned long long *) value;
        case C_FLOAT:
            return (long long) *(long double *) value;
        case C_BOOL:
            return (long long) *(bool *) value;
        case C_STRING:
        case C_UNDEFINED:
            break;
    }
    printf("ParseConstant::get_signed(): 警告 : 试图获取非法常量值\n");
    return 0;
}

unsigned long long ParseConstant::get_unsigned() const {
    switch (type) {
        case C_SIGNED:
            return (unsigned long long) *(long long *) value;
        case C_UNSIGNED:
            return *(unsigned long long *) value;
        case C_FLOAT:
            return (unsigned long long) *(long double *) value;
        case C_BOOL:
            return (unsigned long long) *(bool *) value;
        case C_STRING:
        case C_UNDEFINED:
            break;
    }
    printf("ParseConstant::get_unsigned(): 警告 : 试图获取非法常量值\n");
    return 0;
}

long double ParseConstant::get_float() const {
    switch (type) {
        case C_SIGNED:
            return *(long long *) value;
        case C_UNSIGNED:
            return *(unsigned long long *) value;
        case C_FLOAT:
            return *(long double *) value;
        case C_BOOL:
            return (long double) *(bool *) value;
        case C_STRING:
        case C_UNDEFINED:
            break;
    }
    printf("ParseConstant::get_float(): 警告 : 试图获取非法常量值\n");
    return 0;
}

bool ParseConstant::get_bool() const {
    switch (type) {
        case C_SIGNED:
            return (bool) *(long long *) value;
        case C_UNSIGNED:
            return (bool) *(unsigned long long *) value;
        case C_FLOAT:
            return (bool) *(long double *) value;
        case C_BOOL:
            return *(bool *) value;
        case C_STRING:
        case C_UNDEFINED:
            break;
    }
    printf("ParseConstant::get_bool(): 警告 : 试图获取非法常量值\n");
    return false;
}

string ParseConstant::get_string() const {
    if (type != C_STRING) {
        printf("ParseConstant::get_string(): 警告 : 试图获取非法常量值\n");
        return "";
    }
    return *(string *) value;
}

ParseConstant::~ParseConstant() {
    switch (type) {
        case C_STRING:
            delete (string *) value;
            break;
        case C_SIGNED:
            delete (long long *) value;
            break;
        case C_UNSIGNED:
            delete (unsigned *) value;
            break;
        case C_FLOAT:
            delete (long double *) value;
            break;
        case C_BOOL:
            delete (bool *) value;
            break;
        case C_UNDEFINED:
            break;
    }
}

string ParseConstant::get_const_type_name(ConstValueType _type) {
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

string ParseConstant::get_info() const {
    char buff[64];
    switch (type) {
        case C_STRING:
            return "const string: " + *((string *) value);
        case C_SIGNED: {
            sprintf(buff, "const signed: %lld", *(long long *) value);
            return string(buff);
        }
        case C_UNSIGNED: {
            sprintf(buff, "const unsigned: %llu", *(unsigned long long *) value);
            return string(buff);
        }
        case C_FLOAT: {
            sprintf(buff, "const float: %Lf", *(long double *) value);
            return string(buff);
        }
        case C_BOOL: {
            sprintf(buff, "const bool: %s", (*(bool *) value) ? "true" : "false");
            return string(buff);
        }
        case C_UNDEFINED: {
            return "undefined";
        }
    }
    return "unknown";
}

bool ParseConstant::operator<(const ParseConstant &other) const {
    if (type < other.type) {
        return true;
    } else if (other.type < type) {
        return false;
    }
    switch (type) {
        case C_STRING: {
            const string &this_value = *(string *) value, &that_value = *(string *) other.value;
            if (this_value.length() < that_value.length()) {
                return true;
            } else if (that_value.length() < this_value.length()) {
                return false;
            }
            for (size_t i = 0; i < this_value.length(); ++i) {
                if (this_value[i] < that_value[i]) {
                    return true;
                } else if (that_value[i] < this_value[i]) {
                    return false;
                }
            }
            break;
        }
        case C_SIGNED: {
            const long long &this_value = *(long long *) value, &that_value = *(long long *) other.value;
            if (this_value < that_value) {
                return true;
            } else if (that_value < this_value) {
                return false;
            }
            break;
        }
        case C_UNSIGNED: {
            const unsigned long long &this_value = *(unsigned long long *) value,
                    &that_value = *(unsigned long long *) other.value;
            if (this_value < that_value) {
                return true;
            } else if (that_value < this_value) {
                return false;
            }
            break;
        }
        case C_FLOAT: {
            const long double &this_value = *(long double *) value,
                    &that_value = *(long double *) other.value;
            if (this_value < that_value) {
                return true;
            } else if (that_value < this_value) {
                return false;
            }
            break;
        }
        case C_BOOL: {
            bool this_value = *(bool *) value,
                    that_value = *(bool *) other.value;
            if (!this_value && that_value) {
                return true;
            } else if (!that_value && this_value) {
                return false;
            }
            break;
        }
        case C_UNDEFINED:
            break;
    }
    return false;
}

void ParseConstant::assign(ParseConstant &constant, const ParseConstant &from_constant) {
    constant.type = from_constant.type;
    switch (constant.type) {
        case C_STRING:
            constant.value = (size_t) new string(*(string *) from_constant.value);
            break;
        case C_SIGNED:
            constant.value = (size_t) new long long(*(long long *) from_constant.value);
            break;
        case C_UNSIGNED:
            constant.value = (size_t) new unsigned long long(*(unsigned long long *) from_constant.value);
            break;
        case C_FLOAT:
            constant.value = (size_t) new long double(*(long double *) from_constant.value);
            break;
        case C_BOOL:
            constant.value = (size_t) new bool(*(bool *) from_constant.value);
            break;
        case C_UNDEFINED:
            constant.value = 0;
            break;
    }
}

ParseConstant &ParseConstant::operator=(const ParseConstant &other) {
    assign(*this, other);
    return *this;
}

ConstValueType ParseConstant::get_type() const {
    return type;
}

ConstValueType ParseConstant::wider_const_type(ConstValueType type1, ConstValueType type2) {
    switch (type1) {
        case C_BOOL:
        case C_STRING:
        case C_UNDEFINED:
            if (type1 == type2) {
                return type1;
            }
            break;
        case C_SIGNED: {
            switch (type2) {
                case C_SIGNED:
                    return C_SIGNED;
                case C_UNSIGNED:
                    return C_UNSIGNED;
                case C_FLOAT:
                    return C_FLOAT;
                case C_STRING:
                case C_BOOL:
                case C_UNDEFINED:
                    break;
            }
            break;
        }
        case C_UNSIGNED: {
            switch (type2) {
                case C_SIGNED:
                case C_UNSIGNED:
                    return C_UNSIGNED;
                case C_FLOAT:
                    return C_FLOAT;
                case C_STRING:
                case C_BOOL:
                case C_UNDEFINED:
                    break;
            }
            break;
        }
        case C_FLOAT: {
            switch (type2) {
                case C_SIGNED:
                case C_UNSIGNED:
                case C_FLOAT:
                    return C_FLOAT;
                case C_STRING:
                case C_BOOL:
                case C_UNDEFINED:
                    break;
            }
            break;
        }
    }
    printf("ParseConstant::wider_const_type(ConstValueType type1, ConstValueType type2):"
           "警告:常量类型%s 与 %s无法进行运算\n", get_const_type_name(type1).c_str(),
           get_const_type_name(type2).c_str());
    return C_UNDEFINED;
}

#pragma clang diagnostic pop