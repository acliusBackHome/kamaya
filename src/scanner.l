%option yylineno

%{
#include "kamaya.hpp"

using namespace std;
using namespace x86;

map<int, string> nameTable;
map<string, string> symbolTabel;
map<string, string> stringTabel;
vector<string> filestrings, errorStrings, parse_error_strs;
ParseTree tree("program");
string before;
int yycolumn = 0;
bool generating_code = true;
FILE* file;
Assembler assembler(cout);
IR ir;

unsigned long install_id() {
  before = yytext;
  if (symbolTabel.find(yytext) != symbolTabel.end()) {
    return (unsigned long)symbolTabel[yytext].c_str();
  }
  symbolTabel[yytext] = yytext;
  return (unsigned long)symbolTabel[yytext].c_str();
}

unsigned long install_num() {
  if (symbolTabel.find(before) != symbolTabel.end()) {
    return atoi(yytext);
  } else {
    return atoi(yytext);
  }
}

unsigned long install_string() {
  // 因为这里匹配的字符串包括了两端的引号,所以要去除引号
  before = yytext;
  if(!before.empty()) {
    if(before[0] == '"') {
      before.erase(0, 1);
    }
    if(before[before.length() - 1] == '"') {
      before.erase(before.length() - 1, 1);
    }
  }
  if (stringTabel.find(before) != stringTabel.end()) {
    return (unsigned long)stringTabel[before].c_str();
  }
  stringTabel[before] = before;
  return (unsigned long)stringTabel[before].c_str();
}
%}

delim                               [\t\r\f\v ]
lf                                  \n
ws                                  {delim}+
letter                              [A-Za-z]
digit                               [0-9]
id                                  {letter}({letter}|{digit})*
number                              {digit}+(\.{digit}+)?(E[+\-]?{digit}+)?

%%

{lf}                                {yycolumn = 0;}
{ws}                                {}
{digit}+{letter}                    {return ERRORFORMAT;}
L?\"(\\.|[^\\"\n])*\"	              {yylval = install_string(); return STRING_LITERAL;}
"void"                              {return VOID;}
"char"                              {return CHAR;}
"bool"                              {return BOOL;}
"float"                             {return FLOAT;}
"long"                              {return LONG;}
"short"                             {return SHORT;}
"double"                            {return DOUBLE;}
"main"                              {return MAIN;}
"return"                            {return RETURN;}
"("                                 {return LP;}
")"                                 {return RP;}
"{"                                 {return LB;}
"}"                                 {return RB;}
"["                                 {return ML;}
"]"                                 {return MR;}
"int"                               {return INT;}
";"                                 {return SEMICOLON;}
","                                 {return COMMA;}
"..."                               {return ELLIPSIS;}
"."                                 {return POINT;}
"->"                                {return PTR_OPERATOR;}
"if"                                {return IF;}
"else"                              {return ELSE;}
"switch"                            {return SWITCH;}
"case"                              {return CASE;}
"default"                           {return DEFAULT;}
"while"                             {return WHILE;}
"for"                               {return FOR;}
"do"                                {return DO;}
"goto"                              {return GOTO;}
"continue"                          {return CONTINUE;}
"break"                             {return BREAK;}
"restrict"                          {return RESTRICT;}
"volatile"                          {return VOLATILE;}
"extern"                            {return EXTERN;}
"static"                            {return STATIC;}
"auto"                              {return AUTO;}
"register"                          {return REGISTER;}
"inline"                            {return INLINE;}
"sizeof"                            {return SIZEOF;}
"struct"                            {return STRUCT;}
"union"                             {return UNION;}
"enum"                              {return ENUM;}
"true"                              {return TRUE;}
"false"                             {return FALSE;}
{id}                                {yylval = install_id(); return ID;}
{number}                            {yylval = install_num(); return NUMBER;}
"<"                                 {return LT;}
"<="                                {return LE;}
"=="                                {return EQ;}
"!="                                {return NE;}
">"                                 {return GT;}
">="                                {return GE;}
"="                                 {return ASSIGN;}
"*="                                {return MUL_ASSIGN;}
"/="                                {return DIV_ASSIGN;}
"%="                                {return MOD_ASSIGN;}
"+="                                {return ADD_ASSIGN;}
"-="                                {return SUB_ASSIGN;}
"<<="                               {return LEFT_ASSIGN;}
">>="                               {return RIGHT_ASSIGN;}
"&="                                {return AND_ASSIGN;}
"^="                                {return XOR_ASSIGN;}
"|="                                {return OR_ASSIGN;}
"!"                                 {return NOT;}
"~"                                 {return BIT_NOT;}
"&&"                                {return LOGICAND;}
"&"                                 {return AND;}
"||"                                {return LOGICOR;}
"|"                                 {return OR;}
\/\/[^\n]*                          {}
\/\*(\*+[^\/]|[^\*\/])*\*\/         {}
"++"                                {return ADDONE;}
"+"                                 {return ADD;}
"--"                                {return SUBONE;}
"-"                                 {return SUB;}
"*"                                 {return MUL;}
"/"                                 {return DIV;}
"%"                                 {return MOD;}
"^"                                 {return XOR;}
"<<"                                {return LEFT_OP;}
">>"                                {return RIGHT_OP;}
"?"                                 {return QUESTION_MARK;}
":"                                 {return COLON;}
.                                   {
                                      string msg = "error: stray ‘" + string(yytext) + "’ in program";
                                      yyerror(msg.c_str());
                                      cout << errorStrings[errorStrings.size() - 1] << endl;
                                      exit(0);
                                    }
