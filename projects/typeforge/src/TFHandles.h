#ifndef TYPEFORGE_HANDLES_H
#define TYPEFORGE_HANDLES_H

#include "sage3basic.h"
#include "AstTerm.h"
#include <iterator>

namespace TFHandles{

  std::string getAbstractHandle(SgNode* node);
  SgNode*     getNodeFromHandle(SgProject* project, std::string handle);

  std::vector<std::string> getHandleVector(std::set<SgNode*>  nodeSet);
  std::vector<std::string> getHandleVector(std::vector<SgNode*> nodeVector);
  std::vector<SgNode*>     getNodeVector(SgProject* project, std::vector<std::string> handleVector);

  std::string            getStringFromVector(std::vector<std::string> stringVector);
  std::vector<std::string> getVectorFromString(std::string stringVector);
 
  std::string        getHandleVectorString(std::set<SgNode*>  nodeSet);
  std::string        getHandleVectorString(std::vector<SgNode*> nodeVector);
  std::vector<SgNode*> getNodeVectorFromString(SgProject* project, std::string stringVector);

}

#endif
