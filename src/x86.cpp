#include "x86.hpp"
#define TAB of << T
namespace x86 {
const string Assembler::T = "\t";
const string Assembler::N = "\n";
const string Assembler::C = ",";
const string Assembler::L = ":";

string Assembler::getWideStr(size_t size) {
  if (wideMap.find(size) == wideMap.end()) {
    log("IN getWide: ERROR SIZE " + to_string(size));
    return "BadWide";
  }
  return wideMap[size];
}

void Assembler::quaADD(const Qua &qua) {
  // 2	(+,[esp+8],1,[esp+13])
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  mov("eax", x);
  mov("ebx", y);
  add("eax", "ebx");
  mov(z, "eax");
}

void Assembler::quaSUB(const Qua &qua) {
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  mov("eax", x);
  mov("ebx", y);
  sub("eax", "ebx");
  mov(z, "eax");
}

void Assembler::quaMUL(const Qua &qua) {
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  mov("eax", x);
  mov("ebx", y);
  mul("ebx");
  mov(z, "eax");
}

void Assembler::quaDIV(const Qua &qua) {
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  mov("eax", x);
  mov("ebx", y);
  div("ebx");
  mov(z, "eax"); // eax存商 edx存余数
}

void Assembler::quaPOW(const Qua &qua) {
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  mov("eax", "1");
  mov("ebx", x);
  mov("ecx", y);
  pow_label();
  mul("ebx");
  loop_label();
  mov(z, "eax");
}

void Assembler::quaMOD(const Qua &qua) {
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &z = get<3>(qua);
  mov("eax", x);
  mov("ebx", y);
  div("ebx");
  mov(z, "edx"); // eax存商 edx存余数
}

void Assembler::quaASSIGN(const Qua &qua) {
  // (:=,0,_,[esp+0])
  const string &dist = get<3>(qua);
  const string &src = get<1>(qua);
  if (dist == "eax" || src == "eax") {
    mov(dist, src);
  } else {
    mov("eax", src);
    mov(dist, "eax");
  }
}

void Assembler::quaALLOC(const Qua &qua) {
  static int stackTop = 0;
  // (alloc,1648,4,_2@T2)
  string top = get<1>(qua);
  string sz = get<2>(qua);
  stackTop = max(stackTop, stoi(top));
  if (stackTop < stoi(top)) {
    stackTop = stoi(top);
    sub("esp", sz);
  }
}

/* TODO: 未使用 */
void Assembler::quaLT(const Qua &qua) { log("undefined instruction"); }
void Assembler::quaLE(const Qua &qua) { log("undefined instruction"); }
void Assembler::quaGT(const Qua &qua) { log("undefined instruction"); }
void Assembler::quaGE(const Qua &qua) { log("undefined instruction"); }
void Assembler::quaEQ(const Qua &qua) { log("undefined instruction"); }
void Assembler::quaNE(const Qua &qua) { log("undefined instruction"); }
void Assembler::quaLOGICNOT(const Qua &qua) { log("undefined instruction"); }
/* 未使用 */

void Assembler::quaLOGICAND(const Qua &qua) {
  // (||,[ebp+28],[ebp+29],[ebp+30])
  const string &a = get<1>(qua);
  const string &b = get<2>(qua);
  const string &c = get<3>(qua);
  mov("eax", a);
  mov("ebx", b);
  _and("eax", "ebx");
  mov(c, "eax");
}

void Assembler::quaLOGICOR(const Qua &qua) {
  // (||,[ebp+28],[ebp+29],[ebp+30])
  const string &a = get<1>(qua);
  const string &b = get<2>(qua);
  const string &c = get<3>(qua);
  mov("eax", a);
  mov("ebx", b);
  _or("eax", "ebx");
  mov(c, "eax");
}

void Assembler::quaJMP(const Qua &qua) {
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);
  jmp(blockName);
}

void Assembler::quaJLT(const Qua &qua) {
  // (j<,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);
  cmp(x, y);
  jlt(blockName);
}

