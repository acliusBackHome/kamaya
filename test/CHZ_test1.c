// 测试:
// 1.注释
// 2.while循环
// 3.表达式运算(+ - * / % ^) 表达式赋值
// 4.if语句
// 5.初始化赋值
// 6.自动类型转换(if括号内表达式类型转换)
// 7.常量运算优化

int main() {
    int output = 3;
    int input;
    if(input) {
        output = input * 4 / 2 + 1 - 2 ^ 2 % 3;
        while(input > 0) {
            output = output + input;
            input = input - 1;
        }
    }
    return 0;
}