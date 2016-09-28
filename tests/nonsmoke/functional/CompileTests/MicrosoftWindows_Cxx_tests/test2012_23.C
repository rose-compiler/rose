
class X
   {
     public:
       // template <typename T>
          friend bool operator== (const X & x,int y);
   };

// template <typename T>
bool operator== (const X & x,int y)
   {
     return false;
   }
