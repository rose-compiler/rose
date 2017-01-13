namespace XXX 
   {
     void foobar(const int def[3]);

     namespace Y 
        {

          const unsigned ArraySize = 3;
        }

     void foobar(const int def[Y::ArraySize])
#if 1
        {
       // Y::AAA abc;
       // abc.isPeriodic = false;
        }
#else
        ;
#endif
   } // namespace XXX
