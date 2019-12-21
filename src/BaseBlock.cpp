#include "BaseBlock.hpp"
#include <set>
#include <string>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

BaseBlock::BaseBlock() {
    block_id = begin_index = end_index = (size_t) -1;
};

BaseBlock::BaseBlock(const BaseBlock &other) : qua_list(other.qua_list) {
    block_id = other.block_id;
    begin_index = other.begin_index;
    end_index = other.end_index;
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
    vector<BaseBlock> res;
    BaseBlock block_now;
    block_now.block_id = block_now.begin_index = 0;
    auto iter = tags.begin();
    block_now.qua_list.emplace_back(qua_list[*iter]);
    iter++;
    for (size_t i = 1; i < qua_list.size(); ++i) {
        if (iter == tags.end()) {
            break;
        }
        if (i == *iter) {
            block_now.end_index = i - 1;
            res.emplace_back(block_now);
            block_now.begin_index = i;
            block_now.qua_list.clear();
            block_now.qua_list.emplace_back(qua_list[i]);
            block_now.block_id++;
            iter++;
        } else {
            block_now.qua_list.emplace_back(qua_list[i]);
        }
    }
    block_now.end_index = *iter - 1;
    res.emplace_back(block_now);
    return res;
}

size_t BaseBlock::get_id() const {
    return block_id;
}

const vector<Qua> &BaseBlock::get_qua_list() const {
    return qua_list;
}

string BaseBlock::get_info() const {
    string res = "BaseBlock(id=" + to_string(block_id);
    res += ", begin=" + to_string(begin_index);
    res += ", end=" + to_string(end_index);
    res += ")\n[\n";
    for(const auto &each : qua_list) {
        res += "\t(";
        res += get<0>(each);
        res += ", " + get<1>(each);
        res += ", " + get<2>(each);
        res += ", " + get<3>(each);
        res += "),\n";
    }
    res += "]";
    return res;
}

#pragma clang diagnostic pop