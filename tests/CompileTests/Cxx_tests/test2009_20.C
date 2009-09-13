#define bit_test(a)

void
testCode()
   {
     int x = 1;
     int y = 1;

  // Note that this is really just "&y" on the rhs.
     int *z = bit_test(x) bitand y;

     x++;
     z++;
   }


