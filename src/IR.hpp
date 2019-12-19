#ifndef __IR_HPP__
#define __IR_HPP__

#include <tuple>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stack>

class ParseTree;

extern bool generating_code;

void set_node_begen_code(size_t node_id, size_t code_id);

#define IR_EMIT if(generating_code)

using namespace std;

typedef tuple<string, string, string, string> Qua;
// jnz, j, j<, j>, :=, read, write

class ParseTree;
class ParseType;
class ParseExpression;

class IR {
  size_t offset;
  size_t nextinstr;
  vector<Qua> quas;
  stack<size_t> env;
  stack<size_t> stk;
 public:
  IR() {
    offset = nextinstr = 0;
  }
  inline size_t getOffset() {
    return offset;
  }
  inline void setOffset(size_t next) {
    offset = next;
  }
  inline void addOffset(size_t add) {
    offset += add;
  }
  inline size_t getNextinstr() {
    return nextinstr;
  }
  inline void envpush(size_t pos) {
    env.push(pos);
  }
  inline void stkpush(size_t pos) {
    stk.push(pos);
  }
  inline size_t envpop() {
    size_t ret = env.top(); env.pop();
    return ret;
  }
  inline size_t stkpop() {
    size_t ret = stk.top(); stk.pop();
    return ret;
  }
  inline void gen(const string &op, const string &arg1,
                  const string &arg2, const string &result, size_t node_id) {
    quas.push_back(make_tuple(op, arg1, arg2, result));
    if(node_id) {
      // 设置该节点的第一条指令编号
      set_node_begen_code(node_id, nextinstr);
    }
    nextinstr++;
  }
  inline void label(size_t *addr) {
    *addr = nextinstr;
  }
  inline vector<size_t> makelist(size_t i) {
    return vector<size_t>(1, i);
  }
  inline vector<size_t> makelist() {
    return vector<size_t>();
  }
  void print();
  vector<size_t> merge(const vector<size_t> &p1, const vector<size_t> &p2);
  void backpatch(const vector<size_t> &p, size_t i);
  void recordBegin();
  void recordEnd();
  void exprEmit(const ParseExpression &init_expr, const ParseType &this_type, const string &symbol, ParseTree &tree, size_t node_id);
  void relopEmit(ParseTree &tree, size_t p0, size_t p1, size_t p3, const string &relop, size_t node_id);
};

#endif