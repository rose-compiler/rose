/* 
 * File:   defUseChains.C
 * Author: Cong Hou [hou_cong@gatech.edu]
 */

#include "sage3basic.h"                                 // every librose .C file must start with this
#include "defUseChains.h"
#include "util.h"
#include <ssaUnfilteredCfg.h>
#include <VariableRenaming.h>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace ssa_unfiltered_cfg;

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
    SSA_UnfilteredCfg ssa(project);
    ssa.run();
        
    vector<SgNode*> astNodes = NodeQuery::querySubTree(project, V_SgNode);
    foreach (SgNode* node, astNodes)
    {
        set<SgVarRefExp*> uses = ssa.getUsesAtNode(node);
        foreach (SgVarRefExp* varRef, uses)
        {
            ReachingDef::ReachingDefPtr reachingDef = ssa.getDefinitionForUse(varRef);

            set<VirtualCFG::CFGNode> defs = reachingDef->getActualDefinitions();

            foreach (const VirtualCFG::CFGNode& cfgNode, defs)
            {
                defUseChains[cfgNode.getNode()].insert(varRef);
            }
        }
    }
    
    // For each function, find all reaching defs at the end of the function, and add Def-Use chains from
    // the def of parameters which are passed by reference to parameters themselves.
    vector<SgFunctionDefinition*> funcDefs = 
            SageInterface::querySubTree<SgFunctionDefinition>(project, V_SgFunctionDefinition);
    foreach (SgFunctionDefinition* funcDef, funcDefs)
    {
        SgFunctionDeclaration* funcDecl = funcDef->get_declaration();
    
        set<SgInitializedName*> argsPassedByRef;
        
        // Get all parameters passed by reference.
        const SgInitializedNamePtrList& args = funcDecl->get_args();
        foreach (SgInitializedName* initName, args)
        {   
            // If the parameter is passed by reference, create a formal-out node.
            if (isParaPassedByRef(initName->get_type()))
                argsPassedByRef.insert(initName);
        }

        
        typedef SSA_UnfilteredCfg::NodeReachingDefTable NodeReachingDefTable;
        const NodeReachingDefTable& reachingDefs = ssa.getLastVersions(funcDef);
        foreach (const NodeReachingDefTable::value_type& varAndDefs, reachingDefs)
        {
            if (varAndDefs.first.size() > 1)
                continue;
            SgInitializedName* initName = varAndDefs.first[0];
            
            if (argsPassedByRef.count(initName) == 0)
                continue;
            
            set<VirtualCFG::CFGNode> defs = varAndDefs.second->getActualDefinitions();
            foreach (const VirtualCFG::CFGNode& cfgNode, defs)
            {
                defUseChains[cfgNode.getNode()].insert(initName);
            }     
        }
    }
}



} // end of namespace
