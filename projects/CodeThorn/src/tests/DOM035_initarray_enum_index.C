#include <stdio.h>

enum BAR {
  ASLOT,
  BSLOT,
  CSLOT
};

int globalArray[2];

int main() {
  globalArray[ASLOT] = 10;
  globalArray[BSLOT] = 101;
  if(globalArray[BSLOT] == 101) {
    /* do something */
    printf("globalArray[ASLOT]:%d\n",globalArray[ASLOT]);
    printf("globalArray[BSLOT]:%d\n",globalArray[BSLOT]);
  }
}
