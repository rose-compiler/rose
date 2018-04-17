template<class T>
class ABC
   {
     public:
          T xyz;
          T foo();
   };

template<class T>
T ABC<T>::foo ()
   {
     return xyz;
   }

// If uncommented this causes an error with g++
// class ABC< int >;
class ABC<int>
   {
     public:
          int xyz;
          int foo();
   };

#if 1
int ABC<int>::foo ()
   {
     return xyz;
   }
#endif

int main()
   {
     class ABC<int> object1;
     object1.xyz = 7;
#if 1
     object1.xyz = object1.foo();
#endif
     return 0;
   }

