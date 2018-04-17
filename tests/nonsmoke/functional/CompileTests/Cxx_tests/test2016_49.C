template < typename _Key >
class map
   {
     public:
          typedef _Key iterator;
   };

class foo
   {
     public:
          void doSomething()
             {
            // Note that this will resolve to "map<int>::iterator" instead of "map< map<int>::iterator >::iterator" 
            // because "map<int>::iterator" is an alias for "int".  Unclear if this is something we can fix in ROSE.
               map< map<int>::iterator >::iterator it;
             }
   };

