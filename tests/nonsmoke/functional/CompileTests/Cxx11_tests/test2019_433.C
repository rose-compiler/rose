
namespace N
   {
     class any {};

     template < typename T, typename S = any >
     class array
        {
          public:
               array(int x);
        };
   }

namespace M
   {
     class any {};
   }

void foo()
   {
     N::array<N::any,M::any>* Z2 = 0L;

     Z2 = (N::array<N::any,M::any>*) 0L;
   }
