// 测试:
/*
ajshdisahdiahdiashdaisdaisjdaishdi
*/
// 1.注释
// 2.输入输出
// 3.幂运算
// 4.while循环
// 5.if else 语句
// 6.大于等于号, 小于等于号
// 功能: 输出 前n个自然数的平方和
int main() {
  int output, input;
  if (input >= 0) {
      int t = 1, p = 0;
      while (t <= input) {
          p = p + t ^ 2;
          t = t + 1;
     }
     output = p;
  } else {
    output = -1234;
  }
  return 0;
}