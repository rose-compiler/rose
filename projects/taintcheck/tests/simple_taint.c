// This is just a simple case showing how our SDG
// IFC analysis can find taint even when it is 
// passed out of a function and modified.

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

  x += 5;
#pragma leek trusted
  return x;
}
