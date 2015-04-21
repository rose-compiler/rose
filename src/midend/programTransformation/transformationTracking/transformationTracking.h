
/*!
 *  Transformation tracking and IR mapping support
 *  ROSE by default will recycle memory space for deleted AST nodes, 
 *  which complicate how we can uniquely identify AST nodes.
 *
 *  To avoid this, you must reconfigured with --enable-memory-pool-no-reuse 
 *  so the transformation tracking (or IR mapping) can work properly.
 * 
 *  To store transformation info., we use unique ID based on a pre-order traversal order to identify a node.
 *  It is most for convenience and persistence across different executions.
 *  Memory addresses can change from one run to another. But the Logic ID based on 
 *  traversal should be the same even with different 
 *
 *  TODO: Moving is tricky to model: it introduces the same input and output nodes
 *   In this case, the node itself cannot be directly used since the node can be moved around
 *  several times during transformation process. Each time it should have a new ID.
 *  This complicate implementation.
 *  We don't track this for now
 *
 *  Each transformation is modeled as
 *    trans_name, node_ID, start/end_file_Info, inputIDs, and outputIDs 
 *  For simplicity, we only support backtracking the transformation so only
 *   inputIDs will be saved.
 *
 *  A challenge is the granularity of tracking: statement level vs. expression level 
 *  Liao 3/24/2014
 */
#ifndef TRANSFORMATION_TRACKING_H
#define TRANSFORMATION_TRACKING_H

#include <map>
#include <utility>

class SgNode;
class Sg_File_Info;

typedef unsigned int AST_NODE_ID;
namespace TransformationTracking
{
  //X. Support for unique integer IDs for AST nodes. 
  // My first idea was to add a new field into SgNode. 
  // Markus reminded me that this will increase the size of AST for any situation. 
  // He suggested a more flexible way is to use separated data structures (map, vector, etc.) to 
  // maintain the IDs for each SgNode. 
  //
  // IDs start from 1, Default value of 0 means a node has not assigned an ID.
 
  //------------------------------------
  // Public interface functions
  //! Assign Ids and save current File Info.
  void registerAstSubtreeIds (SgNode* root);

#if 0 // cannot really reassign IDs for the same node in this version
  //! Clear ID for a single node, preparing re-assigning new IDs
  void clearId(SgNode* node);

  //! Clear IDs for an entire sub tree , preparing re-assigning new IDs
  void clearAstSubtreeIds(SgNode* root);

  // This happens automatically now when an ID is assigned to a node.
  // Replaced by an internal function copyBeginAndEndFileInfo()
  // Store the file info. for each AST node
  // current transformation process may erase file_info., even for subtree coming from the original source code. 
  // we need to keep the information at least for the top level node
  // store begin and end file info for a node
  void storeFileInfo(AST_NODE_ID id, Sg_File_Info* start, Sg_File_Info* end);
#endif 
  AST_NODE_ID getId (SgNode* ); 
  
  // check if a node has been assigned a unique ID
  bool hasId (SgNode* n);

  //Obtain the SgNode from an ID
  SgNode* getNode (AST_NODE_ID id);

  // retrieve stored file info: both begin and end info.
  std::pair <Sg_File_Info*, Sg_File_Info*> getFileInfo (AST_NODE_ID id);

 // To simplify the problem, 
 // we keep track of information at a single line statement level.
 //
 // For a transformed (statement) node, add an input (statement) node. 
 // Internally, the input node's ID will be saved. 
 //
 // This should be called after the transformation is completed and all IDs are assigned. 
  void addInputNode(SgNode* affected_node, SgNode* inputnode);

  // Obtain the transformation input nodes for n
  // directly use the exposed map variable inputIDs
  //std::set getInputNodes (SgNode* n);
  extern std::map<AST_NODE_ID, std::set<AST_NODE_ID> > inputIDs;
  //-------------------------------------------------
  // Internal functions
  void setId (SgNode*);

  // return the next Id which will be used (assigned to an AST node)
  // This is also the same as the total number of IDs + 1. It can be used to control loop iteration over all IDs
  AST_NODE_ID getNextId (); 


} // end namespace TransformationTracking

#endif //TRANSFORMATION_TRACKING_H
