unsigned int fwd(unsigned int x)
   {
     return x*3;
   }

unsigned int rev(unsigned int x)
   {
     unsigned int rv;
     if (x % 3 == 0)
          rv = x/3;
     else if (x % 3 == 2)
          rv = x/3 + 1431655766;
     else
          rv = x/3 + 2863311531;
     return rv;
   }

int test(unsigned int x)
   {
     assert(rev(fwd(x)) == x);
     assert(fwd(rev(x)) == x);
     return 0;
   }

