

#include "rose.h"

using namespace std;


class DeleteAST2 : public SgSimpleProcessing {
public:
  //! Required traversal function
  void visit (SgNode* node) {
    if (node) {
//      if (!isSgAsmExecutableFileFormat(node)) {
      	delete node;
//      }
    }
  }
};


int main(int argc, char** argv) {

  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT(project);

  VariantVector vv1 = V_SgNode;
  vector<SgNode*> vec = NodeQuery::queryMemoryPool(vv1);
  std::cout << "Number of nodes before AST deleting: " <<
    vec.size() << std::endl;

  DeleteAST2 deleteTree;
   deleteTree.traverse(project,postorder);

   vec = NodeQuery::queryMemoryPool(vv1);
   std::cout << "Number of nodes after AST deletion: " <<
     vec.size() << std::endl;

  map<std::string, int> nodes;
  vector<SgNode*>::const_iterator it = vec.begin();
  for (;it!=vec.end();++it) {
    SgNode* node = *it;
    ROSE_ASSERT(node);
    std::string name  = node->class_name();
    nodes[name]++;
    delete node;
  }

  map<std::string, int>::const_iterator it2 = nodes.begin();
  for (;it2!=nodes.end();++it2) {
	  string name = it2->first;
	  int amount = it2->second;
	  cout << "  Could not delete : " << name << "  -  " << amount << endl;
  }

  std::cout << "Number of nodes after deleting in Memory pool: " <<
    NodeQuery::queryMemoryPool(vv1).size() << std::endl;

  std::cout << ">> Rerunning frontend ..." << std::endl;

  project = frontend(argc,argv);
  ROSE_ASSERT(project);


}
