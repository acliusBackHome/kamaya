
/*
 * 双层循环
 */
int main() {
  int output = 0;
  for (int i = 1; i <= 9; i = i + 1) {
    for (int j = i; j <= 9; j = j + 1) {
      output = output + i * j;
    }
  }
  return 0;
}