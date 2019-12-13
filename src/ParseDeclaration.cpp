#include "ParseDeclaration.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

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

vector<ParseScope> ParseScope::scopes;

ParseScope::ParseScope(size_t parent) {
    this_scope = parent_scope = parent;
}

ParseScope::ParseScope(const ParseScope &other) {
    assign(*this, other);
}

void ParseScope::declaration(const string &symbol, const ParseVariable &variable) {
    if (symbol.empty()) {
        printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: 试图声明空符号\n");
        return;
    }
    auto it = symbol2dec_ptr.find(symbol);
    if (it != symbol2dec_ptr.end()) {
        printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: 试图重声明符号"
               "%s 为 变量:%s 跳过该操作\n", symbol.c_str(), variable.get_info().c_str());
        return;
    }
    vector<size_t> var_ptr;
    var_ptr.emplace_back((size_t) new ParseVariable(variable));
    symbol2dec_ptr[symbol] = pair<DeclarationType, vector<size_t> >(D_VARIABLE, var_ptr);
}

void ParseScope::declaration(const string &symbol, const ParseFunction &function) {
    if (symbol.empty()) {
        printf("ParseDeclaration(const string &symbol, const ParseFunction &function): 警告: 试图声明空符号\n");
        return;
    }
    if (parent_scope) {
        // TODO: 加入结构体后需要判断当前语句块是否是结构体声明里的
        printf("ParseDeclaration(const string &symbol, const ParseFunction &function): 警告: 试图在非静态区声明函数%s\n",
               function.get_info().c_str());
        return;
    }
    auto it = symbol2dec_ptr.find(symbol);
    if (it != symbol2dec_ptr.end()) {
        if (it->second.first != D_FUNCTION) {
            printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: "
                   "符号%s已被声明但不是函数声明, 试图重声明为函数%s\n", symbol.c_str(),
                   function.get_info().c_str());
            return;
        }
        vector<size_t> &dec_ptr = it->second.second;
        if (dec_ptr.empty()) {
            printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: 发现异常:"
                   "符号%s被声明为函数, 但是找不到其记录, 请检查实现和调用\n", symbol.c_str());
            return;
        }
        const ParseType &ret_type = ((ParseFunction *) dec_ptr[0])->get_ret_type(),
                &this_ret_type = function.get_ret_type();
        if (ret_type < this_ret_type || this_ret_type < ret_type) {
            printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: "
                   "试图重载返回值冲突的函数:%s, 返回值%s, 试图定义返回值%s\n", symbol.c_str(),
                   ret_type.get_info().c_str(), this_ret_type.get_info().c_str());
            return;
        }
        const auto &this_args = function.get_args();
        for (const auto &func_ptr : dec_ptr) {
            const auto &args = ((ParseFunction *) func_ptr)->get_args();
            if (this_args.size() == args.size()) {
                bool is_the_same = true;
                for (size_t i = 0; is_the_same && i < this_args.size(); ++i) {
                    if (this_args[i] < args[i] || args[i] < this_args[i]) {
                        is_the_same = false;
                    }
                }
                if (is_the_same) {
                    printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告:"
                           "试图处重复定义函数%s: %s, 跳过此操作\n", symbol.c_str(), function.get_info().c_str());
                    return;
                }
            }
        }
        dec_ptr.emplace_back((size_t) new ParseFunction(function));
        return;
    }
    vector<size_t> func_ptr;
    func_ptr.emplace_back((size_t) new ParseFunction(function));
    symbol2dec_ptr[symbol] = pair<DeclarationType, vector<size_t> >(D_FUNCTION, func_ptr);
}

vector<ParseFunction> ParseScope::get_function_declaration(const string &symbol) {
    vector<ParseFunction> res;
    auto it = symbol2dec_ptr.find(symbol);
    if (it == symbol2dec_ptr.end()) {
        if (parent_scope == this_scope) {
            printf("ParseDeclaration::get_function_declaration(const string &symbol): 警告 :未定义符号:"
                   "%s\n", symbol.c_str());
            return res;
        }
        return get_scope(parent_scope).get_function_declaration(symbol);
    }
    if (it->second.first != D_FUNCTION) {
        printf("ParseDeclaration::get_function_declaration(const string &symbol): 警告 :非函数声明"
               "%s\n", symbol.c_str());
        return res;
    }
    const vector<size_t> decs = it->second.second;
    for (const auto &each : decs) {
        res.emplace_back((*(ParseFunction *) each));
    }
    return res;
}

