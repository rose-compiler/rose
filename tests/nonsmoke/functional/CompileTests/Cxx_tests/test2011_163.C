// Only the class and constant integer cases seem to work!

#if 1
template<class T>
class TestClassArgument
   {
     public:
          T xyz;
       // This is a SgTemplateInstantiationMemberFunction in ROSE using EDG 3.3, so we should 
       // preserve this (also is currently in the AST this way).
       // However, by default these template instantiations are not output as specializations.
       // A larger change to ROSE would be that there is an opportunity to have this be a member 
       // function of the template class, instead of a template member function.
          T foo();
   };

// Note: in instantiated template for foo() it should be uparsed as: "int foo();"
// Note: the function declaration for the template function foo() should be: "int TestClassArgument<int>::foo()"

template<class T>
T TestClassArgument<T>::foo ()
   {
     return xyz;
   }
#endif

#if 0
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

// DQ (1/16/2012): Since the template instantiations are not transformed the template instantion for the 
// templated class should not be output.  To compound problems, the non-defining member function instantiation 
// declaration is NOT output which it should be if the class is output.  So there are two issues to fix! 
// (tomorrow)...

int main()
   {
  // It seems that only one of these can be turned on at a time!

#if 1
     TestClassArgument<int> object1;
     object1.xyz = 7;
     object1.xyz = object1.foo();
#endif

#if 0
     TestIntegerArgument<2> object2;
     object2.xyz = object2.foo();
#endif

     return 0;
   }


