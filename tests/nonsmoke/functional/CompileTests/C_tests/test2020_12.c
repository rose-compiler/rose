
#ifndef FALLTHROUGH
# if __GNUC__ < 6
#  define FALLTHROUGH ((void) 0)
# else
#  define FALLTHROUGH __attribute__ ((__fallthrough__))
# endif
#endif

// Note that EDG will ignore the __fallthrough__ attribute 
// (and it has not useful semantic meaning either).
int u8_uctomb_aux (int *s, int uc, int n) {
  int count;

  switch (count)
    {
    case 3: uc |= 0x800;
      FALLTHROUGH;
    case 2: uc |= 0xc0;
  /*case 1:*/ s[0] = uc;
    }

  return count;
}

