#if 0
template< typename T >
class _Rb_tree
   {
     public:
          typedef T iterator;
   };
#endif

template < typename _Key >
class map
   {
     public:
       // typedef int key_type;

  // private:
     public:
       // typedef _Rb_tree<key_type> _Rep_type;

     public:
       // typedef typename _Rep_type::iterator iterator;
          typedef _Key iterator;
   };

class foo
   {
     public:
          void doSomething()
             {
               map<int>::iterator it;
             }
   };

