void putchar(int a){
int main() {
    int a, b = 1;
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
    putchar(a) // you can change this to your output function
}