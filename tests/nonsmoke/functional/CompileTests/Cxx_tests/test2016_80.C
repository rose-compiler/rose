template < typename T >
class X
   {
     public:
          int a;
       // Confirm that the raw position information is present for the 
       // expressions in the constructor pre-initialization list.
          X() : a(1) {}
   };

X<int> x;
