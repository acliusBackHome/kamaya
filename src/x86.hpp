#ifndef __X86_HPP__
#define __X86_HPP__
#include "IR.hpp"
#include "ParseDeclaration.hpp"
#include <bits/stdc++.h>
namespace x86 {
using namespace std;
enum QuaType {
  Q_ADD,
  Q_SUB,
  Q_MUL,
  Q_DIV,
  Q_MOD,
  Q_POW,
  Q_LT,
  Q_LE,
  Q_GT,
  Q_GE,
  Q_JMP,
  Q_JLT,
  Q_JLE,
  Q_JGT,
  Q_JGE,
  Q_ASSIGN,
  Q_ALLOC,
};
class Assembler {
  // CodeHolder *code;
  ostream &of;
  static const string T, C, N, L;
  map<string, ParseVariable> sectionData;
  map<string, ParseFunction> sectionText;
  map<string, QuaType> quaMap;
  map<size_t, string> wideMap;
  map<QuaType, std::function<void(const Qua &)>> quaEmit;
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
  // Assembler(CodeHolder *code) {
  //   this.CodeHolder = code;
  // }
  Assembler(ostream &out) : of(out) {
    quaEmit[QuaType::Q_ADD] =
        std::bind(&Assembler::quaADD, this, std::placeholders::_1);
    quaEmit[QuaType::Q_SUB] =
        std::bind(&Assembler::quaSUB, this, std::placeholders::_1);
    quaEmit[QuaType::Q_MUL] =
        std::bind(&Assembler::quaMUL, this, std::placeholders::_1);
    quaEmit[QuaType::Q_DIV] =
        std::bind(&Assembler::quaDIV, this, std::placeholders::_1);
    quaEmit[QuaType::Q_MOD] =
        std::bind(&Assembler::quaMOD, this, std::placeholders::_1);
    quaEmit[QuaType::Q_POW] =
        std::bind(&Assembler::quaPOW, this, std::placeholders::_1);
    quaEmit[QuaType::Q_LT] =
        std::bind(&Assembler::quaLT, this, std::placeholders::_1);
    quaEmit[QuaType::Q_LE] =
        std::bind(&Assembler::quaLE, this, std::placeholders::_1);
    quaEmit[QuaType::Q_GT] =
        std::bind(&Assembler::quaGT, this, std::placeholders::_1);
    quaEmit[QuaType::Q_GE] =
        std::bind(&Assembler::quaGE, this, std::placeholders::_1);
    quaEmit[QuaType::Q_JMP] =
        std::bind(&Assembler::quaJMP, this, std::placeholders::_1);
    quaEmit[QuaType::Q_JLT] =
        std::bind(&Assembler::quaJLT, this, std::placeholders::_1);
    quaEmit[QuaType::Q_JLE] =
        std::bind(&Assembler::quaJLE, this, std::placeholders::_1);
    quaEmit[QuaType::Q_JGT] =
        std::bind(&Assembler::quaJGT, this, std::placeholders::_1);
    quaEmit[QuaType::Q_JGE] =
        std::bind(&Assembler::quaJGE, this, std::placeholders::_1);
    quaEmit[QuaType::Q_ASSIGN] =
        std::bind(&Assembler::quaASSIGN, this, std::placeholders::_1);
    quaEmit[QuaType::Q_ALLOC] =
        std::bind(&Assembler::quaALLOC, this, std::placeholders::_1);
    quaMap = map<string, QuaType>{
        {"+", QuaType::Q_ADD},      {"-", QuaType::Q_SUB},
        {"*", QuaType::Q_MUL},      {"/", QuaType::Q_DIV},
        {"%", QuaType::Q_MOD},      {"^", QuaType::Q_POW},
        {"<", QuaType::Q_LT},       {"<=", QuaType::Q_LE},
        {">", QuaType::Q_GT},       {">=", QuaType::Q_GE},
        {"jmp", QuaType::Q_JMP},    {"j<", QuaType::Q_JLT},
        {"j<=", QuaType::Q_JLE},    {"j>", QuaType::Q_JGT},
        {"j>=", QuaType::Q_JGE},    {":=", QuaType::Q_ASSIGN},
        {"alloc", QuaType::Q_ALLOC}};
    wideMap = map<size_t, string>{{1, "db"}, {2, "dw"}, {4, "dd"}, {8, "dq"}};
  }
  string getWideStr(size_t size);
  void handleNasm();
  void handleProgram();
  void handleData();
  void handleExpr(ParseExpression expr);
  void handleFunction(ParseFunction func);
  void handleQuas(const vector<Qua> &quas);
  void setSectionData(map<string, ParseVariable> data);
  void setSectionText(map<string, ParseFunction> text);
  void log(const string &str);
  void quaADD(const Qua &qua);
  void quaSUB(const Qua &qua);
  void quaMUL(const Qua &qua);
  void quaDIV(const Qua &qua);
  void quaPOW(const Qua &qua);
  void quaMOD(const Qua &qua);
  void quaASSIGN(const Qua &qua);
  void quaALLOC(const Qua &qua);
  void quaLT(const Qua &qua);
  void quaLE(const Qua &qua);
  void quaGT(const Qua &qua);
  void quaGE(const Qua &qua);
  void quaJMP(const Qua &qua);
  void quaJLT(const Qua &qua);
  void quaJLE(const Qua &qua);
  void quaJGT(const Qua &qua);
  void quaJGE(const Qua &qua);
};
} // namespace x86
#endif