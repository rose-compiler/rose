// __PRETTY_FUNCTION__ is an internally defined compiler value (of type: const char*)
// const char* c = __PRETTY_FUNCTION__;
void foobar(const char* c);
void foo()
   {
  // foobar(__PRETTY_FUNCTION__);
     foobar(__PRETTY_FUNCTION__);
   }

