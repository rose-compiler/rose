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
            // void for_true_friends_only (void) const;
        };
   }
