class X
   {
     public:
//          template <typename T> friend bool operator== (T,int);
   };

#if 1
template <typename T>
bool operator== (T a,int b)
   {
     return false;
   }
#endif
