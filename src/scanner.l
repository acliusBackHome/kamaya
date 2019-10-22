%{
#include "kamaya.hpp"

using namespace std;

map<int, string> nameTable;
map<string, string> symbolTabel;
string before;

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
