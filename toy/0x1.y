%{
  #include <bits/stdc++.h>
  #include "0x1.hpp"
  #define YYSTYPE double
  using namespace std;
  void yyerror(const char* msg) {
    cout << msg << endl;
  }
  int yylex();
%}
%token NUMBER MARK
%left '+' '-'
%left '*' '/'
%%
S : S E '\n'        {cur_lineno()++; cout << "ans = " << $2 << endl;}
  | S '\n'          {cur_lineno()++; cout << "empty line" <<endl;}
  |                 {}
  ;
E : E '+' E         {$$ = $1 + $3;}
  | E '-' E         {$$ = $1 - $3;}
  | E '*' E         {$$ = $1 * $3;}
  | E '/' E         {$$ = $1 / $3;}
  | NUMBER          {$$ = $1;}
  | '(' E ')'       {$$ = $2;}
  ;
%%
int& cur_lineno(){
  static int lineno = 0;
  return lineno;
}
int main(int argc, char** argv){
  return yyparse();
}