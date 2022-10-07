// Bug specific to EDG 6.3

// Declaration of template class A
template<typename T, bool value = false>
class A
   {
#if 0
     public:
          void foo();
#endif
   };

// Specialization of template class A
template<typename T>
class A<T, false>
   {
   };

// Forward declaration of undefined template struct B.
template<typename T> struct B; 

// Declaration of another template struct B
template<typename T1, typename T2>
struct B<T1 T2::*>
   : A<T1 T2::*>
   {
  // DQ: fails on this line building a using declaration for a base-class
  // In EDG 6.0 this is a type and then resolved to be a class.
  // In EDG 6.3 it is a base-class.
     using A<T1 T2::*>::A;
#if 0
     public:
          void foobar() { A a; }
#endif
   };

