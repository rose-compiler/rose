
#include "assert.h"
#include <stdlib.h>

void testme() {
  // case 1
  int size = 5;
  char* str = (char*) malloc(size+1);
  if (str==NULL) {
    //*str = '5';
  }
  char z = *str;

  // case 2
  int *p = 0;
  int l = *p;
  
  // case 3
  char *k=0;
  free(k);

}


