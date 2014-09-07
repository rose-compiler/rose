
/*!
 *  Transformation tracking and IR mapping support
 *
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
  // IDs start from 1, Default value of 0 means a node has not assigned an ID.
  AST_NODE_ID getId (SgNode* ); 
  void setId (SgNode*);
  //! Assign Ids and save current File Info.
  void registerAstSubtreeIds (SgNode* root);

  // return the next Id which will be used
  AST_NODE_ID getNextId (); 

  // Store the file info. for each AST node
  // current transformation process may erase file_info., even for subtree coming from the original source code. 
  // we need to keep the information
  // store begin and end file info for a node
  void storeFileInfo(AST_NODE_ID id, Sg_File_Info* start, Sg_File_Info* end);

  // retrieve stored file info: both begin and end info.
  std::pair <Sg_File_Info*, Sg_File_Info*> getFileInfo (AST_NODE_ID id);

} // end namespace TransformationTracking

#endif //TRANSFORMATION_TRACKING_H
