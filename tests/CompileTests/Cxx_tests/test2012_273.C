namespace std
   {
     template<typename _Alloc> class allocator {};
   }

namespace std
   {
  // This should be a forward declaration, but is not maked properly (the bug).
     template<typename _Alloc> class allocator;

  // template< typename _Alloc = allocator<char> > class basic_string;
   }
