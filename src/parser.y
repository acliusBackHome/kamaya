%locations
%define parse.error verbose
%{
#include "kamaya.hpp"
using namespace std;
int yylex(void);
char buff[1024];
// 当前全局语句块空间, 用于声明的时候
size_t scope_now = 0;

string this_function_symbol = "";

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
  if (generating_code) {
    if (expr.is_const()) { return; }
    const string symbol = ir.newTemp();
    size_t addr = ir.allocEmit(scope_now, symbol, expr.get_ret_type().get_size(), tree.get_node_num() - 1);
    if (expr.is_variable()) {
      expr.set_address(expr.get_variable().get_address());
    } else {
      expr.set_address(addr);
    }
    ir.exprEmit(expr, expr.get_ret_type(), symbol, tree, tree.get_node_num() - 1, scope_now);
  }
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
    const string &symbol = tree.node($1).get_symbol();
    switch(ParseScope::get_scope(scope_now).get_symbol_dec_type(symbol)) {
      case D_VARIABLE : {
        ParseVariable variable;
        variable = ParseScope::get_scope(scope_now).get_variable_declaration(symbol);
        $$ = tree.make_expression_node(ParseExpression(variable));
        break;
      }
      case D_FUNCTION :{
        auto function_list = ParseScope::get_scope(scope_now).get_function_declaration(symbol);
        $$ = tree.make_expression_node(ParseExpression(function_list));
        break;
      }
      case D_UNKNOWN :{
        // 找不到声明
        // 将错误信息记录后声明一个符号为symbol的无效的变量
        // 找不到变量声明:声明一个同样符号的变量, 防止重复报错
        generating_code = false;
        // 错误恢复
        $$ = tree.make_expression_node(ParseExpression());
        // 错误记录
        string error_info = to_string(yylineno) + ": error :'";
        error_info += symbol + "' was not declared in this scope";
        parse_error_strs.emplace_back(error_info);
        string info = "id_delaration->primary_expression";
        info += " symbol=" + symbol;
        info += " node_id=" + to_string($$);
        info += " scope_id=" + to_string(scope_now);
      }
    }
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

    if (generating_code) {
      // 已经向上传递 truelist falselist
    }
  }
  ;

