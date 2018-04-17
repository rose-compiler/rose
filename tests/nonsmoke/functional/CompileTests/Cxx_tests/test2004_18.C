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

#if 0
// I can't figure out how to make this work!
template<char* T>
class TestStringArgument
   {
     public:
          char* xyz;
          char* foo();
   };

template<char* T>
char* TestStringArgument<T>::foo ()
   {
     return T;
   }
#endif

#if 0
// EDG reports that use of a float as a template parameter is "non-standard"
// Can't build test code to test this case.
template<float T>
class TestFloatArgument
   {
     public:
          float xyz;
          float foo();
   };

template<float T>
float TestFloatArgument<T>::foo ()
   {
     return T;
   }
#endif

#if 0
// Can't build test code to test this case (address case).
template<class T>
class TestAddressArgument
   {
     public:
          int* xyz;
          int* foo();
   };

template<class T>
int* TestAddressArgument<T>::foo ()
   {
     return T;
   }
#endif

#if 0
// Can't build test code to test this case (aggregate case).
// This case generates an enum value which is out of range within EDG!
template<int T>
class TestAggregateArgument
   {
     template<int U> 
     class TestAggregateArgument_inner
        {
          public:
               int xyz;
               int foo();
        };

     public:
          int xyz;
          int foo();
          TestAggregateArgument_inner<T> innerObject;
   };

template<int T>
int TestAggregateArgument<T>::foo ()
   {
     return foo;
   }

template<int T>
template<int U>
int TestAggregateArgument<T>::TestAggregateArgument_inner<U>::foo ()
   {
     return U+T;
   }
#endif

#if 0
// EDG reports that use of a float as a template parameter is "non-standard"
// Can't build test code to test this case.
template<class T>
class TestTemplateArgument
   {
     public:
          T xyz;
          T foo();
   };

template<class T>
T TestTemplateArgument<T>::foo ()
   {
     return 42;
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

#if 0
  // const char* nameString = "Quinlan";
  // TestStringArgument<nameString> object3;
     TestStringArgument<"Quinlan"> object3;
     object3.xyz = object3.foo();
#endif

#if 0
     TestFloatArgument<3.14159265> object4;
     object4.xyz = object4.foo();
#endif

#if 0
     TestAddressArgument<int&> object5;
     object5.xyz = object5.foo();
#endif

#if 0
     TestAggregateArgument<1+2> object6;
     object6.xyz = object6.foo();
#endif

#if 0
// Not sure this is a real test of a template parameter as a tmplate argument
     TestTemplateArgument< TestTemplateArgument<int> > object7;
     object7.xyz = object7.foo();
#endif

     return 0;
   }


