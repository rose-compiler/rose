#include <stdio.h> 
#include <assert.h>

// MS: 

        // inputs
        int f= 6;

int calculate_output(int input) {
  if(input==21 && f==7) {
	f=6;
	return 22;
  } else if(f==6) {
	f=5;
	return 23;
  } else if(f==5) {
	f=4;
	return 24;
  } else if(f==4) {
	f=3;
  }
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
