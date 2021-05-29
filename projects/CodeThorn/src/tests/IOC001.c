#include <cstdio>

int main() {
  char s[80];
  int x;
  scanf("%s",s);
  scanf ("%79s",s);
  scanf ("%d",&x);
  
  printf("output: %d %s",x,s);
  
  FILE *fptr = fopen("sample.txt", "w");
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
