#include <stdio.h>

extern void adainit (void);
extern void adafinal (void);
extern void ada_proc (void);

int main (int argc, char *argv[])
{
  adainit();
  printf ("c_main: Calling ada_proc\n");
  ada_proc();
  printf ("c_main: Returned from ada_proc\n");
  adafinal();
}
