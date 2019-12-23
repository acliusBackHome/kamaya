// 1.注释
// 2.表达式运算(+ - * /)
// 3.if跳转测试(< > == != <= >= 跳转测试)

int main() {
    int output = 3, input;
    if (input != 2) {
        if (input > 2) {
            output = input * 10;
            if (input < 10) {
                output = output + 2;
                if (input <= 8) {
                    output = output + 1;
                    if (input >= 4) {
                        output = output - 20;
                        if (input == 6) {
                            output = output + 3;
                        }
                        else {
                            output = input + 100;
                        }
                    }
                    else {
                        output = input + 10;
                    }
                }
                else {
                    output = input + 5;
                }
            }
            else {
                output = input * 4;
            }
        }
        else {
            output = input * 3;
        }
    }
    else {
        output = input + 3;
    }
    return 0;
}