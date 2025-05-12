/* 
 * File:   cfgNodeFilter.h
 * Author: Cong Hou [hou_cong@gatech.edu]
 */

namespace VirtualCFG {
    class CFGNode;
}

namespace SDG
{

bool filterCfgNode(const VirtualCFG::CFGNode& cfgNode);
bool filterCFGNodesByKeepingStmt(const VirtualCFG::CFGNode& cfgNode);

}
