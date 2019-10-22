%{
#define YYSTYPE unsigned long

#include "kamaya.hpp"
using namespace std;
int yylex(void);

// extern "C"//为了能够在C++程序里面调用C函数，必须把每一个需要使用的C函数，其声明都包括在extern "C"{}
//           //块里面，这样C++链接时才能成功链接它们。extern "C"用来在C++环境下设置C链接类型。
// {
//   //lex.l中也有类似的这段extern "C"，可以把它们合并成一段，放到共同的头文件main.h中
//   void yyerror(const char *s);
// }
%}

%token MAIN
%token LP RP LB RB ML MR
%token SEMICOLON COMMA
%token IF ELSE WHILE FOR DO
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
  : external_declaration
  | program external_declaration
  ;

external_declaration
	: function_definition
	| var_declaration
  | function_declaration
	;

function_declaration
  : type_specifier ID LP RP SEMICOLON
  | type_specifier ID LP arugument_list RP SEMICOLON
  ;

function_definition
  : main_function_definition
  | type_specifier ID LP RP compound_statement
  | type_specifier ID LP arugument_list RP compound_statement
  ;

arugument_list
  : type_specifier id_dclaration
  | type_specifier id_dclaration COMMA arugument_list
  ;

type_specifier
  : VOID {
    $$ = VOID;
    cout << "void: " << nameTable[VOID] << endl;
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
<<<<<<< HEAD

  id_clearation
    : ID {
      $$ = $1;
      cout << "ID Declearation:" << $1 << endl;
    }
    ;

decleration_list
    : id_clearation
=======
  
id_dclaration
  : ID {
    $$ = $1;
    cout << "ID Declearation:" << $1 << endl;
  }
  ;

var_declaration_list
    : id_dclaration 
>>>>>>> 实现循环语句和主函数外部声明的判断
    | assign_expression
    | id_dclaration COMMA var_declaration_list
    | assign_expression COMMA var_declaration_list
    ;

var_declaration
  : type_specifier var_declaration_list SEMICOLON {
    cout << "decleration:" << $1 << " " << $2 << endl;
  }
  ;

assign_expression
  : id_dclaration ASSIGN expression {
    $$ = $1;
    cout << "assignment:" << $1 << '=' << $3 << endl;
  }
  ;

constant_expression
  : NUMBER {
    cout << "Const Declearation: number" << $1 << endl;
  }
  ;

multiplicative_expression
  : constant_expression
  | ID
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

bool_expression
  : additive_expression {
    $$ = $1;
  }
  | bool_expression RELOP additive_expression {
    cout << "bool expression" << endl;
    /*switch($3) {
      case LT:
        $$ = $1 < $3;
      break;
      case LE:
        $$ = $1 <= $3;
      break;
      case EQ:
        $$ = $1 == $3;
      break;
      case NW:
        $$ = $1 != $3;
      break;
      case GT:
        $$ = $1 > $3;
      break;
      case GE:
        $$ = $1 >= $3;
      break;
    }*/
  }
  ;

expression
// $$表示式子左边的成员，$1，$2分别表示式子右边的第一个成员和第二个成员
// （空格分割）
  : LP expression RP {
    $$ = $2;
  }
  | assign_expression {
    // $$ = get_ID_value[$1]
  }
  | bool_expression
;

iteration_statement
  : WHILE LP expression RP statement
  | DO statement WHILE LP expression RP SEMICOLON
  | FOR LP expression_statement expression_statement RP statement
  | FOR LP expression_statement expression_statement expression RP statement
  | FOR LP var_declaration expression_statement RP statement
  | FOR LP var_declaration expression_statement expression RP statement
  ;

expression_statement
  : expression SEMICOLON
  | SEMICOLON
  ;

statement
  : var_declaration
  | expression_statement
  | iteration_statement
  | compound_statement
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

main_function_definition
  : type_specifier MAIN LP RP compound_statement {
    cout << "main_function_definition : type_specifier: " << $1 << endl;
  }
  | type_specifier MAIN LP arugument_list RP compound_statement {}
  ;

%%

void yyerror (const char* s) {
  cout << s << endl;
}

int main() {
  initName();
  yyparse();
  return  0;
}
