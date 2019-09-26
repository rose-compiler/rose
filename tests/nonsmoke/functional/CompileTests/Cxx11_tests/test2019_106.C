namespace N
   {
     template <class T>
     class X
        {
          public:
               X(T t);
        };

  // Unparsed as: template<> class X< long  > final;
  // And statement is unparsed twice.
     template<> class N::X<long>; 
   }

template<>
class N::X<long> final
   {
     public:
          X(long t);
   };

void foobar()
   {
     N::X<long> nxl(3);
   }
