void main() {
    int a, b = 1;
    char* c = "123abc";
    1 = 2;
    a = c[0];
    a[0] = a;
    b = c;
    a = b + 10 / 2 ^ 3 ^ 2 ^ 3; // pay attention to operation priority
    if (a != 0) {
        while (a < 10 && b < 10) {
            a = a * b;
            b = b + 2;
        }
        for (int i = 0; i < 1; i++) { // it's OK if you cannot handle the operator "++"
            i = b;
        }
    }
    print(a); // you can change this to your output function
}
