// ROSE-42 (Kull)
// ROSE-1307

namespace std
{
  typedef __SIZE_TYPE__ size_t;

//NEEDED:
  template<typename>
    struct rank
    {
      typedef std::size_t value_type;
    };

    class allocator
    {
    };

  template<
typename _Key,
typename _Val,
typename _Alloc = allocator >
    class _Rb_tree
    {
    public:
      typedef _Key    key_type;
      typedef size_t size_type;
      size_type
      erase(const key_type& __x);
    };

//NEEDED:
  template<
typename _Key,
typename _Val,
typename _Alloc>
    typename _Rb_tree<
_Key,
_Val,
_Alloc>::size_type
    _Rb_tree<
_Key,
_Val,
_Alloc>::
    erase(const _Key& __x)
    {
      const size_type __old_size = 0;
    }

  template<
typename _Tp,
typename _Alloc = std::allocator >
    class vector {
    public:
      // NEEDED:
      typedef size_t size_type;
    };
} // namespace std 

class Class1;

void func1() {
  const std::vector<Class1*>::size_type local1 = 0;
}

 
#if 0
gets this error:
rose_ROSE-42.cpp: In function 'void func1()':
rose_ROSE-42.cpp:58:68: error: wrong number of template arguments (1, should be 3)
const std::_Rb_tree<  typename _Key, typename _Val, typename _Alloc> ::size_type local1 = 0;
                                                                    ^
rose_ROSE-42.cpp:19:11: error: provided for 'template<class _Key, class _Val, class _Alloc> class std::_Rb_tree'
     class _Rb_tree
           ^
rose_ROSE-42.cpp:58:82: error: qualified-id in declaration before 'local1'
const std::_Rb_tree<  typename _Key, typename _Val, typename _Alloc> ::size_type local1 = 0;

because this code:
  const std::vector<Class1*>::size_type local1 = 0;
gets unparsed into this code:
const std::_Rb_tree<  typename _Key, typename _Val, typename _Alloc> ::size_type local1 = 0;
#endif
