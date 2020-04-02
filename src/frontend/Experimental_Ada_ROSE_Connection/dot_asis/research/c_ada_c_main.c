#include <stdio.h>

extern void adainit (void);
extern void adafinal (void);
extern void ada_c_caller (void);

int main (int argc, char *argv[])
{
  adainit();
  printf ("c_main: Calling ada_c_caller\n");
  ada_c_caller();
  printf ("c_main: Returned from ada_c_caller\n");
  adafinal();
}
