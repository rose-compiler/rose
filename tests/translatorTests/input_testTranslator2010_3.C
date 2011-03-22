// The function MIN here has a defining declaration, but a NULL definition
template <class T>
const T& MIN( const T& a, const T& b )
   {
     return b < a ? b : a;
   }

class c
   {
     void foo()
        {
          MIN(1, 2);
        }
   };

