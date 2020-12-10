// This test code is not accepted by GNU 5.1: 
// (error: specialization of ‘template<class T> class N::X’ in different namespace [-fpermissive]).

namespace N
   {
     template <class T>
     class X
        {
          public:
               X(T t);
        };
   }

#if __GNUG__ > 6
// Original code: template<> class N::X<long> final
// Unparsed code: template<> class X<long> final
// template<> class N::X<long> final
template<>
class N::X<long>
   {
     public:
          X(long t);
   };
#endif
