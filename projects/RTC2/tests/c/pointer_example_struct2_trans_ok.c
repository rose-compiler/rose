#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct VoidStr{
  void** ptr2ptr;
  void* L;
  void* H;
  void* lock_loc;
  unsigned long key;
} VoidStr;

typedef struct UserStruct{
  char var2;
  float var3;
}UserStruct;

void create_entry(UserStruct* structvar2, VoidStr x)
{
  structvar2->var2 = 'a';
  structvar2->var3 = 3.3;
}

int main()
{
  UserStruct structvar = {'a', 3.5};
  UserStruct *structvar2 = (UserStruct*)malloc(sizeof(UserStruct));
  VoidStr structvar2_structed;
  create_entry(structvar2, structvar2_structed);
  structvar2->var2 = 'a';
  structvar2->var3 = 3.5;

  if (structvar2->var2  == 'a') free(structvar2);
  return 0;
}
