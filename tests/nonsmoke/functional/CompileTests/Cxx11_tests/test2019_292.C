int f_() { return 3; }

template<class T> using A_ = T(&)();

struct B_ 
   {
     template<class T> operator A_<T> () { return f_; }
   };


void foobar()
   {
     B_ b;
     typedef int (*PF)();
     PF p = b;
     p();
   }


