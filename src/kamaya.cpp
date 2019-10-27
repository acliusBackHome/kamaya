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
  nameTable[(int)COMMA] = enum2str(COMMA);
  nameTable[(int)IF] = enum2str(IF);
  nameTable[(int)ELSE] = enum2str(ELSE);
  nameTable[(int)WHILE] = enum2str(WHILE);
  nameTable[(int)LT] = enum2str(LT);
  nameTable[(int)LE] = enum2str(LE);
  nameTable[(int)EQ] = enum2str(EQ);
  nameTable[(int)NE] = enum2str(NE);
  nameTable[(int)GT] = enum2str(GT);
  nameTable[(int)GE] = enum2str(GE);
  nameTable[(int)NUMBER] = enum2str(NUMBER);
  nameTable[(int)ID] = enum2str(ID);
  nameTable[(int)INT] = enum2str(INT);
  nameTable[(int)CHAR] = enum2str(CHAR);
  nameTable[(int)VOID] = enum2str(VOID);
  nameTable[(int)ASSIGN] = enum2str(ASSIGN);
  nameTable[(int)MUL_ASSIGN] = enum2str(MUL_ASSIGN);
  nameTable[(int)DIV_ASSIGN] = enum2str(DIV_ASSIGN);
  nameTable[(int)MOD_ASSIGN] = enum2str(MOD_ASSIGN);
  nameTable[(int)ADD_ASSIGN] = enum2str(ADD_ASSIGN);
  nameTable[(int)SUB_ASSIGN] = enum2str(SUB_ASSIGN);
  nameTable[(int)LEFT_ASSIGN] = enum2str(LEFT_ASSIGN);
  nameTable[(int)RIGHT_ASSIGN] = enum2str(RIGHT_ASSIGN);
  nameTable[(int)AND_ASSIGN] = enum2str(AND_ASSIGN);
  nameTable[(int)XOR_ASSIGN] = enum2str(XOR_ASSIGN);
  nameTable[(int)OR_ASSIGN] = enum2str(OR_ASSIGN);
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
  nameTable[(int)XOR] = enum2str(POW);
  nameTable[(int)LEFT_OP] = enum2str(LEFT_OP);
  nameTable[(int)RIGHT_OP] = enum2str(RIGHT_OP);
  nameTable[(int)QUESTION_MARK] = enum2str(QUESTION_MARK);
  nameTable[(int)COLON] = enum2str(COLON);
  nameTable[(int)ERRORFORMAT] = enum2str(ERRORFORMAT);
}

string getName(int token) {
  if (nameTable.find(token) != nameTable.end()) {
    return nameTable[token];
  }
  return string("unknown");
}
