// This is the simplist example problem that fails the AST tests
// it appears that some SgTemplateInstantiationMemberFunctionDecl
// is shaed and causes the attribute test to fail (detects that
// it has previously added an attribute to an IR node in the AST).

// The problem is now fixed and was:
// The AST Copy mechanism that is being used to generate forward
// declarations (part of template support) was using the shallow 
// copy mechanism which was forcing the function parameter list 
// and initialized names (parameters) to be shared.  This was 
// causing the AST attribute tests to fail since these test for 
// any shared IR nodes (great tests!).

template <typename T>
class X
   {
     public:
//       ~X();
         void foo();
   };

// X<int>::~X() {}

X<int> x;

void X<int>::foo()
   {
   }

void foobar();

void foobar()
   {
     x.foo();
   }

// test for how to support getting function IR nodes 
// from a string representing the function name!
// extern "C++" X<int>::foo();
