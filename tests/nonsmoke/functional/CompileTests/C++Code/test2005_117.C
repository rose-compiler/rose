/* This is a responce from the GNU people relative to Peter's bug (test2005_115.C)

This basically boils down to this question: 
 --------------------- 
 template <class> struct S { typedef int type; }; 
  
 template <class T> 
 int foo(T, typename S<T>::type * ret); 
  
 int j = foo(1, 0); 
 ---------------------- 
 icc accepts this, gcc doesn't. I believe it should. The question is whether 
 the second of the arguments, of type int, should be converted to the  
 expected int*. During template deduction, no conversions are performed. I.e. 
 if we had declared foo as foo(T,T*), then we should reject the call. However, 
 the template type T is used in a non-deductible context in the second argument 
 of foo, so the type of T should be deduced only based on the first argument, 
 to type int then.
*/

template <class T> struct S { typedef int type; };

template <class T>
int foo(T, typename S<T>::type * ret);

// Error in ROSE: was unparsed as "int j = foo(1,(0));" the explicit cast is missing in the generated code
int j = foo(1, (int*)0);

// This compiles using EDG, and war unparsed as "int j = foo(1,(0));", but does not compile with g++
// this is an example of something that EDG permits but g++ does not permit (at least for g++ version 3.3.2)
// Since this is a reported bug in g++ we don't really have to fix it. 
// int k = foo(1, 0);
// #endif

int *l = (int*) 0;
