#include <stdlib.h>

int main() {
  int x;
  x= (rand() % 31) - 5; // random number between -5 .. 25
  int y=0;
  switch(x) {
  case 0 ... 10:
  case 11 ... 20:
    if(x <=10) { 
      y=1; // live
    }
    if(x < 15) { 
      y+=2; // live
    }
    if(x < 0) { 
      y+=4; // dead
    }
  };
}
