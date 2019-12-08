#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "ParseType.hpp"
#include <set>

map<ParseType, size_t> ParseType::type2id;
vector<ParseType> ParseType::id2type;
vector<string> ParseType::id2info;

ParseType::ParseType(const ParseType &other) {
    fields = nullptr;
    assign(*this, other);
}

ParseType::ParseType(BaseType b_type) {
    pointer_level = array_size = 0, lower_type = 0;
    fields = nullptr;
    type_id = base_type = b_type;
    switch (base_type) {
        case T_UNKNOWN:
        case T_VOID:
            type_size = 0;
            break;
        case T_SHORT:
            type_size = sizeof(short);
            break;
        case T_INT:
            type_size = sizeof(int);
            break;
        case T_LONG:
            type_size = sizeof(long);
            break;
        case T_FLOAT:
            type_size = sizeof(float);
            break;
        case T_DOUBLE:
            type_size = sizeof(double);
            break;
        case T_BOOL:
            type_size = sizeof(bool);
            break;
        case T_CHAR:
            type_size = sizeof(char);
            break;
        case T_ENUM:
            type_size = sizeof(int);
            break;
    }
}

string ParseType::get_info() const {
    size_t this_id = get_type_id(*this);
    if (this_id < id2info.size()) {
        return id2info[this_id];
    }
    while (this_id >= id2info.size()) {
        if (this_id == id2info.size()) {
            id2info.emplace_back("");
            break;
        }
        id2info.emplace_back(get_type(id2info.size()).get_info());
    }
    char buff[32];
    sprintf(buff, "Type-%zu {", get_type_id(*this));
    string res(buff);
    if (this_id <= T_BASE) {
        res += get_base_type_name(base_type);
    } else {
        if (array_size) {
            res += "array(";
            if (lower_type <= T_BASE) {
                res += get_base_type_name((BaseType) lower_type) + ",";
            } else {
                sprintf(buff, "Type-%zu,", lower_type);
                res += buff;
            }
            sprintf(buff, " %zu)", array_size);
            res += buff;
        } else {
            if (lower_type) {
                sprintf(buff, "(Type-%zu) ", lower_type);
                res += buff;
            }
            if (fields && !(fields->empty())) {
                for (const auto &each: *fields) {
                    res += each.first;
                    if (each.second <= T_BASE) {
                        sprintf(buff, ": %s, ", get_base_type_name((BaseType) each.second).c_str());
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

void ParseType::assign(ParseType &type, const ParseType &from_type) {
    delete type.fields;
    if (from_type.fields) {
        type.fields = new map<string, size_t>(*from_type.fields);
    } else {
        type.fields = nullptr;
    }
    type.lower_type = from_type.lower_type;
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

string ParseType::get_base_type_name(BaseType base_type) {
    switch (base_type) {
        case T_UNKNOWN:
            return "unknown";
        case T_SHORT:
            return "short";
        case T_INT:
            return "int";
        case T_LONG:
            return "long";
        case T_FLOAT:
            return "float";
        case T_DOUBLE:
            return "double";
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

ParseType ParseType::get_pointer(ParseType &type, size_t ptr_level) {
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
    printf("this new id = %zu\n", new_type_id);
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
//    printf("!%s\n", res.get_info().c_str());
//    printf("!!%s\n", id2type[new_type_id].get_info().c_str());
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
    if (lower_type < other.lower_type) {
        return true;
    } else if (lower_type > other.lower_type) {
        return false;
    }
    if (base_type < other.base_type) {
        return true;
    } else if (base_type > other.base_type) {
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

ParseType ParseType::get_type(size_t type_id) {
    if (id2type.empty()) {
        // 没有经过初始化, 进行初始化
        init();
    }
    if (type_id >= id2type.size()) {
        printf("ParseType &ParseType::get_node_type(size_t type_id): 警告: 试图获取不存在的类型, id:%zu\n", type_id);
        return ParseType(T_UNKNOWN);
    }
    return id2type[type_id];
}

void ParseType::init() {
//    T_UNKNOWN = 0,
//    T_SHORT = 1,
//    T_INT = 2,
//    T_LONG = 3,
//    T_FLOAT = 4,
//    T_DOUBLE = 5,
//    T_BOOL = 6,
//    T_CHAR = 7,
//    T_ENUM = 8,
//    T_VOID = 9,
    vector<ParseType> bases;
    bases.emplace_back(T_UNKNOWN);
    bases.emplace_back(T_SHORT);
    bases.emplace_back(T_INT);
    bases.emplace_back(T_LONG);
    bases.emplace_back(T_FLOAT);
    bases.emplace_back(T_DOUBLE);
    bases.emplace_back(T_BOOL);
    bases.emplace_back(T_CHAR);
    bases.emplace_back(T_ENUM);
    bases.emplace_back(T_VOID);
    for (const auto &each : bases) {
        type2id[each] = id2type.size();
        id2info.emplace_back(get_base_type_name((BaseType) id2type.size()));
        id2type.emplace_back(each);
    }
}

ParseType::ParseType(BaseType bt, size_t low_t, map<string, size_t> *fs, size_t ts, size_t ptr_lv, size_t arr_size) {
    base_type = bt;
    lower_type = low_t;
    fields = fs;
    type_size = ts;
    pointer_level = ptr_lv;
    array_size = arr_size;
    type_id = (size_t) -1;
}

void ParseType::print_all_type() {
    for (size_t i = 0; i < id2type.size(); ++i) {
        printf("%zu: %s\n", i, id2type[i].get_info().c_str());
    }
//    printf("type2id\n");
//    for (const auto &each: type2id) {
//        printf("%s => %zu\n", each.first.get_info().c_str(), each.second);
//    }
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

#pragma clang diagnostic pop