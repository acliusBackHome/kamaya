// 测试:
// 编译期间常量计算优化
// 函数调用, 返回
// 输出
int func() {
    int y = 123 + 4567;
    return y;
}

int main() {
    int output;
    output = func();
    return 0;
}
