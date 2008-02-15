// DQ (6/25/2007): This code demonstrates a friend function
// that will be unparsed in the class without the definition
// and unparsed in the global scope with the definition
// but for the fact that we mark it as a forward declaration
// (using setForward()).  As a result the definition is lost
// likely the definition should be output in global scope
// (since it is eliminated by EDG in the template declaration)
// and this currently an error.  It appears that it is marked
// to not be output (or the statements in the body are marked
// as such so that the unparing is an error.

template <typename T>
class X
   {
     public:
          friend X<T> & operator+( X<T> & i, X<T> & j)
             {
               return i;
             }
   };

#if 0
template <typename T>
T & operator+( T & ii, T & jj)
   {
     return ii;
   }
#endif

// template X<int> & operator+( X<int> & ii, X<int> & jj);

#if 1
int main()
   {
     X<int> y,z;

  // Error used to be unparses as: "x = (+< X< int > > (y,z));"
     y + z;

     return 0;
   }
#endif
