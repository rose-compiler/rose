#include <stdio.h>

int main() {
  char str[80];
  int x;
  scanf("%s",str);
  scanf ("%79s",str);
  scanf ("%d",&x);
  
  printf("output: %d %s",x,str);

  return 0;
}
