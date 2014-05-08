#include "sage3basic.h"  // must be the first to be included
#include "transformationTracking.h"

using namespace std;

namespace TransformationTracking {

  // the following data structures are for internal use only
  std::map <SgNode*, unsigned int > AstToIdTable;
  // reverse lookup 
  std::map <unsigned int, SgNode*> IdToAstTable; 
  static int nextId = 1; 

  AST_NODE_ID getId (SgNode* n)
  {
    return AstToIdTable[n];
  }

  void setId (SgNode* n)
  {
    ROSE_ASSERT (n != NULL);
    AST_NODE_ID cur_id = getId(n);
    // only set when the node has no ID assigned right now
    if (cur_id ==0)
    {
      // find  a unused Id
      while (IdToAstTable[nextId]!= NULL)
        nextId ++;
      //Update the lookup tables  
      AstToIdTable[n] = nextId;
//      cout<<n->class_name() <<" @ "<<n <<" cur_id is set to \n\t"<< nextId << "."<<endl;
      IdToAstTable[nextId] = n; 
      nextId ++;
    }
    else
    {
//      cout<<n->class_name() <<" @ "<<n <<" skipped since cur_id is already set." <<endl;
    }
  }
class visitorTraversal : public AstSimpleProcessing
  {
    protected:
      void virtual visit (SgNode* n)
      {
        if (isSgProject(n)|| isSgSourceFile(n)||isSgInitializedName(n)||isSgLocatedNode(n))
        {
         setId (n);
        }
      }
  };

void registerAstSubtreeIds (SgNode* root)
{
   visitorTraversal myTraversal;
   // we want to traverse full AST to ID them all
   myTraversal.traverse(root, preorder);
}

#if 0
  void registerAstSubtreeIds (SgNode* root)
  {
    // We only care about located nodes for now since they are visible to users in source code
    // we have to include SgProject and SgSourceFile nodes also.
    VariantVector vv;  
    vv.push_back(V_SgProject);
    vv.push_back(V_SgSourceFile);
    vv.push_back(V_SgLocatedNode);
    Rose_STL_Container <SgNode*> nodeList = NodeQuery::querySubTree (root, vv);
    for (Rose_STL_Container <SgNode*>::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++)
    {
      SgNode* n = (*iter);
      setId (n);
    } 
  }
#endif

}

