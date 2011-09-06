/* 
 * File:   cfgNodeFilter.h
 * Author: Cong Hou [hou_cong@gatech.edu]
 */

#include <rose.h>

namespace SDG
{

bool filterCfgNode(const VirtualCFG::CFGNode& cfgNode);
bool filterCFGNodesByKeepingStmt(const VirtualCFG::CFGNode& cfgNode);

}
