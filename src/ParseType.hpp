#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef NKU_PRACTICE_PARSE_TREE_TYPE_HPP
#define NKU_PRACTICE_PARSE_TREE_TYPE_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include "ParseDef.hpp"

using namespace std;

// 用于表示运行时类型的类
class ParseType {
    friend class ParseTree;

public:

    /**
     * 通过基本类型构造Type
     * @param b_type 基本类型
     * @param spe 修饰符
     */
    explicit ParseType(BaseType b_type, int spe = 0);

    /**
     * 从常量中获取类型信息
     * @param constant
     */
    explicit ParseType(const ParseConstant &constant);

    /**
     * 复制构造其他类型的对象
     * @param other
     */
    ParseType(const ParseType &other);

    /**
     * 获得该类型对象的信息,多少级指针就在后面加几个*
     * @return
     */
    string get_info() const;

    /**
    * 获取该类型的大小
    * @return
    */
    size_t get_size() const;

    /**
     * 获取数组大小, 如果不是数组,该值为0
     * @return
     */
    size_t get_array_size() const;

    /**
     * 获取类型的指针级数, 如果不是指针, 则返回0
     * @return
     */
    size_t get_ptr_lv() const;

    /**
     * 获取结构体中字段名对应的类型,如果不存在或者不是结构体
     * 返回无效类型
     * @param key
     * @return
     */
    ParseType field(const string &key) const;

    /**
     * 操作符=, 封装assign函数
     * @param other
     * @return
     */
    ParseType &operator=(const ParseType &other);

    /**
     * 操作符< 用于map的Type键
     * @param other
     * @return
     */
    bool operator<(const ParseType &other) const;

    /**
     * 获取本类型的id
     * @return
     */
    size_t get_id() const;

    /**
     * 获取基础类型的标识符
     * @return
     */
    size_t get_specifier() const;

    /**
     * 获取指针或者数组类型的上一级类型
     * @return
     */
    const ParseType get_lower_type() const;

    ~ParseType();

    /**
     * 获取某个类型的多级指针, 如果type也是个指针,那么就返回一个原type的
     * 指针等级加上ptr_level的Type
     * @param type
     * @param ptr_level 几级指针
     * @return
     */
    static ParseType get_pointer(const ParseType &type, size_t ptr_level = 1);

    /**
     * 获取一个类型的数组
     * @param type
     * @param array_size
     * @return
     */
    static ParseType get_array(const ParseType &type, size_t array_size);

    /**
     * 基础类型的名字
     * @param base_type
     * @return
     */
    static string get_base_type_name(BaseType base_type, int specifier);

    /**
     * 将其他类型组合起来成为一个新类型, 即struct构造,
     * 要求名字不能重复,
     * @param otherTypes <string 字段名, size_t> 的列表 其他类型的字段名和类型对象id
     * @param self_ptr <string, size_t 指针级数> 的列表 本类型指针字段名和指针级数,指针级数>0
     * @return 如果构造不成功,返回一个type_id为T_UNKNOWN的Type
     */
    static ParseType get_struct(const vector<pair<string, size_t> > &otherTypes,
                                const vector<pair<string, size_t>> &self_ptr =
                                vector<pair<string, size_t> >());

    /**
     * 输出所有类型记录
     */
    static void print_all_type();

    /**
     * 根据id获取已经存在的类型
     * @param type_id
     * @return
     */
    static const ParseType &get_type(size_t type_id);

    /**
     * 获得两个类型进行指定运算之后的返回值类型id
     * 如果是单目运算, type2的id一定要是0,否则无法找到
     * 如果无法进行自动类型转换, 抛出异常
     * @param op_type
     * @param type1
     * @param type2
     * @return
     */
    static size_t convert(ExpressionType op_type,
                          const ParseType &type1, const ParseType &type2);

