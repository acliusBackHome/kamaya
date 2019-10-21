%{
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include "parser.tab.hpp"

#define enum2str(name) string(#name)

using namespace std;

map<int, string> nameTable;
map<string, string> symbolTabel;
string before;

enum {
  LT, LE, EQ, NE, GT, GE
};

unsigned long install_id(){
  before = yytext;
  if (symbolTabel.find(yytext) != symbolTabel.end()) {
    return (unsigned long)symbolTabel[yytext].c_str();
  }
  symbolTabel[yytext] = yytext;
  return (unsigned long)symbolTabel[yytext].c_str();
}

unsigned long install_num(){
  if (symbolTabel.find(before) != symbolTabel.end()) {
    symbolTabel[before] = yytext;
    return atoi(yytext);
  }else{
    return atoi(yytext);
  }
}
%}

delim                   [\t\n\r\f\v ]
ws                      {delim}+
letter                  [A-Za-z]
digit                   [0-9]
id                      {letter}({letter}|{digit})*
number                  {digit}+(\.{digit}+)?(E[+\-]?{digit}+)?

%%

{ws}                    {}
{digit}+{letter}        {return ERRORFORMAT;}
"void"                  {return VOID;}
"char"                  {return CHAR;}
"bool"                  {return BOOL;}
"float"                 {return FLOAT;}
"long"                  {return LONG;}
"short"                 {return SHORT;}
"double"                {return DOUBLE;}
"main"                  {return MAIN;}
"("                     {return LP;}
")"                     {return RP;}
"{"                     {return LB;}
"}"                     {return RB;}
"["                     {return ML;}
"]"                     {return MR;}
"int"                   {return INT;}
";"                     {return SEMICOLON;}
","                      {return COMMA;}
"if"                    {return IF;}
"else"                  {return ELSE;}
"while"                 {return WHILE;}
{id}                    {yylval = install_id(); return ID;}
{number}                {yylval = install_num(); return NUMBER;}
"<"                     {yylval = LT; return RELOP;}
"<="                    {yylval = LE; return RELOP;}
"=="                    {yylval = EQ; return RELOP;}
"!="                    {yylval = NE; return RELOP;}
">"                     {yylval = GT; return RELOP;}
">="                    {yylval = GE; return RELOP;}
"="                     {return ASSIGN;}
"!"                     {return NOT;}
"&&"                    {return LOGICAND;}
"&"                     {return AND;}
"||"                    {return LOGICOR;}
"|"                     {return OR;}
\/\/[^\n]*              {}
\/\*(\*+[^\/]|[^\*\/])*\*\/ {}
"++"                    {return ADDONE;}
"+"                     {return ADD;}
"--"                    {return SUBONE;}
"-"                     {return SUB;}
"*"                     {return MUL;}
"/"                     {return DIV;}
"^"                     {return POW;}
%%

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

int yywrap()
{
  return 1;
}

/*int main() {
  int token = -0x3f3f3f3f;
  initName();
  while(token = yylex()){
    printf("%-20s%-20s", getName(token).c_str(), yytext);
    if (token == NUMBER || token ==ID) {
      printf("%-20lu", (unsigned long)yylval);
    }
    printf("\n");
  }
  return 0;
}*/
