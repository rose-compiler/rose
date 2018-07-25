#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1


struct GStruct {
  int* ptr2obj;
};

typedef struct GStruct IntStruct;

int main()
{
  IntStruct ptr;
  IntStruct ptr2;
  ptr.ptr2obj = (int*)malloc(PTR_SIZE*sizeof(int));
  ptr2.ptr2obj = (int*)malloc(PTR2_SIZE*sizeof(int));

  IntStruct start_ptr = ptr;
  IntStruct start_ptr2 = ptr2;

  return 0;
}
