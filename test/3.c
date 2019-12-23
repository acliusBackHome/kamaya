int main() {
  int output;
  if (false) {
      int t = 0;
      int p = 0;
      while (t < 3) {
          p = p + t ^ 2;
          t = t + 1;
     }
     output = p;
  } else {
    output = -1234;
  }
  return 0;
  // test 3
}