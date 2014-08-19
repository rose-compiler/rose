template <typename T>
class X
   {
     public:
          friend X<T> & operator+( X<T> & i, X<T> & j)
             {
               return i;
             }
   };

// This template is not unparsed in the generated code (but it is present in the ROSE AST).
template <typename T>
T & operator+( T & i, T & j)
   {
     return i;
   }

#if 1
void foobar()
   {
     X<int> x,y,z;

     x = y + z;
   }
#endif
