#include <stdio.h>
#include <string.h>

#define SRC_STR "0123456789abcdef0123456789abcde"

void printLine(char* s) {
  printf("%s",s);
}

int* g(int* x) {
  return x;
}

typedef struct _charVoid
{
  char charFirst[16];
  void * voidSecond;
  void * voidThird;
} charVoid;

void f()
{
  charVoid structCharVoid;
  structCharVoid.voidSecond = (void *)SRC_STR;
  printLine((char *)structCharVoid.voidSecond);
  memcpy(structCharVoid.charFirst, SRC_STR, sizeof(structCharVoid));
  structCharVoid.charFirst[(sizeof(structCharVoid.charFirst)/sizeof(char))-1] = '\0';
  printLine(((char *)structCharVoid.charFirst)+1);
  printLine((char *)structCharVoid.voidSecond);
}

int main() {
  f();
  return 0;
}
