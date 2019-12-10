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
    // 平凡节点, 什么都没有, 但是可以负责向下传递修改节点信号
            N_NORMAL = 0,
    // 异常节点, 什么都没有, 一般树中不应该存在异常节点, 一旦存在则说明有异常
            N_UNKNOWN = 1,
    // 标识符:
    //symbol: 符号
            N_IDENTIFIER = 2,
    // 常量节点:
    //value: 常量值
            N_CONST = 3,
    // 类型声明:
    // type: 声明的类型
            N_TYPE_SPE = 4,
    // 类型修饰声明:
    // 目前没有直接键值
            N_DECLARATION_SPE = 5,
    // 声明器:
    // 目前没有直接键值
            N_DECLARATOR = 6,
    // 直接声明:
    // 目前没有直接键值
            N_DIRECT_DEC = 7,
    // 参数列表:
    // 目前没有直接键值
            N_PARAM_LIST = 8,
    // 参数声明
    // variable参数变量
            N_PARAM_DECLARATION = 9,
    //函数声明
    //function参数记录
            N_FUNCTION_DEFINITION = 10,

};

enum NodeKey {
    // 符号, string*
            K_SYMBOL = 0,
    // 常量类型, ConstValueType*
            K_CONST_TYPE = 1,
    // 常量值: size_t 根据类型表示不同的指针:
            K_CONST_VALUE = 2,
    // 变量
            K_VARIABLE = 3,
    // 类型, 用于类型声明中
            K_TYPE = 4,
    // 是否声明为指针标记
            K_IS_PTR = 5,
    // 函数记录
            K_FUNCTION = 6,
};

#endif //NKU_PRACTICE_PARSE_DEF_H
