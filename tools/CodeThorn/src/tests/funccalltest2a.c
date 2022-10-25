#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main() {
  char* p;
  char port;
  while(isdigit(*p) && (port<65535))
    {
      port = port*10+(*p) - '0';
      p++;
    }
  
  return 0;
}
