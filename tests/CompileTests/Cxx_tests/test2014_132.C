namespace {
bool foobar();   
}  // unnamed namespace, back in XXX namespace

void foobar(float y)
   {
  // Call the one in the un-named namespace.
     foobar();
   }
