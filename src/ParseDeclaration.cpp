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

vector<ParseDeclaration::Scope> ParseDeclaration::scope_stack;

void ParseDeclaration::declaration(const string &symbol, const ParseVariable &variable) {
    if (symbol.empty()) {
        printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: 试图声明空符号\n");
        return;
    }
    if (scope_stack.empty()) {
        init();
    }
    size_t top_scope = scope_stack.size() - 1;
    auto &id2var = scope_stack[top_scope].id2var;
    auto &id2func = scope_stack[top_scope].id2func;
    auto &symbol2dec = scope_stack[top_scope].symbol2dec;
    auto it = symbol2dec.find(symbol);
    if (it != symbol2dec.end()) {
        printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: 试图重声明符号"
               "%s 为 变量:%s 跳过该操作\n", symbol.c_str(), variable.get_info().c_str());
        return;
    }
    vector<size_t> var_id;
    var_id.emplace_back(id2var.size());
    symbol2dec[symbol] = pair<DeclarationType, vector<size_t> >(D_VARIABLE, var_id);
    id2var.emplace_back(variable);
    id2func.emplace_back(ParseFunction());
}

void ParseDeclaration::declaration(const string &symbol, const ParseFunction &function) {
    if (symbol.empty()) {
        printf("ParseDeclaration(const string &symbol, const ParseFunction &function): 警告: 试图声明空符号\n");
        return;
    }
    if (scope_stack.empty()) {
        init();
    }
    size_t top_scope = scope_stack.size() - 1;
    if(top_scope) {
        // TODO: 加入结构体后需要判断当前语句块是否是结构体声明里的
        printf("ParseDeclaration(const string &symbol, const ParseFunction &function): 警告: 试图在非静态区声明函数%s\n",
            function.get_info().c_str());
        return;
    }
    auto &id2var = scope_stack[top_scope].id2var;
    auto &id2func = scope_stack[top_scope].id2func;
    auto &symbol2dec = scope_stack[top_scope].symbol2dec;
    auto it = symbol2dec.find(symbol);
    if (it != symbol2dec.end()) {
        if (it->second.first != D_FUNCTION) {
            printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: "
                   "符号%s已被声明但不是函数声明, 试图重声明为函数%s\n", symbol.c_str(),
                   function.get_info().c_str());
            return;
        }
        vector<size_t> &dec_funcs = it->second.second;
        if (dec_funcs.empty()) {
            printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: 发现异常:"
                   "符号%s被声明为函数, 但是找不到其记录, 请检查实现和调用\n", symbol.c_str());
            return;
        }
        const ParseType &ret_type = id2func[dec_funcs[0]].get_ret_type(),
                &this_ret_type = function.get_ret_type();
        if (ret_type < this_ret_type || this_ret_type < ret_type) {
            printf("ParseDeclaration(const string &symbol, const ParseVariable &variable): 警告: "
                   "试图重载返回值冲突的函数:%s, 返回值%s, 试图定义返回值%s\n", symbol.c_str(),
                   ret_type.get_info().c_str(), this_ret_type.get_info().c_str());
            return;
        }
        const auto &this_args = function.get_args();
        for (const auto &func_id : dec_funcs) {
            const auto &args = id2func[func_id].get_args();
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
        dec_funcs.emplace_back(id2var.size());
        id2var.emplace_back(ParseVariable());
        id2func.emplace_back(function);
        return;
    }
    vector<size_t> func_id;
    func_id.emplace_back(id2var.size());
    symbol2dec[symbol] = pair<DeclarationType, vector<size_t> >(D_FUNCTION, func_id);
    id2var.emplace_back(ParseVariable());
    id2func.emplace_back(function);
}

