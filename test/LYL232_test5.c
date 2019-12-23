// 测试:
// 识别字符串
// 识别指针
// 类型判断: 没有定义 char*和 int 的+运算符
// 错误恢复
int main () {
    char *c = "abcdefg";
    int a, output;
    output = a + c;
    return 0;
}