
namespace std
   {
     template<typename _Tp>
     class allocator // : public __gnu_cxx::new_allocator<_Tp>
        {
          public:
               allocator() throw() { }
        };

     template<typename _Tp>
     struct less // : public binary_function<_Tp, _Tp, bool>
        {
          bool operator()(const _Tp& __x, const _Tp& __y) const { return __x < __y; }
        };

     template<typename _Key , typename _Compare = std::less<_Key> , typename _Alloc = std::allocator<_Key> >
     class set
        {
          public:
               set(const _Compare& __comp /* , const allocator_type& __a = allocator_type() */) { }
        };
   }

bool compareSolutions(int x);

void foobar()
   {
     ::std::set<int, bool(*)(int)> XXX_triedSolutions(compareSolutions);
   }
