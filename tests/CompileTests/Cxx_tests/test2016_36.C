
namespace std
   {
     typedef long unsigned int 	MY_size_t;
     typedef long int	MY_ptrdiff_t;


     template<typename _Tp>
     struct MY_less // : public binary_function<_Tp, _Tp, bool>
        {
          bool operator()(const _Tp& __x, const _Tp& __y) const
          { return __x < __y; }
        };
   }

namespace std 
   {
     template<typename _Pair> struct _Select1st {};
   }

namespace std 
   {
     template<typename _Tp> struct _Rb_tree_iterator {};

     template<typename _Key, typename _Val, typename _KeyOfValue, typename _Compare, typename _Alloc >
     class _Rb_tree
        {
          public:
               typedef _Val 				value_type;

               typedef _Rb_tree_iterator<value_type>       iterator;
        };
   }

namespace MY__gnu_cxx 
   {
 
     template<typename _Alloc>
     struct __alloc_traits
        {
          template<typename _Tp>
          struct rebind
             { typedef typename _Alloc::template rebind<_Tp>::other other; };
        };
   } 

namespace std 
   {
     template<typename> class MY_allocator;
   }

namespace std 
   {
     template<typename _Tp>
     class MY_allocator
        {
          public:
               typedef MY_size_t     size_type;
               typedef MY_ptrdiff_t  difference_type;
               typedef _Tp*       pointer;
               typedef const _Tp* const_pointer;
               typedef _Tp&       reference;
               typedef const _Tp& const_reference;
               typedef _Tp        value_type;

               template<typename _Tp1> struct rebind { typedef MY_allocator<_Tp1> other; };
        };
   }

namespace std 
   {
     template<class _T1, class _T2> struct MY_pair { };

     template <typename _Key, typename _Tp, typename _Compare = std::MY_less<_Key>,typename _Alloc = std::MY_allocator<std::MY_pair<const _Key, _Tp> > >
     class map
        {
          public:
               typedef _Key                                          key_type;
               typedef std::MY_pair<const _Key, _Tp>                    value_type;
               typedef _Compare                                      key_compare;
               typedef _Alloc                                        allocator_type;

          private:
               typedef typename MY__gnu_cxx::__alloc_traits<_Alloc>::template rebind<value_type>::other _Pair_alloc_type;

               typedef _Rb_tree<key_type, value_type, _Select1st<value_type>, key_compare, _Pair_alloc_type> _Rep_type;
      
          public:
               typedef typename _Rep_type::iterator               iterator;
        };
   }

class foo
   {
     public:
          void doSomething()
             {
               std::map<int, int>::iterator it;
             }
   };

