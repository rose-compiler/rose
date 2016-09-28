template<class _T1, class _T2> struct pair {};
template<typename _Tp> struct _Rb_tree_iterator {};

template<typename _Key_rb_tree >
class _Rb_tree
   {
     public:
          typedef _Key_rb_tree value_type;

          typedef _Rb_tree_iterator<value_type> iterator_rb_tree;

       // This is the first point at which the "pair<iterator_rb_tree, bool >" type is built, so it is reused from here within EDG.
          pair<iterator_rb_tree, bool > _M_insert_unique();
   };

template <typename _Key_map>
class map
   {
     public:
          typedef _Key_map key_type;

     private:
          typedef _Rb_tree<key_type> _Rep_type;

     public:
       // typedef typename _Rep_type::iterator_rb_tree iterator_map;
          typedef typename _Rb_tree < key_type > :: iterator_rb_tree iterator_map;
       // typedef typename _Rb_tree < _Key_map > :: iterator_rb_tree iterator_map;
   };

void foobar() 
   {
  // Error unparses as:
  // pair< _Rb_tree< _Key_rb_tree> ::iterator_rb_tree  , bool  > insert_result;
  // Wanted alternative:
  // pair< _Rb_tree<int> ::iterator_rb_tree  , bool  > insert_result;
     pair<map<int>::iterator_map, bool > insert_result; 
   }
