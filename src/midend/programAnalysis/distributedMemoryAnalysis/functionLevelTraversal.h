
#ifndef PARALLEL_COMPASS_FUNCTIONLEVEL_H
#define PARALLEL_COMPASS_FUNCTIONLEVEL_H

// LOAD THIS FILE ONLY IF ROSE_MPI IS ENABLED
#include <string>
#include "DistributedMemoryAnalysis.h"

// ************************************************************
// Use this class to run on function granulty instead of files
// ************************************************************
class MyAnalysis: public DistributedMemoryTraversal<int, std::string>
{
protected:
  std::string analyzeSubtree(SgFunctionDeclaration *funcDecl, int depth)
  {
    std::string funcName = funcDecl->get_name().str();
    std::stringstream s;
    s << "process " << myID() << ": at depth " << depth << ": function " << funcName;
    return s.str();
  }

  // The user must implement this method to pack a synthesized attribute (a string in this case) into an array of bytes
  // for communication. The first component of the pair is the number of bytes in the buffer.
  std::pair<int, void *> serializeAttribute(std::string attribute) const
  {
    int len = attribute.size() + 1;
    char *str = strdup(attribute.c_str());
    return std::make_pair(len, str);
  }

  // This method must be implemented to convert the serialized data to the application's synthesized attribute type.
  std::string deserializeAttribute(std::pair<int, void *> serializedAttribute) const
  {
    return std::string((const char *) serializedAttribute.second);
  }

  // This method is optional (the default implementation is empty). Its job is to free memory that may have been
  // allocated by the serializeAttribute() method.
  void deleteSerializedAttribute(std::pair<int, void *> serializedAttribute) const 
  {
    std::free(serializedAttribute.second);
  }
};

#endif
