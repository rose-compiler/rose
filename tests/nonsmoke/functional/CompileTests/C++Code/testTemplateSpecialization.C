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

#if 0
class ABC<int>
   {
     public:
          int xyz;
#pragma instantiate ABC<int>::foo
          int foo();
   };
#endif

#if 0
int ABC<int>::foo ()
   {
     return xyz;
   }
#endif

int main()
   {
     ABC<int> object1;
     object1.xyz = 7;
#if 1
     object1.xyz = object1.foo();
#endif
     return 0;
   }

