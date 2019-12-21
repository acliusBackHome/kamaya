#include "x86.hpp"
#define TAB of << T
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
  switch (size) {
  case 1:
    return "db";
  case 2:
    return "dw";
  case 4:
    return "dd";
  case 8:
    return "dq";
  }
  cerr << "IN getWide: ERROR SIZE " << to_string(size) << N;
  exit(1);
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
  // 2	(:=,[esp+0],_,[esp+4])
  const string &x = get<1>(qua);
  const string &y = get<3>(qua);
  TAB;
  mov("eax", x);
  TAB;
  mov("ebx", y);
  TAB;
  add("eax", "ebx");
  TAB;
  mov(x, "eax");
}

void Assembler::quaASSIGN(const Qua &qua) {
  // (:=,0,_,[esp+0])
  const string &dist = get<3>(qua);
  const string &src = get<1>(qua);
  TAB;
  mov(dist, src);
}

void Assembler::handleQuas(const vector<Qua> &quas) {
  for (auto i : quas) {
    string sign = get<0>(i);
    if (quaMap.find(sign) == quaMap.end()) {
      log("cannot find this sign in quaMap" + sign);
      continue;
    }
    QuaType quaType = quaMap[sign];
    if (quaEmit.find(quaType) == quaEmit.end()) {
      log("cannot find this QuaType in quaMap");
      continue;
    }
    quaEmit[quaType]()

    QuaType qsign = quaMap[sign];
    switch (qsign) {
    case QuaType::Q_ADD: {
      quaADD(i);
      break;
    }
    case QuaType::Q_ASSIGN: {
      quaASSIGN(i);
      break;
    }
    default: {
      log("skip " + sign + " qua");
      break;
    }
    }
  }
}

void Assembler::log(const string &str) {
  cout << "Assembler log: " << str << "\n";
}

map<string, QuaType> Assembler::quaMap = map<string, QuaType>{
    {"+", QuaType::Q_ADD}, {"-", QuaType::Q_SUB}, {":=", QuaType::Q_ASSIGN}};
};
} // namespace x86

