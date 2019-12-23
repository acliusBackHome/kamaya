
/*
 * 静态数据调用
 * 访问外层作用域变量
 */

int x;
int func(){
  int y = 123;
  return y;
}
int main() {
  x = func();
  int output = x;
  return 0;
}