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

            // Put this before the friend declaration, in case it can be found first.
               void bar (const A&);

               friend void ::bar (const A&); // '::' must be preserved.

            // Put this after the friend declaration, in case it can be found last.
            // void bar (const A&);

          private:
            // void for_true_friends_only (void) const;
        };
   }