ParseVariable ParseScope::get_variable_declaration(const string &symbol) {
    auto it = symbol2dec_ptr.find(symbol);
    if (it == symbol2dec_ptr.end()) {
        if (parent_scope == this_scope) {
            printf("ParseDeclaration::get_variable_declaration(const string &symbol): 警告 :未定义符号:"
                   "%s\n", symbol.c_str());
            return ParseVariable();
        }
        return get_scope(parent_scope).get_variable_declaration(symbol);
    }
    if (it->second.first != D_VARIABLE) {
        printf("ParseDeclaration::get_variable_declaration(const string &symbol): 警告: 非变量声明"
               "%s\n", symbol.c_str());
        return ParseVariable();
    }
    const auto &var_ptr_list = it->second.second;
    if (var_ptr_list.empty()) {
        printf("ParseDeclaration::get_variable_declaration(const string &symbol): 警告: 数据一致性被破坏,"
               "请检查实现或者调用\n");
        return ParseVariable();
    }
    return *((ParseVariable *) var_ptr_list[0]);
}

size_t ParseScope::get_parent_scope_id() const {
    return parent_scope;
}

void ParseScope::print_all_declaration() {
    for (const auto &scope: scopes) {
        const auto &symbol2dec = scope.symbol2dec_ptr;
        printf("scope: %zu(%zu):\n",  scope.this_scope, scope.parent_scope);
        for (const auto &each : symbol2dec) {
            if(each.first.empty()) {
                continue;
            }
            printf("%s: ", each.first.c_str());
            DeclarationType dec_type = each.second.first;
            switch (dec_type) {
                case D_FUNCTION: {
                    putchar('\n');
                    const auto &func_ptr_list = each.second.second;
                    for (const auto &func_ptr:func_ptr_list) {
                        printf("    %s\n", ((ParseFunction *) func_ptr)->get_info().c_str());
                    }
                    break;
                }
                case D_VARIABLE: {
                    if (each.second.second.empty()) {
                        printf("异常: 定义为变量但未发现记录, 请检查实现或者调用\n");
                        continue;
                    }
                    const auto &var_ptr = each.second.second[0];
                    printf("%s\n", ((ParseVariable *) var_ptr)->get_info().c_str());
                    break;
                }
                default: {
                    printf("%s 未知声明\n", each.first.c_str());
                    break;
                }
            }
        }
    }
}

void ParseScope::init() {
    scopes.emplace_back(0);
    ParseScope &static_scope = scopes[0];
    vector<size_t> ptr;
    ptr.emplace_back(0);
    static_scope.symbol2dec_ptr[string()] = pair<DeclarationType, vector<size_t> >(D_UNKNOWN, ptr);
}

size_t ParseScope::new_scope(size_t parent) {
    if (scopes.empty()) {
        init();
    }
    if (parent >= scopes.size()) {
        printf("ParseDeclaration::new_scope(size_t parent): 警告: 试图获取不存在的作用域%zu\n", parent);
        return 0;
    }
    scopes.emplace_back(parent);
    scopes[scopes.size() - 1].this_scope = scopes.size() - 1;
    return scopes.size() - 1;
}

ParseScope &ParseScope::get_scope(size_t scope_id) {
    if (scopes.empty()) {
        init();
    }
    if (scope_id >= scopes.size()) {
        return scopes[0];
    }
    return scopes[scope_id];
}

void ParseScope::assign(ParseScope &scope, const ParseScope &from_scope) {
    scope.parent_scope = from_scope.parent_scope;
    scope.this_scope = from_scope.this_scope;
    for (const auto &each : from_scope.symbol2dec_ptr) {
        const auto &symbol = each.first;
        const auto &type = each.second.first;
        const auto &from_ptr = each.second.second;
        vector<size_t> ptr_list;
        switch (type) {
            case D_FUNCTION:
                for (const auto &each_ptr: from_ptr) {
                    ptr_list.emplace_back((size_t) new ParseFunction(*(ParseFunction *) each_ptr));
                }
                scope.symbol2dec_ptr[symbol] = pair<DeclarationType, vector<size_t> >(type, ptr_list);
                break;
            case D_VARIABLE:
                for (const auto &each_ptr: from_ptr) {
                    ptr_list.emplace_back((size_t) new ParseVariable(*(ParseVariable *) each_ptr));
                }
                scope.symbol2dec_ptr[symbol] = pair<DeclarationType, vector<size_t> >(type, ptr_list);
                break;
            case D_UNKNOWN:
                scope.symbol2dec_ptr[symbol] = pair<DeclarationType, vector<size_t> >(type, ptr_list);
                break;
        }
    }
}

ParseScope::~ParseScope() {
    for (const auto &each : symbol2dec_ptr) {
        switch (each.second.first) {
            case D_UNKNOWN:
                break;
            case D_FUNCTION:
                for (const auto &each_ptr : each.second.second) {
                    delete (ParseFunction *) (each_ptr);
                }
                break;
            case D_VARIABLE:
                for (const auto &each_ptr : each.second.second) {
                    delete (ParseVariable *) (each_ptr);
                }
                break;
        }
    }
}

ParseScope &ParseScope::operator=(const ParseScope &other) {
    assign(*this, other);
    return *this;
}

#pragma clang diagnostic pop