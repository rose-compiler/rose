

#include "rose.h"

using namespace std;

int main(int argc, char** argv) {

  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT(project);

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

  project = frontend(argc,argv);
  ROSE_ASSERT(project);


}
