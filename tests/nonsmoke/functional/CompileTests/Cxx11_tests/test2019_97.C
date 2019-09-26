template<class T, class... T2>
void A(T2..., T) { }

void foobar()
   {
  // BUG: This unparses as: ::A< int  > (5,7);
     A<int, long>(5, 7L);
   }
