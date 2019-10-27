%option yylineno
%{
  #include "0x1.hpp"
  using namespace std;
  int yycolumn = 0;
%}
delim      [\t\f\v\r ]
ws         {delim}+
digit      [0-9]
number     {digit}*(\.{digit}+)?([Ee][+\-]?{digit}+)?
mark       [\+\-\*\(\)]
%%
{ws}       {}
{number}   {ECHO; yylval = stod(yytext); return NUMBER;}
{mark}     {ECHO; return yytext[0];}
\n         {ECHO; yycolumn = 0; return yytext[0];}
.          {ECHO; return ERROR;}
%%
int yywrap(){
  return 1;
}
