// This is testcode is currently a bug in ROSE. Now that we avoid over
// qualification, the "::" global qualification is not output as before.
// However this is an example of where Argument Dependent Lookup (ADL) 
// forces the requirement of global qualification on "::bar(a);" in function
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
namespace N { class A; }
void bar (const N::A&);

// ====================================================
namespace N
{
  // This 'bar' is not the true friend.
  void bar (const A&);

  // Class declaring true friend.
  class A
  {
  public:
    A () { ::bar (*this); }
    friend void ::bar (const A&); // '::' must be preserved.

  private:
    void for_true_friends_only (void) const;
  };
}

// ====================================================
// This 'bar' is the true friend.
void bar (const N::A& x)
{
  x.for_true_friends_only ();
}



