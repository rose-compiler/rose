#ifndef CFG_TO_DOT_H
#define CFG_TO_DOT_H
#include <string>
#include "rosedll.h"

namespace VirtualCFG {

  // From cfgToDot.C:
  ROSE_DLL_API std::ostream& cfgToDot(std::ostream& o, std::string graphName, CFGNode start);
  ROSE_DLL_API std::ostream& cfgToDot(std::ostream& o, std::string graphName, InterestingNode start);

  ROSE_DLL_API std::ostream& cfgToDotForDebugging(std::ostream& o, std::string graphName, CFGNode start);
  ROSE_DLL_API std::ostream& cfgToDotForDebugging(std::ostream& o, std::string graphName, InterestingNode start);

  //! Dump the filtered dot graph of a virtual control flow graph starting from SgNode (start)
  ROSE_DLL_API void cfgToDot (SgNode* start, const std::string& file_name);
  
  //! Dump a CFG with only interesting nodes for a SgNode
  ROSE_DLL_API void interestingCfgToDot (SgNode* start, const std::string& file_name);

  //! Dump the full dot graph of a virtual control flow graph starting from SgNode (start)
  ROSE_DLL_API void cfgToDotForDebugging(SgNode* start, const std::string& file_name);
}

// endif for CFG_TO_DOT_H
#endif
