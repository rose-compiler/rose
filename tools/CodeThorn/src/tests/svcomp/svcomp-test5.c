#include <stdio.h> 

// MS: 
// inputs
int f= 6;

extern void __VERIFIER_error();
extern int __VERIFIER_nondet_int(void);
extern void exit(int);

int calculate_output(int input) {
  if(input==1 && f==7) {
    f=6;
    if(f!=6) {
       exit(0); // unreachable!
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
      //__VERIFIER_error(); no reachable __VERIFIER error
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
	input=__VERIFIER_nondet_int();

        // operate eca engine
        output = calculate_output(input);

        if(output == -2)
                fprintf(stderr, "Invalid input: %d\n", input);
        else if(output != -1)
                        printf("%d\n", output);
    }
}
