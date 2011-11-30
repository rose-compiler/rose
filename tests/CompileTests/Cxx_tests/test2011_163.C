// Only the class and constant integer cases seem to work!

#if 1
template<class T>
class TestClassArgument
   {
     public:
          T xyz;
          T foo();
   };

template<class T>
T TestClassArgument<T>::foo ()
   {
     return xyz;
   }
#endif

#if 1
template<int T>
class TestIntegerArgument
   {
     public:
          int xyz;
          int foo();
   };

template<int T>
int TestIntegerArgument<T>::foo ()
   {
     return T;
   }
#endif


int main()
   {
  // It seems that only one of these can be turned on at a time!

#if 1
     TestClassArgument<int> object1;
     object1.xyz = 7;
     object1.xyz = object1.foo();
#endif

#if 1
     TestIntegerArgument<2> object2;
     object2.xyz = object2.foo();
#endif

     return 0;
   }


