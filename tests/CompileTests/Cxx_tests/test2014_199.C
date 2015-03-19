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
      X<float> b;
      a.free(a);
      b.free(b);
   }

