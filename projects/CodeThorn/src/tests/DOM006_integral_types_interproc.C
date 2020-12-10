#include "DOM.h"
void f() {
  {signed char            x='a';}
  {signed short     int   x=500; }
  {signed           int   x=500; }
  {signed long      int   x=500; }
  {signed long long int   x=500; }
  {unsigned char          x='a';}
  {unsigned short     int   x=500; }
  {unsigned           int   x=500; }
  {unsigned long      int   x=500; }
  {unsigned long long int   x=500; }
}

int main() {
  f();
  return 0;
}
