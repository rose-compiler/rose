#include "sage3basic.h"  // must be the first to be included
#include "transformationTracking.h"

using namespace std;

namespace TransformationTracking {

  // the following data structures are for internal use only
  std::map <SgNode*, unsigned int > AstToIdTable;
  // reverse lookup 
  std::map <unsigned int, SgNode*> IdToAstTable; 
  static int nextId = 1; 
  
  // internal storage to store file info for each node
  std::map<AST_NODE_ID, std::pair <Sg_File_Info*, Sg_File_Info*> > IdToFileInfoTable;

  // public interface implementation
  AST_NODE_ID getNextId ()
  {
    return nextId;
  }

  AST_NODE_ID getId (SgNode* n)
  {
    //TODO ROSE reuses released memory space to be reused by new AST nodes. 
    //Using  memory address as the key is not reliable to indicate unique AST nodes! 
    return AstToIdTable[n]; 
  }

  // an internal helper function
  static std::pair <Sg_File_Info*, Sg_File_Info*> getBeginAndEndFileInfo (SgNode* n)
  {
    std::pair <Sg_File_Info*, Sg_File_Info*> rtvalue; 
    ROSE_ASSERT (n != NULL);
    if (isSgPragma(n)||isSgLocatedNode(n))
    {
      rtvalue = std::make_pair (n->get_startOfConstruct(), n->get_endOfConstruct());
    }
    return rtvalue;
  }

  std::pair<Sg_File_Info*, Sg_File_Info*>  getFileInfo(AST_NODE_ID id)
  {
    return IdToFileInfoTable[id];  
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
// We store the file info at the same time
      IdToFileInfoTable[nextId] = getBeginAndEndFileInfo (n);
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
        if (isSgProject(n)
          ||isSgSourceFile(n)
          ||isSgInitializedName(n)
          ||isSgLocatedNode(n)
          ||isSgFileList(n))
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

