#include <stdio.h> 
#include <assert.h>

// inputs
int b=2;
//oututs
int u=21;

int a1=1;

int calculate_output(int input1) {
  if(input1==2) {
    return 21;
  } else {
  error_1: assert(0);
  }
}

int main() {
    int output = -1;
    while(1) {
        int input;
        scanf("%d", &input);        
        output = calculate_output(input);
        if(output == -2)
          fprintf(stderr, "Invalid input: %d\n", input);
        else if(output != -1)
          printf("%d\n", output);
    }
}
