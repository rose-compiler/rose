#include <string.h>
#include <stdlib.h>

int main() {
  /* ... */
  char buff[256];
  strcpy(buff, getenv("EDITOR"));
  /* ... */
  return 0;
}

