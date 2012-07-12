#include <stdio.h>
#include <stdlib.h>

char* foo (char * input) {
   char* p, *q, *r;
   char** pp; 
  char m;
  q = &m;
  r = q;
   pp = &p;
   *pp = r;
  char * p1 = *pp + 2;
   return input;
}


void goo (char* f(char*))
{
  char* content = "dklafj";
  f(content);
   char* content2 = new char[4];
   char* output = foo(content);
   goo(foo);
}

void test() 
{
        int intArray[10];
       int *intArray2 = (int *) malloc(sizeof(int) * 10);

        intArray2[1] = 1;
        intArray[1] = 1;
}

int main(int argc, char* argv[]) {

        int i;
        int a = 0, b = 0;
        i = 0;
        while(i < 10) {
                b-=5;
                i++;
        }
        printf("%d %d\n", a,b);

}
