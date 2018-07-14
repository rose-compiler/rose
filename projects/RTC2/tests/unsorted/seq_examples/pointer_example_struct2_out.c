#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "foo2_c.h" 
typedef struct UserStruct {
char var2;
float var3;}UserStruct;

int main()
{
  StartClock();
  UserStruct structvar = {('a'), (3.5)};
  UserStruct *structvar2 = (UserStruct *)(malloc((sizeof(UserStruct ))));
  structvar2 -> UserStruct::var2 = 'a';
  structvar2 -> UserStruct::var3 = 3.5;
  EndClock();
  return 1;
}
