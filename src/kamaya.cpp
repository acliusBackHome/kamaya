#include "kamaya.hpp"
using namespace std;

void initName(){
  nameTable[(int)MAIN] = enum2str(MAIN);
  nameTable[(int)LP] = enum2str(LP);
  nameTable[(int)RP] = enum2str(RP);
  nameTable[(int)LB] = enum2str(LB);
  nameTable[(int)RB] = enum2str(RB);
  nameTable[(int)ML] = enum2str(ML);
  nameTable[(int)MR] = enum2str(MR);
  nameTable[(int)SEMICOLON] = enum2str(SEMICOLON);
  nameTable[(int)IF] = enum2str(IF);
  nameTable[(int)ELSE] = enum2str(ELSE);
  nameTable[(int)WHILE] = enum2str(WHILE);
  nameTable[(int)RELOP] = enum2str(RELOP);
  nameTable[(int)NUMBER] = enum2str(NUMBER);
  nameTable[(int)ID] = enum2str(ID);
  nameTable[(int)INT] = enum2str(INT);
  nameTable[(int)CHAR] = enum2str(CHAR);
  nameTable[(int)VOID] = enum2str(VOID);
  nameTable[(int)ASSIGN] = enum2str(ASSIGN);
  nameTable[(int)NOT] = enum2str(NOT);
  nameTable[(int)OR] = enum2str(OR);
  nameTable[(int)AND] = enum2str(AND);
  nameTable[(int)LOGICAND] = enum2str(LOGICAND);
  nameTable[(int)LOGICOR] = enum2str(LOGICOR);
  nameTable[(int)ADD] = enum2str(ADD);
  nameTable[(int)ADDONE] = enum2str(ADDONE);
  nameTable[(int)SUB] = enum2str(SUB);
  nameTable[(int)SUBONE] = enum2str(SUBONE);
  nameTable[(int)MUL] = enum2str(MUL);
  nameTable[(int)DIV] = enum2str(DIV);
  nameTable[(int)MOD] = enum2str(MOD);
  nameTable[(int)POW] = enum2str(POW);
  nameTable[(int)ERRORFORMAT] = enum2str(ERRORFORMAT);
}

string getName(int token) {
  if (nameTable.find(token) != nameTable.end()) {
    return nameTable[token];
  }
  return string("unknown");
}