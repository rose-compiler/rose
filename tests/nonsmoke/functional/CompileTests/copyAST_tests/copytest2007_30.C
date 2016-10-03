// #include<string>

namespace std
   {
     template<typename _Alloc> class allocator {};
   }

namespace std
   {
     template<typename _Alloc> class allocator;

     template< typename _Alloc = allocator<char> > class basic_string;
   }
