template <typename T>
class X
   {
     public:
         void free(X* ptr) 
             {
            // delete ptr;
             }
   };

void foo()
   {
      X<int>* a = 0L;
      a->free(a);
   }

