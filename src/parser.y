%{
#include "kamaya.hpp"
using namespace std;
int yylex(void);
MessageTree tree("program");
char buff[1024];
%}

%token MAIN RETURN
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
  : external_declaration {
    tree.set_parent($1, 0);
  }
  | program external_declaration {
    tree.set_parent($2, 0);
  }
  ;

external_declaration
	: function_definition {
    $$ = $1;
  }
	| var_declaration {
    $$ = $1;
  }
  | function_declaration {
    $$ = $1;
  }
	;

function_declaration
  : type_specifier id_delaration LP RP SEMICOLON {
    sprintf(buff, "function definition %s", nameTable[$1].c_str());
    $$ = tree.new_node(buff);
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  | type_specifier id_delaration LP argument_list RP SEMICOLON {
    sprintf(buff, "function definition %s", nameTable[$1].c_str());
    $$ = tree.new_node(buff);
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.set_parent($4, $$);
  }
  ;

function_definition
  : main_function_definition {
    $$ = $1;
  }
  | type_specifier id_delaration LP RP compound_statement {
    sprintf(buff, "function definition %s", nameTable[$1].c_str());
    $$ = tree.new_node(buff);
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.set_parent($5, $$);
  }
  | type_specifier id_delaration LP argument_list RP compound_statement {
    sprintf(buff, "function definition %s", nameTable[$1].c_str());
    $$ = tree.new_node(buff);
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.set_parent($4, $$);
    tree.set_parent($6, $$);
  }
  ;

argument_list
  : type_specifier id_delaration {
    $$ = tree.new_node("argument_list");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  | type_specifier id_delaration COMMA argument_list {
    tree.set_parent($1, $4);
    tree.set_parent($2, $4);
    $$ = $4;
  }
  ;

type_specifier
  : VOID {
    sprintf(buff, "type specifier %s", nameTable[VOID].c_str());
    $$ = tree.new_node(buff);
  }
	| CHAR {
    sprintf(buff, "type specifier %s", nameTable[CHAR].c_str());
    $$ = tree.new_node(buff);
  }
	| SHORT {
    sprintf(buff, "type specifier %s", nameTable[SHORT].c_str());
    $$ = tree.new_node(buff);
  }
	| INT {
    sprintf(buff, "type specifier %s", nameTable[INT].c_str());
    $$ = tree.new_node(buff);
  }
	| LONG {
    sprintf(buff, "type specifier %s", nameTable[LONG].c_str());
    $$ = tree.new_node(buff);
  }
	| FLOAT {
    sprintf(buff, "type specifier %s", nameTable[FLOAT].c_str());
    $$ = tree.new_node(buff);
  }
	| DOUBLE {
    sprintf(buff, "type specifier %s", nameTable[DOUBLE].c_str());
    $$ = tree.new_node(buff);
  }
	| BOOL {
    sprintf(buff, "type specifier %s", nameTable[BOOL].c_str());
    $$ = tree.new_node(buff);
  }
	;

id_delaration
  : ID {
    sprintf(buff, "ID declaration %s", (const char*)$1);
    $$ = tree.new_node(buff);
  }
  ;

var_declaration_list
    : id_delaration {
      $$ = tree.new_node("var_declaration_list");
      tree.set_parent($1, $$);
    }
    | assign_expression {
      $$ = tree.new_node("var_declaration_list");
      tree.set_parent($1, $$);
    }
    | id_delaration COMMA var_declaration_list {
      tree.set_parent($1, $3);
      $$ = $3;
    }
    | assign_expression COMMA var_declaration_list {
      tree.set_parent($1, $3);
      $$ = $3;
    }
    ;

var_declaration
  : type_specifier var_declaration_list SEMICOLON {
    $$ = tree.new_node("var declaration");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  ;

// 以下是表达式
constant_expression
  : NUMBER {
    sprintf(buff, "const declaration %lu", $1);
    $$ = tree.new_node(buff);
  }
  ;

primary_expression
	: id_delaration {
    $$ = $1;
  }
	| constant_expression {
    $$ = $1;
  }
	| LP expression RP {
    $$ =  $2;
  }

postfix_expression
	: primary_expression {
    $$ = $1;
  }
	| postfix_expression '[' expression ']'
	//| postfix_expression '(' ')' 函数调用
	//| postfix_expression '(' argument_expression_list ')'
	//| postfix_expression '.' id_delaration
	//| postfix_expression PTR_OP IDENTIFIER PTR_OP = "->"
	| postfix_expression ADDONE {
    $$ = tree.new_node("postfix self increment");
    tree.set_parent($1, $$);
  }
	| postfix_expression SUBONE {
    $$ = tree.new_node("postfix self decrement");
    tree.set_parent($1, $$);
  }
	//| '(' type_name ')' '{' initializer_list '}'
	//| '(' type_name ')' '{' initializer_list ',' '}'

unary_expression
	: postfix_expression {
    $$ = $1;
  }
	| ADDONE unary_expression {
    $$ = tree.new_node("prefix self increment");
    tree.set_parent($2, $$);
  }
	| SUBONE unary_expression {
    $$ = tree.new_node("prefix self decrement");
    tree.set_parent($2, $$);
  }
	;

assign_expression
  : id_delaration ASSIGN expression {
    $$ = tree.new_node("expression operator = ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

multiplicative_expression
  : unary_expression {
    $$ = $1;
  }
  | multiplicative_expression MUL unary_expression {
    $$ = tree.new_node("expression operator * ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | multiplicative_expression DIV unary_expression {
    $$ = tree.new_node("expression operator / ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

additive_expression
	: multiplicative_expression {
    $$ = $1;
  }
	| additive_expression ADD multiplicative_expression {
    $$ = tree.new_node("expression operator + ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
	| additive_expression SUB multiplicative_expression {
    $$ = tree.new_node("expression operator - ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
	;

bool_expression
  : additive_expression {
    $$ = $1;
  }
  | bool_expression RELOP additive_expression {
    /*switch($3) {
      case LT:
        $$ = tree.new_node("expression operator < ");
      break;
      case LE:
        $$ = tree.new_node("expression operator <= ");
      break;
      case EQ:
        $$ = tree.new_node("expression operator == ");
      break;
      case NE:
        $$ = tree.new_node("expression operator != ");
      break;
      case GT:
        $$ = tree.new_node("expression operator > ");
      break;
      case GE:
        $$ = tree.new_node("expression operator >= ");
      break;
      default:
        cout << "bool expression error" << endl;
      break;
    }*/
    $$ = tree.new_node("bool expression operator");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

expression
  : assign_expression {
    $$ = $1;
  }
  | bool_expression {
    $$ = $1;
  }
;

iteration_statement
  : WHILE LP expression RP statement {
    $$ = tree.new_node("while statement");
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
  }
  | DO statement WHILE LP expression RP SEMICOLON {
    $$ = tree.new_node("do while statement");
    tree.set_parent($2, $$);
    tree.set_parent($5, $$);
  }
  | FOR LP expression_statement expression_statement RP statement {
    $$ = tree.new_node("for statement");
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($6, $$);
  }
  | FOR LP expression_statement expression_statement expression RP statement {
    $$ = tree.new_node("for statement");
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($7, $$);
  }
  | FOR LP var_declaration expression_statement RP statement {
    $$ = tree.new_node("for statement");
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($6, $$);
  }
  | FOR LP var_declaration expression_statement expression RP statement {
    $$ = tree.new_node("for statement");
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($7, $$);
  }
  ;

expression_statement
  : expression SEMICOLON {
    $$ = $1;
  }
  | SEMICOLON {
    $$ = tree.new_node("empty statement");
  }
  ;

statement
  : var_declaration {
    $$ = $1;
  }
  | expression_statement {
    $$ = $1;
  }
  | iteration_statement {
    $$ = $1;
  }
  | compound_statement {
    $$ = $1;
  }
  | RETURN SEMICOLON {
    $$ = tree.new_node("return statement");
  }
  | RETURN expression SEMICOLON {
    $$ = tree.new_node("return statement");
    tree.set_parent($1, $$);
  }
  ;

statement_list
  : statement {
    $$ = tree.new_node("statement_list");
    tree.set_parent($1, $$);
  }
  | statement_list statement {
    $$ = $1;
    tree.set_parent($2, $$);
  }
  ;

compound_statement
  : LB RB {
    $$ = tree.new_node("compound statement");
  }
  | LB statement_list RB {
    $$ = tree.new_node("compound statement");
    tree.set_parent($2, $$);
  }
  ;

main_function_definition
  : type_specifier MAIN LP RP compound_statement {
    $$ = tree.new_node("main function definition");
    tree.set_parent($1, $$);
    tree.set_parent($5, $$);
  }
  | type_specifier MAIN LP argument_list RP compound_statement {
    $$ = tree.new_node("main function definition");
    tree.set_parent($1, $$);
    tree.set_parent($4, $$);
    tree.set_parent($6, $$);
  }
  ;

%%

void yyerror (const char* s) {
  cout << s << endl;
}

int main() {
  initName();
  yyparse();
  tree.print();
  printf("%lu\n", tree.get_node_num());
  return  0;
}
