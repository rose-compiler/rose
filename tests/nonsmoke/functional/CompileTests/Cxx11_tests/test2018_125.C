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
      typedef _Key 	key_type;
      typedef size_t 	size_type;
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
