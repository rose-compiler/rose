typedef unsigned int size_t;

class A
   { 
     public: 
          A(int) {};
          void *operator new (size_t Size) {};
   };