%%

int yywrap()
{
  return 1;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << argv[0] << ": fatal error: no input files" << endl
         << "compilation terminated." << endl;
    return 1;
  }
  inputFile = argv[1]; // In fact, this is exec file
  char strline[2048];
  if (!(file = fopen(argv[1], "r"))) {
    cout << argv[0] << ": error: " << argv[1] << ": " << "No such file or directory" << endl
         << argv[0] << ": fatal error: no input files" << endl
         << "compilation terminated." << endl;
    return 1;
  }
  while (fgets(strline, sizeof(strline), file) != NULL) {
    int len = strlen(strline);
    if (strline[len - 1] == '\n') {
      strline[len - 1] = 0;
    }
    filestrings.push_back(strline);
  }
  rewind(file);
  yyset_in(file);
  initName();
  try {
    yyparse();
    // 连接错误结点
    tree.last_combine();
    tree.print();
  } catch (ParseException &exc) {
    printf("未处理异常:%s\n", exc.get_info().c_str());
    tree.print();
  }

  fclose(file);

  printf("节点数%lu\n已扫描的行数 %d \n", tree.get_node_num(), yylineno);
  printf("所有类型:\n");
  ParseType::print_all_type();
  printf("所有表达式:\n");
  ParseExpression::print_all_expression();
  printf("所有语句块空间声明:\n");
  ParseScope::print_all_declaration();
  int sz = tree.error_cnt();
  if (sz || !parse_error_strs.empty()) {
    cout << "错误报告" << endl;
    // 语法错误
    for(int i = 0; i < sz; i++) {
      size_t eid = tree.get_error(i);
      size_t pid = tree.get_parent(eid);
      cout << inputFile + ": In " << to_string(pid) << ": " << tree.get_msg(pid) << endl;
      cout << errorStrings[i] << endl;
    }
    // 语义错误
    for(const auto &each : parse_error_strs) {
      cout << inputFile << ":" << each << endl;
    }
  }
  ir.print();
  // assembler.setSectionData(ParseScope::emitData2Nasm());
  // assembler.setSectionText(ParseScope::emitText2Nasm());
  // printf("ParseTree 2 Nasm x86\n");
  // assembler.handleNasm();
  return  0;
}
