#ifndef NKU_PRACTICE_PARSE_DEF_H
#define NKU_PRACTICE_PARSE_DEF_H

// 基本类型的枚举
enum ValueType {
    // 变量:编译时只知道其值的类型信息和地址,不知道值

    V_SHORT = 0,
    V_INT = 1,
    V_LONG = 2,
    V_FLOAT = 3,
    V_DOUBLE = 4,
    V_BOOL = 5,
    V_CHAR = 6,
    V_ENUM = 7,
    V_STRUCT = 8,
    V_UNION = 9,
    V_VOID = 10,
    V_POINTER = 11,
    // 如果值类型为一个常量, 则在编译时就知道该值的所有信息,所以与变量稍有不同
            V_UNDEFINED
};

// 常量类型
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
