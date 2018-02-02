class B_
   {
     public:
          int i;
       // int operator + () { return ( i + 11 );}
       // int operator - () const { return ( i + 12 );}
          B_(int j) : i(j) { }
   };

int operator * (B_  s) { return ( s.i + 13 );}

#if 0
class D_ : public B_
   {
     public:
          D_(int i) : B_(i) { }
   };

int operator & (const D_ &s) { return ( s.i + 14 );}
#endif

int ivalue(int);
int iequals(int, int);

void foobar()
   {
     B_ a (1);
  // B_ *       p  = &a;
     const B_ * pc = &a;

#if 0
     D_ a2 (1);
     D_ &      r  = a2;
     const D_ &rc = a2;
#endif

#if 0
     int i1 = +r;
     iequals(ivalue( i1 ), 12);
#endif
#if 0
     int i2 = -*p;
     iequals(ivalue( i2 ), 13);
#endif
#if 1
     int i3 = **pc;
     iequals(ivalue( i3 ), 14);
#endif
#if 0
     int i4 = &rc;
     iequals(ivalue( i4 ), 15);
#endif
   }
