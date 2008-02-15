#include <stdlib.h>

char *globalString1 = "test1";
char *globalString2 = NULL;
char *globalString3;
char *globalString4 = "";

void foo()
   {
     char *localString1 = "local_test1";
     char *localString2 = NULL;
     char *localString3;
     char *localString4 = "";
   }

