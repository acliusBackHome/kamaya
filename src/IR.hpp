#ifndef __IR_HPP__
#define __IR_HPP__

#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
#include <tuple>
#include <vector>

class ParseTree;

extern bool generating_code;

void set_node_begen_code(size_t node_id, size_t code_id);

#define IR_EMIT if (generating_code)

using namespace std;

typedef tuple<string, string, string, string> Qua;
// jnz, j, j<, j>, :=, read, write

class ParseTree;
class ParseType;
class ParseExpression;

class IR {
  struct Alloc {
    size_t begin;
    size_t width;
  };
  struct SData {
    string name;
    string type;
    string value;
  };
  size_t offset;
  size_t nextinstr;
  vector<Qua> quas;
  vector<SData> sectionData;
  stack<size_t> env;
  stack<size_t> stk;
  map<string, Alloc> allocMap;  // 运行时 alloc
 public:
  IR() {
    offset = nextinstr = 0;
    recordBegin();  // 必须初始化符号栈
  }
  inline string formKey(const size_t &scope, const string &symbol) {
    return "_" + to_string(scope) + "@" + symbol;
  }
  inline size_t getOffset() { return offset; }
  inline void setOffset(size_t next) { offset = next; }
  inline void addOffset(size_t add) { offset += add; }
  inline size_t getNextinstr() { return nextinstr; }
  inline void envpush(size_t pos) { env.push(pos); }
  inline void stkpush(size_t pos) { stk.push(pos); }
  inline size_t envpop() {
    size_t ret = env.top();
    env.pop();
    return ret;
  }
  inline size_t stkpop() {
    size_t ret = stk.top();
    stk.pop();
    return ret;
  }
  inline string newTemp() {
    static int idx = 0;
    return "L" + to_string(idx++);
  }
  inline void label(size_t *addr) { *addr = nextinstr; }
  inline vector<size_t> makelist(size_t i) { return vector<size_t>(1, i); }
  inline vector<size_t> makelist() { return vector<size_t>(); }
  inline void log(const string &msg) { cout << "IR log: " << msg << endl; }
  inline void recordBegin() {
    stkpush(offset);
    offset = 0;
  }
  inline void recordEnd() { offset = stkpop(); }
  inline void assignEmit(size_t left, size_t right) {}

  void print();
  string size2type(size_t size);
  vector<size_t> merge(const vector<size_t> &p1, const vector<size_t> &p2);
  void backpatch(const vector<size_t> &p, size_t i);
  void gen(const string &op, const string &arg1, const string &arg2,
           const string &result, size_t node_id);
  void relopEmit(ParseTree &tree, size_t p0, size_t p1, size_t p3,
                 const string &relop, size_t node_id);
  void exprEmit(const ParseExpression &init_expr, const ParseType &this_type,
                const string &symbol, ParseTree &tree, size_t node_id,
                size_t scope_id);
  size_t allocEmit(const size_t &scope, const string &symbol, const size_t &size,
                 const size_t &node_id);
  void dataEmit(const string &name, size_t size, const string &value, size_t node_id);
  void dataUndefinedEmit(const string &name, size_t size, size_t node_id);
  void varDecEmit(const string &symbol, const ParseExpression &init_expr, size_t node_id, size_t scope_id);
};

#endif