    /**
     * 试图自动转换类型from成to
     * 返回是否成功, 实际上只会返回true
     * 因为转换失败会抛出异常
     * @param from
     * @param to
     * @return
     */
    static bool convert(const ParseType &from, const ParseType &to);

    /**
     * 比较两个typeid所对应的type的表示范围大小
     * 返回 t1 >= t2
     * @param t1_id
     * @param t2_id
     * @return
     */
    static bool cmp_wider_type(size_t t1_id, size_t t2_id);

private:
    // 如果是基础类型, 则是非T_UNKNOWN, 否则是T_UNKNOWN, 该字段无效
    BaseType base_type{};
    // 当BaseType不为T_UNKNOWN或者lower_type小于等于T_BASE时有效, 修饰符
    // 如unsigned long long 是 BaseType, lower_type = 1(unsigned 标识特征) | 2(long 标识特征) = 3
    int specifier;
    // 下一级的类型, 如果是基础类型则是0, 主要用来表示指针和数组的直接作用的类型
    size_t lower_type{};
    // 该类型所包含的其他类型(结构体或者类),字段名到Type的id映射,
    // 如果是基础类型或者指针或者数组, 则该项为nullptr
    map<string, size_t> *fields{};

    size_t type_id{},//类型的id, 唯一标识符
            type_size{}, // 该类型的所占字节数,
            pointer_level{},// 该类型的指针等级, 不是指针就是0
            array_size{};// 该类型的数组大小, 如果不是数组就是0

    /**
     * 该构造函数构造的对象是无效的
     */
    ParseType();

    // 类型字典, 用于记录现有所有类型
    // 将类型映射到类型对应的id, 防止结构体自指针指向自己导致无限循环
    static map<ParseType, size_t> type2id;
    // 上面的反映射
    static vector<ParseType> id2type;
    // 为了减少字符串运算, 增加一点冗余信息, 记录每个节点的get_info结果
    // 要求不能修改已经有id的类型的信息, 否则会造成信息不一致的情况
    static vector<string> id2info;

    // 进行类型自动转换的时候的记录表
    // 有缓存机制: 在进行搜索时会将有效的运算符转化记录插入
    // 下次可以直接查询到
    // 映射对象size_t可以是一个指针, 在成熟后可以用来表示一个
    // 编译器内部的函数(这个函数就是运算符函数)id
    // 这个函数的运算符和参数就是OpAble记录
    // 在此, 因为时间不足原因, 先只记录返回值的id
    static map<OpAble, size_t> op_able;
    // 在DFS中记录正在搜索的OpAble, 防止搜索死循环
    static set<OpAble> op_trying;

    /**
     * dfs搜索是否某个OpAble记录能够成功应用, 传进来的参数不一定能够成功
     * 是尝试, 所以是try, 如果返回非(size_t)-1,说明尝试成功, 返回值就是OpAble对应的返回值类型id
     * 如果返回(size_t)-1说明尝试失败, 标记传入参数的记录为不可转化
     * @param op_try
     * @return
     */
    static size_t dfs_convert(const OpAble &op_try);

    /**
     * 实现方便的构造函数,只是简单的赋值
     * @param bt
     * @param low_t
     * @param fs
     * @param ts
     * @param ptr_lv
     * @param arr_size
     */
    ParseType(BaseType bt, size_t low_t, map<string, size_t> *fs, size_t ts, size_t ptr_lv, size_t arr_size,
        int spe = 0);

    /**
     * 获取类型id, 如果没有该类型id的记录, 那么则加入该记录并返回其id
     * @param type
     * @return
     */
    static size_t get_type_id(const ParseType &type);

    /**
     * 初始化
     */
    static void init();

    /**
     * from_type的信息全部复制给type
     * @param type
     * @param from_type
     */
    static void assign(ParseType &type, const ParseType &from_type);

};

#endif //NKU_PRACTICE_PARSE_TREE_TYPE_HPP

#pragma clang diagnostic pop