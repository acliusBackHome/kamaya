%{
#define YYSTYPE unsigned long

void yyerror(const char* msg) {}

int yylex();
%}

%token MAIN
%token LP RP LB RB
%token SEMICOLON
%token IF ELSE
// %token LT LE EQ NE GT GE
%token RELOP NUMBER ID
%token INT CHAR VOID
%token ASSIGN
%token NOT OR

%left '+' '-'
%left '*' '/'
%right U_neg

%%
S:
  ID '=' E              {}
;

E:
  E '+' E               {}
| E '-' E               {}
| E '*' E               {}
| E '/' E               {}
| '(' E ')'             {}
| NUMBER
;

%%

// int main() {
//   return yyparse();
// }