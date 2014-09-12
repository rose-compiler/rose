#include <stdio.h> 
#include <assert.h>

// inputs
int b=2;
//oututs
int u=21;

int a1=1;
int calculate_output(int input1) {
  if(input1==2 && a1==2) {
    return 21;
  } else if(input1==3) {
    return 22;
  } else if(input1!=4) {
    return 23;
  } else {
  error_0: assert(0);
  }
}

int main() {
    int output = -1;
    while(1) {
        int input;
        scanf("%d", &input);        
        output = calculate_output(input);
        printf("%d\n", output);
        // State={(a1,1), (output,22), (input,top)}, constraints={input==3}
        // State={(a1,1), (output,23), (input,top)}, constraints={input==2}
        // State={(a1,1), (output,23), (input,top)}, constraints={input!=2,input!=3,input!=4}
    }
    return 0;
}
