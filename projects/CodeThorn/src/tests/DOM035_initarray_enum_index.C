#include <stdio.h>

enum BAR {
  ASLOT,
  BSLOT,
  CSLOT
};

int foo[2];

int main() {
  foo[ASLOT] = 10;
  if(foo[BSLOT] == 0) {
    /* do something */
    printf("foo[ASLOT]:%d\n",foo[ASLOT]);
    printf("foo[BSLOT]:%d\n",foo[BSLOT]);
  }
}
