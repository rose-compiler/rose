#include "rose.h"
#include <string>
#include "abstract_handle.h"
#include "roseAdapter.h"

using namespace std;
int main(int argc, char** argv)
{
  SgProject *project = frontend (argc, argv);

  Rose_STL_Container<SgNode*> sg_nodes = NodeQuery::querySubTree(project,V_SgLocatedNode);
  cout<<"Building and verifying abstract handles......:\n"<<endl;
  for (Rose_STL_Container<SgNode *>::iterator i = sg_nodes.begin(); i != sg_nodes.end(); i++)
  {
    SgNode* cur_node = *i;
    // We skip nodes from headers, builtin functions.
    // TODO some builtin functions cause some troubles
    if (!cur_node->get_file_info()->isCompilerGenerated())
    {
      AbstractHandle::abstract_handle* handle = SageBuilder::buildAbstractHandle(cur_node);
      cout<<handle->toString()<<endl;
      // test and verification
      SgNode* match_node = SageInterface::getSgNodeFromAbstractHandleString(handle->toString());
      ROSE_ASSERT(cur_node == match_node);
    }
  }
  AstTests::runAllTests(project);
  return backend (project);
}
