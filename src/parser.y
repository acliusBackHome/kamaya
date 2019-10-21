%{
#define YYSTYPE unsigned long

// #include "../dist/scanner.yy.cpp"
#include<iostream>
using namespace std;
int yylex(void);
extern "C"//为了能够在C++程序里面调用C函数，必须把每一个需要使用的C函数，其声明都包括在extern "C"{}      
          //块里面，这样C++链接时才能成功链接它们。extern "C"用来在C++环境下设置C链接类型。  
{
  //lex.l中也有类似的这段extern "C"，可以把它们合并成一段，放到共同的头文件main.h中  
  void yyerror(const char *s); 
}
%}

%token MAIN
%token LP RP LB RB ML MR
%token SEMICOLON COMMA
%token IF ELSE WHILE
%token RELOP NUMBER ID
%token INT CHAR VOID BOOL DOUBLE FLOAT LONG SHORT
%token ASSIGN
%token NOT OR AND
%token LOGICAND LOGICOR
%token ADDONE SUBONE ADD SUB MUL DIV MOD POW
%token ERRORFORMAT

%left ADD SUB
%left MUL DIV 
%right U_neg POW

%%
program
  : function {}
  | program function {}
  ;

function
  : main_function
  ;

arugument_list
  : type_specifier id_clearation
  | type_specifier id_clearation COMMA arugument_list
  ;

type_specifier
  : VOID {
    $$ = VOID;
    cout << "void" << endl;
  }
	| CHAR {
    $$ = CHAR;
    cout << "char" << endl;
  }
	| SHORT {
    $$ = SHORT;
    cout << "short" << endl;
  }
	| INT {
    $$ = INT;
    cout << "int" << endl;
  }
	| LONG {
    $$ = LONG;
    cout << "long" << endl;
  }
	| FLOAT {
    $$ = FLOAT;
    cout << "float" << endl;
  }
	| DOUBLE {
    $$ = DOUBLE;
    cout << "double" << endl;
  }
	| BOOL {
    $$ = BOOL;
    cout << "bool" << endl;
  }
	;
  
  id_clearation
    : ID {
      $$ = $1;
      cout << "ID Declearation:" << $1 << endl;
    }
    ;

decleration_list
    : id_clearation 
    | assign_expression
    | id_clearation COMMA decleration_list
    | assign_expression COMMA decleration_list
    ;

declaration
  : type_specifier decleration_list SEMICOLON {
    cout << "decleration:" << $1 << " " << $2 << endl;
  }
  ;

assign_expression
  : id_clearation ASSIGN expression {
    $$ = $1;
    cout << "assignment:" << $1 << '=' << $3 << endl;
  }
  ;

multiplicative_expression
  : NUMBER
  | multiplicative_expression MUL NUMBER {
    $$ = $1 * $3;
    cout << $1 << '*' << $3 << endl;
  }
  | multiplicative_expression DIV NUMBER {
    $$ = $1 / $3;
    cout << $1 << '/' << $3 << endl;
  }
  ;

additive_expression
	: multiplicative_expression
	| additive_expression ADD multiplicative_expression {
    $$ = $1 + $3;
    cout << $1 << '+' << $3 << endl;
  }
	| additive_expression SUB multiplicative_expression {
    $$ = $1 - $3;
    cout << $1 << '-' << $3 << endl;
  }
	;

expression
// $$表示式子左边的成员，$1，$2分别表示式子右边的第一个成员和第二个成员
// （空格分割）
  : LP expression RP {
    $$ = $2;
    cout << "(" << $2 << ")" << endl;
  }
  | assign_expression {
    // $$ = get_ID_value[$1]
  }
  | additive_expression
;

statement
  : declaration {}
  | expression SEMICOLON {
    cout<< "expression:" << $1 << endl;
  }
  | SEMICOLON {
    cout << "empty statement" << endl;
  }
  ;

statement_list
  : statement {}
  | statement_list statement {}
  ;

compound_statement
  : LB RB {
    cout << "empty compound_statement" << endl;
  }
  | LB statement_list RB {
    cout << "compound_statement: with statement_list" << endl;
  }
  ;

main_function
  : type_specifier MAIN LP RP compound_statement {
    cout << "main_function : type_specifier: " << $1 << endl;
  }
  | type_specifier MAIN LP arugument_list RP compound_statement {

  }
  ;

%%

void yyerror (const char* s) {
  cout << s << endl;
}

int main() {
  yyparse();
  return  0;
}