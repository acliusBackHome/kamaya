// 测试
// 语义识别结构体声明
// 单语句多声明
// 识别类型 int, char short float double long bool 
// 函数正确重载声明定义
struct ABC {
    int a,b,c;
    char d;
    short e;
    float f;
    double g;
    long h;
    bool i;
};

int func() {
    return 0;
}

int func(int i) {
    return 0;
}

int func(char c) {
    return 0;
}

int main () {
    return 0;
}