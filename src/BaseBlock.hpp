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

    const vector<Qua> &get_qua_list() const;

    static vector<BaseBlock> get_base_blocks(const vector<Qua>& qua_list);

private:
    size_t block_id;
    vector<Qua> qua_list;
};


#endif //NKU_PRACTICE_BASE_BLOCK_HPP

#pragma clang diagnostic pop