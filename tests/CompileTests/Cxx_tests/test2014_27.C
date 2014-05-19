// ROSE bug from Chombo application code.

namespace Chombo
   {
     class Box
        {
          friend Box minBox (const Box& b1,const Box& b2);
        };

     Box minBox (const Box& b1,const Box& b2);

     class IntVectSet
        {
          public:
               IntVectSet& operator|=(const IntVectSet& ivs);

               const Box& minBox() const;

        };

     IntVectSet& IntVectSet::operator|= (const IntVectSet& ivs)
        {
          Chombo::Box a;
          Chombo::minBox(a,a);

          return *this;
        }
   }
