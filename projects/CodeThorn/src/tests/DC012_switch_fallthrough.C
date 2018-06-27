#include <stdlib.h>

int main() {
  int x;
  x= rand() % 30 - 5; // random number between -5 .. 24
  int y=0;
  switch(x) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 10:
    if(x <=10) { 
      y=1; // live
    }
    if(x == 1) { 
      y+=2; // live
    }
    if(x < 0) { 
      y+=4; // dead
    }
  break;
 default:
   y+=8;
  };
}