void Assembler::quaJLE(const Qua &qua) {
  // (j<=,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);
  cmp(x, y);
  jle(blockName);
}

void Assembler::quaJGT(const Qua &qua) {
  // (j>,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);
  cmp(x, y);
  jgt(blockName);
}

void Assembler::quaJGE(const Qua &qua) {
  // (j>=,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);
  cmp(x, y);
  jge(blockName);
}

void Assembler::quaJE(const Qua &qua) {
  //(j==,[ebp+20],10,25)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);
  cmp(x, y);
  je(blockName);
}

void Assembler::quaJNE(const Qua &qua) {
  //(j!=,[ebp+20],10,25)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);
  cmp(x, y);
  jne(blockName);
}

void Assembler::quaDATA(const Qua &qua) {
  // 0	(.data,dd,0,a)
  // 1	(.data,dd,0,b)
  const string &symbol = get<3>(qua);
  const string &init = get<2>(qua);
  const string &wide = get<1>(qua);
  sectionData.push_back(DataVar{.symbol = symbol, .init = init, .wide = wide});
}

void Assembler::quaCALL(const Qua &qua) {
  const string &func = get<3>(qua);
  of << T << "call" << T << func << N;
}

void Assembler::quaRET(const Qua &qua) {
  const string &rv = get<1>(qua);
  if (rv != "_") {
    mov("eax", rv);
  }
  pop("ebp");
  //  of << "leave" << N;
  of << "ret" << N;
}

void Assembler::quaFUNC(const Qua &qua) {
  const string &l = get<3>(qua);
  label(l);
  push("ebp");
  mov("ebp", "esp");
}

void Assembler::quaREAD(const Qua &qua) {
  const string &dist = get<3>(qua);
  of << "read" << endl;
  mov(dist, "eax");
}

void Assembler::quaPRINT(const Qua &qua) {
  const string &src = get<3>(qua);
  of << "push dword" << T << src << N;
  of << "print \"%d\"" << N;
}

void Assembler::quaEXIT(const Qua &qua) {
  mov("eax", "1");
  mov("ebx", "0");
  of << "int 0x80" << N;
}

void Assembler::getBaseBlockMap() { bbla = BaseBlock::get_base_blocks(quas); }

void Assembler::handleQuas() {
  const string beginning = "global _start";
  const string block_prefix = "block";
  const string text_label = "[section .text]";
  const string data_label = "[section .data]";
  // 若 bbla(BaseBlockListAndMap)已经赋值，可以直接调用接口打印
  // 否则指令为空
  of << beginning << N;
  of << text_label << N;
  of << "_start:\n"
        "call main\n"
        "mov ebx, eax\n"
        "mov eax, 1\n"
        "int 0x80\n";
  // 打印汇编指令
  map<size_t, size_t> &lineNumToBlockID = get<1>(bbla);
  vector<BaseBlock> &baseblock = get<0>(bbla);
  for (size_t i = 0; i < quas.size(); ++i) {
    auto &qua = quas[i];
    size_t blockid = lineNumToBlockID[i];
    size_t begin_index = baseblock[blockid].get_begin_index();
    if (i == begin_index) {
      of << block_prefix << blockid << ":" << N;
    }
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
  of << data_label << N;
  for (int i = 0; i < sectionData.size(); i++) {
    of << sectionData[i].symbol << ":" << T << sectionData[i].wide << T
       << sectionData[i].init << N;
  }
}

string Assembler::getBlockName(string jto) {
  string blockName;
  const string &block = "block";
  size_t linenum = stoi(jto);
  map<size_t, size_t> &lineToBlockID = get<1>(bbla);
  map<size_t, size_t>::iterator iter = lineToBlockID.find(linenum);
  if (iter != lineToBlockID.end()) {
    blockName = block + to_string(iter->second);
  } else {
    log("cannot find this linenum");
  }
  return blockName;
}

void Assembler::log(const string &str) {
  of << "Assembler log: " << str << "\n";
}
} // namespace x86
