#include <stdio.h>

int main() {
  char str[80];

  int i, n=2;
  FILE *fptr = fopen("IOC002.txt", "w");
  if (fptr == NULL) {
    printf("Could not open file");
    return 0;
  }
   
  for (i = 0; i < n; i++) {
    puts("Enter a name");
    scanf("%[^\n]%*c", str);
    fprintf(fptr,"%d.%s\n", i, str);
  }
  fclose(fptr);
  return 0;
}
