namespace SAMRAI {
namespace tbox {

class Dimension
   {
     public:
          unsigned short getValue() const;
   };

}
}

namespace XXX {

class PertHypreSolver
   {
     public:
          void allocateHypreData();

     private:
          SAMRAI::tbox::Dimension m_dim;
};

} // namespace XXX

// With this using declaration, we do NOT properly name qualify "SAMRAI::tbox::Dimension m_dim;"
using namespace SAMRAI;

namespace XXX {

void PertHypreSolver::allocateHypreData()
   {
  // DQ (7/8/2014): This is the problem line of code.
     int flag[m_dim.getValue()];
   }

} // namespace XXX

