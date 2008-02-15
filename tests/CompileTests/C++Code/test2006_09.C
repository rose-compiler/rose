
class X
   {
     public:
          X operator=(const X & x);

#if 0
          void foo()
             {
               X x;
               operator=(x);
             }
#endif
   };


class Y : public X
   {
     public:
          Y operator=(const Y & y);
          Y operator=(const X & x);

#if 0
          void foo()
             {
               Y y;
               operator=(y);
               X x;
               X::operator=(x);
             }
#endif
          void foobar()
             {
               X x;

            // This is unparsed as "= x;".  It is not clear if it should be "*this = x;" or 
            // "X::operator=(x);", though I guess it would have to be "X::operator=(x);" 
            // since "*this = x;" would be a call to "Y::operator=(X&x);".
               X::operator=(x);

            // This is correctly unparsed as "(*this) = x;"
               Y::operator=(x);
             }
   };


