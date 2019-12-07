#include "foo.h"
void _start() {
  PRINT("%d", 1);
  // 这里产生了一个段错误，即使不调用 PRINT 也会产生，原因尚不明确
}