#include <stdio.h> 
#include <assert.h>

// MS: 
// inputs
int f= 6;

extern void __VERIFIER_error();

int calculate_output(int input) {
  if(input==1 && f==7) {
    f=6;
    if(f!=6) {
       assert(0); // unreachable!
    }
    return 23; // W (unreachable!)
  } else if(input==1 && f==6) {
    f=5;
    return 24; // X
  } else if(f==5) {
    f=4;
    return 25; // Y
  } else if(f==4) {
    f=3;
  } else {
    if(f==3) {
      __VERIFIER_error(); // reachable
    }
  }
  if(f==2) {
    // intentionally empty block to be reduce
  }
  if(input==7)
    return -2;
  return -1;
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
        output = calculate_output(input);

        if(output == -2)
                fprintf(stderr, "Invalid input: %d\n", input);
        else if(output != -1)
                        printf("%d\n", output);
    }
}
