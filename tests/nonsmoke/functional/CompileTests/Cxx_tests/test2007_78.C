namespace X
{
   namespace Y
   {
      class B;
   }
}

namespace Y {
   using namespace X::Y;
   B* b1;
// DQ (2/21/2010): This is ambiguous with the 4.0 compiler.
#if !( (__GNUC__ == 4) && (__GNUC_MINOR__ == 0) )
   void B();
#endif
// TOO (1/16/2011): This is ambiguous with the Thrifty 3.4.4 compiler
#if !( (__GNUC__ == 3) && (__GNUC_MINOR__ == 4) )
   class B* b2;
#endif
}


