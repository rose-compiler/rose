template <typename T>
class foobar 
   {
     public:
          foobar() {};
       // A();
   };

// This is a special case of building multiple defining declarations (of different functions from ROSE's perspective but using the same function pointer from EDG's perspective)
// A<int> SgTemplateMemberFunctionDeclaration constructor (defining declaration) is referenced twice.
class B : public foobar<int>
   {
     public:
       // B() : foobar<int>() {};
          B() {};
   };

