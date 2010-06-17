#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
  write(1, "ZZZWWW\n", 7);
  fputs("XXXYYY\n", stdout);
  fputs("XXXYYYXXXYYY\n", stdout);
  printf("Hello world\n");
  printf("World\n");
  printf("HelloWorldXXXYYY %d\n", argc);
  return 7;
}
