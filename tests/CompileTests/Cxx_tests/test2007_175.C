// This is a simplified version of test2006_159.C (from Rich).
// It is an example of where Argument Dependent Lookup (ADL) forces the 
// requirement of global qualification on "::bar(a);" in function
// outlinedExample().  This fails because ROSE name qualification 
// does not support ADL.

// Example from Rich:

/* ====================================================
 * This example shows the need for name qualification
 * for 'friend' function declarations. In ROSE 0.8.9a,
 * class N::A's 'friend' declaration for 'bar' loses
 * its global qualifier ('::') when unparsed.
 * ==================================================== */

// Forward declaration of the true friend.
// namespace N { class A; }
// void bar (const N::A&);

// ====================================================
namespace N
   {
  // Class declaring true friend.
     class A  { };

  // This 'bar' is not the true friend.
     void bar (const A&);
   }

// ====================================================
// This 'bar' is the true friend.
void bar (const N::A& x)
{
}

void outlinedExample ()
   {
     N::A a;
     ::bar(a);
   }
