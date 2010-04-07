
void foo()
   {
     int x;
     switch (x)
        {
       // static double zero;
          int one;

          case 0:
            // zero = 0.0 ;
               one = 1;
               break;

          default:
       // DQ (2/20/2010): g++ requires "{}" (at least for g++ 4.2).
#if (__GNUC__ >= 3)
             {}
#endif

        }
   }

