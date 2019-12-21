#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "ParseType.hpp"
#include "ParseTree.hpp"
#include <set>
#include <queue>

map<ParseType, size_t> ParseType::type2id;
vector<ParseType> ParseType::id2type;
vector<string> ParseType::id2info;
set<OpAble> ParseType::op_trying;
map<OpAble, size_t> ParseType::op_able;

ParseType::ParseType(const ParseType &other) {
    fields = nullptr;
    assign(*this, other);
}

ParseType::ParseType(BaseType b_type, int spe) {
    pointer_level = array_size = lower_type = 0;
    specifier = 0;
    if (spe & S_UNSIGNED) {
        specifier |= S_UNSIGNED;
    }
    if (spe & S_LONG) {
        specifier |= S_LONG;
    }
    fields = nullptr;
    type_id = (size_t) -1;
    if (!specifier) {
        type_id = b_type;
    }
    base_type = b_type;
    switch (base_type) {
        case T_UNKNOWN:
        case T_VOID:
            type_size = 0;
            break;
        case T_SHORT:
            // 取消long 修饰符, short没有long 修饰符
            specifier &= (~((int) S_LONG));
            type_size = sizeof(short);
            break;
        case T_INT:
            if (specifier & S_LONG) {
                type_size = sizeof(long int);
            } else {
                type_size = sizeof(int);
            }
            break;
        case T_LONG:
            if (specifier & S_LONG) {
                type_size = sizeof(long long);
            } else {
                type_size = sizeof(long);
            }
            break;
        case T_FLOAT:
            // 取消unsigned long修饰符, float没有unsigned long 修饰符
            // long float  在C89标准中已经删除
            specifier &= (~((int) S_UNSIGNED));
            specifier &= (~((int) S_LONG));
            type_size = sizeof(float);
            break;
        case T_DOUBLE:
            specifier &= (~((int) S_UNSIGNED));
            if (specifier & S_LONG) {
                type_size = sizeof(long double);
            } else {
                type_size = sizeof(double);
            }
            break;
        case T_BOOL:
            specifier &= (~((int) S_UNSIGNED));
            specifier &= (~((int) S_LONG));
            type_size = sizeof(bool);
            break;
        case T_CHAR:
            specifier &= (~((int) S_UNSIGNED));
            type_size = sizeof(char);
            break;
        case T_ENUM:
            specifier &= (~((int) S_UNSIGNED));
            specifier &= (~((int) S_LONG));
            type_size = sizeof(int);
            break;
    }
    if (base_type <= T_DOUBLE && base_type >= T_BOOL) {
        // 注册能够自动转换的类型
        // undefined表示可以直接自动类型转换
        size_t this_type_id = get_id();
        op_able.insert(pair<OpAble, size_t>(OpAble(E_UNDEFINED, this_type_id, this_type_id), this_type_id));
        for (size_t i = T_BOOL; i <= T_DOUBLE; ++i) {
            op_able.insert(pair<OpAble, size_t>(OpAble(E_UNDEFINED, this_type_id, i), i));
        }
    }
}

ParseType::ParseType(const ParseConstant &constant) {
    pointer_level = array_size = 0, lower_type = 0;
    fields = nullptr;
    switch (constant.get_type()) {
        case C_STRING: {
            ParseType arr_c = get_array(get_pointer(ParseType(T_CHAR)),
                                        constant.get_string().length());
            assign(*this, arr_c);
            break;
        }
        case C_SIGNED:
            base_type = T_LONG;
            type_id = (size_t) -1;
            specifier = S_LONG;
            type_size = sizeof(long long);
            break;
        case C_UNSIGNED:
            base_type = T_LONG;
            type_id = (size_t) -1;
            specifier = S_LONG | S_UNSIGNED;
            type_size = sizeof(unsigned long long);
            break;
        case C_FLOAT:
            base_type = T_DOUBLE;
            type_id = (size_t) -1;
            specifier = S_LONG;
            type_size = sizeof(long double);
            break;
        case C_BOOL:
            type_id = base_type = T_BOOL;
            type_size = sizeof(bool);
            break;
        case C_UNDEFINED:
            type_id = base_type = T_UNKNOWN;
            type_size = 0;
            break;
    }
}

