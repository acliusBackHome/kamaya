%{
#include "kamaya.hpp"

using namespace std;

map<int, string> nameTable;
map<string, string> symbolTabel;
string before;
int row_now = 1;

unsigned long install_id() {
  before = yytext;
  if (symbolTabel.find(yytext) != symbolTabel.end()) {
    return (unsigned long)symbolTabel[yytext].c_str();
  }
  symbolTabel[yytext] = yytext;
  return (unsigned long)symbolTabel[yytext].c_str();
}

unsigned long install_num() {
  if (symbolTabel.find(before) != symbolTabel.end()) {
    return atoi(yytext);
  } else {
    return atoi(yytext);
  }
}
%}

delim                               [\t\r\f\v ]
lf                                  \n
ws                                  {delim}+
letter                              [A-Za-z]
digit                               [0-9]
id                                  {letter}({letter}|{digit})*
number                              {digit}+(\.{digit}+)?(E[+\-]?{digit}+)?

%%

{lf}                                {row_now++;}
{ws}                                {}
{digit}+{letter}                    {return ERRORFORMAT;}
"void"                              {return VOID;}
"char"                              {return CHAR;}
"bool"                              {return BOOL;}
"float"                             {return FLOAT;}
"long"                              {return LONG;}
"short"                             {return SHORT;}
"double"                            {return DOUBLE;}
"main"                              {return MAIN;}
"return"                            {return RETURN;}
"("                                 {return LP;}
")"                                 {return RP;}
"{"                                 {return LB;}
"}"                                 {return RB;}
"["                                 {return ML;}
"]"                                 {return MR;}
"int"                               {return INT;}
";"                                 {return SEMICOLON;}
","                                 {return COMMA;}
"..."                               {return ELLIPSIS;}
"."                                 {return POINT;}
"->"                                {return PTR_OPERATOR;}
"if"                                {return IF;}
"else"                              {return ELSE;}
"switch"                            {return SWITCH;}
"case"                              {return CASE;}
"default"                           {return DEFAULT;}
"while"                             {return WHILE;}
"for"                               {return FOR;}
"do"                                {return DO;}
"goto"                              {return GOTO;}
"continue"                          {return CONTINUE;}
"break"                             {return BREAK;}
"restrict"                          {return RESTRICT;}
"volatile"                          {return VOLATILE;}
"extern"                            {return EXTERN;}
"static"                            {return STATIC;}
"auto"                              {return AUTO;}
"register"                          {return REGISTER;}
"inline"                            {return INLINE;}
"sizeof"                            {return SIZEOF;}
"struct"                            {return STRUCT;}
"union"                            {return UNION;}
"enum"                            {return ENUM;}
{id}                                {yylval = install_id(); return ID;}
{number}                            {yylval = install_num(); return NUMBER;}
"<"                                 {return LT;}
"<="                                {return LE;}
"=="                                {return EQ;}
"!="                                {return NE;}
">"                                 {return GT;}
">="                                {return GE;}
"="                                 {return ASSIGN;}
"*="                                {return MUL_ASSIGN;}
"/="                                {return DIV_ASSIGN;}
"%="                                {return MOD_ASSIGN;}
"+="                                {return ADD_ASSIGN;}
"-="                                {return SUB_ASSIGN;}
"<<="                               {return LEFT_ASSIGN;}
">>="                               {return RIGHT_ASSIGN;}
"&="                                {return AND_ASSIGN;}
"^="                                {return XOR_ASSIGN;}
"|="                                {return OR_ASSIGN;}
"!"                                 {return NOT;}
"~"                                 {return BIT_NOT;}
"&&"                                {return LOGICAND;}
"&"                                 {return AND;}
"||"                                {return LOGICOR;}
"|"                                 {return OR;}
\/\/[^\n]*                          {}
\/\*(\*+[^\/]|[^\*\/])*\*\/         {}
"++"                                {return ADDONE;}
"+"                                 {return ADD;}
"--"                                {return SUBONE;}
"-"                                 {return SUB;}
"*"                                 {return MUL;}
"/"                                 {return DIV;}
"%"                                 {return MOD;}
"^"                                 {return XOR;}
"<<"                                {return LEFT_OP;}
">>"                                {return RIGHT_OP;}
"?"                                 {return QUESTION_MARK;}
":"                                 {return COLON;}
%%

int yywrap()
{
  return 1;
}
