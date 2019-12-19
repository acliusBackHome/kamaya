#ifndef NKU_PRACTICE_PARSE_DEF_H
#define NKU_PRACTICE_PARSE_DEF_H

#include<string>
#include <tuple>

using namespace std;

// 表示表达式的数据结构
class ParseExpression;

// 表示变量的数据结构
class ParseVariable;

// 表示函数的数据结构
class ParseFunction;

// 表示常量的数据结构
class ParseConstant;

// 表示自定义异常的数据结构
class ParseException;

// 表示语法树的数据结构
class ParseTree;

//类型声明所需信息<符号, 初始化赋值表达式, 声明的指针级数(如果不是指针为0), 数组大小(如果不是数组则是0,
// 没有声明大小为(size_t)-1, 参数列表节点(如果声明为函数, 该值不为0, 表示带有参数列表信息的节点)>
typedef tuple<string, ParseExpression, size_t, size_t, size_t> InitDeclarator;

// 基本类型的枚举
enum BaseType {
    T_UNKNOWN = 0,
    T_BOOL = 1,
    T_CHAR = 2,
    T_SHORT = 3,// 因为所占空间一样, 所以unsigned 没必要在这标出
    T_INT = 4,
    T_LONG = 5,// 其实就是long long 和unsigned long long的表示
    T_FLOAT = 6,
    T_DOUBLE = 7,
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
    // is_pointer: 是否声明为指针
    // is_function: 是否声明为函数
            N_DECLARATOR = 6,
    // 直接声明:
    // is_array: 是否声明成数组标记
    // expression: 表达式, 表示数组大小的表达式
            N_DIRECT_DEC = 7,
    // 参数列表:
    // scope_id: 表示该参数声明所在的新空间id
            N_PARAM_LIST = 8,
    // 参数声明
    // variable参数变量
            N_PARAM_DECLARATION = 9,
    //函数声明
    //function参数记录
            N_FUNCTION_DEFINITION = 10,
    // 初始化声明器
    // 可进行的操作:
            N_INIT_DECLARATOR = 11,
    // 初始化声明器列表
    // symbol_initializers: vector<InitDeclarator>,
    // string 是符号, ParseExpression是初始化表达式,
    // 用于给声明提供带初始化的符号信息
            N_INIT_DECLARATOR_LIST = 12,
    // 初始化器
    // 目前没有直接键值
            N_INITIALIZER = 13,
    //声明
    //目前没有直接键值
            N_DECLARATION = 14,
    //表达式节点
    // expression: 该节点表示的表达式
            N_EXPRESSION = 15,
    // 语句块语句列表节点
    // 目前没有直接键值
            N_BLOCK_ITEM_LIST = 16,
    // 语句块语句
    // scope_id: 这个语句块对应的语句块空间id
            N_COMP_STMT = 17,
    // for语句
    // 目前没有直接键值
            N_FOR_STMT = 18,
    //指针标识节点
    // ptr_lv 指针等级, 表示声明了多少级指针
            N_POINTER = 19
};

// 用64位整数表示该节点所拥有的键值
// 如果一个节点有相应的键值, 那么该节点的size_t keys字段按位与上下列的值
// 后不会是0值, 以此判断该节点是否有指定键值
enum NodeKey {
    // 符号, string
            K_SYMBOL = 1,
    // 常量, ParseConstant
            K_CONST = 1 << 1,
    // 变量, ParseVariable
            K_VARIABLE = 1 << 2,
    // 类型, 用于类型声明中, ParseExpression
            K_TYPE = 1 << 3,
    // 是否声明为指针标记, bool
            K_PTR_LV = 1 << 4,
    // 函数记录, ParseFunction
            K_FUNCTION = 1 << 5,
    // 表达式记录, ParseExpression
            K_EXPRESSION = 1 << 6,
    // 是否被声明成数组布尔值, bool
            K_IS_ARRAY = 1 << 7,
    // 符号和初始化表达式列表, vector<InitDeclarator>
            K_INIT_DECLARATORS = 1 << 8,
    // 语句块空间id, size_t
            K_SCOPE_ID = 1 << 9,
    // 是否声明为函数, size_t ,
    // 如果声明为参数, 该值不为0, 表示树中的一个节点, 该节点存有声明函数的参数列表
            K_PARAM_LIST_NODE = 1 << 10,
    //next, size_t
            K_NEXT = 1 << 11,
    //begin size_t
            K_BEGIN = 1 << 12,
    //code size_t
            K_CODE = 1 << 13,
    // instr size_t
            K_INSTR = 1 << 14,
    // true_list vector<size_t>
            K_TRUE_LIST = 1 << 15,
    // false_list vector<size_t>
            K_FALSE_LIST = 1 << 16,
    // true_jump size_t
            K_TRUE_JUMP = 1 << 17,
    // false_jump size_t
            K_FALSE_JUMP = 1 << 18,
    // next_list vector<size_t>
            K_NEXT_LIST = 1 << 19,
    // 参数列表: vector<ParseVariable>
            K_PARAM_LIST = 1 << 20,
    // 初始化声明器 InitDeclarator
            K_INIT_DEC = 1 << 21,
};
#define NUM_OF_NODE_KEY_TYPE 22
#define MAX_NODE_KEY_TYPE ((1 << (NUM_OF_NODE_KEY_TYPE - 1)) + 1)

