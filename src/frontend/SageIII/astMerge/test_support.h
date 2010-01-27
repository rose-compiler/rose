#ifndef __TEST_SUPPORT
#define __TEST_SUPPORT
class TestUniqueNameGenerationTraversal : public ROSE_VisitTraversal
   {
     public:
          void visit ( SgNode* node);

       // This avoids a warning by g++
          virtual ~TestUniqueNameGenerationTraversal(){};
   };

void testUniqueNameGenerationTraversal();

class ReportUnsharedDeclarationsTraversal : public ROSE_VisitTraversal
   {
     public:
          void visit ( SgNode* node);

       // This avoids a warning by g++
          virtual ~ReportUnsharedDeclarationsTraversal(){};
   };

void reportUnsharedDeclarationsTraversal();

// Hides details of the use of the STL set_difference algorithm
std::set<SgNode*> computeSetDifference(const std::set<SgNode*> & listToDelete, const std::set<SgNode*> & requiredNodesTest);

std::set<SgNode*> computeSetIntersection(const std::set<SgNode*> & listToDelete, const std::set<SgNode*> & requiredNodesTest);
#endif
