// This code has a simplified representation in test2015_53.c and test2015_93.c.

// This test code demonstrates that the use of the GNU language extension (statement-expression)
// in the default statement of the switch causes the while statement after the switch to not
// be unparsed (maybe even not in the AST).

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

char *findneedle(char *haystack)
{
  return  ({ char * random_var_name179345532=(char *)strstr(haystack,"needle");random_var_name179345532; }) ;
// need to wrap function above since we can't put a free here after return!
}

int main(int argc,char **argv)
{
  const char *test = "testval";
  int x = 5;
  printf("testing special character: ");
// should not split these:
  printf("\\");
  printf("\n");
  printf("Now I know my a b c'");
// should split 's
  printf("s\n");
  if ( ({ char * random_var_name179345533=(char *)strstr(test,"val");random_var_name179345533; })  ||  ({ char * random_var_name179345534=(char *)strstr(test,"test");random_var_name179345534; }) ) {
    printf("Found test or val!\n");
    printf("%d\n",x);
    goto badwrap;
    x++;
    badwrap:
// better not put a declaration here! should add null statement
    printf("5 should equal %d\n",x);
  }
  printf("I found: %s\n",(findneedle("Is the word needle in this haystack?")?"needle" : "nothing"));
  switch( ({ char * random_var_name179345535=(char *)strstr(test,"val");random_var_name179345535; })  == ((void *)0)){
    case 0:
{
      printf("Case zero!\n");
      break; 
    }
    default:
     ({ int random_var_name179345536=(int)printf("Default case!\n");random_var_name179345536; }) ;
  }
  while( ({ int random_var_name179345537=(int)printf("Hi there\n");random_var_name179345537; }) ){
    break; 
  }
// should print true
  if ( ({ char * random_var_name179345538=(char *)strstr((x?"(bool)x = true" : "x!=0\n"),"true");random_var_name179345538; }) ) {
    printf("%s\n",(strstr((x?"(bool)x = true" : "x!=0\n"),"true")));
  }
// should NOT print true, should print false
  if ( ({ char * random_var_name179345539=(char *)strstr((x?"(bool)x = true" : "x!=0\n"),"false");random_var_name179345539; }) ) {
    printf("%s\n",(strstr((x?"(bool)x = true" : "x!=0"),"true")));
  }
   else {
    printf("%s\n",(strstr((!x?"(bool)x = true" : "!(bool)x = false"),"false")));
  }
  do {
    printf("In Do While Loop!\n");
  }while (! ({ int random_var_name179345540=(int)printf("Goodbye!\n");random_var_name179345540; }) );
  return 0;
}
