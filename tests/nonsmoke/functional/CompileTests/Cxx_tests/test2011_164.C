// Only the class and constant integer cases seem to work!

template<int T>
class TestIntegerArgument
   {
     public:
//        int xyz;
          int foo();
   };

template<int S>
int TestIntegerArgument<S>::foo ()
   {
  // Do we need a templateParameterSymbol just to support this in the IR?
  // Or we could have a IR node for SgTemplateParameterValue, I like this best so far.

  // Note that the name can't be used directly, I think; edg records the parameter as 
  // having class template parameter with name == "T" even though the member function 
  // template;'s parameter is named "S".  How do we handle this?
  // WARNING: Using "T" as the name of the template parameter is an error?

#if 1
  // This is not supported well in the new EDG 4.3 connection (need IR support).
     return S;
#else
  // This works fine (but is not the original failing code).
     return 0;
#endif
   }

