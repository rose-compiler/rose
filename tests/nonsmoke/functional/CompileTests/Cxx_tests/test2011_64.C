
namespace N
   {
     class any {};

     template < typename T >
     class array
        {
          public:
               array(int x);
        };
   }

N::array<N::any> X = 0;
N::array<N::any> Y = N::array<N::any>(0);

// Assignment using an explicit cast
// std::string s = std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("");

