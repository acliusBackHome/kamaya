#include "IR.hpp"
vector<size_t> IR::merge(vector<size_t> *p1, vector<size_t> *p2) {
  vector<size_t> ret;
  std::merge((*p1).begin(), (*p1).end(), (*p2).begin(), (*p2).end(), back_inserter(ret));
  return ret;
}
void IR::backpatch(vector<size_t> *p, size_t i) {
  cout << "backpatch" << endl;
  // 将 i 填入 p 指向四元式的所有待填域
  for(size_t i : *p) {
    get<3>(quas[i]) = to_string(i);
  }
}