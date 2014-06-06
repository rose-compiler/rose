

template <typename T = int>
class X {};

// class X<>;
// X b;

void foobar()
   {
     X< > a;

#if 1
     class X{};
     X b;
#endif

     ::X< > c;
   }
