//#include <iostream>

// DQ (9/9/2009): Fudora version 9 requires stdlib.h to define the "exit()" function.
#include <stdlib.h>


using namespace std;

int main(int argc, char** argv) {

  int* x= new int[2];
  int* k ;

  x[1]=2;
  x[0]=3;

  int y=x[5];
  int a=x[0]/y;

  x[0]=*k;
}
