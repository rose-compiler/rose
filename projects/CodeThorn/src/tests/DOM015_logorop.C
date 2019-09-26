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
  if(twoIntsStructPointer==0 || twoIntsStructPointer!=0) {
    x++;
  }
  return 0;
}
