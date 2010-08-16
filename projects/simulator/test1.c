#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    time(NULL);
  write(1, "ZZZWWW\n", 7);
  fputs("XXXYYY\n", stdout);
  fputs("XXXYYYXXXYYY\n", stdout);
  printf("Hello world\n");
  printf("World\n");
  
#if 0
  /* This next statement causes problems when stdout is a terminal and libc is compiled with gcc-4.3.2 (and possibly others).
   * One of the instructions that gets executed is
   *    0x4007ccd9:movdqu xmm0, DQWORD PTR ds:[edx]
   * which is not handled by ROSE's instruction semantics. [RPM 2010-07-29] */
  printf("HelloWorldXXXYYY %d\n", argc);
#endif
  return 7;
}
