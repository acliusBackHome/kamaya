#ifndef NKU_PRACTICE_PARSE_DEF_H
#define NKU_PRACTICE_PARSE_DEF_H

#include<string>

using namespace std;

// 基本类型的枚举
enum BaseType {
    T_UNKNOWN = 0,
    T_SHORT = 1,// 因为所占空间一样, 所以unsigned 没必要在这标出
    T_INT = 2,
    T_LONG = 3,// 其实就是long long 和unsigned long long的表示
    T_FLOAT = 4,
    T_DOUBLE = 5,
    T_BOOL = 6,
    T_CHAR = 7,
    T_ENUM = 8,
    T_VOID = 9,
    T_BASE = 9 //当type_id小于等于这个值时,说明是基本类型
};

// 各种类型标识: 用32位整数表示:
// 这个32位整数 是下面的枚举按位或之后的结果
// 具有某个特征的基本类型则按位或某个枚举类型
// 检查时按位与就能查出来具有哪些特征
// 每个枚举为2的n次方
enum Specifier {
    S_UNSIGNED = 1, //无符号标识
    S_LONG = 2, //long 标识
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
            N_UNKNOWN,
    // 标识符:
    //symbol: 符号
            N_IDENTIFIER,
    // 常量节点:
    //value: 常量值
            N_CONST,
    // 类型声明:
    // type: 声明的类型
            N_TYPE_SPE,
    // 类型修饰声明:
    // 目前没有直接键值
            N_DECLARATION_SPE,
    // 声明器:
    // is_pointer: 是否声明为指针
    // is_function: 是否声明为函数
            N_DECLARATOR,
    // 直接声明:
    // is_array: 是否声明成数组标记
    // expression: 表达式, 表示数组大小的表达式
            N_DIRECT_DEC,
    // 参数列表:
    // scope_id: 表示该参数声明所在的新空间id
            N_PARAM_LIST,
    // 参数声明
    // variable参数变量
            N_PARAM_DECLARATION,
    //函数声明
    //function参数记录
            N_FUNCTION_DEFINITION,
    // 初始化声明器
    // 可进行的操作:
            N_INIT_DECLARATOR,
    // 初始化声明器列表
    // symbol_initializers: vector<InitDeclarator>,
    // string 是符号, ParseExpression是初始化表达式,
    // 用于给声明提供带初始化的符号信息
            N_INIT_DECLARATOR_LIST,
    // 初始化器
    // 目前没有直接键值
            N_INITIALIZER,
    //声明
    //目前没有直接键值
            N_DECLARATION,
    //表达式节点
    // expression: 该节点表示的表达式
            N_EXPRESSION,
    // 语句块语句列表节点
    // 目前没有直接键值
            N_BLOCK_ITEM_LIST,
    // 语句块语句
    // scope_id: 这个语句块对应的语句块空间id
            N_COMP_STMT,
    // for语句
    // 目前没有直接键值
            N_FOR_STMT,
};

enum NodeKey {
    // 符号, string*
            K_SYMBOL = 0,
    // 常量, ParseConstant*
            K_CONST = 1,
    // 变量, ParseVariable*
            K_VARIABLE = 3,
    // 类型, 用于类型声明中, ParseType*
            K_TYPE = 4,
    // 是否声明为指针标记, bool*
            K_IS_PTR = 5,
    // 函数记录, ParseFunction*
            K_FUNCTION = 6,
    // 表达式记录, ParseExpression*
            K_EXPRESSION = 7,
    // 是否被声明成数组布尔值, ParseExpression*
            K_IS_ARRAY = 8,
    // vector<InitDeclarator>* 符号和初始化表达式列表
            K_INIT_DECLARATORS = 9,
    // 语句块空间id size_t*
            K_SCOPE_ID = 10,
    // 是否声明为函数, 如果声明为参数, 该值不为0,
    // 表示树中的一个节点, 该节点存有声明函数的参数列表  size_t*
            K_PARAM_LIST_NODE = 11,
    //next, size_t *
            K_NEXT = 12,
    //begin size_t *
            K_BEGIN = 13,
    //code size_t *
            K_CODE = 14,
    // instr size_t *
            K_INSTR = 15,
    // true_list vector<size_t>*
            K_TRUE_LIST = 16,
    // false_list vector<size_t>*
            K_FALSE_LIST = 17

};

enum ExpressionType {
    E_UNDEFINED = 0,
    E_ADD, //加
    E_SUB,// 减
    E_MUL,// 乘
    E_DIV,// 除
    E_MOD,// 取余
    E_POW,// 求幂
    E_VAR,// 变量值
    E_CONST,// 常量
};

enum DeclarationType {
    D_UNKNOWN, //未知,可以占位
    D_FUNCTION,// 函数声明
    D_VARIABLE,// 变量声明
    // TODO: 类或者结构体声明
};

class ParseExpression;

/**
 *类型声明所需信息
 * <符号, 初始化赋值表达式, 是否声明为指针, 数组大小(如果不是数组则是0, 没有声明大小为(size_t)-1, 
 *   参数列表节点(如果声明为函数, 该值不为0, 表示带有参数列表信息的节点)>
 */
typedef tuple<string, ParseExpression, bool, size_t, size_t> InitDeclarator;


#endif //NKU_PRACTICE_PARSE_DEF_H
