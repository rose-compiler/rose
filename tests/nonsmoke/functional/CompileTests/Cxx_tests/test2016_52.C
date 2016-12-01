namespace std 
{
  template<class _T1, class _T2> struct pair {};
  template<typename> class allocator;
} 

namespace std 
{
  template<typename _Tp>
  class allocator
    {
      public:
      template<typename _Tp2> class new_allocator;
      template<typename _Tp1> struct rebind { typedef new_allocator<_Tp1> other; };
    };
} 

namespace std 
{
  template<typename _Tp> struct less {};

  template<typename _Tp> struct _Rb_tree_iterator {};

  template<typename _Key, typename _Val, typename _Compare, typename _Alloc = allocator<_Val> >
  class _Rb_tree
    {
    public:
      typedef _Val 				value_type;

    public:
      typedef _Rb_tree_iterator<value_type>       iterator;

      pair<iterator, bool> _M_insert_unique(const value_type& __x);
    };
} 

namespace std 
{
  template <typename _Key, typename _Tp, typename _Compare = std::less<_Key>, typename _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
  class map
    {
    public:
      typedef _Key                                          key_type;
      typedef std::pair<const _Key, _Tp>                    value_type;
      typedef _Compare                                      key_compare;
      typedef _Alloc                                        allocator_type;

    private:
      typedef typename _Alloc::template rebind<value_type>::other _Pair_alloc_type;
      typedef _Rb_tree<key_type, value_type, key_compare, _Pair_alloc_type> _Rep_type;

    public:      
      typedef typename _Rep_type::iterator               iterator;
    };

}

using namespace std;  

class  ExtensionSet {
 public:
  struct Extension {};

// DQ: added simpler function for debugging.
  void foobar();
};

void ExtensionSet::foobar() 
   {
  // Error unparses as:
  // std::pair< _Rb_tree< _Key,_Val,_Compare,_Alloc> ::iterator  , bool  > insert_result;

  // Note (g++ 4.8.3): unparses as: std::pair< std::_Rb_tree< std::map< int ,Extension ,class std::less< int  > ,class std::allocator< class std::pair< const int  , Extension  >  > > ::key_type ,std::map< int ,Extension ,class std::less< int  > ,class std::allocator< class std::pair< const int  , Extension  >  > > ::value_type ,std::map< int ,Extension ,class std::less< int  > ,class std::allocator< class std::pair< const int  , Extension  >  > > ::key_compare ,std::map< int ,Extension ,class std::less< int  > ,class std::allocator< class std::pair< const int  , Extension  >  > > ::_Pair_alloc_type > ::iterator  , bool  > insert_result;

  // Error (g++ 6.1): unparses as: std::pair< std::_Rb_tree< std::map< int ,Extension ,class std::less< int  > ,class std::allocator< class std::pair< const int  , Extension  >  > > ::key_type ,std::map< int ,Extension ,class std::less< int  > ,class std::allocator< class std::pair< const int  , Extension  >  > > ::value_type ,std::map< int ,Extension ,class std::less< int  > ,class std::allocator< class std::pair< const int  , Extension  >  > > ::key_compare ,std::map< int ,Extension ,class std::less< int  > ,class std::allocator< class std::pair< const int  , Extension  >  > > ::_Pair_alloc_type > ::iterator  , bool  > insert_result;

  // Better organized: unparses as: 
  //      std::pair< 
  //                 std::_Rb_tree< 
  //                                std::map< int ,Extension ,class std::less< int  > , class std::allocator< class std::pair< const int  , Extension  >  >  > ::key_type ,
  //                                std::map< int ,Extension ,class std::less< int  > , class std::allocator< class std::pair< const int  , Extension  >  >  > ::value_type ,
  //                                std::map< int ,Extension ,class std::less< int  > , class std::allocator< class std::pair< const int  , Extension  >  >  > ::key_compare ,
  //                                std::map< int ,Extension ,class std::less< int  > , class std::allocator< class std::pair< const int  , Extension  >  > > ::_Pair_alloc_type 
  //                              > ::iterator , 
  //                 bool  > 
  //      insert_result;

     pair<map<int, Extension>::iterator, bool> insert_result; 
   }
