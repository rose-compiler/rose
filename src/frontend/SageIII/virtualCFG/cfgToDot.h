#ifndef CFG_TO_DOT_H
#define CFG_TO_DOT_H
#include <string>

namespace VirtualCFG {

  // From cfgToDot.C:
  std::ostream& cfgToDot(std::ostream& o, std::string graphName, CFGNode start);
  std::ostream& cfgToDot(std::ostream& o, std::string graphName, InterestingNode start);

  std::ostream& cfgToDotForDebugging(std::ostream& o, std::string graphName, CFGNode start);
  std::ostream& cfgToDotForDebugging(std::ostream& o, std::string graphName, InterestingNode start);

  std::ostream& interproceduralCfgToDotForDebugging(std::ostream& o, std::string graphName, CFGNode start);
  std::ostream& interproceduralCfgToDotForDebugging(std::ostream& o, std::string graphName, InterestingNode start);

  //! Dump the filtered dot graph of a virtual control flow graph starting from SgNode (start)
  void cfgToDot (SgNode* start, const std::string& file_name);
  
  //! Dump a CFG with only interesting nodes for a SgNode
  void interestingCfgToDot (SgNode* start, const std::string& file_name);

  //! Dump the full dot graph of a virtual control flow graph starting from SgNode (start)
  void cfgToDotForDebugging(SgNode* start, const std::string& file_name);

  //! Dump the interprocedural dot graph of a virtual control from graph starting from SgNode (start)
  void interproceduralCfgToDotForDebugging(SgNode* start, const std::string& file_name);
}

// endif for CFG_TO_DOT_H
#endif
