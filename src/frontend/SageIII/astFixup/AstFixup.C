#include "sage3basic.h"
#include "AstFixup.h"

// Most source code originally in this file has been separated into individual source files

// DQ (2/6/2010): This codes was moved from tests/trestTranslator.C
// It is not currently called within the the fixup of the AST.
// Note that this used to be called using: 
//      if (getenv("ROSE_TEST_ELSE_DISAMBIGUATION") != NULL) removeEmptyElses(project);
// But I didn't feel that we wanted evnvironment variable based testing like this.
// This is a fixup that we might want later.
void removeEmptyElses(SgNode* top)
   {
     std::vector<SgNode*> ifs = NodeQuery::querySubTree(top, V_SgIfStmt);
     for (size_t i = 0; i < ifs.size(); ++i)
        {
          SgIfStmt* s = isSgIfStmt(ifs[i]);
          if (isSgBasicBlock(s->get_false_body()) && isSgBasicBlock(s->get_false_body())->get_statements().empty())
             {
               s->set_false_body(NULL);
             }
        }
   }



