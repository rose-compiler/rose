namespace SAMRAI {
namespace tbox {

class Dimension {};

}
}

class PertHypreSolver
   {
     public:
          void allocateHypreData();

     private:
          SAMRAI::tbox::Dimension m_dim;
};

// With this using declaration, we do NOT properly name qualify "SAMRAI::tbox::Dimension m_dim;"
using namespace SAMRAI;

