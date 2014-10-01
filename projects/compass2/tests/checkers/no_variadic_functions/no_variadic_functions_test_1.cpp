// 1 Matches
#include <stdarg.h>

double average(int count, ...)
{
    va_list ap;
    int j;
    double tot = 0;
    va_start(ap, count); /* Requires the last fixed parameter (to get the address) */
    for(j = 0; j < count; j++)
        tot += va_arg(ap, double); /* Increments ap to the next argument. */
    va_end(ap);
    return tot / count;
}

int non_variadic()
{
  return 7;
}

int main()
{
  return 0;
}
