int a_[2];

template<class T> using A_ = T(&)[2];

struct B_ 
   {
     template<class T> operator A_<T>() { return a_; }
   };

void foobar()
   {
     B_ b;

  // This fails as is for the frontend of GNU 5.1
     int* p = b;

     p[1];
   }
