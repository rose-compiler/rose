// Bug specific to EDG 6.3

// Declaration of template class A
class A
   {
   };

class B
   : public A
   {
  // DQ: fails on this line building a using declaration for a base-class
  // In EDG 6.0 this is a type and then resolved to be a class.
  // In EDG 6.3 it is a base-class.
     using A::A;
   };
