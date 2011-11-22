// Example of template parameter requiring new support for 
// internal SgInitializedName and a default initializer.

template<class T, int U = 42 >
class ABC
   {
     public:
          T xyz;
   };

