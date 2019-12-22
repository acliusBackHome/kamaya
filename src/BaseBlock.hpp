#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_BASE_BLOCK_HPP
#define NKU_PRACTICE_BASE_BLOCK_HPP

#include "IR.hpp"
#include <tuple>

class BaseBlock;

// 表示基本块列表和其对应的行号到基本快的id的映射
typedef tuple<vector<BaseBlock>, map<size_t, size_t> > BaseBlockListAndMap;

/**
 * 表示基本块的数据结构
 */
class BaseBlock {
public:

    BaseBlock();

    BaseBlock(const BaseBlock &other);

    size_t get_id() const;

    size_t get_begin_index() const;

    /**
     * 获取本基本块的四元式列表
     * @return
     */
    const vector<Qua> &get_qua_list() const;

    /**
     * 获得基本块的出口
     * @return
     */
    const vector<size_t> &get_jump_to() const;

    /**
     * 获取该基本块的信息
     * @return
     */
    string get_info() const;

    /**
     * 根据四元式列表得到基本块列表
     * @param qua_list
     * @return
     */
    static BaseBlockListAndMap get_base_blocks(const vector<Qua>& qua_list);

private:
    size_t block_id;
    // 表示基本块中四元式列表
    vector<Qua> qua_list;
    // 表示基本块出口
    vector<size_t> jump_to;
    // 分别是该块第一个四元式和最后一个四元式的标号
    size_t begin_index, end_index;
};


#endif //NKU_PRACTICE_BASE_BLOCK_HPP

#pragma clang diagnostic pop