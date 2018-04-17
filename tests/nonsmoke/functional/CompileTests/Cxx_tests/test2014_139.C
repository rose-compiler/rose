namespace XXX 
   {
     void foobar(const int def[3]); // (const int def[3]);
   } // namespace XXX

namespace XXX 
   {
  // namespace {
     namespace Y 
        {

          const unsigned size = 3;
#if 0
          struct AAA
             {
               bool isPeriodic;
             };
#endif
        } // unnamed namespace, back to XXX

#if 0
     void foobar(const int def[3])
#else
     void foobar(const int def[Y::size])
#endif
#if 1
        {
       // Y::AAA abc;
       // abc.isPeriodic = false;
        }
#else
        ;
#endif
   } // namespace XXX
