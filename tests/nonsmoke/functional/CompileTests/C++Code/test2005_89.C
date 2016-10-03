// This file was sugmitted by Kyle Chand

// According to the C++ standard in temp.dep part 3:
/* " In the definition of a class template or in the definition of a
   member of such a template that appears outside of the template
   definition, if a base class of this template depends on a
   template-parameter, the base class scope is not examined during
   name lookup until the class template is instantiated. "

   from : http://library.n0i.net/programming/c/cp-iso/template.html#temp.dep
*/

// Below are some examples of how seemingly ok code is "wrong" under the above
//       statement.  Since templated base classes are not searched for names involving
//       one of the template parameters when a templated subclass is declared we
//       can get undefined members and types.

// comment the next line to get things to compile
// #define BROKEN 

// Consider the non-templated base class C
class C {

protected:
  void *ptr;
};

// Csub inherits from C and can see C's protected data 
class Csub : public C {
public:
  void *getp() { return ptr; }
};

// Consider the templated base class A:
template<typename T>
class A {

protected:
  T* ptr;
  int i;
};

// We can make a concrete sub class that can see the templated base class's protected data
//    without further name lookup directives
class Bconcrete : public A<void> {
public:
  void *getp() { return ptr; }
};

// sadly, name lookup clues are needed for templated sub-classes, either 
//        with the using directive...
template<typename T>
class Busing : public A<T> {

public:

  using A<T>::ptr;
  T *getp() { return this->ptr; }

};

// ... or qualifying the name with the this pointer
template<typename T>
class Bthis : public A<T> {

public:

  T *getp() { return this->ptr; }
  
};

#ifdef BROKEN

// Using the same approach as for concrete classes is NOT allowed:
//  (the following code should not compile, and does not under g++ 3.4+)
template<typename T>
class Bbroken : public A<T> {

public:

  T *getp() { return ptr; }
};

#endif

// DQ: comment out to skip explicit declaration of specialization of templated class
// #define INSTANTIATE_TEMPLATE_CLASS
#ifdef INSTANTIATE_TEMPLATE_CLASS
// DQ: Forward declaration of instantiated class
template <> class Bthis < double >;
#endif

// Amusingly, this works since "i" does not depend on the template parameter
template<typename T>
class BtryInt : public A<void> {

public:
  int getInt() { return i; }
};

#ifdef INSTANTIATE_TEMPLATE_CLASS
// DQ: This requires the base class specification within the specialization!
// This does not appear to be output by EDG!!!
template <> class Bthis < double > : public A<double> {} ;
#endif

// Again, a concrete subclass works as expected
class BconcreteSubclass : public Bthis<double> {
using Bthis<double>::ptr;
public:
  double *getp1() { return ptr; }
};

// ... and just in case instantiation causes any other troubles ...
void foo() {
  A<int> a;
  Bthis<int> bt;
  Busing<int> bu;
  C c;
  BtryInt<double> bti;

}
