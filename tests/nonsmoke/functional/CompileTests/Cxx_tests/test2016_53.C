template<class _T1, class _T2> struct pair {};
template<typename _Tp> struct _Rb_tree_iterator {};

template<typename _Key >
class _Rb_tree
   {
     public:
          typedef _Key 				value_type;

          typedef _Rb_tree_iterator<value_type>       iterator_rb_tree;

       // This is the first point at which the "pair<iterator_rb_tree, bool >" type is built, so it is reused from here within EDG.
          pair<iterator_rb_tree, bool > _M_insert_unique();
   };

template <typename _Key>
class map
   {
     public:
          typedef _Key key_type;

     private:
          typedef _Rb_tree<key_type> _Rep_type;

     public:
          typedef typename _Rep_type::iterator_rb_tree iterator_map;
   };

void foobar() 
   {
  // Error unparses as:
  // pair< _Rb_tree< _Key> ::iterator  , bool  > insert_result;
  // pair<map<int>::iterator, bool > insert_result; 
     pair<map<int>::iterator_map, bool > insert_result; 

     int x;
   }
