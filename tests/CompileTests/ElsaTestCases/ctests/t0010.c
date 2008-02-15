// t0010.c
// unsigned long and void* as operands to ?:

// from mozilla/gfx/src/xlibrgb/xlibrgb.c

unsigned long foo(int y)
{
  unsigned long x;

  // gcc doesn't seem to care that this is a null pointer constant
  return y? x : (void*)0;
}


