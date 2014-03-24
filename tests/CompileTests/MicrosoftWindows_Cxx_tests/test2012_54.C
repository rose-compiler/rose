// test template function
template <typename T>
void foo3(void) {}


void foobar()
   {
     foo3<char>();
  // foo3();  // This is an error, since it does not contain enough information for type resolution
   }
