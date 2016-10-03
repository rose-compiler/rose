
// Test for where parent pointers are not a correct indicator of scope.
// Basically we need to represent scope explicitly for some C++ constructs since 
// we can't tell from the structure what the scope is, and we can't be 
// parsing to determine such things else why build the AST in the first place :-).

// The reason for worrying about this is that I believe it is the root cause for 
// the remaining bug in ROSE which is preventing us from compiling KULL. 
// Comments about the problem and the proposed fix are welcome.  But please
// get back to me quickly, as I will likely start on the fixes shortly.

// There are numerous places where within C++ the structural location of the
// definition is no particular indicator of its scope.  In general this is
// because the definition can be separated from the declaration and while the
// must appear in the structural location which does indicate scope, the 
// definition may appear in another scope using a qualified name!  Currently
// in ROSE we use the structural representation (encoded using the parent pointer)
// to determine the scope, and this is a mistake.

// I expect to fix this by doing what is done currently for the case of member functions.
// In this case an obvious, or at least better understood and more common case where the 
// structural representation does not represent the scope; because a function may be defined
// either inside the class or outside the class (both cases are common).  In the case of 
// member functions, we explicitly store a pointer to the class declaration to which the 
// member function belongs.  I think that that is exactly what is required in numerous 
// other cases as well.  To build the list of cases where this technique must also be applied
// I have written the following code to show that there are numerous C++ constructions of
// code where the scope must be saved explicitly (and can't be derived from the structural
// representation of the code (such as we have in the AST).  The list unexpectedly long and
// includes: just about all types of declarations (except enums, which can not have a forward 
// declaration and for which the declaration and definition are tightly bound).
// (as a detail, forward declarations of enums appear the be the most common language extension
// as it seems that all compilers except g++ implement it, those it is disallowed by the C++ 
// standard).

// Effected declarations:
//     class declarations (and derived classes (e.g. template instantiation declarations)
//     template declarations (which covers all forms of template declarations for classes, functions, and member functions)
//     function declarations
//     typedef declarations
//     variable declarations (this was a little surprising to see!)
//          Actually the scope is added to the SgInitilizedName objects instead of the SgVariableDeclaration!
// These appear to be the only sorts of declarations which can define constructs 
// scopes other than the ones where they appear.

// I propose that each of these classes within Sage III be extended to include a data member
// SgScopeStatement *p_scope (though the member function currently has a data member 
// "SgClassDeclaration *p_scope;" which is sufficient for that specific case. I would suggest that
// it be removed in favor of "SgScopeStatement *p_scope;" in the SgFunctionDeclaration base class.
// of course this will effect the parameter list of the constructor for the but only the within the
// last parameter or two so not so much).

class A 
   {
  // forward declaration of B
     class B;
   };

// definition of B (defined in global scope using qualified name), but B is in the scope of A
class A::B {};

namespace C
   {
  // forward declaration of D
     class D;
     void foo();

  // forward declaration of variable in scope C
     extern int x;
   }

// Class definition of D with scope in C but defined in global scope
class C::D {};

// Function with scope in C but declaration in global scope using qualified name
void C::foo() {}

// Variable declaration with scope of C defined in global scope
int C::x = 0;

class D
   {
     class E
        {
          class F;
        };

  // DQ (11/18/2004): Note that g++ allows this but EDG does not!  I think that EDG is correct here!
#if 0
  // Definition of F with parent which is a class declaration (designed this test to trip up unparser 
  // which will in this specific case detect that the parent is a class definition and only use the 
  // unqualified name; "F" instead of "E::F").

  // This is illegal C++ (but accepted by g++)
     class E::F {};
   };
#else
   };
// This is what I think is the correct C++ construction
class D::E::F {};
#endif

namespace G
   {
     namespace H
        {
           class I;
        }
   }

class G::H::I {};

// namespace aliasing can only build an alias in the scope of the namespace alias declaration
namespace GG = G;

// This is currently a bug in ROSE (isolated into test2004_135.C)
// namespace GH = G::H;

// this is fortunately not allowed (defining an alias in another scope is not allowed)
// namespace G::HH = G::H;

#if 0
// It does not appear that namespaces can be referenced from other scopes using qualified names (at least not in g++ 3.3.2)
// this make life simpler, so I'm all for it, but I'm not clear what the standard says about this detail.  But it means
// that namespaces don't have to have an explicit scope representation (since it would be redundant with the parent pointer).
namespace G::H
   {
     class J;
   }

// class H::J {};
#else
namespace G
   {
     namespace H
        {
          class J;
        }
   }

class G::H::J {};
#endif

namespace K
   {
     class L;
     typedef L M;
   }

// A reasonable typedef of a type in namespace K to build an alias in global scope
typedef K::M N;

// Can't use typedef to specify a type in another scope (not surprising since there is no concept of a 
// typedef definition separated from a typedef declaration, though it is called "typedef" instead of 
// "typedecl" :-).
// typedef K::L K::M;

namespace G
   {
     template <typename T> class O;
   }

// Template declarations clearly need an explicit representation of their scope
template <typename T> class G::O {};
