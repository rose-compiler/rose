class X
   {
     public:
          template <typename T>
          friend bool operator== (T,int);
   };


template <typename T>
bool operator== (T,int)
   {
     return false;
   }
