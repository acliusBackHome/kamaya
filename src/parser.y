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
%token SEMICOLON
%token IF ELSE WHILE
%token RELOP NUMBER ID
%token INT CHAR VOID
%token ASSIGN
%token NOT OR AND
%token LOGICAND LOGICOR
%token ADDONE SUBONE ADD SUB MUL DIV MOD POW
%token ERRORFORMAT

%left ADD SUB
%left MUL DIV 
%right U_neg POW

%%
PROGRAM: S {}
 | S PROGRAM {}
;

S:
  ID ASSIGN EXPR {
      cout<<"eq:" << $1 << ' '<< $3 << endl;
    }
  | EXPR SEMICOLON {
      cout<< "expr:" << $1 << endl;
    }
;

EXPR:
// $$表示式子左边的成员，$1，$3分别表示式子右边的第一个成员和第二个成员
// （空格分割）
  EXPR ADD  EXPR {
    $$ = $1  + $3; cout << $1 << " + " << $3 << endl;
  }
| EXPR SUB EXPR {
    $$ = $1  - $3; cout  << $1 << " - " << $3 << endl;
  }
| EXPR MUL EXPR {
  $$ = $1  * $3; cout << $1 << " * " << $3 << endl;
}
| EXPR DIV EXPR {
  $$ = $1  / $3; cout << $1 << " / " << $3 << endl;
}
| LP EXPR RP {
    $$ = $2; cout << "(" << $2 << ")" << endl;
  }
| NUMBER {
    $$ = $1; cout << "number: " << $1 << endl; 
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