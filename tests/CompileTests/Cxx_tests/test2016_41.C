template<typename _Tp> struct _Rb_tree_iterator {};

template< typename _Val >
class _Rb_tree
   {
     public:
          typedef _Val 				value_type;

          typedef _Rb_tree_iterator<value_type>       iterator;
   };

template < typename _Key >
class map
   {
     public:
          typedef _Key                                          key_type;

     private:
          typedef _Rb_tree<key_type> _Rep_type;

     public:
          typedef typename _Rep_type::iterator iterator;
   };

class foo
   {
     public:
          void doSomething()
             {
               map<int>::iterator it;
             }
   };