string ParseType::get_info() const {
    size_t this_id = get_type_id(*this);
    if (this_id < id2info.size()) {
        return id2info[this_id];
    }
    for (size_t i = id2info.size(); i < this_id; ++i) {
        get_type(i).get_info();
    }
    id2info.emplace_back("");
    char buff[32];
    sprintf(buff, "Type-%zu {", this_id);
    string res(buff);
    if (this_id <= T_BASE) {
        res += get_base_type_name(base_type, 0);
    } else {
        if (base_type != T_UNKNOWN) {
            res += get_base_type_name(base_type, specifier);
        }
        if (array_size) {
            res += "array(";
            if (lower_type <= T_BASE) {
                res += get_base_type_name((BaseType) lower_type, specifier) + ",";
            } else {
                sprintf(buff, "Type-%zu,", lower_type);
                res += buff;
            }
            sprintf(buff, " %zu)", array_size);
            res += buff;
        } else {
            if (lower_type) {
                if (lower_type <= T_BASE) {
                    sprintf(buff, "%s", get_base_type_name((BaseType) lower_type, specifier).c_str());
                } else {
                    sprintf(buff, "Type-%zu ", lower_type);
                }
                res += buff;
            }
            if (fields && !(fields->empty())) {
                for (const auto &each: *fields) {
                    res += each.first;
                    if (each.second <= T_BASE) {
                        sprintf(buff, ": %s, ", get_base_type_name((BaseType) each.second, 0).c_str());
                    } else {
                        sprintf(buff, ": Type-%zu ,", each.second);
                    }
                    res += buff;
                }
            }
            for (size_t i = 0; i < pointer_level; ++i) {
                res += "*";
            }
        }
    }
    res += "}";
    id2info[this_id] = res;
    return res;
}

size_t ParseType::get_size() const {
    return type_size;
}

size_t ParseType::get_array_size() const {
    return array_size;
}

void ParseType::assign(ParseType &type, const ParseType &from_type) {
    delete type.fields;
    if (from_type.fields) {
        type.fields = new map<string, size_t>(*from_type.fields);
    } else {
        type.fields = nullptr;
    }
    type.lower_type = from_type.lower_type;
    type.specifier = from_type.specifier;
    type.base_type = from_type.base_type;
    type.type_size = from_type.type_size;
    type.pointer_level = from_type.pointer_level;
    type.array_size = from_type.array_size;
    type.type_id = from_type.type_id;
}

ParseType &ParseType::operator=(const ParseType &other) {
    assign(*this, other);
    return *this;
}

ParseType::ParseType() {
    base_type = T_UNKNOWN;
    array_size = pointer_level = type_size = 0;
    lower_type = 0;
    type_id = (size_t) -1;
    fields = nullptr;
}

ParseType::~ParseType() {
    delete fields;
}

string ParseType::get_base_type_name(BaseType base_type, int specifier) {
    string res;
    if (specifier & S_UNSIGNED) {
        res += "unsigned ";
    }
    if (specifier & S_LONG) {
        res += "long ";
    }
    switch (base_type) {
        case T_UNKNOWN:
            return "unknown";
        case T_SHORT:
            return res + "short";
        case T_INT:
            return res + "int";
        case T_LONG:
            return res + "long";
        case T_FLOAT:
            return res + "float";
        case T_DOUBLE:
            return res + "double";
        case T_BOOL:
            return "bool";
        case T_CHAR:
            return "char";
        case T_ENUM:
            return "enum";
        case T_VOID:
            return "void";
    }
    return "unknown";
}

ParseType ParseType::get_pointer(const ParseType &type, size_t ptr_level) {
    if (ptr_level == 0) {
        printf("ParseType::get_pointer(const ParseType &type, size_t ptr_level): 警告: 获取类型%s的0级指针\n",
               type.get_info().c_str());
        return type;
    }
    if (type.pointer_level == 0) {
        return ParseType(T_UNKNOWN, get_type_id(type), nullptr, sizeof(void *), ptr_level, 0);
    } else {
        auto res = ParseType(type);
        res.type_id = (size_t) -1;
        res.pointer_level += ptr_level;
        return res;
    }
}


