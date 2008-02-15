#ifndef CFG_TO_DOT_H
#define CFG_TO_DOT_H

namespace VirtualCFG {

  // From cfgToDot.C:
  std::ostream& cfgToDot(std::ostream& o, std::string graphName, CFGNode start);
  std::ostream& cfgToDot(std::ostream& o, std::string graphName, InterestingNode start);
  std::ostream& cfgToDotForDebugging(std::ostream& o, std::string graphName, CFGNode start);
  std::ostream& cfgToDotForDebugging(std::ostream& o, std::string graphName, InterestingNode start);
}

// endif for CFG_TO_DOT_H
#endif
