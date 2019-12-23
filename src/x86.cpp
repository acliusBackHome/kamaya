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
  mov("eax", "1");
  TAB;
  mov("ebx", x);
  TAB;
  mov("ecx", y);
  TAB;
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
  if (dist == "eax" || src == "eax") {
    TAB; mov(dist, src);
  } else {
    TAB; mov("eax", src);
    TAB; mov(dist, "eax");
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
    TAB; sub("esp",sz);
  }

  // TAB; mov("eax", "ebp");
  // TAB; sub("eax", to_string(stackTop));
  // TAB; mov("esp", "eax");
}

void Assembler::quaLT(const Qua &qua) {}

void Assembler::quaLE(const Qua &qua) {}

void Assembler::quaGT(const Qua &qua) {}

void Assembler::quaGE(const Qua &qua) {}

void Assembler::quaEQ(const Qua &qua) {}

void Assembler::quaNE(const Qua &qua) {}

void Assembler::quaJMP(const Qua &qua) {
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);

  TAB;
  jmp(blockName);
}

void Assembler::quaJLT(const Qua &qua) {
  // (j<,[esp+8],10,19)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);

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
  string blockName = getBlockName(jto);

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
  string blockName = getBlockName(jto);

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
  string blockName = getBlockName(jto);

  TAB;
  cmp(x, y);
  TAB;
  jge(blockName);
}

void Assembler::quaJE(const Qua &qua) {
  //(j==,[ebp+20],10,25)
  const string &x = get<1>(qua);
  const string &y = get<2>(qua);
  const string &jto = get<3>(qua);
  string blockName = getBlockName(jto);

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
  string blockName = getBlockName(jto);

  TAB;
  cmp(x, y);
  TAB;
  jne(blockName);
}

void Assembler::quaDATA(const Qua &qua) {
}
void Assembler::quaCALL(const Qua &qua) {
  const string &func = get<3>(qua);
  of << T << "call" << T << func << N;
}
void Assembler::quaRET(const Qua &qua) {
  const string &rv = get<1>(qua);
  if (rv != "_") {
    TAB;
    mov("eax", rv);
  }
  TAB;
  pop("ebp");
  // TAB; of << "leave" << N;
  TAB;
  of << "ret" << N;
}
void Assembler::quaFUNC(const Qua &qua) {
  const string &l = get<3>(qua);
  label(l);
  TAB;
  push("ebp");
  TAB;
  mov("ebp", "esp");
}

void Assembler::quaREAD(const Qua &qua) {
  const string &dist = get<3>(qua);
  TAB; of << "read" << endl;
  TAB; mov(dist, "eax");
}
void Assembler::quaPRINT(const Qua &qua) {
  const string &src = get<3>(qua);
  TAB; of << "push dword" << T << src << N;
  TAB; of << "print \"%d\"" << N;
}
void Assembler::quaEXIT(const Qua &qua) {
  TAB;
  mov("eax", "1");
  TAB;
  mov("ebx", "0");
  TAB;
  of << "int 0x80" << N;
}

void Assembler::getBaseBlockMap() { bbla = BaseBlock::get_base_blocks(quas); }

void Assembler::handleQuas() {
  const string beginning = "global _start";
  const string block_prefix = "block";
  const string text_label = "[section .text]";
  const string data_label = "[section .data]";
  // 打印开头
  // 此时of为cout，且bbla(BaseBlockListAndMap)已经赋值，可以直接调用接口打印
  of << beginning << N;
  // 标记指令区
  of << text_label << N;
  of << "_start:\n"\
        "call main\n"\
        "mov ebx, eax\n"\
        "mov eax, 1\n"\
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
    // TODO: 抛出异常
  }
  return blockName;
}

void Assembler::log(const string &str) {
  of << "Assembler log: " << str << "\n";
}

} // namespace x86