ParseType ParseType::get_struct(const vector<pair<string, ParseType> > &otherTypes,
                                const vector<pair<string, size_t> > &self_ptr) {
    size_t new_type_id = id2type.size();
    // 给新类型预分配位置
    // 这有个"特性": 如果重复调用此函数试图生成一模一样的结构体会被看成两个
    id2type.emplace_back(T_UNKNOWN);
    ParseType &res = id2type[new_type_id];
    res.base_type = T_UNKNOWN;
    res.type_id = (size_t) -1;
    res.lower_type = 0;
    res.fields = new map<string, size_t>();
    res.type_size = res.pointer_level = res.array_size = 0;
    auto &res_fields = *res.fields;
    for (const auto &field: otherTypes) {
        if (res_fields.find(field.first) == res_fields.end()) {
            (*res.fields)[field.first] = get_type_id((ParseType &) field.second);
            res.type_size += field.second.get_size();
        } else {
            return ParseType();
        }
    }
    for (const auto &self_ptr_field: self_ptr) {
        if (res_fields.find(self_ptr_field.first) == res_fields.end()) {
            ParseType ptr(T_UNKNOWN, new_type_id, nullptr, sizeof(void *), self_ptr_field.second, 0);
            res.type_size += ptr.type_size;
            (*res.fields)[self_ptr_field.first] = get_type_id(ptr);
        } else {
            return ParseType();
        }
    }
    type2id[res] = res.type_id = new_type_id;
    return res;
}

ParseType ParseType::get_array(const ParseType &type, size_t array_size) {
    return ParseType(T_UNKNOWN, get_type_id((ParseType &) type), nullptr, array_size * type.get_size(), 0, array_size);
}

bool ParseType::operator<(const ParseType &other) const {
    if (type_size < other.type_size) {
        return true;
    } else if (type_size > other.type_size) {
        return false;
    }
    if (base_type < other.base_type) {
        return true;
    } else if (base_type > other.base_type) {
        return false;
    }
    if (specifier < other.specifier) {
        return true;
    } else if (specifier > other.specifier) {
        return false;
    }
    if (lower_type < other.lower_type) {
        return true;
    } else if (lower_type > other.lower_type) {
        return false;
    }
    if (pointer_level < other.pointer_level) {
        return true;
    } else if (pointer_level > other.pointer_level) {
        return false;
    }
    if (array_size < other.array_size) {
        return true;
    } else if (array_size > other.array_size) {
        return false;
    }
    if (!fields && other.fields) {
        return true;
    } else if (fields && !other.fields) {
        return false;
    } else if (fields && other.fields) {
        if (fields->size() < other.fields->size()) {
            return true;
        } else if (fields->size() > other.fields->size()) {
            return false;
        }
        set<string> all_fields;
        // 字典比较遵循以下原则: 遍历字典: 对两者都有的键来说, 没有键的一方小
        // 同样键中, 值小的一方小
        for (const auto &each : *fields) {
            if (all_fields.find(each.first) == all_fields.end()) {
                all_fields.insert(each.first);
            }
        }
        for (const auto &each : *other.fields) {
            if (all_fields.find(each.first) == all_fields.end()) {
                all_fields.insert(each.first);
            }
        }
        for (const auto &each: all_fields) {
            auto iter_this = fields->find(each), iter_other = other.fields->find(each);
            if (iter_this == fields->end() && iter_other != other.fields->end()) {
                return true;
            } else if (iter_this != fields->end() && iter_other == other.fields->end()) {
                return false;
            } else {
                // 到这一步一定都存在该键值
                if (iter_this->second < iter_other->second) {
                    return true;
                } else if (iter_this->second > iter_other->second) {
                    return false;
                }
            }
        }
    }
    // 到了这一步说明相等
    return false;
}

size_t ParseType::get_type_id(const ParseType &type) {
    if (id2type.empty()) {
        // 没有经过初始化, 进行初始化
        init();
    }
    if (type.type_id == (size_t) -1) {
        auto iter = type2id.find(type);
        if (iter == type2id.end()) {

            ((ParseType &) type).type_id = id2type.size();
            type2id[type] = type.type_id;
            id2type.emplace_back(type);
            return type.type_id;
        }
        return ((ParseType &) type).type_id = iter->second;
    }
    return type.type_id;
}

