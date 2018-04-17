

#include "rose.h"

using namespace std;

class DeleteAST : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

void
DeleteAST::visit(SgNode* node)
   {
     delete node;
   }

void DeleteSgTree( SgNode* root)
{
  DeleteAST deleteTree;
  deleteTree.traverse(root,postorder);
}


int main(int argc, char** argv) {


  //DeleteAST
  //QueryMemoryPool all nodes
  //    num

  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT(project);

  DeleteSgTree(project);


  std::cout << "Statistics" << std::endl;
  std::cout << AstNodeStatistics::IRnodeUsageStatistics() << std::endl;
  std::cout << "End Statistics" << std::endl;

  VariantVector vv1 = V_SgNode;
  vector<SgNode*> vec = NodeQuery::queryMemoryPool(vv1);
  std::cout << "Number of nodes before deleting: " <<
    vec.size() << std::endl;

  vector<SgNode*>::const_iterator it = vec.begin();
  for (;it!=vec.end();++it) {
    SgNode* node = *it;
    ROSE_ASSERT(node);
    delete node;
  }

  std::cout << "Number of nodes after deleting in Memory pool: " <<
    NodeQuery::queryMemoryPool(vv1).size() << std::endl;

 // project = frontend(argc,argv);
 // ROSE_ASSERT(project);


}
