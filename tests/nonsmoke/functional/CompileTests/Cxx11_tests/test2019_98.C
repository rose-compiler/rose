template<class T, class... T2>
// This fails
// void A(T2..., T) { }

// This works.
void A(T, T2...) { }

void foobar()
   {
     A<int, int>(5, 7);
   }
