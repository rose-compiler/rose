#include <stdio.h> 
#include <assert.h>

// MS: 

        // inputs
        int f= 6;

void calculate_output(int input) {
  if(input==1 && f==7) {
    f=6;
    if(f!=6) {
    error_1: assert(0); // unreachable!
    }
    printf("%d",23); // W (unreachable!)
  } else if(input==1 && f==6) {
    f=5;
    printf("%d",24); // X
  } else if(f==5) {
    f=4;
    printf("%d",25); // Y
  } else if(f==4) {
    f=3;
  } else {
    if(f==3) {
    error_2: assert(0); // reachable
    }
  }
  if(f==2) {
    // intentionally empty block to be reduce
  }
  printf("%d",-1);
}


int main()
{
    // default output
    int output = -1;

    // main i/o-loop
    while(1)
    {
        // read input
        int input;
        scanf("%d", &input);        

        // operate eca engine
        calculate_output(input);

        if(output == -2)
                fprintf(stderr, "Invalid input: %d\n", input);
    }
}
