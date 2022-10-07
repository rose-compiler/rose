#include <stdlib.h>

char* strcpy(char dest[], const char source[]) {
  char *temp = dest;
  int i = 0;
  //printf("strcpy: dest:%s source:%s\n",dest,source);
  while (1) {
    dest[i] = source[i];
    //printf("strcpy copy: %d\n",source[i]);
    if (dest[i] == '\0') {
      break;
    }
    i++;
  }
  return temp;
}

int main() {
  char target[3];
  strcpy(target,"abc"); // literal consists of 4 chars
}
