//#include <stdio.h> 
int scanf(char*,int*);
int printf(char*,int);
#include <assert.h>

// MS: 

        // inputs
        int f= 6;

// reachable: 1,4,5
// NOT reachable: 3,6
int main()
{
    // main i/o-loop
    while(1)
    {
        // read input
        int input;
        scanf("%d", &input);        

        if(input!=1 && input!=2 && input!=3) {
          printf("%d",21);
          if(input==2) {
            printf("%d",22);
          error_3: assert(0);
          }
          if(input==4) {
            printf("%d",22);
          error_4:assert(0);
          }
          f=5;
          f=f+1;
          f=f+5;
        } else {
          if(input==1) {
           assert(0);
           f=42;
          }
          f=6;
          if(input!=3) {
          error_5: assert(0);
          }
          f=6;
          if(!(input==2||input==3)) {
          error_6: assert(0);            
          }
          f=6;
        }
        if(f==6) {
          assert(0);
        }
    }
}

