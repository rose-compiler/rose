template <typename T> bool operator== (T a,int b);

#if 1
class X
   {
     public:
#if 1
       // MSVC has different friend function symbol injection than GNU.
       // template <typename T> friend bool operator== (T,int);
#else
          template <typename T>
          friend bool operator== (T a,int b)
             {
               return false;
             }
#endif
          int x;
   };
#endif

#if 1
// MSVC has different friend function symbol injection than GNU.
template <typename T>
bool operator== (T a,int b)
   {
     return false;
   }
#endif
