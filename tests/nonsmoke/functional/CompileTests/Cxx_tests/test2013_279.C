// I think this test code demonstrates an error in the symbol table lookup for template member functions.

# define _GLIBCXX_BEGIN_NAMESPACE(X) namespace X { 
# define _GLIBCXX_END_NAMESPACE } 

#  define _GLIBCXX_BEGIN_NESTED_NAMESPACE(X, Y) _GLIBCXX_BEGIN_NAMESPACE(X)
#  define _GLIBCXX_END_NESTED_NAMESPACE _GLIBCXX_END_NAMESPACE


_GLIBCXX_BEGIN_NAMESPACE(std)

template<class _T1, class _T2>
struct pair
   {
  // typedef _T1 first_type;    ///<  @c first_type is the first bound type
     typedef _T2 second_type;   ///<  @c second_type is the second bound type

  // _T1 first;                 ///< @c first is a copy of the first object
     _T2 second;                ///< @c second is a copy of the second object
   };

_GLIBCXX_END_NAMESPACE

_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)

template<typename _Tp>
class new_allocator
   {
     public:
          template<typename _Tp1>
          struct rebind { typedef new_allocator<_Tp1> other; };
   };

_GLIBCXX_END_NAMESPACE

#define __glibcxx_base_allocator  __gnu_cxx::new_allocator

_GLIBCXX_BEGIN_NAMESPACE(std)

template<typename _Tp> class allocator;

#if 1
template<typename _Tp> class allocator : public __glibcxx_base_allocator<_Tp> {};
#endif

_GLIBCXX_END_NAMESPACE
