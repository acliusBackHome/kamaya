#include "x86.hpp"
#define TAB of<<T
namespace x86 {
  const string Assembler::T = "\t";
  const string Assembler::N = "\n";
  const string Assembler::C = ",";
  const string Assembler::L = ":";
  string Assembler::getWideStr(size_t size) {
    /*
      db 1
      dw 2
      dd 4
      dq 8
      */
    switch(size) {
      case 1: return "db";
      case 2: return "dw";
      case 4: return "dd";
      case 8: return "dq";
    }
    cerr << "IN getWide: ERROR SIZE " << to_string(size) << N;
    exit(1);
  }
  void Assembler::handleData(){
    for (auto i : sectionData) {
      of << i.first << L << T << Assembler::getWideStr(i.second.get_type().get_size()) << T << "0" << N;
    }
  }
  void Assembler::setSectionData(map<string, ParseVariable> data) {
    sectionData = data;
  }
  void Assembler::setSectionText(map<string, ParseFunction> text) {
    sectionText = text;
  }
}
void x86::Assembler::handleNasm() {
  handleProgram();
  of << T << "section" << T << ".text" << N;
  // handleFunction();
  for (auto i : sectionText) {
    handleFunction(i.second);
  }
  of << T << "section" << T << ".data" << N;
  handleData();
}
void x86::Assembler::handleProgram() {
  of << T << "global main" << N;
}
void x86::Assembler::handleFunction(ParseFunction func) {
  of << func.get_symbol() << L << N;
  TAB; push("ebp");
  TAB; mov("ebp", "esp");

  int paramBytes = 0;
  for (auto i : func.get_args()) {
    paramBytes += i.get_type().get_size();
  }

  // 为参数提供栈帧
  TAB; sub("esp", to_string(paramBytes));

  // 装载参数，上下二选一
  for (auto i : func.get_args()) {
    // TODO
  }

  TAB; of << "leave" << N;
  TAB; of << "ret" << N;
}