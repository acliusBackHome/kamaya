%locations
%define parse.error verbose
%{
  #include "0x1.hpp"
  using namespace std;
  int yylex();
%}
%token NUMBER MARK ERROR
%left '+' '-'
%left '*' '/'
%%
S : S E '\n'        {/*cur_lineno()++;*/ printYYLTYPE(&(@2)); cout << "ans = " << $2 << endl;}
  | S '\n'          {/*cur_lineno()++;*/ cout << "empty line" <<endl;}
  | S W '\n'        {unrecognizedChar(yytext);}
  |
  ;
E : E '+' E         {$$ = $1 + $3;}
  | E '-' E         {$$ = $1 - $3;}
  | E '*' E         {$$ = $1 * $3;}
  | E '/' E         {$$ = $1 / $3;}
  | NUMBER          {$$ = $1;}
  | '(' E ')'       {$$ = $2;}
  ;
W : ERROR
  | W ERROR
  ;
%%
int main(int argc, char** argv){
  return yyparse();
}