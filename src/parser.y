%locations
%define parse.error verbose
%{
#include "kamaya.hpp"
using namespace std;
int yylex(void);
char buff[1024];
// 当前全局语句块空间, 用于声明的时候
size_t scope_now = 0;

/**
 * 处理规约表达式中的双目表达式的处理方式抽取
 * @param expr_1
 * @param expr_2
 * @param expr_type 表达式操作符
 * @param where 用于标识位置信息
 * @param op 操作符字符串
 */
size_t handle_expression(unsigned int expr_1, unsigned int expr_2, ExpressionType expr_type) {
    const ParseExpression &expr1 = tree.node(expr_1).get_expression(),
            &expr2 = tree.node(expr_2).get_expression();
    try {
        switch (expr_type) {
            case E_ADD:
                return tree.make_expression_node(expr1 + expr2);
            case E_SUB:
                return tree.make_expression_node(expr1 - expr2);
            case E_MUL:
                return tree.make_expression_node(expr1 * expr2);
            case E_DIV:
                return tree.make_expression_node(expr1 / expr2);
            case E_MOD:
                return tree.make_expression_node(expr1 % expr2);
            case E_POW:
                return tree.make_expression_node(expr1 ^ expr2);
            case E_LOGIC_OR:
            case E_LOGIC_AND:
            case E_G:
            case E_GE:
            case E_EQUAL:
            case E_NE:
            case E_L:
            case E_LE:
                return tree.make_expression_node(
                        ParseExpression::get_logic_expression(expr_type, expr1, expr2)
                );
            default:
                // 保证调用不到
                break;
        }
    } catch (ParseException &exc) {
        generating_code = false;
        if (exc.get_code() != EX_TYPE_CAN_NOT_CONVERT) {
            // 这一层只处理不能进行自动类型转化的表达式
            string info = "in handle_expression(unsigned int expr_1, unsigned int expr_2, ExpressionType expr_type)";
            info += " expr1=" + to_string(expr1.get_id());
            info += " expr2=" + to_string(expr2.get_id());
            info += " expr_type=" + ParseExpression::get_expr_type_name(expr_type);
            exc.push_trace(info);
            throw exc;
        }
        const ParseType &type1 = expr1.get_ret_type(), &type2 = expr2.get_ret_type();
        if (type1.get_id() != T_UNKNOWN || type2.get_id() != T_UNKNOWN) {
            string error_info = to_string(yylineno) + ": error : no match operator ";
            error_info += ParseExpression::get_expr_type_name(expr_type);
            error_info += " between " + type1.get_info() + " and " + type2.get_info();
            parse_error_strs.emplace_back(error_info);
        }
        return tree.make_expression_node(ParseExpression());
    }
}

void expr_call_back(const ParseExpression& expr) {
}

void set_node_begen_code(size_t node_id, size_t code_id) {
  if(!(tree.node(node_id).has_key(K_BEGIN_CODE))) {
    tree.node(node_id).set_begin_code(code_id);
  }
}

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
%token TRUE FALSE

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
    ParseVariable variable;
    const string &symbol = tree.node($1).get_symbol();
    try {
      variable = ParseScope::get_scope(scope_now).get_variable_declaration(symbol);
    } catch (ParseException &exc) {
      // 找不到变量声明:声明一个同样符号的变量, 防止重复报错
      generating_code = false;
      if(exc.get_code() != EX_NOT_DECLARED) {
      	// 这一层只处理变量未声明的异常
        string info = "in id_delaration->primary_expression";
        exc.push_trace(info);
      	throw exc;
      }
      // 将错误信息记录后声明一个符号为symbol的无效的变量
      string error_info = to_string(yylineno) + ": error :'";
      error_info += symbol + "' was not declared in this scope";
      parse_error_strs.emplace_back(error_info);
      ParseScope::get_scope(scope_now).declaration(symbol, ParseVariable(ParseType(T_UNKNOWN), symbol));
      variable = ParseScope::get_scope(scope_now).get_variable_declaration(symbol);
    }
    $$ = tree.make_expression_node(ParseExpression(variable));
    tree.set_parent($1, $$);
  }
  | NUMBER {
    $$ = tree.make_expression_node(ParseExpression(ParseConstant((long long)$1)));
  }
  | STRING_LITERAL {
    $$ = tree.make_expression_node(ParseExpression(ParseConstant(string((const char*)$1))));
    //$$ = tree.make_const_node();
  }
  | LP expression RP {
    $$ = $2;

    IR_EMIT {
      // 已经向上传递 truelist falselist
    }
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

    IR_EMIT {
      ParseNode& testNode = tree.node($1);
      // TODO
    }
  }
  | SIZEOF LP unary_expression RP {
    $$ = tree.new_node("sizeof");
    tree.set_parent($3, $$);
  }
  | SIZEOF LP type_name RP {
    $$ = tree.new_node("sizeof");
    tree.set_parent($3, $$);
  }
  | error RP {
    $$ = tree.get_error(tree.error_cnt()-1);
    if (!feof(file)) yyerrok;
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
    $$ = handle_expression($1, $3, E_POW);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

multiplicative_expression
  : power_expression {
    $$ = $1;
  }
  | multiplicative_expression MUL power_expression {
    $$ = handle_expression($1, $3, E_MUL);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | multiplicative_expression DIV power_expression {
    $$ = handle_expression($1, $3, E_DIV);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | multiplicative_expression MOD power_expression {
    $$ = handle_expression($1, $3, E_MOD);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  ;

additive_expression
  : multiplicative_expression {
    $$ = $1;
  }
  | additive_expression ADD multiplicative_expression {
    $$ = handle_expression($1, $3, E_ADD);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | additive_expression SUB multiplicative_expression {
    $$ = handle_expression($1, $3, E_SUB);
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
    $$ = handle_expression($1, $3, E_L);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    IR_EMIT {
      ir.relopEmit(tree, $$, $1, $3, "j<", $$);
    }
  }
  | relational_expression LE shift_expression {
    $$ = handle_expression($1, $3, E_LE);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    IR_EMIT {
      ir.relopEmit(tree, $$, $1, $3, "j<=", $$);
    }
  }
  | relational_expression GT shift_expression {
    $$ = handle_expression($1, $3, E_G);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    IR_EMIT {
      ir.relopEmit(tree, $$, $1, $3, "j>", $$);
    }
  }
  | relational_expression GE shift_expression {
    $$ = handle_expression($1, $3, E_GE);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    IR_EMIT {
      ir.relopEmit(tree, $$, $1, $3, "j>=", $$);
    }
  }
  ;

equality_expression
  : relational_expression {
    $$ = $1;
  }
  | equality_expression EQ relational_expression {
    $$ = handle_expression($1, $3, E_EQUAL);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    IR_EMIT {
      ir.relopEmit(tree, $$, $1, $3, "j==", $$);
    }
  }
  | equality_expression NE relational_expression {
    $$ = handle_expression($1, $3, E_NE);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    IR_EMIT {
      ir.relopEmit(tree, $$, $1, $3, "j!=", $$);
    }
  }
  | TRUE {
    $$ = tree.make_const_node((bool)true);

    IR_EMIT {
      ParseNode& B = tree.node($$);
      B.set_false_list(ir.makelist(ir.getNextinstr()));
      ir.gen("jmp", "_", "_", "_", $$);
    }
  }
  | FALSE {
    $$ = tree.make_const_node((bool)false);

    IR_EMIT {
      ParseNode& B = tree.node($$);
      B.set_true_list(ir.makelist(ir.getNextinstr()));
      ir.gen("jmp", "_", "_", "_", $$);
    }
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
  | logic_and_expression LOGICAND backpatch_instr inclusive_or_expression {
    $$ = handle_expression($1, $4, E_LOGIC_AND);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);

    IR_EMIT {
      ParseNode& B = tree.node($$);
      ParseNode& B1 = tree.node($1);
      ParseNode& M = tree.node($3);
      ParseNode& B2 = tree.node($4);
      ir.backpatch(B1.get_true_list(), M.get_instr());
      
      B.set_true_list(B2.get_true_list());
      B.set_false_list(ir.merge(B1.get_false_list(), B2.get_false_list()));
    }
  }
  ;

logic_or_expression
  : logic_and_expression {
    $$ = $1;
  }
  | logic_or_expression LOGICOR backpatch_instr logic_and_expression {
    $$ = handle_expression($1, $4, E_LOGIC_OR);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);

    IR_EMIT {
      ParseNode& B = tree.node($$);
      ParseNode& B1 = tree.node($1);
      ParseNode& M = tree.node($3);
      ParseNode& B2 = tree.node($4);
      ir.backpatch(B1.get_false_list(), M.get_instr());
      
      B.set_true_list(ir.merge(B1.get_true_list(), B2.get_true_list()));
      B.set_false_list(B2.get_false_list());
    }
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
    const ParseExpression &u_expr = tree.node($1).get_expression(),
                                          &a_expr = tree.node($3).get_expression();
    try {
      $$ = tree.make_expression_node(ParseExpression::get_assign_expression(u_expr, a_expr));
    } catch (ParseException &exc) {
        generating_code = false;
        if (exc.get_code() != EX_TYPE_CAN_NOT_CONVERT) {
            // 这一层只处理不能进行自动类型转化的表达式
            string info = "in unary_expression ASSIGN assignment_expression->assignment_expression";
            exc.push_trace(info);
            throw exc;
        }
        const ParseType &u_type = u_expr.get_ret_type(), &a_type = a_expr.get_ret_type();
        if (u_type.get_id() != T_UNKNOWN || a_type.get_id() != T_UNKNOWN) {
            string error_info = to_string(yylineno) + ": error : can not convert ";
            error_info += a_type.get_info() + " to ";
            error_info += u_type.get_info();
            parse_error_strs.emplace_back(error_info);
        }
        $$ =  tree.make_expression_node(ParseExpression());
    }
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
    // 这里是只声明不给定义
    // 比如 class A;
    $$ = tree.make_declaration_node();
    tree.set_parent($1, $$);
  }
  | declaration_specifiers init_declarator_list SEMICOLON {
    $$ = tree.make_declaration_node();
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.node($$).action_declaration(scope_now, ir);
  }
  ;

declaration_specifiers
  : storage_class_specifier {
    $$ = tree.new_node("declaration specifiers 1");
    tree.set_parent($1, $$);
  }
  | storage_class_specifier declaration_specifiers {
    $$ = tree.new_node("declaration specifiers 2");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  | type_specifier {
    $$ = tree.make_declaration_specifier_node();
    tree.set_parent($1, $$);
  }
  | type_specifier declaration_specifiers {
    $$ = tree.new_node("declaration specifiers 4");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  | type_qualifier {
    $$ = tree.new_node("declaration specifiers 5");
    tree.set_parent($1, $$);
  }
  | type_qualifier declaration_specifiers {
    $$ = tree.new_node("declaration specifiers 6");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  | function_specifier {
    $$ = tree.new_node("declaration specifiers 7");
    tree.set_parent($1, $$);
  }
  | function_specifier declaration_specifiers {
    $$ = tree.new_node("declaration specifiers 8");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  ;

init_declarator_list
  : init_declarator {
    $$ = tree.make_init_declarator_list_node();
    tree.node($$).add_init_declarator(tree.node($1).get_init_declarator());
    tree.set_parent($1, $$);
  }
  | init_declarator_list COMMA init_declarator {
    tree.node($1).add_init_declarator(tree.node($3).get_init_declarator());
    $$ = $1;
    tree.set_parent($3, $$);
  }
  ;

init_declarator
  : declarator {
    $$ = $1;
  }
  | declarator ASSIGN initializer {
    $$ = tree.make_init_declarator_node();
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
    $$ = tree.make_type_specifier_node(ParseType(T_VOID));
  }
  | CHAR {
    $$ = tree.make_type_specifier_node(ParseType(T_CHAR));
  }
  | SHORT {
    $$ = tree.make_type_specifier_node(ParseType(T_SHORT));
  }
  | INT {
    $$ = tree.make_type_specifier_node(ParseType(T_INT));
  }
  | LONG {
    $$ = tree.make_type_specifier_node(ParseType(T_LONG));
  }
  | FLOAT {
    $$ = tree.make_type_specifier_node(ParseType(T_FLOAT));
  }
  | DOUBLE {
    $$ = tree.make_type_specifier_node(ParseType(T_DOUBLE));
  }
  | BOOL {
    $$ = tree.make_type_specifier_node(ParseType(T_BOOL));
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
    $$ = tree.make_declarator_node(tree.node($1).get_ptr_lv());
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  | direct_declarator {
    $$ = tree.make_declarator_node();
    tree.set_parent($1, $$);
  }
  ;

direct_declarator
  : id_delaration {
    $$ = tree.make_direct_declarator_node();
    tree.node($$).set_param_list_node(0);
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
    // todo: 这里要将$1的变量类型更新为数组, 但是大小为$3的表达式
    $$ = $1;
    tree.set_parent($3, $$);
  }
  //| direct_declarator ML STATIC type_qualifier_list assignment_expression MR
  //| direct_declarator ML type_qualifier_list STATIC assignment_expression MR
  //| direct_declarator ML type_qualifier_list MUL MR
  //| direct_declarator ML MUL MR
  | direct_declarator ML MR {
    tree.node($1).update_is_array(true);
    tree.node($1).set_expression(ParseExpression());
    $$ = $1;
  }
  | direct_declarator LP parameter_type_list RP {
    $$ = tree.make_direct_declarator_node(false);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | direct_declarator LP identifier_list RP {
    $$ = tree.new_node("declaration with identifier");
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | direct_declarator LP RP {
    $$ = tree.make_direct_declarator_node();
    tree.node($$).set_param_list(vector<ParseVariable>());
    tree.node($$).set_param_list_node(0);
    tree.set_parent($1, $$);
  }
  ;

pointer
  : MUL {
    $$ = tree.make_pointer_node();
  }
  | MUL type_qualifier_list {
    $$ = tree.new_node("pointer");
    tree.set_parent($2, $$);
  }
  | MUL pointer {
    $$ = $2;
    tree.node($$).update_ptr_lv(tree.node($$).get_ptr_lv() + 1);
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
    $$ = tree.make_parameter_list_node();
    tree.node($$).set_param_list_node($$);
    tree.set_parent($1, $$);
  }
  | parameter_list COMMA parameter_declaration {
    $$ = $1;
    tree.set_parent($3, $$);
  }
  ;

parameter_declaration
  : declaration_specifiers declarator {
    auto type = tree.node($1).get_type();
    size_t ptr_lv = tree.node($2).get_ptr_lv();
    if(ptr_lv) {
      type = ParseType::get_pointer(type, ptr_lv);
    }
    if(tree.node($2).get_is_array()) {
      type = ParseType::get_array(type, (size_t)-1);
      // todo:获取$2的表达式并提取出常量值填入get_array的第二个参数, 如果不能提取出常量值则置-1继续分析,并抛出error
    }
    $$ = tree.make_parameter_declaration(ParseVariable(type, tree.node($2).get_symbol()));
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
  }
  | declaration_specifiers abstract_declarator {
    $$ = tree.new_node("parameter declaration 2");
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
  | error RP {
    $$ = tree.get_error(tree.error_cnt()-1);
    if (!feof(file)) yyerrok;
  }
  ;

initializer
  : assignment_expression {
    $$ = tree.make_initializer_node();
    tree.set_parent($1, $$);
  }
  | LB initializer_list RB {
    $$ = tree.new_node("initializer 2");
    tree.set_parent($1, $$);
  }
  | LB initializer_list COMMA RB {
    $$ = tree.new_node("initializer 3");
    tree.set_parent($1, $$);
  }
  | error RB {
    $$ = tree.get_error(tree.error_cnt()-1);
    if (!feof(file)) yyerrok;
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
  ;

compound_statement
  : LB RB {
    $$ = tree.new_node("empty compound statement");
  }
  | LB {
    scope_now = ParseScope::new_scope(scope_now);
    ir.recordBegin();
  } block_item_list RB {
    $$ = tree.make_compound_statement_node();
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
    tree.set_parent($3, $$);
    ir.recordEnd();
  }
  | error RB {
    $$ = tree.get_error(tree.error_cnt()-1);
    if (!feof(file)) yyerrok;
  }
  ;

block_item_list
  : block_item {
    $$ = tree.new_node(N_BI_LIST);
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
  | error SEMICOLON {
    $$ = tree.get_error(tree.error_cnt()-1);
    if (!feof(file)) yyerrok;
  }
  ;

backpatch_instr : {
  $$ = tree.new_node(N_BP_INST);

  IR_EMIT {
    tree.node($$).set_instr(ir.getNextinstr());
  }
};

backpatch_next_list : {
  $$ = tree.new_node(N_BP_NEXT_LIST);

  IR_EMIT {
    ParseNode& N = tree.node($$);
    N.set_next_list(ir.makelist(ir.getNextinstr()));
    ir.gen("jmp", "_", "_", "_", $$);
  }
}

selection_statement
  : IF LP expression RP backpatch_instr statement {
    $$ = tree.new_node(N_IF_STMT);
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
    tree.set_parent($6, $$);

    IR_EMIT {
      ParseNode& S = tree.node($$);
      ParseNode& B = tree.node($3);
      ParseNode& M = tree.node($5);
      ParseNode& S1 = tree.node($6);
      ir.backpatch(B.get_true_list(), M.get_instr());
      
      S.set_next_list(ir.merge(B.get_false_list(), S1.get_next_list()));

      ir.backpatch(S.get_next_list(), ir.getNextinstr());
    }
  }
  | IF LP expression RP backpatch_instr statement backpatch_next_list ELSE backpatch_instr statement {
    $$ = tree.new_node(N_IF_STMT);
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
    tree.set_parent($6, $$);
    tree.set_parent($7, $$);
    tree.set_parent($9, $$);
    tree.set_parent($10, $$);

    IR_EMIT {
      ParseNode& S = tree.node($$);
      ParseNode& B = tree.node($3);
      ParseNode& M1 = tree.node($5);
      ParseNode& S1 = tree.node($6);
      ParseNode& N = tree.node($7);
      ParseNode& M2 = tree.node($9);
      ParseNode& S2 = tree.node($10);
      ir.backpatch(B.get_true_list(), M1.get_instr());
      ir.backpatch(B.get_false_list(), M2.get_instr());
      
      vector<size_t> temp = ir.merge(S1.get_next_list(), N.get_next_list());
      S.set_next_list(ir.merge(temp, S2.get_next_list()));

      ir.backpatch(S.get_next_list(), ir.getNextinstr());
    }
  }
  | SWITCH LP expression RP statement {
    $$ = tree.new_node("switch statement");
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
  }
  ;

for
  : FOR {
    scope_now = ParseScope::new_scope(scope_now);
  }
  ;

iteration_statement
  : WHILE LP backpatch_instr expression RP backpatch_instr statement {
    $$ = tree.new_node("while statement");
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($6, $$);
    tree.set_parent($7, $$);

    IR_EMIT {
      ParseNode& S = tree.node($$);
      ParseNode& M1 = tree.node($3);
      ParseNode& B = tree.node($4);
      ParseNode& M2 = tree.node($6);
      ParseNode& S1 = tree.node($7);
      ir.backpatch(B.get_true_list(), M2.get_instr());
      ir.backpatch(S1.get_next_list(), M1.get_instr());
      S.set_next_list(B.get_false_list());
      ir.gen("jmp", "_", "_", to_string(M1.get_instr()), $$);
      ir.backpatch(S.get_next_list(), ir.getNextinstr());
    }
  }
  | DO backpatch_instr statement WHILE LP backpatch_instr expression RP SEMICOLON {
    $$ = tree.new_node("do while statement");
    tree.set_parent($2, $$);
    tree.set_parent($3, $$);
    tree.set_parent($6, $$);
    tree.set_parent($7, $$);

    IR_EMIT {
      ParseNode& S = tree.node($$);
      ParseNode& M1 = tree.node($2);
      ParseNode& S1 = tree.node($3);
      ParseNode& M2 = tree.node($6);
      ParseNode& B = tree.node($7);
      ir.backpatch(B.get_true_list(), M1.get_instr());
      ir.backpatch(S1.get_next_list(), M2.get_instr());
      
      S.set_next_list(B.get_false_list());

      ir.backpatch(S.get_next_list(), ir.getNextinstr());
    }
  }
  | for LP expression_statement expression_statement RP statement {
    $$ = tree.make_for_statement_node();
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($6, $$);
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
  }
  | for LP expression_statement expression_statement expression RP statement {
    $$ = tree.make_for_statement_node();
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($7, $$);
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
  }
  | for LP declaration expression_statement RP statement {
    $$ = tree.make_for_statement_node();
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($6, $$);
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
  }
  | for LP declaration expression_statement expression RP statement {
    $$ = tree.make_for_statement_node();
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($7, $$);
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
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
    $$ = tree.new_node("function definition 1");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
  }
  | declaration_specifiers declarator compound_statement {
    const auto &symbol = tree.node($2).get_symbol();
    const auto &ret_type = tree.node($1).get_type();
    const auto &args = tree.node($2).get_parameters_list();
    $$ = tree.make_function_definition_node(ret_type, symbol, args);
    ParseScope::get_scope(scope_now).declaration(symbol, ParseFunction(ret_type,
      symbol, args));
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
                     (filestrings.size() == yylloc.first_line ? "" : filestrings[yylloc.first_line]) + '\n';
  for (int i = 0; i < yylloc.first_column; i++) appendstr += " ";
  appendstr += "^";
  errorStrings.push_back(appendstr);
  errorStr = msg;
  size_t eid =  tree.new_node(errorStr);
  tree.error_push(eid);
}
