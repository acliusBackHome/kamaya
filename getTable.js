let tokenArray = `%token MAIN
%token LP RP LB RB
%token SEMICOLON
%token IF ELSE
%token RELOP NUMBER ID
%token INT CHAR VOID
%token ASSIGN
%token NOT OR`
tokenArray = tokenArray.replace(/%token/g, " ");
tokenArray = tokenArray.replace(/\n/g, " ");
tokenArray = tokenArray.split(" ");
for (let i of tokenArray) {
  if (i === '') {
    continue;
  }
  console.log(`nameTable[(int)${i}] = enum2str(${i});`);
}