int main() {
  int output, input;
  if (input > 0) {
      int t = 1, p = 0;
      while (t <= input) {
          p = p + t ^ 2;
          t = t + 1;
     }
     output = p;
  } else {
    output = -1234;
  }
  return 0;
}