namespace SAMRAI 
   {
     class Dimension {};
   }

class XXX
   {
     public:
       // BUG: this will unparse as "Dimension m_dim;" instead of "SAMRAI::Dimension m_dim;" 
       // because of the SgUsingDirectiveStatement below.
          SAMRAI::Dimension m_dim;
   };

// With this using declaration, we do NOT properly name qualify "SAMRAI::Dimension m_dim;"
using namespace SAMRAI;

