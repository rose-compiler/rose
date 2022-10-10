#include <cassert>
#include "DOM.h"

typedef struct _twoIntsStruct
{
  int intOne;
  int intTwo;
} twoIntsStruct;

int main() {
  int x=1;
  twoIntsStruct* twoIntsStructPointer = NULL;
  if(twoIntsStructPointer==0 || twoIntsStructPointer!=0) {
    x++;
  }
  assert(x==2);
  printf("x:%d\n",x);
  return 0;
}
