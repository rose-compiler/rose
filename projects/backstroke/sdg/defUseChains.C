#include "defUseChains.h"
#include <ssa/staticSingleAssignment.h>
#include <VariableRenaming.h>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

using namespace std;

namespace SDG
{


void generateDefUseChainsFromVariableRenaming(SgProject* project, DefUseChains& defUseChains)
{
    VariableRenaming varRenaming(project);
    varRenaming.run();

    const VariableRenaming::DefUseTable& useTable = varRenaming.getUseTable();
    foreach (const VariableRenaming::DefUseTable::value_type& usesOnNode, useTable)
    {
        foreach (const VariableRenaming::TableEntry::value_type& entry, usesOnNode.second)
        {
            foreach (SgNode* node, entry.second)
            {
                defUseChains[node].insert(usesOnNode.first);
            }
        }
    }
}


void generateDefUseChainsFromSSA(SgProject* project, DefUseChains& defUseChains)
{
    StaticSingleAssignment ssa(project);
    ssa.run();
        
    vector<SgNode*> astNodes = NodeQuery::querySubTree(project, V_SgNode);
    foreach (SgNode* node, astNodes)
    {
        set<SgVarRefExp*> uses = ssa.getUsesAtNode(node);
        foreach (SgVarRefExp* varRef, uses)
        {
            StaticSingleAssignment::ReachingDefPtr reachingDef = ssa.getDefinitionForUse(varRef);

            set<VirtualCFG::CFGNode> defs = reachingDef->getActualDefinitions();

            foreach (const VirtualCFG::CFGNode& cfgNode, defs)
            {
                defUseChains[cfgNode.getNode()].insert(varRef);
            }
        }
    }
}



} // end of namespace
