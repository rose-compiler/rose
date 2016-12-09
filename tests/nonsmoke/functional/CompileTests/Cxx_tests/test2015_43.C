

namespace __gnu_cxx
   {
     template < typename _Tp >
     class new_allocator
        {
          public :
        };

   }


namespace std 
   {
     template < typename _Tp >
     class allocator : public __gnu_cxx::new_allocator<_Tp>
        {
          public :
        };
   }


namespace std 
   {
     template<> class allocator< char  > : public __gnu_cxx::new_allocator< char> 
        {
        };
      
   }
