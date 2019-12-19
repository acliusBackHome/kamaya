#ifndef __X86_HPP__
#define __X86_HPP__
#include "ParseDeclaration.hpp"
#include <bits/stdc++.h>
namespace x86 {
using namespace std;
class Assembler {
  // CodeHolder *code;
  ostream &of;
  static const string T, C, N, L;
  map<string, ParseVariable> sectionData;
  map<string, ParseFunction> sectionText;
  // Assembler(CodeHolder *code) {
  //   this.CodeHolder = code;
  // }
  inline static string getReg() { return "TODO"; }
  inline static string r64(int idx) { return string("r") + to_string(idx); }
  inline static string r32(int idx) {
    return string("r") + to_string(idx) + "d";
  }
  inline static string r16(int idx) {
    return string("r") + to_string(idx) + "w";
  }
  inline static string r8(int idx) {
    return string("r") + to_string(idx) + "b";
  }
  inline static string xmm128(int idx) {
    return string("xmm") + to_string(idx);
  }
  /*
    mov  x, y  x ← y
    and  x, y  x ← x and y
    or   x, y  x ← x or y
    xor  x, y  x ← x xor y
    add  x, y  x ← x + y
    sub  x, y  x ← x – y
    inc  x     x ← x + 1
    dec  x     x ← x – 1
   */
  inline void mov(string dist, string src) {
    static string movins = "mov";
    of << movins << T << dist << C << src << N;
  }
  inline void add(string dist, string src) {
    static string addins = "add";
    of << addins << T << dist << C << src << N;
  }
  inline void _or(string dist, string src) {
    static string orins = "or";
    of << orins << T << dist << C << src << N;
  }
  inline void _xor(string dist, string src) {
    static string xorins = "xor";
    of << xorins << T << dist << C << src << N;
  }
  inline void _and(string dist, string src) {
    static string andins = "and";
    of << andins << T << dist << C << src << N;
  }
  inline void sub(string dist, string src) {
    static string subins = "sub";
    of << subins << T << dist << C << src << N;
  }
  inline void inc(string dist) {
    static string incins = "xor";
    of << incins << T << dist << N;
  }
  inline void dec(string dist) {
    static string decins = "dec";
    of << decins << T << dist << N;
  }
  inline void pop(string reg) {
    static string popinc = "pop";
    of << popinc << T << reg << N;
  }
  inline void push(string reg) {
    static string pushinc = "push";
    of << pushinc << T << reg << N;
  }

public:
  Assembler(ostream &out) : of(out) {}
  void handleNasm();
  void handleProgram();
  void handleData();
  void handleExpr(ParseExpression expr);
  void handleFunction(ParseFunction func);
  string getWideStr(size_t size);
  void setSectionData(map<string, ParseVariable> data);
  void setSectionText(map<string, ParseFunction> text);
};
} // namespace x86
#endif