
template<typename TI, typename BODY>
void forallN(TI const &is_i, BODY const &body){}

void foobar()
   {
     int is; 
     forallN(is, [=] (int i) { 42; });
   }
