#ifndef TYPEFORGE_HANDLES_H
#define TYPEFORGE_HANDLES_H

#include "sage3basic.h"
#include "AstTerm.h"
#include <iterator>

namespace Typeforge {

namespace TFHandles{
  
  //Methods to take a single node to a handle and a single handle to a node
  std::string getAbstractHandle(SgNode* node);
  SgNode*     getNodeFromHandle(SgProject* project, std::string handle);

  //Methods to take a set or vector of nodes to a vector of string handles and a vector of handles to a vector of nodes
  std::vector<std::string> getHandleVector(std::set<SgNode*>  nodeSet);
  std::vector<std::string> getHandleVector(std::vector<SgNode*> nodeVector);
  std::vector<SgNode*>     getNodeVector(SgProject* project, std::vector<std::string> handleVector);

  //Methods to convert between a single string and a vector of strings
  std::string            getStringFromVector(std::vector<std::string> stringVector);
  std::vector<std::string> getVectorFromString(std::string stringVector);
 
  //Methods to go straight from vector or set of nodes to a single string and back again
  std::string        getHandleVectorString(std::set<SgNode*>  nodeSet);
  std::string        getHandleVectorString(std::vector<SgNode*> nodeVector);
  std::vector<SgNode*> getNodeVectorFromString(SgProject* project, std::string stringVector);

}

}

#endif
