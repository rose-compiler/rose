
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

N::array<N::any> X = 0;
N::array<N::any> Y = N::array<N::any>(0);
N::array<N::any,M::any> Z = N::array<N::any,M::any>(0);

// Assignment using an explicit cast
// std::string s = std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("");


void foo()
   {
     N::array<N::any> X = 0;
     N::array<N::any> Y = N::array<N::any>(0);
     N::array<N::any,M::any> Z = N::array<N::any,M::any>(0);

     N::array<N::any,M::any> Z1(0);
     Z1 = N::array<N::any,M::any>(0);
     Z1 = (N::array<N::any,M::any>) 0;

     N::array<N::any,M::any>* Z2 = 0L;
     N::array<N::any,M::any>* Z3 = 0L;
     Z2 = (N::array<N::any,M::any>*) 0L;
   }
