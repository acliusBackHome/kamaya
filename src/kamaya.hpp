#ifndef KAMAYA_HPP
#define KAMAYA_HPP

#include <bits/stdc++.h>

#define YY_USER_ACTION yyUserActon();
#define YYSTYPE unsigned long

#include "parser.tab.hpp"
#include "MessageTree.hpp"

#define enum2str(name) string(#name)

extern std::map<int, std::string> nameTable;
extern std::map<std::string, std::string> symbolTabel;
extern std::map<std::string, std::string> stringTabel;
extern std::string before, errorstr, inputfile;
extern int row_now;
extern int yycolumn;
extern int yylineno;
extern int yyleng;
extern char *yytext;

void initName();
std::string getName(int);
void yyerror(const char *);
void yyUserActon();

#endif