template <typename V>
class test
   {
     V v;
   };

namespace std
{
  template<typename> class allocator;
  template<typename _Tp> struct less {};
  template<typename _Key, typename _Compare = std::less<_Key>, typename _Alloc = std::allocator<_Key> > class set {};
}
       
template<typename V>
void foo()
   {
     std::set<test<V> > s;
     typename std::set<test<V> >::iterator i;
     if (typename std::set<test<V> >::iterator i2=s.begin()) {}
   }

void foobar()
{}
