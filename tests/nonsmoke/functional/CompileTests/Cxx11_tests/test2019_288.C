namespace N_
   {
     class X;
   }

void f_(const N_::X &x);

template<class T> void g_(T &x);

namespace N_
   {
     class X
        {
          public:
               class Y
                  {
                    friend void g_<const Y>(const Y &);
                  };

          private:
               friend void ::f_(const X &);
               Y y;
        };

     X x;
   }

template<class T> void g_(T &y) {}

void f_(const N_::X &x)
   {
  // Original code is: g_(x.y);
     g_(x.y);
   }

