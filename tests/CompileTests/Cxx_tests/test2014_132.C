#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 4) )
// This test fails for the 4.2.4 version of GNU g++.

namespace {
bool foobar();   
}  // unnamed namespace, back in XXX namespace

void foobar(float y)
   {
  // Call the one in the un-named namespace.
     foobar();
   }

#endif
