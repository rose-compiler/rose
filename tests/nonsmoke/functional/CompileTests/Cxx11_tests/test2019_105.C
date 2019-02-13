namespace N
   {
     template <class T>
     class X
        {
          public:
               X(T t) : m(t) { }
               T m;
               T f() { return ++m; }
        };

     template<>
     class N::X<long>; 
   }

template<>
class N::X<long> final : public N::X<int>
   {
     public:
          X(long t) : N::X<int>(1), n(t) { }
          long n;
          long f() { return ++n + N::X<int>::f(); }
   };

void foobar()
   {
     N::X<long> nxl(3);
   }
