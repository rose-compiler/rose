namespace __gnu_cxx
{
template < typename _Tp >
     class new_allocator
        {
          public :
        };
}
template < typename _Tp >
     class allocator : public __gnu_cxx :: new_allocator < _Tp >
        {
          public :
        };
template<> class allocator< char  > ;
namespace __gnu_cxx 
   {
     template<> class new_allocator< char  > 
        {
        };
}
template<> class allocator< char  > : public __gnu_cxx::new_allocator< char  > 
{
};
