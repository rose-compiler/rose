// #include "rose.h"

// using namespace std;

class SectionBoundaryTraversal : public AstSimpleProcessing
   {
  // This traversal detects boundaries of sections.  The information gathered is
  // used by other traversals to classify addresses that fall within the section
  // boundaries.

     public:
         std::vector<SgAsmGenericSection*> sectionList;
          virtual void visit(SgNode* n);
   };

std::vector<SgAsmGenericSection*> generateSectionList ( SgProject* project );

