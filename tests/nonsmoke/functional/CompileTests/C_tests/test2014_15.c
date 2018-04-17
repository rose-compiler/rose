# define EXTERNALLY_VISIBLE __attribute__(( visibility("default") ))

void foo(int x) EXTERNALLY_VISIBLE;
void foo(int x)
   {
   }

