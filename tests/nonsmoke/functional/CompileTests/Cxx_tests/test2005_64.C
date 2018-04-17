// Example showing problems with template specialization
// Controled by:
//      DEFAULT_INSTANTIATIONS_PERMITTED_IN_CLASS_SRC_SEQ_LIST (currently set to FALSE)

#if 0
template <class T> int f(T) { return 0; }
  class A {
    static class N { } n;
    void g(int = f(n));
  };

// When the flag is TRUE, the C++-generating back end produces this:
template <class T> int f(T) { return 0; }
  class A {
    static class N { } n;
    template<> f(N);        // invalid location of explicit specialization
    void g(int = f(n));
  };

// and when it is FALSE, the result is this:
template <class T> int f(T) { ... }
  class A;
  template<> f(A::N);       // undefined name
  class A {
    static class N { } n;
    void g(int = f(n));
  };
#endif

// Example of template that, it seems, could not be transformed using ROSE, since
// the output specialization would involved a private type specified outside of 
// the class.
// In this case we need to detect that if this is output as an instantiated template
// then it will use a private type (this should disable the output of the template 
// instantiation in ROSE).  Templates like this can not be transformed using
// ROSE since we could not output the instantiation (even as a specialization).
template <class T> int f(T) { return 0; }

class A
   {
     static class N { } n;
     void g(int = f(n));
   };

