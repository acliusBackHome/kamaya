// 测试
// 同名多次声明
// 参数与函数不匹配
// 初始化列表中声明指针和原类型变量
int func(int a, int b, int c) {
    return 0;
}

int main(int argc, char** argv) {
    char a, b, c , *d;
    return func(argc);
}