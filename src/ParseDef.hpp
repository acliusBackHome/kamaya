#ifndef NKU_PRACTICE_PARSE_DEF_H
#define NKU_PRACTICE_PARSE_DEF_H

// 基本类型的枚举
enum VariableType {
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
    // 字符串常量其value是指向一个const char*
            V_CONST_STRING = 12,
    //未定义的类型
            V_UNDEFINED = 13
};

enum NodeType {
    // 平凡节点, 什么都没有
            N_NORMAL = 0,
    // 标识符:
    //symbol: 变量名
            N_IDENTIFIER = 1,
    // 常量
    //const_value: 常量值
    //type: 值的类型
            N_CONST = 2,
};

enum NodeKey {
    // 符号, string*
            K_SYMBOL = 0,
    // 值类型: Type枚举 int*
            K_TYPE = 1,
    // 常量值 是<VariableType, void*>二元组的指针, void*随着 Type类型变化
            K_CONST_VALUE = 2,

};

#endif //NKU_PRACTICE_PARSE_DEF_H
