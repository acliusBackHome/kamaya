%locations
%define parse.error verbose
%{
  #include "kamaya.hpp"
  using namespace std;
  int yylex(void);
  char buff[1024];
%}

%token MAIN RETURN
%token LP RP LB RB ML MR
%token SEMICOLON COMMA ELLIPSIS POINT PTR_OPERATOR
%token IF ELSE WHILE FOR DO SWITCH CASE DEFAULT GOTO CONTINUE BREAK CONST RESTRICT VOLATILE TYPEDEF EXTERN STATIC AUTO REGISTER INLINE SIZEOF UNION STRUCT ENUM
%token LT LE EQ NE GT GE NUMBER ID STRING_LITERAL
%token INT CHAR VOID BOOL DOUBLE FLOAT LONG SHORT
%token ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token NOT OR AND XOR BIT_NOT
%token LOGICAND LOGICOR
%token ADDONE SUBONE ADD SUB MUL DIV MOD
%token LEFT_OP RIGHT_OP
%token QUESTION_MARK COLON
%token ERRORFORMAT

%left ADD SUB
%left MUL DIV MOD
%right U_neg POW

%start program
%%
program
  : external_declaration {
    tree.set_parent($1, 0);
  }
  | program external_declaration {
    tree.set_parent($2, 0);
  }
  ;

id_delaration
  : ID {
    $$ = tree.make_identifier_node(string((const char*)$1));
  }
  | MAIN {
    $$ = tree.make_identifier_node("main");
  }
  ;

// 以下是表达式
primary_expression
  : id_delaration {
    $$ = $1;
  }
  | NUMBER {
    $$ = tree.make_const_node((long long)$1);
  }
  | STRING_LITERAL {
    $$ = tree.make_const_node(string((const char*)$1));
  }
  | LP expression RP {
    $$ = $2;
  }
  ;

