#include <stdio.h> 
#include <assert.h>

// inputs
int b=2;
//oututs
int u=21;

// this example has a larger cycle in the STG

int a1=1;
int calculate_output(int input1) {
  if(input1==2 && a1==2) {
    a1=1;
    printf("%d\n", a1);
    return 21;
  } else if(input1==1 && a1==1) {
    a1 = 2;
    printf("%d\n", a1);
    return 22;
  } else {
    return 20;
  }
}

int main() {
    int output = -1;
    while(1) {
        int input;
        scanf("%d", &input);        
        output = calculate_output(input);
        printf("%d\n", output);
    }
    return 0;
}
