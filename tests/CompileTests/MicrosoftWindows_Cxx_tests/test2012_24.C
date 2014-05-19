class X
   {
     public:
       // MSVC has different friend function symbol injection than GNU.
       // template <typename T> friend bool operator== (T,int);
   };


template <typename T>
bool operator== (T,int)
   {
     return false;
   }
