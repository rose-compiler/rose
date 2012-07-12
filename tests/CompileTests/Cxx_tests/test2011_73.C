// ********************************************
// This test code is a subset of test2005_114.C
// ********************************************

// This works fine! But it has not declaration (SgNamespaceDeclaration)
namespace {}

static union {};

#if 0
void foo ()
   {
  // This does not unparse to be ANYTHING, it is ignored (perhaps as it should be)
     union {};
//   union { int x; int y; };
   }
#endif

#if 0
// This does not unparse to be ANYTHING, it is ignored (perhaps as it should be)
static union {};

// This works fine!
struct A { struct {}; };

// This works fine (now)!
void bar1() try {} catch (int) {}

// Need to implement throw list (comment about this is placed into unparsed code)!
void bar2() throw (int, float) {} /* no throw list */

// This works fine!
typedef enum E {} E; /* unparser problem */

// This is interesting code!!!  But now it works...
struct T { enum c { i }; };
struct U : T { using T::i; };
#endif
