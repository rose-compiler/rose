// Demonstration of overzelious use of name qualification

#include<list>

using namespace std;

#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )
template<typename _Tp, typename _Alloc = allocator<_Tp> >
class X : protected _List_base<_Tp, _Alloc>
   {
  public:
     typedef _List_base<_Tp, _Alloc>  _Base;
     typedef const std::list < int , std::allocator< int > > ::value_type &const_reference;
     typedef _List_iterator<_Tp,const _Tp&,const _Tp*>     const_iterator;

  protected:
     using _Base::_M_node;

  public:
     explicit X ( const allocator_type& __a = allocator_type() ) : _Base(__a) { }

     const_iterator end() const { return _M_node; }
     const_reference back() const { return *(--end()); }
   };

int main()
   {
  // DQ (11/19/2004): Temporarily commented out since this is a demonstrated bug now that we qualify everything!
     list<int> integerList;

     X<int> x;

     return 0;
   }
#else
  #warning "Case not tested for version 3.4 and higher."
#endif

