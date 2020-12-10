template< class T > void f(T);              // #1: template overload
template< class T > void f(T*);             // #2: template overload
void                     f(double);         // #3: nontemplate overload
template<>          void f(int);            // #4: specialization of #1
 
// DQ (7/21/2020): Moved function calls into a function.
void foobar()
   {
     f('a');        // calls #1
     f(new int(1)); // calls #2
     f(1.0);        // calls #3
     f(1);          // calls #4
   }


