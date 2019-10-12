let tokenArray = `%token MAIN
%token LP RP LB RB ML MR
%token SEMICOLON
%token IF ELSE WHILE
%token RELOP NUMBER ID
%token INT CHAR VOID
%token ASSIGN
%token NOT OR AND
%token LOGICAND LOGICOR
%token ADD SUB MUL DIV MOD POW`
tokenArray = tokenArray.replace(/%token/g, " ");
tokenArray = tokenArray.replace(/\n/g, " ");
tokenArray = tokenArray.split(" ");
for (let i of tokenArray) {
  if (i === '') {
    continue;
  }
  console.log(`nameTable[(int)${i}] = enum2str(${i});`);
}