%{
#define YYSTYPE unsigned long

void yyerror(const char* msg) {}

int yylex();
%}

%token MAIN
%token LP RP LB RB ML MR
%token SEMICOLON
%token IF ELSE WHILE
%token RELOP NUMBER ID
%token INT CHAR VOID
%token ASSIGN
%token NOT OR AND
%token LOGICAND LOGICOR
%token ADD SUB MUL DIV MOD POW

%left ADD SUB
%left MUL DIV POW
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