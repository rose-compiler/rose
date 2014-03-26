#include "transformationTracking.h"
#include "sage3basic.h" 

namespace TransformationTracking {

  // the following data structures are for internal use only
  std::map <SgNode*, unsigned int > AstToIdTable;
  // reverse lookup 
  std::vector <SgNode*> IdToAstTable; 
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
      IdToAstTable[nextId] = n; 
      nextId ++;
    }
  }

  void registerAstSubtreeIds (SgNode* root)
  {
    // We only care about located nodes for now since they are visible to users in source code
    Rose_STL_Container <SgNode*> nodeList = NodeQuery::querySubTree (root, V_SgLocatedNode);
    for (Rose_STL_Container <SgNode*>::iterator iter = nodeList.begin(); iter != nodeList.end(); iter++)
    {
      SgNode* n = (*iter);
      setId (n);
    } 
  }


}

