
/*!
 *  Transformation tracking and IR mapping support
 *
 *  Liao 3/24/2014
 */
#ifndef TRANSFORMATION_TRACKING_H
#define TRANSFORMATION_TRACKING_H

//#include "astQuery.h"
#include <map>

class SgNode;

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
  //! Assign Ids 
  void registerAstSubtreeIds (SgNode* root);

} // end namespace TransformationTracking

#endif //TRANSFORMATION_TRACKING_H
