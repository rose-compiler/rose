class X {};

class Y
   {
     public:
          template< typename T> explicit Y(const T& i);
   };

template< typename T> 
Y::Y(const T & i)
   {
     if ( i.empty() ) 
        {
          new X[i.size()];
        }
   }
