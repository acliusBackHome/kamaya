#ifndef KAMAYA_HPP
#define KAMAYA_HPP

#include "IR.hpp"
#include "x86.hpp"
#include <bits/stdc++.h>

#define YY_USER_ACTION yyUserActon();
#define YYSTYPE unsigned long

#include "ParseTree.hpp"
#include "parser.tab.hpp"

#define enum2str(name) string(#name)

extern std::map<int, std::string> nameTable;
extern std::map<std::string, std::string> symbolTabel;
extern std::map<std::string, std::string> stringTabel;
extern std::string before, errorStr, inputFile;
extern std::vector<std::string> filestrings,
    errorStrings,     // 这个是语法分析时的错误信息
    parse_error_strs; // 这个是语义分析时的错误信息
extern ParseTree tree;
extern int yycolumn;
extern int yylineno; // 当前语法分析到的行数
extern int yyleng;
extern bool generating_code;
extern char *yytext;
extern FILE *file;
extern x86::Assembler assembler;
extern IR ir;

void initName();

std::string getName(int);

void yyerror(const char *);

void yyUserActon();

#endif