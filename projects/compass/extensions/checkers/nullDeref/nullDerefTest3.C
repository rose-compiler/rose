#include "assert.h"
#include <stdlib.h>

void testme() {
  int *m = 0;
  int *p = m;
  *p = *m;
  int l = *p;

  if (p != NULL) {
    *p;
  }

  assert (m != NULL);
  *m;

  p[0] = 5;
}