const ParseType &ParseType::get_type(size_t type_id) {
    if (id2type.empty()) {
        // 没有经过初始化, 进行初始化
        init();
    }
    if (type_id >= id2type.size()) {
        printf("ParseType &ParseType::get_node_type(size_t expr_id): 警告: 试图获取不存在的类型, id:%zu\n", type_id);
        return id2type[T_UNKNOWN];
    }
    return id2type[type_id];
}

void ParseType::init() {
//    T_UNKNOWN = 0,
//    T_BOOL = 1,
//    T_CHAR = 2,
//    T_SHORT = 3,
//    T_INT = 4,
//    T_LONG = 5,
//    T_FLOAT = 6,
//    T_DOUBLE = 7,
//    T_ENUM = 8,
//    T_VOID = 9,
    vector<ParseType> bases;
    bases.emplace_back(T_UNKNOWN);
    bases.emplace_back(T_BOOL);
    bases.emplace_back(T_CHAR);
    bases.emplace_back(T_SHORT);
    bases.emplace_back(T_INT);
    bases.emplace_back(T_LONG);
    bases.emplace_back(T_FLOAT);
    bases.emplace_back(T_DOUBLE);
    bases.emplace_back(T_ENUM);
    bases.emplace_back(T_VOID);
    for (const auto &each : bases) {
        type2id[each] = id2type.size();
        id2info.emplace_back(get_base_type_name((BaseType) id2type.size(), 0));
        id2type.emplace_back(each);
    }

    // 初始化可进行的自动类型转换
    // 普通运算
    for (size_t i = T_BOOL; i <= T_DOUBLE; ++i) {
        // 单目运算符
        for(size_t j = E_NOT; i <= E_BIT_NOT; ++i) {
            op_able.insert(pair<OpAble, size_t>(OpAble((ExpressionType)j, j, 0), j));
        }
        // 双目运算符, 只有相同类型的运算, 不同类型的运算可以通过搜索得知
        for (size_t j = E_ADD; j <= E_POW; ++j) {
            op_able.insert(pair<OpAble, size_t>(OpAble((ExpressionType) j, i, i), i));
        }
        // undefined表示可以直接自动类型转换
        for (size_t j = T_BOOL; j <= T_DOUBLE; ++j) {
            op_able.insert(pair<OpAble, size_t>(OpAble(E_UNDEFINED, i, j), j));
            // 赋值运算符赋值后的返回值是i
            op_able.insert(pair<OpAble, size_t>(OpAble(E_ASSIGN, i, j), i));
        }
    }
    // 逻辑运算符运算
    for (size_t i = E_LOGIC_OR; i <= E_LE; ++i) {
        op_able.insert(pair<OpAble, size_t>(OpAble((ExpressionType) i, T_BOOL, T_BOOL), T_BOOL));
    }
}

ParseType::ParseType(BaseType bt, size_t low_t, map<string, size_t> *fs, size_t ts, size_t ptr_lv, size_t arr_size, int spe) {
    base_type = bt;
    lower_type = low_t;
    fields = fs;
    specifier = spe;
    type_size = ts;
    pointer_level = ptr_lv;
    array_size = arr_size;
    type_id = (size_t) -1;
}

void ParseType::print_all_type() {
    for (size_t i = 0; i < id2type.size(); ++i) {
        printf("%zu: %s\n", i, id2type[i].get_info().c_str());
    }
}

ParseType ParseType::field(const string &key) const {
    if (fields) {
        auto iter = fields->find(key);
        if (iter == fields->end()) {
            return ParseType();
        }
        return id2type[iter->second];
    }
    return ParseType();
}

size_t ParseType::get_id() const {
    if (type_id == (size_t) -1) {
        return ((ParseType *) this)->type_id = get_type_id(*this);
    }
    return type_id;
}

size_t ParseType::get_specifier() const {
    if (base_type == T_UNKNOWN) {
        printf("ParseType::get_specifier(): 警告: 试图获取非基础类型Type-%zu的标识符\n", get_id());
        return 0;
    }
    return lower_type;
}

