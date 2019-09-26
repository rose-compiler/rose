// This is a collection of difficult cases from Peter

// Note about "conditions" used in loop constructs.
// "Conditionals" are either:
//    1) an expression, or
//    2) a declaration with initializer
// Note also that a SgConditional is different (a ternary operator (<test expr>) ? <true part> : <false part>;)
// There are a few errors in ROSE, locations in the IR where statments are used instead of expressions and
// expression used instead of statements. (reference Appendix A.6)
// The following should have the "condiditonal" concept:
//      if ( condition ) statement     // Currently correct
//      switch ( condition ) statement // Currently uses SgExpression (wrong)
//      while ( condition ) statement  // Currently correct
//      for ( for-init-statment; condition; expression ) statement  // Currently uses SgExpression (wrong)
//  The following should have an expression:
//      do statement while ( expression );  // currently uses SgStatment instead of SgExpression
// 
//  A possible fix would be to make them all statements (use SgStatement for the "conditional" concept)
//  and let the front-end sort out what is allowed and disallowed.

// Double check this detail with Markus:
// In the following example (using a preorder traversal) the test expression is seen before
// the body because the order of the specification of the data fields in ROSETTA should be reversed.
//     do {} while (1);  /* traversal: while-expr (intentionally ?) gets traversed before the block */
//     while (1) {}  // appears to be correct (Peter) and do-while is in reverse order.


// This works fine! But it has not declaration (SgNamespaceDeclaration)
namespace {}

// Function introduced so that we could define union in non-global scope
void foo ()
   {
  // This does not unparse to be ANYTHING, it is ignored (perhaps as it should be)
     union {};
     union { int x; int y; };
   }

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

void func()
   {
  // These cases work fine
#if 1
     for (; ;);
#endif
#if 1
     for (int a = 0,b = 1,c = 2; ;);
#endif

  // Not sure how to handle this, but perhaps a null expression concept would be 
  // helpful (unparsed as: "for (int b = (1); ; ) {}", which is OK)
  // for (; int b = 1; );

#if 0
  // Examples taking a "conditional" concept (which seem like they should work)
     if (int i = 0;);
     while (int i = 0;);
     for (; int b = 1; );

     do ; while (0);  // this should work (and does)
     do ; while (int i = 0; );  // this should fail (it does fail in the front-end)
#endif

     if (int i = 0);
     for (; int b = 1; );
     for (int b = 1;; );
     do ; while (0);  // this should work (and does)

  // switch(0) default: ;
     switch(int y = 1) { default: ; }    
     switch(0) { int y; default: y = 0; }

#if 0
     switch(0) default: ;

     switch(int y = 1) { default: ; }    
     switch(0) { int y; default: y = 0; }
  
     do {} while (1);  /* traversal: while-expr (intentionally ?) gets traversed before the block */
#endif

#if 1
  // Test for loop using declaration for test condition
  // for (; int b = 1; );
     for (; int b = 1; );
#endif

#if 1
  // Test of declaration in conditional of switch statement
     switch(int y = 1) { default: ; }    
#endif
   }
