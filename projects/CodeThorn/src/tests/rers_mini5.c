#include <stdio.h> 
#include <assert.h>

// MS: 
// inputs
int f= 6;

int calculate_output2(int input2) {
  if(input2==21) {
    return 23;
  }
  else if(input2==22 && f==7) {
    return 24;
  }
  return 21;
}

int calculate_output(int input1) {
  if(input1==21 && f==6) {
    f=7;
  }
  return calculate_output2(input1);
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
