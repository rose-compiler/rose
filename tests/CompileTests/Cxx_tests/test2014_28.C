// ROSE bug from Chombo application code.

namespace Chombo
   {
     class Box
        {
          friend void minBox (int n);
        };

     void minBox (int n);

     class IntVectSet
        {
          public:
            // IntVectSet& operator|=(const IntVectSet& ivs);
               void foo();

               void minBox() const;

        };

  // IntVectSet& IntVectSet::operator|= (const IntVectSet& ivs)
     void IntVectSet::foo()
        {
          Chombo::minBox(42);
        }
   }
