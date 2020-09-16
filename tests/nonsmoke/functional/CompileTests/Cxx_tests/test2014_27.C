// ROSE bug from Chombo application code.

namespace Chombo
   {
     class Box
        {
       // Generated code (only an issue of the GNU 10.2 compiler).
       // public: friend class Box Chombo::minBox(const class Box &b1,const class Box &b2);
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