// 用于表示参数2所表示的节点中是否存在它的子节点中是否有参数1类型的唯一的key,
// 如果有, 那么就向其子节点为参数3的子节点中搜索
typedef tuple<NodeKey, NodeType, NodeType> HasNodeKey;

enum ExpressionType {
    E_UNDEFINED = 0,
    E_VAR,// 变量值
    E_CONST,// 常量
    E_ADD, //加
    E_SUB,// 减
    E_MUL,// 乘
    E_DIV,// 除
    E_MOD,// 取余
    E_POW,// 求幂

    // 逻辑运算

    E_NOT, //非 !
    E_LOGIC_OR,//逻辑或 ||
    E_LOGIC_AND,//逻辑与 &&
    E_G,//大于 >
    E_GE, //大于等于>=
    E_EQUAL,//等于==
    E_NE,//不等于!=
    E_L,//小于<
    E_LE,// 小于等于

    // 赋值运算符

    E_ASSIGN, //等于
};

// 表示是否可以进行ExpressionType的运算符运算的记录
// type1 和 type2分别表示ParseType的全局id
// 如果是单目运算符参数T2无效,为03
typedef tuple<ExpressionType, size_t, size_t> OpAble;
//struct OpAble {
//    ExpressionType expr_type;
//    size_t type1, type2;
//
//    OpAble(ExpressionType e_type, size_t t1, size_t t2) {
//        expr_type = e_type;
//        type1 = t1;
//        type2 = t2;
//    }
//
//    bool operator<(const OpAble &other) const {
//        if (expr_type < other.expr_type) {
//            return true;
//        } else if (expr_type > other.expr_type) {
//            return false;
//        }
//        if (type1 < other.type1) {
//            return true;
//        } else if (type1 > other.type1) {
//            return false;
//        }
//        if (type2 < other.type2) {
//            return true;
//        } else if (type2 > other.type2) {
//            return false;
//        }
//        return false;
//    }
//};

enum DeclarationType {
    D_UNKNOWN, //未知,可以占位
    D_FUNCTION,// 函数声明
    D_VARIABLE,// 变量声明
    // TODO: 类或者结构体声明
};

// 自定义异常代码
enum ExceptionCode {
    EX_UNKNOWN = 0,//未知异常
    EX_NODE_KEY_NOT_DEFINED, //NodeKey中未定义指定键,bug
    EX_NODE_NO_SUCH_KEY,//节点没有指定键值
    EX_NODE_KEY_DEFINED_NOT_FOUND,// 节点的keys定义了该值,但是没有找见其对象,bug
    EX_NODE_NOT_ALLOW_OP,//节点未定义的操作但调用之,bug
    EX_TREE_NOT_INCOMPLETE,//树的结构不完整导致的异常,bug
    EX_TREE_NO_SUCH_NODE,//树中并没有指定id的节点,bug
    EX_NOT_DECLARED,//没有此符号声明
    EX_DECLARATION_NOT_A_VARIABLE,//符号已声明但不是声明为变量
    EX_DECLARATION_NOT_A_FUNCTION,//符号已声明但不是声明为函数
    EX_DECLARATION_NOT_FOUND,//符号已声明但没有找到其记录,bug
    EX_EXPRESSION_NOT_CONST,//试图获取不可在编译期获取值的表达式时抛出的异常
    EX_EXPRESSION_DIVIDE_ZERO,//除0异常
    EX_TYPE_CAN_NOT_CONVERT,//不能进行类型转化异常
    EX_EXPRESSION_CAN_NOT_GENERATE,//产生了错误的表达式, bug
    EX_EXPRESSION_CAN_NOT_ACCESS,//获取了获取的不该获取到的表达式ID, bug
};

#endif //NKU_PRACTICE_PARSE_DEF_H
