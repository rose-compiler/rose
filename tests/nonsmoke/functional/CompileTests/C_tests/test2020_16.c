void foobar() 
   {
     int count;

     switch (count)
        {
          case 40: 41;
              #pragma XXX
#if 1
               __attribute__ ((__fallthrough__));
#endif
          int abc;
          case 42: 43;
        }
   }
