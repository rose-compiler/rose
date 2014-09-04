namespace SAMRAI 
   {
     class Dimension {};
   }

using namespace SAMRAI;

class XXX
   {
     public:
       // BUG: this will unparse as "Dimension m_dim;" instead of "SAMRAI::Dimension m_dim;" 
       // because of the SgUsingDirectiveStatement below.
          SAMRAI::Dimension m_dim;
   };


