#include <setjmp.h>

#include "test-new.h"
#include "test-main.h"

const char* description = "Tests setjmp/longjmp";
const char* expectedout = "{0cWSsw1cWSr}";

jmp_buf ctx;

void service(int escape)
{
  printf("S");
  if (escape) longjmp(ctx, 1);
  printf("s");
}


void work(int escape)
{
  printf("W");
  service(escape);
  printf("w");
}

void test(int escape)
{
  printf("%c", '0' + escape);

  if (setjmp(ctx) == 0)
  {
    printf("c");
    work(escape);
  }
  else
  {
    printf("r");
  }
}

void run()
{
  test(0);
  test(1);
}

