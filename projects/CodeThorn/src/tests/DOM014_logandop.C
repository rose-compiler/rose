#include <cassert>

typedef struct _twoIntsStruct
{
  int intOne;
  int intTwo;
} twoIntsStruct;

#define NULL 0

int main() {
  int x=1;
  twoIntsStruct* twoIntsStructPointer = NULL;
  /* FIX: Use && in the if statement so that if the left side of the expression fails then
   * the right side will not be evaluated */
  if ((twoIntsStructPointer != NULL) && (twoIntsStructPointer->intOne == 5)) {
    x=2;
  }
  assert(x==2);
  return 0;
}
