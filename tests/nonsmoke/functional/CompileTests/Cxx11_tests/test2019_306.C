class V_;

class M_
   {
     public:
          friend V_ f_(const M_ &, const V_ &);
          M_(int i) : m(i) { }
          int m;
   };

class V_
   {
     public:
          friend V_ f_(const M_ &, const V_ &);
          V_(int i) : v(i) { }
          int v;
   };

V_ f_(const M_ &m, const V_ &v)
   {
     return m.m * v.v;
   }


void foobar()
   {
     V_ v(91);
     M_ m(2);
     f_(m, v).v;
   }
