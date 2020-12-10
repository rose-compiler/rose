#include "DOM.h"
int main() {
  {signed char            x='a'; printf("%d\n",x);}
  {signed short     int   x=500; printf("%d\n",x);}
  {signed           int   x=500; printf("%d\n",x);}
  //{signed long      int   x=500; printf("%ld\n",x);} not supported
  //{signed long long int   x=500; printf("%lld\n",x);} // l64d on Windows
  {unsigned char          x='a'; printf("%d\n",x);}
  {unsigned short     int   x=500; printf("%d\n",x);}
  {unsigned           int   x=500; printf("%d\n",x);}
  //{unsigned long      int   x=500; printf("%lu\n",x);} not supported
  //{unsigned long long int   x=500; printf("%llu\n",x);} // l64u on Windows
  return 0;
}
