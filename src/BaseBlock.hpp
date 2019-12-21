#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_BASE_BLOCK_HPP
#define NKU_PRACTICE_BASE_BLOCK_HPP

#include "IR.hpp"

/**
 * 表示基本块的数据结构
 */
class BaseBlock {
public:
    BaseBlock();

    BaseBlock(const BaseBlock &other);

    size_t get_id() const;

    /**
     * 获取本基本块的四元式列表
     * @return
     */
    const vector<Qua> &get_qua_list() const;

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
    static vector<BaseBlock> get_base_blocks(const vector<Qua>& qua_list);

private:
    size_t block_id;
    vector<Qua> qua_list;
    // 分别是该块第一个四元式和最后一个四元式的标号
    size_t begin_index, end_index;
};


#endif //NKU_PRACTICE_BASE_BLOCK_HPP

#pragma clang diagnostic pop