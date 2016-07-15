// template<typename _Tp> struct _Rb_tree_iterator {};

template< typename T >
class _Rb_tree
   {
     public:
          typedef T iterator;
   };

template < typename _Key >
class map
   {
     public:
          typedef _Key                                          key_type;

  // private:
     public:
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