vector<ParseFunction> ParseDeclaration::get_function_declaration(const string &symbol) {
    vector<ParseFunction> res;
    if (scope_stack.empty()) {
        init();
        printf("ParseDeclaration::get_function_declaration(const string &symbol): 警告 :未定义符号:"
               "%s\n", symbol.c_str());
        return res;
    }
    for (size_t each_scope = scope_stack.size() - 1;; --each_scope) {
        const auto &id2var = scope_stack[each_scope].id2var;
        const auto &id2func = scope_stack[each_scope].id2func;
        const auto &symbol2dec = scope_stack[each_scope].symbol2dec;
        auto it = symbol2dec.find(symbol);
        if (it == symbol2dec.end()) {
            if (!each_scope) {
                printf("ParseDeclaration::get_function_declaration(const string &symbol): 警告 :未定义符号:"
                       "%s\n", symbol.c_str());
                return res;
            }
            continue;
        }
        if (it->second.first != D_FUNCTION) {
            printf("ParseDeclaration::get_function_declaration(const string &symbol): 警告 :非函数声明"
                   "%s\n", symbol.c_str());
            return res;
        }
        const vector<size_t> decs = it->second.second;
        for (const auto &each : decs) {
            res.emplace_back(id2func[each]);
        }
        return res;
    }
}

const ParseVariable &ParseDeclaration::get_variable_declaration(const string &symbol) {
    if (scope_stack.empty()) {
        init();
        printf("ParseDeclaration::get_variable_declaration(const string &symbol): 警告 :未定义符号:"
               "%s\n", symbol.c_str());
        return scope_stack[0].id2var[0];
    }
    for (size_t each_scope = scope_stack.size() - 1;; --each_scope) {
        const auto &id2var = scope_stack[each_scope].id2var;
        const auto &id2func = scope_stack[each_scope].id2func;
        const auto &symbol2dec = scope_stack[each_scope].symbol2dec;
        auto it = symbol2dec.find(symbol);
        if (it == symbol2dec.end()) {
            if (!each_scope) {
                printf("ParseDeclaration::get_variable_declaration(const string &symbol): 警告 :未定义符号:"
                       "%s\n", symbol.c_str());
                return scope_stack[0].id2var[0];
            }
            continue;
        }
        if (it->second.first != D_VARIABLE) {
            printf("ParseDeclaration::get_variable_declaration(const string &symbol): 警告: 非变量声明"
                   "%s\n", symbol.c_str());
            return scope_stack[0].id2var[0];
        }
        return id2var[it->second.second[0]];
    }
}

void ParseDeclaration::print_all_declaration() {
    for (size_t i = scope_stack.size() - 1;; --i) {
        const auto &id2var = scope_stack[i].id2var;
        const auto &id2func = scope_stack[i].id2func;
        const auto &symbol2dec = scope_stack[i].symbol2dec;
        if (id2var.size() != id2func.size()) {
            printf("ParseDeclaration::print_all_declaration(): 警告: 数据一致性被破坏,请检查实现或者调用\n");
        }
        for (const auto &each : symbol2dec) {
            DeclarationType dec_type = each.second.first;
            switch (dec_type) {
                case D_FUNCTION: {
                    putchar('\n');
                    const auto &func_id_list = each.second.second;
                    for (const auto &func_id:func_id_list) {
                        printf("    %s\n", id2func[func_id].get_info().c_str());
                    }
                    break;
                }
                case D_VARIABLE: {
                    if (each.second.second.empty()) {
                        printf("异常: 定义为变量但未发现记录, 请检查实现或者调用\n");
                        continue;
                    }
                    const auto &var_id = each.second.second[0];
                    printf("%s\n", id2var[var_id].get_info().c_str());
                    break;
                }
                default: {
                    printf("%s 未知声明\n", each.first.c_str());
                    break;
                }
            }
        }
        if (i == 0) {
            break;
        }
    }
}

void ParseDeclaration::init() {
    push_scope();
    Scope &static_scope = scope_stack[0];
    static_scope.symbol2dec[string()] = pair<DeclarationType, vector<size_t> >(D_UNKNOWN, vector<size_t>());
    static_scope.id2var.emplace_back(ParseVariable());
    static_scope.id2func.emplace_back(ParseFunction());
}

void ParseDeclaration::push_scope() {
    scope_stack.emplace_back();
}

void ParseDeclaration::pop_scope() {
    scope_stack.pop_back();
}

#pragma clang diagnostic pop