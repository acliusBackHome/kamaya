#ifndef KAMAYA_HPP
#define KAMAYA_HPP

#include <bits/stdc++.h>
#include "x86.hpp"

#define YY_USER_ACTION yyUserActon();
#define YYSTYPE unsigned long

#include "parser.tab.hpp"
#include "ParseTree.hpp"

#define enum2str(name) string(#name)

extern std::map<int, std::string> nameTable;
extern std::map<std::string, std::string> symbolTabel;
extern std::map<std::string, std::string> stringTabel;
extern std::string before, errorStr, inputFile;
extern std::vector<std::string> filestrings, errorStrings;
extern ParseTree tree;
extern int yycolumn;
extern int yylineno;
extern int yyleng;
extern char *yytext;
extern FILE *file;
extern x86::Assembler assembler;

void initName();
std::string getName(int);
void yyerror(const char *);
void yyUserActon();

#endif