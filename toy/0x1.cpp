#include "0x1.hpp"
using namespace std;

void yyerror(string msg) {
  for (int i = 0; i < yylloc.first_column; i++) cout << " ";
  cout << "^" << "lineno[" << (int)yylloc.last_line << "]columnno[" << (int)yylloc.first_column << "]: " << msg << endl;
}

void unrecognizedChar(string ch) {
  yyerror("Unrecognized string: " + string(ch));
}

// int& cur_lineno() {
//   static int lineno = 0;
//   return lineno;
// }

void printYYLTYPE(YYLTYPE *yyl) {
  // cout << yyl->first_line << ", " << yyl->first_column << ", " << yyl->last_line << ", " << yyl->last_column << endl;
  for (int i = 0; i < yyl->last_column; i++) cout << " ";
  cout << "^" << "lineno[" << (int)yyl->last_line << "]columnno[" << (int)yyl->last_column << "]";
}

void yyUserActon() {
  yylloc.first_line = yylineno - 1;
  yylloc.last_line = yylineno;
  yylloc.first_column = yycolumn;
  yylloc.last_column = yycolumn + yyleng;
  yycolumn += yyleng;
  // cout << endl << yylloc.first_line << yylloc.last_line << yylloc.first_column << yylloc.last_column << endl;
}