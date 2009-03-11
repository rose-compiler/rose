// This test shows how sanatization works.  By placing 
// #pragma leek sanatized before x+=5;, we are telling the
// IFC system that x is now trusted data.
// Normally, of course, sanitization would be on a 
// function that would actually process the data in some way.


#include <stdio.h>

char newchar() {
  char c = 0;
#pragma leek tainted
  c = getchar();
  return c;
}

int main() {

  int x;
  
  x=11;

  x = newchar();

#pragma leek sanatized
  x += 5;
#pragma leek trusted
  return x;
}
