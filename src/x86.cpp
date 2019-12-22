#include "x86.hpp"
#define TAB of << T
namespace x86 {
const string Assembler::T = "\t";
const string Assembler::N = "\n";
const string Assembler::C = ",";
const string Assembler::L = ":";
int Assembler::pow_count = 0;
string Assembler::getWideStr(size_t size) {
  if (wideMap.find(size) == wideMap.end()) {
    log("IN getWide: ERROR SIZE " + to_string(size));
    return "BadWide";
  }
  return wideMap[size];
}
void Assembler::handleData() {
  for (auto i : sectionData) {
    of << i.first << L << T
       << Assembler::getWideStr(i.second.get_type().get_size()) << T << "0"
       << N;
  }
}
void Assembler::setSectionData(map<string, ParseVariable> data) {
  sectionData = data;
}
void Assembler::setSectionText(map<string, ParseFunction> text) {
  sectionText = text;
}
void Assembler::handleNasm() {
  handleProgram();
  of << T << "section" << T << ".text" << N;
  // handleFunction();
  for (auto i : sectionText) {
    handleFunction(i.second);
  }
  of << T << "section" << T << ".data" << N;
  handleData();
}
void Assembler::handleProgram() { of << T << "global main" << N; }
void Assembler::handleFunction(ParseFunction func) {
  of << func.get_symbol() << L << N;
  TAB;
  push("ebp");
  TAB;
  mov("ebp", "esp");

  int paramBytes = 0;
  for (auto i : func.get_args()) {
    paramBytes += i.get_type().get_size();
  }

  // 为参数提供栈帧
  TAB;
  sub("esp", to_string(paramBytes));

  // 装载参数，上下二选一
  for (auto i : func.get_args()) {
    // TODO
  }

  TAB;
  of << "leave" << N;
  TAB;
  of << "ret" << N;
}

void Assembler::quaADD(const Qua &qua) {
  // 2	(+,[esp+8],1,[esp+13])
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  TAB;
  mov("eax", x);
  TAB;
  mov("ebx", y);
  TAB;
  add("eax", "ebx");
  TAB;
  mov(z, "eax");
}

void Assembler::quaSUB(const Qua &qua) {
  //
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  TAB;
  mov("eax", x);
  TAB;
  mov("ebx", y);
  TAB;
  sub("eax", "ebx");
  TAB;
  mov(z, "eax");
}

void Assembler::quaMUL(const Qua &qua) {
  //
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  TAB;
  mov("eax", x);
  TAB;
  mov("ebx", y);
  TAB;
  mul("ebx");
  TAB;
  mov(z, "eax");
}

void Assembler::quaDIV(const Qua &qua) {
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  TAB;
  mov("eax", x);
  TAB;
  mov("ebx", y);
  TAB;
  div("ebx");
  TAB;
  mov(z, "eax"); // eax存商 edx存余数
}

void Assembler::quaPOW(const Qua &qua) {
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  TAB;
  mov("eax", x);
  TAB;
  mov("ebx", "eax");
  TAB;
  mov("ecx", y);
  TAB;
  div("ebx");
  pow_label();
  TAB;
  mul("ebx");
  loop_label();
  mov(z, "eax");
}

void Assembler::quaMOD(const Qua &qua) {
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  TAB;
  mov("eax", x);
  TAB;
  mov("ebx", y);
  TAB;
  div("ebx");
  TAB;
  mov(z, "edx"); // eax存商 edx存余数
}

void Assembler::quaASSIGN(const Qua &qua) {
  // (:=,0,_,[esp+0])
  const string &dist = get<3>(qua);
  const string &src = get<1>(qua);
  TAB;
  mov(dist, src);
}

void Assembler::quaALLOC(const Qua &qua) {
  static int stackTop = 0;
  // (alloc,1648,4,_2@T2)
  string sz = get<1>(qua);
  stackTop = max(stackTop, stoi(sz));
  mov("esp", to_string(stackTop));
}

void Assembler::quaLT(const Qua &qua) {}

void Assembler::quaLE(const Qua &qua) {}

void Assembler::quaGT(const Qua &qua) {}

void Assembler::quaGE(const Qua &qua) {}

void Assembler::quaJMP(const Qua &qua) {
  const string &jto = get<3>(qua);
  // 获取下一个块的块名  通过行号获取块id-->生成块名
  string blockName = getBaseBlockName(jto);

  TAB;
  jmp(blockName);
}

void Assembler::quaJLT(const Qua &qua) {
  // (j<,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  // 获取下一个块的块名  通过行号获取块id-->生成块名
  string blockName = getBaseBlockName(jto);

  TAB;
  cmp(x, y);
  TAB;
  jlt(blockName);
}

void Assembler::quaJLE(const Qua &qua) {
  // (j<=,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  // 获取下一个块的块名  通过行号获取块id-->生成块名
  string blockName = getBaseBlockName(jto);

  TAB;
  cmp(x, y);
  TAB;
  jle(blockName);
}

void Assembler::quaJGT(const Qua &qua) {
  // (j>,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);

  string blockName = getBaseBlockName(jto);

  TAB;
  cmp(x, y);
  TAB;
  jgt(blockName);
}

void Assembler::quaJGE(const Qua &qua) {
  // (j>=,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);

  string blockName = getBaseBlockName(jto);

  TAB;
  cmp(x, y);
  TAB;
  jge(blockName);
}

void Assembler::quaJE(const Qua &qua)  {
  //(j==,[ebp+20],10,25)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);

  string blockName = getBaseBlockName(jto);

  TAB;
  cmp(x, y);
  TAB;
  je(blockName);
}

void Assembler::quaJNE(const Qua &qua) {
  //(j!=,[ebp+20],10,25)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);

  string blockName = getBaseBlockName(jto);

  TAB;
  cmp(x, y);
  TAB;
  jne(blockName);
}

void Assembler::quaDATA(const Qua &qua) {}
void Assembler::quaCALL(const Qua &qua) {}
void Assembler::quaRET(const Qua &qua) {}
void Assembler::quaFUNC(const Qua &qua) {}

void Assembler::getBaseBlockMap() { bbla = BaseBlock::get_base_blocks(quas); }

size_t Assembler::getBaseBlockID(string linenum) {
  size_t line = stoi(linenum);
  map<size_t, size_t> lineNumToBlockID = get<1>(bbla);
  return lineNumToBlockID[line];
}

string Assembler::getBaseBlockName(string linenum) {
  size_t blockid = getBaseBlockID(linenum);
  const string &prefix = "block";
  string blockName = prefix + to_string(blockid);
  return blockName;
}

void Assembler::handleQuas(const vector<Qua> &quas) {
  const string& beginning = "global " + getBaseBlockName("0");
  const string& text_label = "[section .text]";
  const string& data_label = "[section .data]";
  // 打印开头 此时of为cout，且bbla(BaseBlockListAndMap)已经赋值，可以直接调用接口打印
  TAB;
  of << beginning << N;
  // 标记指令区
  of << text_label << N;
  // 打印汇编指令
  for (auto qua : quas) {
    string sign = get<0>(qua);
    if (quaMap.find(sign) == quaMap.end()) {
      log("cannot find this sign in quaMap" + sign);
      continue;
    }
    QuaType quaType = quaMap[sign];
    if (quaEmit.find(quaType) == quaEmit.end()) {
      log("cannot find this QuaType in quaMap");
      continue;
    }
    quaEmit[quaType](qua);
  }
}

void Assembler::log(const string &str) {
  cout << "Assembler log: " << str << "\n";
}

} // namespace x86
