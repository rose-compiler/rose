#include "sage3basic.h"  // must be the first to be included
#include "transformationTracking.h"

using namespace std;

namespace TransformationTracking {

  // the following data structures are for internal use only
  std::map <SgNode*, unsigned int > AstToIdTable;
  // reverse lookup , oops, if they are one-to-one mapping, how a single node obtains multiple IDs?
  std::map <unsigned int, SgNode*> IdToAstTable; 
// IDs start from 1, Default value of 0 means a node has not assigned an ID.
// Using this will save the initialization of the Id value to be uninitialized!
  static int nextId = 1; 
  
  // internal storage to store the original file info for each node 
  std::map<AST_NODE_ID, std::pair <Sg_File_Info*, Sg_File_Info*> > IdToFileInfoTable;

  // IDs of all input Nodes of a transformation
  std::map<AST_NODE_ID, std::set<AST_NODE_ID> > inputIDs; 

 // To simplify the problem, 
 // we keep track of information at a single line statement level for now.
 //
 // For a transformed (statement) node, add an input (statement) node. 
 // Internally, the input node's ID will be saved. 
 //
 // This should be called after the transformation is completed and all IDs are assigned. 
  void addInputNode(SgNode* affected_node, SgNode* input_node)
  {
    ROSE_ASSERT (affected_node != NULL);
    ROSE_ASSERT (input_node!= NULL);

    AST_NODE_ID affected_id = getId (affected_node);
    ROSE_ASSERT (affected_id != 0);
    AST_NODE_ID input_id = getId (input_node);
    ROSE_ASSERT (input_id != 0);
#if 0 // debugging
   cout<<"Trans tracking: affected_node id:"<<affected_id <<" line:" << isSgLocatedNode(affected_node)->get_file_info()->get_line() 
       << " input node id:"<< input_id<<" line:" << isSgLocatedNode(input_node)->get_file_info()->get_line()  <<endl; 
#endif    

    inputIDs[affected_id].insert(input_id);
  }

   bool hasId (SgNode* n)
  {
    return (getId(n)!=0);
  } 

  SgNode* getNode (AST_NODE_ID id)
  {
     return IdToAstTable[id];    
  }

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
  static std::pair <Sg_File_Info*, Sg_File_Info*> copyBeginAndEndFileInfo (SgNode* n)
  {
    std::pair <Sg_File_Info*, Sg_File_Info*> rtvalue; 
    ROSE_ASSERT (n != NULL);
    if (isSgPragma(n)||isSgLocatedNode(n))
    {
      //rtvalue = std::make_pair (n->get_startOfConstruct(), n->get_endOfConstruct());
      rtvalue = std::make_pair (SageInterface::deepCopy(n->get_startOfConstruct()), SageInterface::deepCopy(n->get_endOfConstruct()));
    }
    return rtvalue;
  }

#if 0
  // Store the file info. for each AST node
  // current transformation process may erase file_info., even for subtree coming from the original source code. 
  // we need to keep the information at least for the top level node
  // store begin and end file info for a node
  // We have to make copies of File Info objects since transformation may change fields
  // We use IDs instead of memory addresses of a node since a same node may be involved in
  // multiple transformation, being assigned with multiple IDs. 
  void storeFileInfo(AST_NODE_ID id, Sg_File_Info* start, Sg_File_Info* end)
  {
//TODO
  }
#endif
 
  std::pair<Sg_File_Info*, Sg_File_Info*>  getFileInfo(AST_NODE_ID id)
  {
    return IdToFileInfoTable[id];  
  }

  void clearId(SgNode* n)
  {
    ROSE_ASSERT (n != NULL);
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
      IdToFileInfoTable[nextId] = copyBeginAndEndFileInfo (n);
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

