#include "BaseBlock.hpp"
#include <set>
#include <string>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

BaseBlock::BaseBlock() {
    block_id = (size_t) -1;
};

BaseBlock::BaseBlock(const BaseBlock &other) : qua_list(other.qua_list) {
    block_id = other.block_id;
}

vector<BaseBlock> BaseBlock::get_base_blocks(const vector<Qua> &qua_list) {
    if (qua_list.empty()) {
        return vector<BaseBlock>();
    }
    set<size_t> tags; //标记基本块的标记
    tags.insert(0);
    for (size_t i = 0; i < qua_list.size(); ++i) {
        const string &op = get<0>(qua_list[i]);
        if (op == "jmp" || op == "j>" || op == "j<" || op == "j<=" ||
            op == "j>=" || op == "j==" || op == "j!=") {
            size_t jump_adr = strtoull(get<3>(qua_list[i]).c_str(), nullptr, 10),
                    next_instr = i + 1;
            if (tags.find(jump_adr) == tags.end()) {
                tags.insert(jump_adr);
            }
            if (next_instr < qua_list.size()) {
                if (tags.find(next_instr) == tags.end()) {
                    tags.insert(next_instr);
                }
            }
        }
    }
    size_t last_qua = qua_list.size();
    if (tags.find(last_qua) == tags.end()) {
        tags.insert(last_qua);
    }
//    for (const auto &each : tags) {
//        cout << each << endl;
//    }
    vector<BaseBlock> res;
    BaseBlock block_now;
    block_now.block_id = 0;
    auto iter = tags.begin();
    block_now.qua_list.emplace_back(qua_list[*iter]);
    iter++;
    for (size_t i = 1; i < qua_list.size(); ++i) {
        if(iter == tags.end()) {
            break;
        }
        if (i == *iter) {
            res.emplace_back(block_now);
            block_now.qua_list.clear();
            block_now.qua_list.emplace_back(qua_list[i]);
            block_now.block_id++;
            iter++;
        } else {
            block_now.qua_list.emplace_back(qua_list[i]);
        }
    }
    res.emplace_back(block_now);
    return res;
}

size_t BaseBlock::get_id() const {
    return block_id;
}

const vector<Qua> &BaseBlock::get_qua_list() const {
    return qua_list;
}

#pragma clang diagnostic pop