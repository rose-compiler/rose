#include <cassert>
#include "DOM.h"

typedef struct _twoIntsStruct
{
  int intOne;
  int intTwo;
} twoIntsStruct;

int main() {
  int x=1;
  twoIntsStruct s;
  twoIntsStruct* twoIntsStructPointer = &s;
  twoIntsStructPointer->intOne=5;
  /* FIX: Use && in the if statement so that if the left side of the expression fails then
   * the right side will not be evaluated */
  printf("twoIntsStructPointer->intOne:%d\n",twoIntsStructPointer->intOne);
  bool cond1;
  cond1=twoIntsStructPointer != NULL;
  printf("cond1:%d\n",cond1);
  bool cond2=twoIntsStructPointer->intOne == 5;
  printf("cond2:%d\n",cond2);
  if ((twoIntsStructPointer != NULL) && (twoIntsStructPointer->intOne == 5)) {
    x=2;
  }
  //  assert(x==2);
  printf("x:%d\n",x);
  return 0;
}
