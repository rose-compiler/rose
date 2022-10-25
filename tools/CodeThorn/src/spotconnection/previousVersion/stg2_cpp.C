#include <cstdio>
#include <cassert>

int input;

int main() {
  int output;
  scanf("%d",&input);
  if(input==1) {
    printf("%d",24); // X
    while(1) {
      scanf("%d",&input);
      if(input==2)
        printf("%d",25);
      else {
        if(input!=2) {
        error_1: assert(0);
        }
      }
    }
  }
  if(input!=1) {
  error_2: assert(0);
  }
  return 0;
}
