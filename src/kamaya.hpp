#include <bits/stdc++.h>
#include "parser.tab.hpp"

#define enum2str(name) string(#name)

extern std::map<int, std::string> nameTable;
extern std::map<std::string, std::string> symbolTabel;
extern std::string before;

enum {
  LT, LE, EQ, NE, GT, GE
};

void initName();
std::string getName(int);
void yyerror(const char *);
