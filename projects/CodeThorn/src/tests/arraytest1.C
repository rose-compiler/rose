// test for array/ptr mode
int input;
#include <stdio.h> 
#include <assert.h>
#include <math.h>
#include <stdlib.h>

int output;
// inputs
int inputs[] = {(2), (10), (9), (6), (1), (7), (4), (5), (8), (3)};
int a433 = 0;
int a434 = 2;
int a129[] = {(66), (67), (68), (69), (70), (71)};
int a116[] = {(72), (73), (74), (75), (76), (77)};
int a42[] = {(78), (79), (80), (81), (82), (83)};
int *a68 = a116;

int main()
{
  while(1){
    //if(a42[0]==a116[3]) {
    //  printf("%d",22);
    //}
    output = - 1;
    a433 = a42[1];
    a433 = a42[1+2];
    a433 = a42[a434+2];
    a434 = a116[1+2]+1;
    int input;
    scanf("%d",&input);
    if(a433==a129[1] && a434==a116[3] && a68[1]==5) {
      printf("%d",20);
    }
    if(input==1 && a433==82) {
    error_0: assert(0);
    }
    if(input==2 && a434==76) {
    error_1: assert(0);
    }
    if(input==3 && a116[0]==72) {
    error_2: assert(0);
    }
    if(input==4 && a68[0]==72) {
    error_3: assert(0);
    }
    if(input==5 && a68[1]==73) {
    error_4: assert(0);
    }
    a68=a129;
    if(input==6 && a68[0]==66) {
    error_5: assert(0);
    }
    if(input==7 && a68[1]==67) {
    error_6: assert(0);
    }
    if(input==8 && a68[1]!=66) {
    error_7: assert(0);
    }
    // not reachable
    if(input==9 && a68[1]==66) {
    error_8: assert(0);
    }
    if(1) {
    error_9: assert(0);
    }
  }
}
