// This is a copy of the test2006_159.C test code (simplified).

// Forward declaration of the true friend.
namespace N { class A; }
void bar (const N::A & x);

namespace N
   {
  // This 'bar' is not the true friend.
     void bar (const A&);

  // Class declaring true friend.
     class A
        {
          public:
               friend void ::bar (const A&); // '::' must be preserved.

          private:
               void for_true_friends_only (void) const;
        };
   }

// This 'bar' is the true friend.
void bar (const N::A & x)
   {
     x.for_true_friends_only ();
   }


