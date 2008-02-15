// Test code for globalAndStaticData.C
// This code is input for a program that detects global variables and static member 
// variables of type class (class, struct, or union).  variables which are pointers
// or references to classes are ignored.

// Need to modify to handle templates as well

// Additional cases:
//    1) static variable declarations in functions
//    2) function initializers for global or static data member pointers and references.
//    3) all parameters (including default parameters) for above functions.
//    4) All functions with static data members
//    5) Check all initializers (NULL is OK)

class X
   {
   };

X problemObject;   // Example of problem declaration (found)

X *okPointerToObject;

class Y
   {
     public:
          static X problemObject; // Example of problem declaration (found)

          static X *okStaticPointerToObject;

          X okObject;
          X *okPointerToObject;
          X & okReferenceToObject;

          typedef X typedefOfObject;

          static typedefOfObject problemObjectHiddenBehindTypedef;  // Not currently found as problem declaration

       // Note: interestingly this constructor is not required where as Yt's constructor is required
          Y() : okReferenceToObject(okObject) {};

       // This might be hard to locate!
          static const X staticArrayOfX[100];
          static const typedefOfObject staticArrayOfX[100];
   };

template <typename T = int>
class Xt
   {
   };

Xt<int> problemObjectInstantiation_Xt;   // Example of problem declaration (found)


Xt<int> *okPointerToObjectInstantiation;

template <typename T = int>
class Yt
   {
     public:
          static Xt<int> problemObject; // Example of problem declaration (found in compiler-generated instantiation)

          static Xt<int> *okStaticPointerToObject;

          Xt<int> okObject;
          Xt<int> *okPointerToObject;
          Xt<int> & okReferenceToObject;

          typedef Xt<int> typedefOfObject;

          static typedefOfObject problemObjectHiddenBehindTypedef;  // Not currently found as problem declaration

       // Note: This constructor is required else error "implicitly generated constructor for class "Yt<int>" 
       // cannot initialize: reference member "Yt<T>::okReferenceToObject [with T=int]""
          Yt() : okReferenceToObject(okObject) {};
   };

Yt<int> problemObjectInstantiation_Yt;

