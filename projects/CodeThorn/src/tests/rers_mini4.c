#include <stdio.h> 
#include <assert.h>

// MS: 
// inputs
int f= 6;

int calculate_output2(int input) {
  if(input==21) {
	return 23;
  } else {
	assert(0);
  }
}

int calculate_output(int input) {
  if(input==21 && f==6) {
	assert(0);
  }
  return calculate_output2(input);
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