postfix_expression
  : primary_expression {
    $$ = $1;
  }
  | postfix_expression ML expression MR {
    const ParseExpression &expr1 = tree.node($1).get_expression(),
                          &expr2 = tree.node($3).get_expression();
    try {
      $$ = tree.make_expression_node(expr1.get_item(expr2));
    } catch (ParseException &exc) {
      generating_code = false;
      if (exc.get_code() != EX_NOT_AN_ARRAY_TYPE) {
        // 这一层只处理不能进行后缀中括号表达式的异常
        string info = "in postfix_expression ML expression MR->postfix_expression";
        info += " expr1=" + to_string(expr1.get_id());
        info += " expr2=" + to_string(expr2.get_id());
        exc.push_trace(info);
        throw exc;
      }
      const ParseType &type1 = expr1.get_ret_type(), &type2 = expr2.get_ret_type();
      if (type1.get_id() != T_UNKNOWN || type2.get_id() != T_UNKNOWN) {
          string error_info = to_string(yylineno) + ": error : expression ";
          error_info += expr1.get_info();
          error_info += " is not an array type";
          parse_error_strs.emplace_back(error_info);
      }
      $$ = tree.make_expression_node(ParseExpression());
    }
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);
  }
  | postfix_expression LP RP {
    $$ = tree.new_node(N_FUNCTION_CALL);
    const ParseExpression& func_exprssion = tree.node($1).get_expression();
    if(!func_exprssion.is_function()) {
      string error_info = to_string(yylineno) + ": error : expression ";
      error_info += func_exprssion.get_info();
      error_info += " can not use as a function";
      parse_error_strs.emplace_back(error_info);
    } else {
      bool found = false;
      const DecFuncPtrList &func_list = func_exprssion.get_functions();
      for(const ParseFunction *each_func : func_list) {
        // 对所有重载的函数, 判断其是否有args列表为空的记录, 如果有, 返回之
        if(each_func->get_args().empty()) {
          // 找到了声明
          found = true;
          // 返回一个临时变量生成的表达式
          size_t addr = -1;
          if (generating_code) {
            addr = ir.allocEmit(scope_now, ir.newTemp(), each_func->get_ret_type().get_size(), $$);
          }
          tree.node($$).set_expression(ParseExpression(ParseVariable(each_func->get_ret_type(), "func_ret", addr)));
          break;
        }
      }
      if(!found) {
        // 找不到对应的函数声明
        tree.node($$).set_expression(ParseExpression());
        const ParseFunction &expected_func = *(func_list[0]);
        string error_info = (yylineno) + ": error : no match function for call to ";
        error_info += expected_func.get_symbol() + "()";
        parse_error_strs.emplace_back(error_info);
      }
    }
    tree.set_parent($1, $$);
  }
  | postfix_expression LP argument_expression_list RP {
    $$ = tree.new_node(N_FUNCTION_CALL);
    const ParseExpression& func_exprssion = tree.node($1).get_expression();
    if(!func_exprssion.is_function()) {
      string error_info = to_string(yylineno) + ": error : expression ";
      error_info += func_exprssion.get_info();
      error_info += " can not use as a function";
      parse_error_strs.emplace_back(error_info);
    } else {
      bool found = false;
      const DecFuncPtrList &func_list = func_exprssion.get_functions();
      const vector<size_t> &real_arg_list = tree.node($3).get_expression_list();
      size_t len = real_arg_list.size();
      for(const ParseFunction *each_func : func_list) {
        // 对所有重载的函数, 判断其是否有args列表能够转化的记录, 如果有, 返回之
        const auto &formal_args = each_func->get_args();
        if(formal_args.size() == real_arg_list.size()) {
          // 大小相等
          try {
            for(size_t i = 0; i < len; ++i) {
              // 尝试逐个表达式转化为形参类型
              ParseType::convert(formal_args[i].get_type(),
                ParseExpression::get_expression(real_arg_list[i]).get_ret_type());
            }
            // 转化成功
            found = true;
            // 返回一个临时变量生成的表达式
            tree.node($$).set_expression(ParseExpression(ParseVariable(each_func->get_ret_type(), "func_ret")));
            break;
          } catch(ParseException &exc) {
            // 转化失败
            if(exc.get_code() != EX_TYPE_CAN_NOT_CONVERT) {
              // 不是转化异常, 不能够在此处理
              string info = "in postfix_expression LP argument_expression_list RP->postfix_expression";
              info += " $1=" + to_string($1);
              info += " $3=" + to_string($3);
              tree.node($$).set_expression(ParseExpression());
              tree.set_parent($1, $$);
              tree.set_parent($3, $$);
              exc.push_trace(info);
              throw exc;
            }
            // 继续尝试
          }
          break;
        }
      }
      if(!found) {
        // 找不到对应的函数声明
        tree.node($$).set_expression(ParseExpression());
        const ParseFunction &expected_func = *(func_list[0]);
        string error_info = (yylineno) + ": error : no match function for call to ";
        error_info += expected_func.get_symbol() + "(";
        for(size_t i = 0; i < real_arg_list.size(); ++i) {
          error_info += ParseExpression::get_expression(real_arg_list[i]).get_ret_type().get_info();
          if(i != real_arg_list.size() - 1) {
            error_info += ", ";
          }
        }
        error_info += ")";
        parse_error_strs.emplace_back(error_info);
      }
    }
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
    $$ = tree.new_node(N_EXPRESSION_LIST);
    tree.node($$).add_expression_list(tree.node($1).get_expression());
    tree.set_parent($1, $$);
  }
  | argument_expression_list COMMA assignment_expression {
    $$ = $1;
    tree.node($$).add_expression_list(tree.node($3).get_expression());
    tree.set_parent($3, $$);
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
    $$ = tree.make_expression_node(
        ParseExpression::generate_expression(
          (ExpressionType) $1,
          tree.node($2).get_expression()
        )
    );

    tree.set_parent($2, $$);
    if (generating_code) {
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
    generating_code = false;
    if (!feof(file)) {
      yyerrok;
    }
  }
  ;

unary_operator
  : AND {
    $$ = E_ADR;
  }
  | MUL {
    $$ = E_PTR;
  }
  | ADD {
    $$ = E_POS;
  }
  | SUB {
    $$ = E_NEG;
  }
  | BIT_NOT {
    $$ = E_BIT_NOT;
  }
  | NOT {
    $$ = E_NOT;
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

    if (generating_code) {
      ir.relopEmit(tree, $$, $1, $3, "j<", $$);
    }
  }
  | relational_expression LE shift_expression {
    $$ = handle_expression($1, $3, E_LE);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    if (generating_code) {
      ir.relopEmit(tree, $$, $1, $3, "j<=", $$);
    }
  }
  | relational_expression GT shift_expression {
    $$ = handle_expression($1, $3, E_G);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    if (generating_code) {
      ir.relopEmit(tree, $$, $1, $3, "j>", $$);
    }
  }
  | relational_expression GE shift_expression {
    $$ = handle_expression($1, $3, E_GE);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    if (generating_code) {
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

    if (generating_code) {
      ir.relopEmit(tree, $$, $1, $3, "j==", $$);
    }
  }
  | equality_expression NE relational_expression {
    $$ = handle_expression($1, $3, E_NE);
    tree.set_parent($1, $$);
    tree.set_parent($3, $$);

    if (generating_code) {
      ir.relopEmit(tree, $$, $1, $3, "j!=", $$);
    }
  }
  | TRUE {
    $$ = tree.make_const_node((bool)true);

    if (generating_code) {
      ParseNode& B = tree.node($$);
      B.set_false_list(ir.makelist(ir.getNextinstr()));
      ir.gen("jmp", "_", "_", "_", $$);
    }
  }
  | FALSE {
    $$ = tree.make_const_node((bool)false);

    if (generating_code) {
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

    if (generating_code) {
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

    if (generating_code) {
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
      $$ = tree.make_assign_expression_node(u_expr, a_expr);
    } catch (ParseException &exc) {
        generating_code = false;
        const ParseType &u_type = u_expr.get_ret_type(), &a_type = a_expr.get_ret_type();
        switch(exc.get_code()) {
          case EX_TYPE_CAN_NOT_CONVERT: {
            if (u_type.get_id() != T_UNKNOWN || a_type.get_id() != T_UNKNOWN) {
                string error_info = to_string(yylineno) + ": error : can not convert ";
                error_info += a_type.get_info() + " to ";
                error_info += u_type.get_info();
                parse_error_strs.emplace_back(error_info);
            }
            break;
          }
          case EX_CAN_NOT_ASSIGN_CONST: {
            if (u_type.get_id() != T_UNKNOWN || a_type.get_id() != T_UNKNOWN) {
                string error_info = to_string(yylineno) +
                    ": error : lvalue required as left operand of assignment ";
                parse_error_strs.emplace_back(error_info);
            }
            break;
          }
          default: {
            // 这一层只处理不能进行自动类型转化的表达式
            string info = "in unary_expression ASSIGN assignment_expression->assignment_expression";
            exc.push_trace(info);
            throw exc;
            break;
          }
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
     tree.set_parent($1, $$);
   }
  ;

struct_or_union_specifier
  : struct_or_union id_delaration LB {
    scope_now = ParseScope::new_scope(scope_now);
  } struct_declaration_list RB {
    $$ = tree.make_struct_node($1, $2, $5, scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
  }
  | struct_or_union LB {
    scope_now = ParseScope::new_scope(scope_now);
  } struct_declaration_list RB {
    $$ = tree.new_node(N_STRUCT_ENUM);
    tree.set_parent($1, $$);
    tree.set_parent($4, $$);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
  }
  | struct_or_union id_delaration {
    $$ = tree.new_node("struct or union_specifier 3");
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
    $$ = tree.new_node(N_STRUCT_DEC_LIST);
    tree.set_parent($1, $$);
  }
  | struct_declaration_list struct_declaration {
    $$ = $1;
    tree.set_parent($1, $$);
  }
  ;

struct_declaration
  : specifier_qualifier_list struct_declarator_list SEMICOLON {
    // !
    // $$ = tree.new_node("struct declaration");
    $$ = tree.make_declaration_node();
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.node($$).action_declaration(scope_now, ir);
  }
  ;

// 修饰并声明的列表
specifier_qualifier_list
  : type_specifier {
    // @
    $$ = tree.make_declaration_specifier_node();
    // $$ = tree.new_node("specifier_qualifier_list 1");
    tree.set_parent($1, $$);
  }
  | type_specifier specifier_qualifier_list {
    $$ = $2;
    tree.set_parent($1, $$);
  }
  | type_qualifier {
    $$ = tree.new_node("specifier_qualifier_list 2");
    tree.set_parent($1, $$);
  }
  | type_qualifier specifier_qualifier_list {
    $$ = $1;
    tree.set_parent($2, $$);
  }
  ;

struct_declarator_list
  : struct_declarator {
    $$ = tree.make_init_declarator_list_node();
    tree.node($$).add_init_declarator(tree.node($1).get_init_declarator());
    tree.set_parent($1, $$);
  }
  | struct_declarator_list COMMA struct_declarator {
    tree.node($1).add_init_declarator(tree.node($3).get_init_declarator());
    $$ = $1;
    tree.set_parent($3, $$);
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
    const ParseExpression &expr = tree.node($3).get_expression();
    if(expr.is_const()) {
      size_t array_size = expr.get_const().get_unsigned();
      tree.node($1).add_array_size(array_size);
    } else {
      // 不是常量则出错
      tree.node($1).add_array_size((size_t) -1);
      string error_info = to_string(yylineno) + ": error : can not alloc non const array length declaration";
      parse_error_strs.emplace_back(error_info);
    }
    $$ = $1;
    tree.set_parent($3, $$);
  }
  //| direct_declarator ML STATIC type_qualifier_list assignment_expression MR
  //| direct_declarator ML type_qualifier_list STATIC assignment_expression MR
  //| direct_declarator ML type_qualifier_list MUL MR
  //| direct_declarator ML MUL MR
  | direct_declarator ML MR {
    tree.node($1).add_array_size((size_t) -1);
    tree.node($1).set_expression(ParseExpression());
    $$ = $1;
  }
  | direct_declarator LP parameter_type_list RP {
    $$ = tree.make_direct_declarator_node();
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
    const vector<size_t> &array_size = tree.node($2).get_array_size();
    if(ptr_lv) {
      type = ParseType::get_pointer(type, ptr_lv);
    }
    if(!array_size.empty()) {
      for (size_t i = array_size.size() - 1;; --i) {
        type = ParseType::get_array(type, array_size[i]);
        if (!i) {
          break;
        }
      }
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
    generating_code = false;
    if (!feof(file)) {
      yyerrok;
    }
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
    generating_code = false;
    if (!feof(file)) {
      yyerrok;
    }
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
  } block_item_list RB {
    $$ = tree.make_compound_statement_node();
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
    tree.set_parent($3, $$);
  }
  | error RB {
    $$ = tree.get_error(tree.error_cnt()-1);
    generating_code = false;
    if (!feof(file)) {
      yyerrok;
    }
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
    generating_code = false;
    if (!feof(file)) {
      yyerrok;
    }
  }
  ;

backpatch_instr : {
  $$ = tree.new_node(N_BP_INST);

  if (generating_code) {
    tree.node($$).set_instr(ir.getNextinstr());
  }
};

backpatch_next_list : {
  $$ = tree.new_node(N_BP_NEXT_LIST);

  if (generating_code) {
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

    if (generating_code) {
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

    if (generating_code) {
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

    if (generating_code) {
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

    if (generating_code) {
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
  | for LP expression_statement backpatch_instr expression_statement RP backpatch_instr statement {
    $$ = tree.make_for_statement_node();
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($7, $$);
    tree.set_parent($8, $$);
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();

    if (generating_code) {
      ParseNode& S = tree.node($$);
      ParseNode& M1 = tree.node($4);
      ParseNode& B = tree.node($5);
      ParseNode& M2 = tree.node($7);
      ParseNode& S1 = tree.node($8);

      ir.backpatch(B.get_true_list(), M2.get_instr());
      ir.backpatch(S1.get_next_list(), M1.get_instr());
      ir.gen("jmp", "_", "_", to_string(M1.get_instr()), $$);

      S.set_next_list(B.get_false_list());
      ir.backpatch(S.get_next_list(), ir.getNextinstr());
    }
  }
  | for LP expression_statement backpatch_instr expression_statement backpatch_instr expression backpatch_next_list RP backpatch_instr statement {
    $$ = tree.make_for_statement_node();
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($6, $$);
    tree.set_parent($7, $$);
    tree.set_parent($8, $$);
    tree.set_parent($10, $$);
    tree.set_parent($11, $$);
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();

    if (generating_code) {
      ParseNode& S = tree.node($$);
      ParseNode& M1 = tree.node($4);
      ParseNode& B = tree.node($5);
      ParseNode& M2 = tree.node($6);
      ParseNode& E = tree.node($7);
      ParseNode& N = tree.node($8);
      ParseNode& M3 = tree.node($10);
      ParseNode& S1 = tree.node($11);

      ir.backpatch(B.get_true_list(), M3.get_instr());
      ir.backpatch(S1.get_next_list(), M2.get_instr());
      ir.gen("jmp", "_", "_", to_string(M2.get_instr()), $$);
      ir.backpatch(E.get_next_list(), M1.get_instr());
      ir.backpatch(N.get_next_list(), M1.get_instr());

      S.set_next_list(B.get_false_list());
      ir.backpatch(S.get_next_list(), ir.getNextinstr());
    }
  }
  | for LP declaration backpatch_instr expression_statement RP backpatch_instr statement {
    $$ = tree.make_for_statement_node();
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($7, $$);
    tree.set_parent($8, $$);
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();

    if (generating_code) {
      ParseNode& S = tree.node($$);
      ParseNode& M1 = tree.node($4);
      ParseNode& B = tree.node($5);
      ParseNode& M2 = tree.node($7);
      ParseNode& S1 = tree.node($8);

      ir.backpatch(B.get_true_list(), M2.get_instr());
      ir.backpatch(S1.get_next_list(), M1.get_instr());
      ir.gen("jmp", "_", "_", to_string(M1.get_instr()), $$);

      S.set_next_list(B.get_false_list());
      ir.backpatch(S.get_next_list(), ir.getNextinstr());
    }
  }
  | for LP declaration backpatch_instr expression_statement backpatch_instr expression backpatch_next_list RP backpatch_instr statement {
    $$ = tree.make_for_statement_node();
    tree.set_parent($3, $$);
    tree.set_parent($4, $$);
    tree.set_parent($5, $$);
    tree.set_parent($6, $$);
    tree.set_parent($7, $$);
    tree.set_parent($8, $$);
    tree.set_parent($10, $$);
    tree.set_parent($11, $$);
    tree.node($$).set_scope_id(scope_now);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();

    if (generating_code) {
      ParseNode& S = tree.node($$);
      ParseNode& M1 = tree.node($4);
      ParseNode& B = tree.node($5);
      ParseNode& M2 = tree.node($6);
      ParseNode& E = tree.node($7);
      ParseNode& N = tree.node($8);
      ParseNode& M3 = tree.node($10);
      ParseNode& S1 = tree.node($11);

     ir.backpatch(B.get_true_list(), M3.get_instr());
      ir.backpatch(S1.get_next_list(), M2.get_instr());
      ir.gen("jmp", "_", "_", to_string(M2.get_instr()), $$);
      ir.backpatch(E.get_next_list(), M1.get_instr());
      ir.backpatch(N.get_next_list(), M1.get_instr());
      S.set_next_list(B.get_false_list());
      ir.backpatch(S.get_next_list(), ir.getNextinstr());
    }
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

    if (generating_code) {
      if(ParseScope::get_scope(scope_now).get_symbol_dec_type("output") == D_VARIABLE) {
        const ParseVariable &out = ParseScope::get_scope(scope_now).get_variable_declaration("output");
        ir.printEmit(out.get_address(), $$);
      }
      
      if(this_function_symbol == "main") {
        ir.exitEmit(); // TODO: only 0
      } else {
        ir.returnEmit(tree.node($2).get_expression().get_address());
      }
    }
  }
  | RETURN expression SEMICOLON {
    $$ = tree.new_node("return statement");
    tree.set_parent($2, $$);

    if (generating_code) {
      if(ParseScope::get_scope(scope_now).get_symbol_dec_type("output") == D_VARIABLE) {
        const ParseVariable &out = ParseScope::get_scope(scope_now).get_variable_declaration("output");
        ir.printEmit(out.get_address(), $$);
      }
      
      if(this_function_symbol == "main") {
        ir.exitEmit(); // TODO: only 0
      } else {
        ir.returnEmit(tree.node($2).get_expression().get_address());
      }
    }
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

record_begin : {
  if (generating_code) {
    ir.recordBegin(scope_now);
  }
};

function_declarator
: declaration_specifiers declarator record_begin {
  $$ = tree.new_node(N_FUNCTION_DECLARATOR);
  try {
    const auto &symbol = tree.node($2).get_symbol();
    this_function_symbol = symbol;
    const auto &ret_type = tree.node($1).get_type();
    // 构造函数
    tree.node($$).set_function(ParseFunction(ret_type, symbol, tree.node($2).get_parameters_list()));
    auto &function = (ParseFunction &)tree.node($$).get_function();
    vector<ParseVariable> &args = (vector<ParseVariable> &)function.get_args();//回填变量的地址,所以要转成非常量引用
    ParseScope::get_scope(scope_now).declaration(symbol, function);
    scope_now = ParseScope::new_scope(scope_now);

    if (generating_code) {
      ir.funcEmit(symbol, $$);
    }

    for(auto &arg : args) {
      // TODO: 将-2填上下一个函数参数压栈的地址
      size_t addr = 0;
      if (generating_code) {
        addr = ir.allocEmit(scope_now, arg.get_symbol(), arg.get_type().get_size(), 0);
      }
      arg.set_address((size_t) addr);
      arg.set_scope_id(scope_now);
      ParseScope::get_scope(scope_now).declaration(arg.get_symbol(), arg);
    }
  } catch (ParseException &exc) {
    this_function_symbol = "";
    string info = "in declaration_specifiers declarator record_begin->function_declarator";
    exc.push_trace(info);
  }
  tree.set_parent($1, $$);
  tree.set_parent($2, $$);
}

function_definition
  : declaration_specifiers declarator declaration_list record_begin compound_statement {
    $$ = tree.new_node("function definition 1");
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    tree.set_parent($3, $$);
    tree.set_parent($5, $$);
  }
  | function_declarator compound_statement {
    $$ = tree.make_function_definition_node();
    // 在节点里的函数数据结构
    auto &dec_func = (ParseFunction &)tree.node($1).get_function();
    const auto &dec_func_args = dec_func.get_args();
    if(tree.node($2).has_key(K_BEGIN_CODE)) {
      dec_func.set_address(tree.node($2).get_begin_code());
    }
    // 由于已经在之前声明了, 所以要在此回填声明中的函数的地址为begin_code
    auto functions = ParseScope::get_scope(scope_now).get_function_declaration(dec_func.get_symbol());
    // 由于函数有重载,所以这里通过声明得到的是函数的列表, 需要找到正确的声明
    for(auto each_func : functions) {
      auto &each_func_args = (vector<ParseVariable> &)(each_func->get_args());
      if(each_func_args.size() == dec_func_args.size()) {
        // 大小相同才匹配
        bool found = true;
        for(size_t i = 0; i < dec_func_args.size(); ++i) {
          if(dec_func_args[i].get_type().get_id() != each_func_args[i].get_type().get_id()) {
            found = false;
            break;
          }
        }
        if(found) {
          // 找到匹配的函数, 将变量的地址和scope_id写入声明中
          for(size_t i = 0; i < dec_func_args.size(); ++i) {
            each_func_args[i].set_address(dec_func_args[i].get_address());
            each_func_args[i].set_scope_id(dec_func_args[i].get_scope_id());
          }
          ((ParseFunction *)each_func)->set_address(dec_func.get_address());
          break;
        }
      }
    }
    tree.set_parent($1, $$);
    tree.set_parent($2, $$);
    scope_now = ParseScope::get_scope(scope_now).get_parent_scope_id();
    if (generating_code) {
      ir.recordEnd();
    }
    this_function_symbol = "";
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