postfix_expression
  : primary_expression {
    $$ = $1;
  }
  | postfix_expression ML expression MR {
    $$ = tree.new_node("postfix []");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | postfix_expression LP RP {
    $$ = tree.new_node("function call");
    tree.set_parent($1, $$);
  }
  | postfix_expression LP argument_expression_list RP {
    $$ = tree.new_node("function call");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | postfix_expression POINT id_delaration {
    $$ = tree.new_node("get member operator .");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | postfix_expression PTR_OPERATOR id_delaration {
    $$ = tree.new_node("get member operator ->");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | postfix_expression ADDONE {
    $$ = tree.new_node("postfix self increment");
    tree.set_parent($1, $$);
  }
  | postfix_expression SUBONE {
    $$ = tree.new_node("postfix self decrement");
    tree.set_parent($1, $$);
  }
  | LP type_name RP LB initializer_list RB {
    $$ = tree.new_node("initializer");
    tree.set_parent($2, $$);
    tree.set_parent($5, $$);
  }
  | LP type_name RP LB initializer_list COMMA RB {
    $$ = tree.new_node("initializer");
    tree.set_parent($2, $$);
    tree.set_parent($5, $$);
  }
  ;

argument_expression_list
	: assignment_expression {
    $$ = tree.new_node("argument expression list");
    tree.set_parent($1, $$);
  }
	| argument_expression_list COMMA assignment_expression {
    $$ = $1;
    tree.set_parent($3, $1);
  }
	;

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
  | unary_operator cast_expression {
    $$ = tree.new_node("prefix expression");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  | SIZEOF LP unary_expression RP {
    $$ = tree.new_node("sizeof");
    tree.set_parent($3, $$);
  }
  | SIZEOF LP type_name RP {
    $$ = tree.new_node("sizeof");
    tree.set_parent($3, $$);
  }
  ;

unary_operator
	: AND {
    $$ = tree.new_node("&");
  }
	| MUL {
    $$ = tree.new_node("*");
  }
	| ADD {
    $$ = tree.new_node("+");
  }
	| SUB {
    $$ = tree.new_node("-");
  }
	| BIT_NOT {
    $$ = tree.new_node("~");
  }
	| NOT {
    $$ = tree.new_node("!");
  }
	;

cast_expression
	: unary_expression {
    $$ = $1;
  }
	| LP type_name RP cast_expression {
    $$ = tree.new_node("cast expression");
    tree.set_parent($2, $$);
    tree.set_parent($4, $$);
  }
	;

power_expression
  : cast_expression {
    $$ = $1;
  }
  | cast_expression XOR power_expression {
    $$ = tree.new_node("expression operator ^");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

multiplicative_expression
  : power_expression {
    $$ = $1;
  }
  | multiplicative_expression MUL power_expression {
    $$ = tree.new_node("expression operator * ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | multiplicative_expression DIV power_expression {
    $$ = tree.new_node("expression operator / ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | multiplicative_expression MOD power_expression {
    $$ = tree.new_node("expression operator % ");
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

shift_expression
  : additive_expression {
    $$ = $1;
  }
  | shift_expression LEFT_OP additive_expression {
    $$ = tree.new_node("expression operator << ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | shift_expression RIGHT_OP additive_expression {
    $$ = tree.new_node("expression operator >> ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

relational_expression
  : shift_expression {
    $$ = $1;
  }
  | relational_expression LT shift_expression {
    $$ = tree.new_node("expression operator < ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | relational_expression LE shift_expression {
    $$ = tree.new_node("expression operator <= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | relational_expression GT shift_expression {
    $$ = tree.new_node("expression operator > ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | relational_expression GE shift_expression {
    $$ = tree.new_node("expression operator >= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

equality_expression
  : relational_expression {
    $$ = $1;
  }
  | equality_expression EQ relational_expression {
    $$ = tree.new_node("expression operator == ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | equality_expression NE relational_expression {
    $$ = tree.new_node("expression operator != ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

and_expression
  : equality_expression {
    $$ = $1;
  }
  | and_expression AND equality_expression {
    $$ = tree.new_node("expression operator & ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

inclusive_or_expression
	: and_expression
	| inclusive_or_expression OR and_expression
	;

logic_and_expression
  : inclusive_or_expression {
    $$ = $1;
  }
  | logic_and_expression LOGICAND inclusive_or_expression {
    $$ = tree.new_node("expression operator && ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

logic_or_expression
  : logic_and_expression {
    $$ = $1;
  }
  | logic_or_expression LOGICOR logic_and_expression {
    $$ = tree.new_node("expression operator || ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

conditional_expression
  : logic_or_expression {
    $$ = $1;
  }
  | logic_or_expression QUESTION_MARK expression COLON conditional_expression {
    $$ = tree.new_node("expression operator ?: ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
  }
  ;

assignment_expression
  : conditional_expression {
    $$ = $1;
  }
  | unary_expression ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator = ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression MUL_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator *= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression DIV_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator /= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression MOD_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator %= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression ADD_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator += ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression SUB_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator -= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression LEFT_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator <<= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression RIGHT_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator >>= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression AND_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator &= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression XOR_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator ^= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | unary_expression OR_ASSIGN assignment_expression {
    $$ = tree.new_node("expression operator |= ");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

expression
  : assignment_expression {
    $$ = $1;
  }
  | expression COMMA assignment_expression {
    $$ = tree.new_node("comma expression");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

constant_expression
	: conditional_expression {
    $$ = $1;
  }
	;

declaration
	: declaration_specifiers SEMICOLON {
    $$ = tree.new_node("declaration");
    tree.set_parent($1, $$);
  }
	| declaration_specifiers init_declarator_list SEMICOLON {
    $$ = tree.new_node("declaration");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	;

declaration_specifiers
	: storage_class_specifier {
    $$ = tree.new_node("declaration specifiers");
    tree.set_parent($1, $$);
  }
	| storage_class_specifier declaration_specifiers {
    $$ = tree.new_node("declaration specifiers");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	| type_specifier {
    $$ = tree.new_node("declaration specifiers");
    tree.set_parent($1, $$);
  }
	| type_specifier declaration_specifiers {
    $$ = tree.new_node("declaration specifiers");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	| type_qualifier {
    $$ = tree.new_node("declaration specifiers");
    tree.set_parent($1, $$);
  }
	| type_qualifier declaration_specifiers {
    $$ = tree.new_node("declaration specifiers");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	| function_specifier {
    $$ = tree.new_node("declaration specifiers");
    tree.set_parent($1, $$);
  }
	| function_specifier declaration_specifiers {
    $$ = tree.new_node("declaration specifiers");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	;

init_declarator_list
	: init_declarator {
    $$ = tree.new_node("initialize declarator list");
    tree.set_parent($1, $$);
  }
	| init_declarator_list COMMA init_declarator {
    $$ = $1;
    tree.set_parent($3, $$);
  }
	;

init_declarator
	: declarator {
    $$ = $1;
  }
	| declarator ASSIGN initializer {
    $$ = tree.new_node("assignment declarator");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
	;

storage_class_specifier
	: TYPEDEF {
    $$ = tree.new_node("storage class specifier typedef");
  }
	| EXTERN {
    $$ = tree.new_node("storage class specifier extern");
  }
	| STATIC {
    $$ = tree.new_node("storage class specifier static");
  }
	| AUTO {
    $$ = tree.new_node("storage class specifier auto");
  }
	| REGISTER {
    $$ = tree.new_node("storage class specifier register");
  }
	;

// 类型声明
type_specifier
  : VOID {
    $$ = tree.new_node("type specifier void");
  }
  | CHAR {
    $$ = tree.new_node("type specifier char");
  }
  | SHORT {
    $$ = tree.new_node("type specifier short");
  }
  | INT {
    $$ = tree.new_node("type specifier int");
  }
  | LONG {
    $$ = tree.new_node("type specifier long");
  }
  | FLOAT {
    $$ = tree.new_node("type specifier float");
  }
  | DOUBLE {
    $$ = tree.new_node("type specifier double");
  }
  | BOOL {
    $$ = tree.new_node("type specifier bool");
  }
   | enum_specifier {
     $$ = $1;
   }
   | struct_or_union_specifier {
     $$ = tree.new_node("struct specifier");
     $$ = $1;
   }
  ;

struct_or_union_specifier
	: struct_or_union id_delaration LB struct_declaration_list RB {
    $$ = tree.new_node("struct or union_specifier");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.set_parent($4, $$);
  }
	| struct_or_union LB struct_declaration_list RB {
    $$ = tree.new_node("struct or union_specifier");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
	| struct_or_union id_delaration {
    $$ = tree.new_node("struct or union_specifier");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	;

struct_or_union
	: STRUCT {
    $$ = tree.new_node("struct");
  }
	| UNION {
    $$ = tree.new_node("union");
  }
	;

struct_declaration_list
	: struct_declaration {
    $$ = tree.new_node("struct declaration list");
    tree.set_parent($1, $$);
  }
	| struct_declaration_list struct_declaration {
    $$ = $1;
    tree.set_parent($1, $$);
  }
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list SEMICOLON {
    $$ = tree.new_node("struct declaration");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	;

// 修饰并声明的列表
specifier_qualifier_list
	: type_specifier {
    $$ = tree.new_node("specifier_qualifier_list");
    tree.set_parent($1, $$);
  }
	| type_specifier specifier_qualifier_list {
    $$ = $2;
    tree.set_parent($1, $$);
  }
	| type_qualifier {
    $$ = tree.new_node("specifier_qualifier_list");
    tree.set_parent($1, $$);
  }
	| type_qualifier specifier_qualifier_list {
    $$ = $1;
    tree.set_parent($2, $$);
  }
	;

struct_declarator_list
	: struct_declarator {
    $$ = tree.new_node("struct declarator list");
    tree.set_parent($1, $$);
  }
	| struct_declarator_list COMMA struct_declarator {
    $$ = $1;
    tree.set_parent($1, $$);
  }
	;

struct_declarator
	: declarator {
    $$ = $1;
  }
	//| COLON constant_expression
	//| declarator COLON constant_expression
	;

enum_specifier
	: ENUM LB enumerator_list RB {
    $$ = tree.new_node("enum specifier");
    tree.set_parent($3, $$);
  }
	| ENUM id_delaration LB enumerator_list RB {
    $$ = tree.new_node("enum specifier");
    tree.set_parent($2, $$);
    tree.set_parent($4, $$);
  }
	| ENUM LB enumerator_list COMMA RB {
    $$ = tree.new_node("enum specifier");
    tree.set_parent($3, $$);
  }
	| ENUM id_delaration LB enumerator_list COMMA RB {
    $$ = tree.new_node("enum specifier");
    tree.set_parent($2, $$);
    tree.set_parent($4, $$);
  }
	| ENUM id_delaration {
    $$ = tree.new_node("enum specifier");
    tree.set_parent($2, $$);
  }
	;

enumerator_list
	: enumerator {
    $$ = tree.new_node("enumerator list");
    tree.set_parent($1, $$);
  }
	| enumerator_list COMMA enumerator {
    $$ = $1;
    tree.set_parent($3, $$);
  }
	;

enumerator
	: id_delaration {
    $$ = tree.new_node("enumerator");
    tree.set_parent($1, $$);
  }
	| id_delaration ASSIGN constant_expression {
    $$ = tree.new_node("enumerator");
    size_t temp = tree.new_node("assignment expression");
    tree.set_parent($1, temp);
    tree.set_parent($3, temp);
    tree.set_parent(temp, $$);
  }
	;

// 类型修饰
type_qualifier
	: CONST {
    $$ = tree.new_node("type qualifier const");
  }
	| RESTRICT {
    $$ = tree.new_node("type qualifier restrict");
  }
	| VOLATILE {
    $$ = tree.new_node("type qualifier volatile");
  }
	;

function_specifier
	: INLINE {
    $$ = tree.new_node("inline specifier");
  }
	;

declarator
	: pointer direct_declarator {
    $$ = tree.new_node("declarator");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	| direct_declarator {
    $$ = tree.new_node("declarator");
    tree.set_parent($1, $$);
  }
	;

direct_declarator
	: id_delaration {
    $$ = tree.new_node("direct declarator");
    tree.set_parent($1, $$);
  }
	| LP declarator RP {
    $$ = $1;
  }
	| direct_declarator ML type_qualifier_list assignment_expression MR {
    $$ = $1;
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
  }
	| direct_declarator ML type_qualifier_list MR {
    $$ = $1;
    tree.set_parent($3, $$);
  }
	| direct_declarator ML assignment_expression MR {
    $$ = $1;
    tree.set_parent($3, $$);
  }
	//| direct_declarator ML STATIC type_qualifier_list assignment_expression MR
	//| direct_declarator ML type_qualifier_list STATIC assignment_expression MR
	//| direct_declarator ML type_qualifier_list MUL MR
	//| direct_declarator ML MUL MR
	| direct_declarator ML MR {
    $$ = tree.new_node("array declaration");
    tree.set_parent($1, $$);
  }
	| direct_declarator LP parameter_type_list RP {
    $$ = tree.new_node("declaration with parameters");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
	| direct_declarator LP identifier_list RP {
    $$ = tree.new_node("declaration with identifier");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
	| direct_declarator LP RP {
    $$ = tree.new_node("declaration with empty parentheses");
    tree.set_parent($1, $$);
  }
	;

pointer
	: MUL {
    $$ = tree.new_node("pointer");
  }
	| MUL type_qualifier_list {
    $$ = tree.new_node("pointer");
    tree.set_parent($2, $$);
  }
	| MUL pointer {
    $$ = tree.new_node("pointer");
    tree.set_parent($1, $$);
  }
	| MUL type_qualifier_list pointer {
    $$ = tree.new_node("pointer");
    tree.set_parent($2, $$);
    tree.set_parent($3, $$);
  }
	;

// 类型的修饰符列表
type_qualifier_list
	: type_qualifier {
    $$ = tree.new_node("type_qualifier_list");
    tree.set_parent($1, $$);
  }
	| type_qualifier_list type_qualifier {
    $$ = $1;
    tree.set_parent($2, $$);
  }
	;

parameter_type_list
	: parameter_list {
    $$ = $1;
  }
	| parameter_list COMMA ELLIPSIS {
    $$ = $1;
    tree.set_parent(tree.new_node("varargs ..."), $$);
  }
	;

parameter_list
	: parameter_declaration {
    $$ = tree.new_node("parameter list");
    tree.set_parent($1, $$);
  }
	| parameter_list COMMA parameter_declaration {
    $$ = $1;
    tree.set_parent($3, $$);
  }
	;

parameter_declaration
	: declaration_specifiers declarator {
    $$ = tree.new_node("parameter declaration");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	| declaration_specifiers abstract_declarator {
    $$ = tree.new_node("parameter declaration");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	| declaration_specifiers {
    $$ = $1;
  }
	;

identifier_list
	: id_delaration {
    $$ = tree.new_node("identifier list");
    tree.set_parent($1, $$);
  }
	| identifier_list COMMA id_delaration {
      $$ = $1;
      tree.set_parent($3, $$);
  }
	;

type_name
	: specifier_qualifier_list {
    $$ = tree.new_node("type name");
    tree.set_parent($1, $$);
  }
	| specifier_qualifier_list abstract_declarator {
    $$ = tree.new_node("type name");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
	;

abstract_declarator
	: pointer {
    $$ = $1;
  }
	| direct_abstract_declarator {
    $$ = $1;
  }
	| pointer direct_abstract_declarator {
    $$ = $1;
    tree.set_parent($2, $$);
  }
	;

// 用于声明数组或者多维数组或者函数指针
direct_abstract_declarator
	: LP abstract_declarator RP {
    $$ = $1;
  }
  // 例:int a[][][];
	| ML MR {
    $$ = tree.new_node("direct abstract declarator []");
  }
	| ML assignment_expression MR {
    $$ = tree.new_node("direct abstract declarator []");
    tree.set_parent($2, $$);
  }
	| direct_abstract_declarator ML MR {
    $$ = $1;
    tree.set_parent(tree.new_node("direct abstract declarator []"), $$);
  }
	| direct_abstract_declarator ML assignment_expression MR {
    $$ = $1;
    size_t temp = tree.new_node("direct abstract declarator []");
    tree.set_parent($3, temp);
    tree.set_parent(temp, $$);
  }
	// | ML '*' MR
	// | direct_abstract_declarator ML '*' MR
  // 用于函数指针: int (*p)(int,int)
	| LP RP {
    $$ = tree.new_node("direct abstract declarator ()");
  }
  | LP parameter_type_list RP {
    $$ = tree.new_node("direct abstract declarator ()");
    tree.set_parent($2, $$);
  }
	| direct_abstract_declarator LP RP {
    $$ = $1;
    tree.set_parent(tree.new_node("direct abstract declarator ()"), $$);
  }
	| direct_abstract_declarator LP parameter_type_list RP {
    $$ = $1;
    size_t temp = tree.new_node("direct abstract declarator ()");
    tree.set_parent($3, temp);
    tree.set_parent(temp, $$);
  }
	;

initializer
	: assignment_expression {
    $$ = tree.new_node("initializer");
    tree.set_parent($1, $$);
  }
	| LB initializer_list RB {
    $$ = tree.new_node("initializer");
    tree.set_parent($1, $$);
  }
	| LB initializer_list COMMA RB {
    $$ = tree.new_node("initializer");
    tree.set_parent($1, $$);
  }
	;

initializer_list
	: initializer {
    $$ = tree.new_node("initializer list");
    tree.set_parent($1, $$);
  }
	| designation initializer {
    size_t temp = tree.new_node("assignment initializer");
    tree.set_parent($1, temp);
    tree.set_parent($2, temp);
    $$ = tree.new_node("initializer list");
    tree.set_parent(temp, $$);
  }
	| initializer_list COMMA initializer {
    $$ = $1;
    tree.set_parent($3, $$);
  }
	| initializer_list COMMA designation initializer {
    $$ = $1;
    size_t temp = tree.new_node("assignment initializer");
    tree.set_parent($3, temp);
    tree.set_parent($4, temp);
    tree.set_parent(temp, $$);
  }
	;

designation
	: designator_list ASSIGN {
    $$ = $1;
  }
	;

designator_list
	: designator {
    $$ = tree.new_node("designator list");
    tree.set_parent($1, $$);
  }
	| designator_list designator {
    $$ = $1;
    tree.set_parent($2, $$);
  }
	;

designator
	: ML constant_expression MR {
    $$ = tree.new_node("designator []");
    tree.set_parent($2, $$);
  }
	| POINT id_delaration {
    $$ = tree.new_node("designator .");
    tree.set_parent($2, $$);
  }
	;

statement
  : labeled_statement {
    $$ = $1;
  }
  | compound_statement {
    $$ = $1;
  }
  | expression_statement {
    $$ = $1;
  }
  | selection_statement {
    $$ = $1;
  }
  | iteration_statement {
    $$ = $1;
  }
  | jump_statement {
    $$ = $1;
  }
  ;

labeled_statement
  : id_delaration COLON statement {
    $$ = tree.new_node("id colon statement");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | CASE constant_expression COLON statement {
    $$ = tree.new_node("case statement");
    tree.set_parent($2, $$);
    tree.set_parent($4, $$);
  }
  | DEFAULT COLON statement {
    $$ = tree.new_node("default statement");
    tree.set_parent($3, $$);
  }

compound_statement
  : LB RB {
    $$ = tree.new_node("compound statement");
  }
  | LB block_item_list RB {
    $$ = tree.new_node("compound statement");
    tree.set_parent($2, $$);
  }
  ;

block_item_list
	: block_item {
    $$ = tree.new_node("block item list");
    tree.set_parent($1, $$);
  }
	| block_item_list block_item {
    $$ = $1;
    tree.set_parent($2, $$);
  }
	;

block_item
	: declaration {
    $$ = $1;
  }
	| statement {
    $$ = $1;
  }
	;

expression_statement
  : expression SEMICOLON {
    $$ = $1;
  }
  | SEMICOLON {
    $$ = tree.new_node("empty statement");
  }
  | error {
    // size_t last_node = tree.last_node;
    $$ =  tree.new_node(errorStr);
    // tree.set_parent(last_node, $$);
    // errorNodes.push_back($$);
    tree.error_push($$);
    yyerrok;
  }
  ;

selection_statement
  : IF LP expression RP statement {
    $$ = tree.new_node("if statement");
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
  }
  | IF LP expression RP statement ELSE statement {
    $$ = tree.new_node("if else statement");
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
    tree.set_parent($7, $$);
  }
  | SWITCH LP expression RP statement {
    $$ = tree.new_node("switch statement");
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
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
  | FOR LP declaration expression_statement RP statement {
    $$ = tree.new_node("for statement");
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($6, $$);
  }
  | FOR LP declaration expression_statement expression RP statement {
    $$ = tree.new_node("for statement");
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($7, $$);
  }
  ;

jump_statement
  : GOTO id_delaration SEMICOLON {
    $$ = tree.new_node("goto statement");
    tree.set_parent($2, $$);
  }
  | CONTINUE SEMICOLON {
    $$ = tree.new_node("continue statement");
  }
  | BREAK SEMICOLON {
    $$ = tree.new_node("break statement");
  }
  | RETURN SEMICOLON {
    $$ = tree.new_node("return statement");
  }
  | RETURN expression SEMICOLON {
    $$ = tree.new_node("return statement");
    tree.set_parent($2, $$);
  }
  ;

external_declaration
  : function_definition {
    $$ = $1;
  }
  | declaration {
    $$ = $1;
  }
  ;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement {
    $$ = tree.new_node("function definition");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
  }
	| declaration_specifiers declarator compound_statement {
    $$ = tree.new_node("function definition");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.set_parent($3, $$);
  }
	;

declaration_list
	: declaration {
    $$ = tree.new_node("declaration list");
    tree.set_parent($1, $$);
  }
	| declaration_list declaration {
    $$ = $1;
    tree.set_parent($2, $$);
  }
	;

%%

string errorStr, inputFile;

void yyerror(const char* msg) {
  string appendstr = inputFile + ":" + to_string(yylloc.first_line + 1) + ":" + to_string(yylloc.first_column + 1) + ": " + msg + '\n' +
                     filestrings[yylloc.first_line] + '\n';
  for (int i = 0; i < yylloc.first_column; i++) appendstr += " ";
  appendstr += "^";
  errorStrings.push_back(appendstr);
  // errorStr = "^ lineno[" + to_string((int)yylloc.last_line) + "]columnno[" + to_string((int)yylloc.first_column) + "]: " + msg;
  errorStr = msg;
}
