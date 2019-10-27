#ifndef __0X1HPP__
#define __0x1HPP__

#include <bits/stdc++.h>

#define YY_USER_ACTION yyUserActon();

#define YYSTYPE double

#include "0x1.tab.hpp"

extern int yycolumn;
extern int yylineno;
extern int yyleng;
extern char *yytext;

void yyerror(std::string msg);
void unrecognizedChar(std::string ch);
// int& cur_lineno();
void printYYLTYPE(YYLTYPE *yyl);
void yyUserActon();

// struct YYLTYPE
// {
//   int first_line;
//   int first_column;
//   int last_line;
//   int last_column;
// };

#endif