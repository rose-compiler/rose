// number #37

// Example from Rich:

/* ====================================================
 * This example shows the need for name qualification
 * for 'friend' function declarations. In ROSE 0.8.9a,
 * class N::A's 'friend' declaration for 'bar' loses
 * its global qualifier ('::') when unparsed.
 * ==================================================== */

// Forward declaration of the true friend.
void bar();

// ====================================================
namespace N
{
  // This 'bar' is not the true friend.
  void bar ();

  // Class declaring true friend.
  class A
   {
     public:
          friend void ::bar(); // '::' must be preserved.
   };
}
