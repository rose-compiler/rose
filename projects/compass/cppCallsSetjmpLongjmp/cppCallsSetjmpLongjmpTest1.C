#include <setjmp.h>

int main()
{
  jmp_buf env;

  setjmp(env);
  longjmp(env,2);

  return 0;
} //main()
