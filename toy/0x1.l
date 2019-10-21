%{
  #include "0x1.tab.hpp"
  #include <bits/stdc++.h>
  #include "0x1.hpp"
  #define YYSTYPE double
  using namespace std;
  void unrecognized_char(string);
  void yyerror(string);
%}
delim      [\t\f\v\r ]
ws         {delim}+
digit      [0-9]
number     {digit}*(\.{digit}+)?([Ee][+\-]?{digit}+)?
mark       [\+\-\*/\n\(\)]
%%
{ws}       {cout << "ws" << endl;}
{number}   {yylval = stod(yytext); cout << yylval; return NUMBER;}
{mark}     {cout << yytext; return yytext[0];}
.          {unrecognized_char(yytext);}
%%
int yywrap(){
  return 1;
}
void unrecognized_char(string ch){
  yyerror("Unrecognized string: " + string(ch));
}
void yyerror(string msg){
  cout << "Error at line " << cur_lineno() << " " << msg << endl;
}