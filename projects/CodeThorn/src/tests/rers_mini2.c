#include <stdio.h> 
#include <assert.h>

// MS: this program does NOT generate output but finishes because of an assert
int f=5;

int calculate_output(int input) {
  if(f==5)
    assert(0);
  return 6;
}

int main()
{
  int input;
  scanf("%d",&input);
  int output;
  output = calculate_output(input);
  printf("%d",output);
  return 0;
}
