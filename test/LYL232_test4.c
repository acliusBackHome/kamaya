// 测试:
// 错误的函数重载: 不同的返回值
// 错误恢复, 跳过错误声明, 继续扫描生成语法树
int func() {
    return 0;
}

char func(char c) {
    return c;
}

int main () {
    return 0;
}