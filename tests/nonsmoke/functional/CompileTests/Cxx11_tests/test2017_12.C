
template<typename BODY> void forallN(BODY const &body){}

void foobar()
   {
     forallN([=] (int i) { 42; });
   }
