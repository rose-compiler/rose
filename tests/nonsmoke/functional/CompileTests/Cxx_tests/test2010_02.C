// DQ (2/8/2010): This may not be the minimal version of an example that demonstrats this unparser bug.

#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"

using namespace std;

set<SgNode*>
generateNodeListFromAST ( SgNode* node )
   {
  // Generate a list of the SgNode* in the AST (subset of all IR nodes in the memory pool)
     class NodeListTraversal : public SgSimpleProcessing
        {
          public:
              set<SgNode*> nodeList;
              void visit (SgNode* n )
                 {
// #ifndef USE_ROSE
                // DQ (2/8/2010): This code demonstrates a bug in the unparser for ROSE when run using tests/nonsmoke/functional/testCodeGeneration
                   ROSE_ASSERT(n != NULL);
                // printf ("In generateNodeListFromAST building nodeList n = %p = %s \n",n,n->class_name().c_str());
                   nodeList.insert(n);
// #endif
                 }
        };

     NodeListTraversal t;
     t.traverse(node,preorder);

     return t.nodeList;
   }
