#include "IR.hpp"

void IR::print() {
  cout << "quas" << endl;
  for (int i = 0; i < quas.size(); i++) {
    cout << to_string(i) << "\t(" << get<0>(quas[i]) << "," <<
                                          get<1>(quas[i]) << "," <<
                                          get<2>(quas[i]) << "," <<
                                          get<3>(quas[i]) << ")" << endl;
  }
}

vector<size_t> IR::merge(const vector<size_t> &p1, const vector<size_t> &p2) {
  vector<size_t> ret;
  std::merge(p1.begin(), p1.end(), p2.begin(), p2.end(), back_inserter(ret));
  return ret;
}

void IR::backpatch(const vector<size_t> &p, size_t pp) {
  // 将 i 填入 p 指向四元式的所有待填域
  for(size_t i : p) {
    get<3>(quas[i]) = to_string(pp);
  }
}

void IR::recordBegin() {
  stkpush(offset);
  offset = 0;
}

void IR::recordEnd() {
  offset = stkpop();
}