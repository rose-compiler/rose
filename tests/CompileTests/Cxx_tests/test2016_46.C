template < typename _Key >
class map
   {
     public:
          typedef _Key key_type;

     public:
          typedef key_type iterator;
   };

// This causes the name qualification of the type in doSomething() to be "map<int>::iterator".
// map<int>::iterator it;

#if 0
void doSomething()
   {
     map<int>::iterator it;
   }
#endif

#if 1
class foo
   {
     public:
#if 0
          map<int>::iterator it;
#endif
#if 1
          void doSomething()
             {
               map<int>::iterator it;
             }
#endif
   };
#endif
