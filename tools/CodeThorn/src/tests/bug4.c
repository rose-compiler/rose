#include <stdlib.h>
#include <stdio.h>

int main() {

  int i = rand();
  switch(i) 
  {
    case 1804289000 ... 1900000000:  //My rand always returns the same number in this range, you may need to change this.
      int tmp;
      tmp=i+1;
      tmp=i/2;
      tmp=i%2;
      tmp=i%10;
      tmp=i%100;
      if(tmp == 0) {
	printf("good %d\n", i);
      } else {
	printf("bad %d\n", i);
      }
  }
  return i;
}
