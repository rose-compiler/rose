#include <stdarg.h>

int vatest(int x, ...) {
     va_list ap;
     int sum = 0;
     va_start(ap, x);
     while (x--)
        {
          sum += va_arg(ap, int);
        }
     va_end(ap);
     return sum;
}

int test(int x) {
     return vatest(5, 4, 3, 2, 1, 0);
}