size_t ParseType::convert(ExpressionType op_type, const ParseType &type1, const ParseType &type2) {
    size_t type1_id = type1.get_id(), type2_id = type2.get_id();
    auto iter = op_able.find(OpAble(op_type, type1_id, type2_id));
    if (iter != op_able.end()) {
        // 找到记录, 返回
        if (iter->second != (size_t) -1) {
            // 有效记录
            return iter->second;
        }
        // 已经搜索过且被标记为不可转化(-1)抛出异常
    } else {
        // 进行DFS
        op_trying.clear();
        OpAble op_try(op_type, type1_id, type2_id);
        size_t res = dfs_convert(op_try);
        // 插入记录 缓存
        op_able[op_try] = res;
        if (res != (size_t) -1) {
            return res;
        }
    }
    string info = "ParseType::convert(ExpressionType op_type, const ParseType &type1, const ParseType &type2)";
    info += " type1=" + type1.get_info();
    info += " type2=" + type2.get_info();
    throw ParseException(EX_TYPE_CAN_NOT_CONVERT, info);
}

bool ParseType::convert(const ParseType &from, const ParseType &to) {
    return convert(E_ASSIGN, to, from) != (size_t) -1;
}

size_t ParseType::dfs_convert(const OpAble &op_try) {
    const auto record = op_able.find(op_try);
    if (record != op_able.end()) {
        return record->second;
    }
    ExpressionType op_type = get<0>(op_try);
    size_t type1_id = get<1>(op_try), type2_id = get<2>(op_try);
    if (type1_id == type2_id) {
        // 完全一致
        op_able[op_try] = type1_id;
        return type1_id;
    }
    const ParseType &type1 = get_type(type1_id), &type2 = get_type(type2_id);
    if (type1.base_type != T_UNKNOWN && type2.base_type != T_UNKNOWN) {
        // 发现是基础类型的派生
        if (type1.base_type > type2.base_type) {
            op_able[op_try] = type1_id;
            return type1_id;
        }
        if (type1.base_type < type2.base_type) {
            op_able[op_try] = type2_id;
            return type2_id;
        }
        // base_type一致, 比较声明符号
        if (type1.specifier > type2.specifier) {
            op_able[op_try] = type1_id;
            return type1_id;
        }
        if (type1.specifier < type2.specifier) {
            op_able[op_try] = type2_id;
            return type2_id;
        }
        //完全一致
        op_able[op_try] = type1_id;
        return type1_id;
    }
    // 向下一层搜索
    op_trying.insert(op_try);
    // 拿到所有type1和type2能够无条件转化的类型
    vector<size_t> type1_to, type2_to;
    for (const auto &each : op_able) {
        // 遍历目前记录表能够拿到的所有记录
        // 这个可以用多层map进行优化, 但是...现在还没必要...
        const auto &op = each.first;
        if (get<0>(each.first) == E_UNDEFINED) {
            if (get<1>(each.first) == type1_id) {
                type1_to.emplace_back(each.second);
            } else if (get<1>(each.first) == type2_id) {
                type2_to.emplace_back(each.second);
            }
        }
    }
    for (size_t type1_convert_id : type1_to) {
        for (size_t type2_convert_id : type2_to) {
            OpAble op_try_next(op_type, type1_convert_id, type2_convert_id);
            if (op_trying.find(op_try_next) != op_trying.end()) {
                //这个已经有在之前搜索了, 跳过
                // 防止DFS环
                continue;
            }
            size_t try_res = dfs_convert(op_try_next);// op_try_next已经在下一层标记了
            if (try_res != (size_t) -1) {
                // 结果有效
                op_able[op_try] = try_res;
                return try_res;
            }
            // 无效继续搜索
        }
    }
    // 搜索完毕, type1和type2怎么转化都无法得到表达式op_type的返回值
    op_able[op_try] = (size_t) -1;
    return (size_t) -1;
}

bool ParseType::cmp_wider_type(size_t t1_id, size_t t2_id) {
    if (t1_id <= T_BASE && t2_id <= T_BASE) {
        return t1_id >= t2_id;
    } else {
        const ParseType &t1 = ParseType::get_type(t1_id),
                &t2 = ParseType::get_type(t2_id);
        if (t1.base_type == t2.base_type && t1.base_type != T_UNKNOWN) {
            // 是基础类型的特殊声明 long unsigned等
            return t1.specifier >= t2.specifier;
        } else {
            return t1_id >= t2_id;
        }
    }
}

size_t ParseType::get_ptr_lv() const {
    return pointer_level;
}

const ParseType ParseType::get_lower_type() const {
    return get_type(lower_type);
}

#pragma clang diagnostic pop