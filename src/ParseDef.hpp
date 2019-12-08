#ifndef NKU_PRACTICE_PARSE_DEF_H
#define NKU_PRACTICE_PARSE_DEF_H

// 基本类型的枚举
enum BaseType {
    T_UNKNOWN = 0,
    T_SHORT = 1,
    T_INT = 2,
    T_LONG = 3,
    T_FLOAT = 4,
    T_DOUBLE = 5,
    T_BOOL = 6,
    T_CHAR = 7,
    T_ENUM = 8,
    T_VOID = 9,
            T_BASE = 9 //当type_id小于等于这个值时,说明是基本类型
};

// 常量类型, 编译期用来表示用户输入常量的
enum ConstValueType {
    C_STRING = 0,// 字符串常量其表示的指针是指向一个const char*
    C_SIGNED = 1,    // 有符号整形常量 long long *
    C_UNSIGNED = 2,    //无符号整形常量 unsigned long long *
    C_FLOAT = 3,    // 浮点型常量 long double *
    C_BOOL = 4,    //布尔值 bool *
    C_UNDEFINED = 5    //未定义的类型
};

enum NodeType {
    // 平凡节点, 什么都没有
            N_NORMAL = 0,
    // 异常节点, 什么都没有
            N_UNKNOWN = 1,
    // 标识符:
    //symbol: 符号
            N_IDENTIFIER = 2,
    // 常量节点:
    //value: 常量值
            N_CONST = 3,
    // 变量节点:
    // symbol: 变量名
    // value: 变量值
};

enum NodeKey {
    // 符号, string*
            K_SYMBOL = 0,
    // 常量类型, ConstValueType*
            K_CONST_TYPE = 1,
    // 常量值: size_t 根据类型表示不同的指针:
            K_CONST_VALUE = 2,
};

#endif //NKU_PRACTICE_PARSE_DEF_H
