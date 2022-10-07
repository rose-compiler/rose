#include <stdio.h>

#define BUG

/* MS: this testcase shows
1) that the constraint-sets also contribute to 
context information, as states are kept separate if the constraint sets
are different.
2) that constraints are propagated from the caller to the calle's argument
variables. Therefore for tests in the analyser it is sufficient to test the constraints on the calle's formal arguments
since they carry all information with them. This may change when we use a canonical representation of constraints (the tests on the constraints need to be adapted then)
*/

/*

0 -> 5 
1 -> 1 a->a
2 -> 3 b->c
3 -> 4 c->d
4 -> 2 d->b
5 -> 5 x->e when x!=a and x!=b and x!=c and x!=d
6 -> 5
7 -> 5
8 -> 5
9 -> 5
10 -> 5

 */


int x=0;

int f2(int input2) {
  if(input2==2)
    return 3;
  else
    return 4;
}

int f1(int input1) {
  if(input1==1)
    return 1;
  else
    return 2;
}

#ifndef BUG
int f3(int input3) {
  if(input3==2)
    return f1(input3);
  if(input3==1)
    return f2(input3);
  else
    return 5;
}
#endif 

int main() {
  int input, output;
  scanf("%d",&input);
  output=6;
  if(input==1)
    output=f1(input);
  else if(input==2)
    output=f2(input);
#ifndef BUG
  else if(input==3)
    output=f3(1);
  else if(input==4)
    output=f3(2);
  else
    output=f3(input);
#endif
  if(output != -1)
    printf("%d",output);
  return 0;
}

