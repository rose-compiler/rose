template < typename _Key >
class map
   {
     public:
          typedef _Key key_type;

     public:
          typedef key_type iterator;
   };

// This causes the name qualification of the type in doSomething() to be "map<int>::iterator".
map<int>::iterator it;

#if 1
class foo
   {
     public:
          void doSomething()
             {
               map<int>::iterator it;
             }
   };
#endif

