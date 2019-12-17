#ifndef __IR_HPP__
#define __IR_HPP__

#include <tuple>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

typedef tuple<string, string, string, string> Qua;
// jnz, j, j<, j>, :=, read, write

class IR {
  size_t offset;
  size_t nextinstr;
  vector<Qua> quas;
 public:
  IR() {
    offset = nextinstr = 0;
  }
  size_t getNextinstr() {
    return nextinstr;
  }
  size_t getOffset() {
    return nextinstr;
  }
  inline void gen(string op, string arg1, string arg2, string result) {
    quas.push_back(make_tuple(op, arg1, arg2, result));
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
  vector<size_t> merge(vector<size_t> *p1, vector<size_t> *p2);
  void backpatch(vector<size_t> *p, size_t i);
};

#endif