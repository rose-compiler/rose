template <class ...T> int f(T*...);  // #1
template <class T>  int f(const T&); // #2

void foobar()
   {
     f((int*)0); // OK: selects #1
                 // (was ambiguous before DR1395 because deduction failed in both directions)
   }
