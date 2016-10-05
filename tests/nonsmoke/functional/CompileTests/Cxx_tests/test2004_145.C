
template <typename T>
class A
   {
     public:
          class Y;
   };

template <typename T>
class X
   {
     public:
          static void foo();
   };

// The template definition of foo() should be:
//    extern "C++" template<> void X< A< int > ::Y *> ::foo();
// but is:
//    extern "C++" template<> void X< Y *> ::foo();

void foobar()
   {
  // This example used a template argument which is a pointer to a class, 
  // as a result it fails and unparses "class A<int> {}*" instead of "A<int>*"
  // This also shows the case where the template parameter will be unparsed 
  // as the base_type of the typedef ("A<int>::Y" instead of "Z")
     typedef A<int>::Y Z;

  // This should be:
  //    X< A< int > ::Y *> :: foo ();
  // but is:
  //    X< Y *> :: foo ();
     X<Z*>::foo();
   }


