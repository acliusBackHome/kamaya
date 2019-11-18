#ifndef NKU_PRACTICE_PARSE_DEF_H
#define NKU_PRACTICE_PARSE_DEF_H

using namespace std;

enum NodeType {
    // 平凡节点, 什么都没有
            NORMAL = 0,
    // 标识符:
    //symbol: 变量名
            IDENTIFIER = 1,
};

enum NodeKey {
    // 符号, string*
            SYMBOL = 0,

};

#endif //NKU_PRACTICE_PARSE_DEF_H
