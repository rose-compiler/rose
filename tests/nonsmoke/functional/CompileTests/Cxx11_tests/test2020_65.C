// This is from the paper copy of test_25.cpp

void bafoobar() {}

template <class T> class A {};

class B
   {
     public:
          template <class T>
          void foobar (T* x)
             {
               A<T>* var1;
               C var2;
               var2.xyz = 42;
             }

     private:
          struct C
             {
               float xyz;
             };
   };


