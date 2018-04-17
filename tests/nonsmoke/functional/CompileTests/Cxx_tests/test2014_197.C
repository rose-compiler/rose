template <typename T>
class X
   {
     public:
         void free(X ptr) 
             {
             }
   };

void foo()
   {
      X<int> a;
      a.free(a);
   }

