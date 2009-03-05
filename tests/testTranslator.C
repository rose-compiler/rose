// Example ROSE Translator
// used for testing ROSE infrastructure

#include "rose.h"

// DQ (2/22/2009): This should be removed, if we want this sort of 
// normalization then it should be put elsewhere in ROSE (astPostProcessing, for example).
static void removeEmptyElses(SgNode* top) {
  std::vector<SgNode*> ifs = NodeQuery::querySubTree(top, V_SgIfStmt);
  for (size_t i = 0; i < ifs.size(); ++i) {
    SgIfStmt* s = isSgIfStmt(ifs[i]);
    if (isSgBasicBlock(s->get_false_body()) &&
        isSgBasicBlock(s->get_false_body())->get_statements().empty()) {
      s->set_false_body(NULL);
    }
  }
}

int main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);
     AstTests::runAllTests(project);

     if (getenv("ROSE_TEST_ELSE_DISAMBIGUATION") != NULL) {
       removeEmptyElses(project);
     }

     return backend(project); // only backend error code is reported
   }
