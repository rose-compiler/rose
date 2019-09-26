class T_
   {
     public:
          int j;
          T_(int jj) : j(jj) { }
          T_(const T_& r) : j(r.j) { }
   };

int i_ = 1;
template<class T_, const T_* i_>
long f_(T_ t)
   {
     T_ t1 = *i_;      // template-arguments T and i 
     ::T_ t2 = ::i_;  // globals T and i
     return t1.k * 100 + t2.j;
   }

struct X_
   {
     int k;
     X_(int kk) : k(kk) { }
     X_(const X_& r) : k(r.k) { }
   };

extern const X_ x_ (3);

void foobar()
   {
     long n = f_<X_, &x_>(x_);
   }
