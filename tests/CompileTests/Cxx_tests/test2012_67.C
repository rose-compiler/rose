template <typename T>
class A 
   {
     public:
          A() {};
       // A();
   };

// This is a special case of building multiple defining declarations (of different functions from ROSE's perspective but using the same function pointer from EDG's perspective)
// A<int> SgTemplateMemberFunctionDeclaration constructor (defining declaration) is referenced twice.
class B : public A<int>
   {
     public:
       // B() : A<int>() {};
          B() {};
   };

