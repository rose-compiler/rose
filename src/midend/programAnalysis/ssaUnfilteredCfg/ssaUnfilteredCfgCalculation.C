//Author: George Vulov <georgevulov@hotmail.com>
//Based on work by Justin Frye <jafrye@tamu.edu>

// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

#include "CallGraph.h"
#include "ssaUnfilteredCfg.h"
#include "sageInterface.h"
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <fstream>
#include <stack>
#include <boost/timer.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>
#include "uniqueNameTraversal.h"
#include <defsAndUsesUnfilteredCfg.h>
#include <iteratedDominanceFrontier.h>
#include <fstream>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

//#define DISPLAY_TIMINGS

using namespace std;
using namespace ssa_unfiltered_cfg;
using namespace boost;

//Initializations of the static attribute tags
SSA_UnfilteredCfg::VarName SSA_UnfilteredCfg::emptyName;

bool SSA_UnfilteredCfg::isBuiltinVar(const VarName& var)
{
    string name = var[0]->get_name().getString();
    if (name == "__func__" ||
            name == "__FUNCTION__" ||
            name == "__PRETTY_FUNCTION__")
        return true;

    return false;
}

set<SSA_UnfilteredCfg::VarName> getAllVarsUsedOrDefined(
        const SSA_UnfilteredCfg::CFGNodeToVarNamesMap& defs,
        const SSA_UnfilteredCfg::ASTNodeToVarRefsMap& uses,
        const map<CFGNode, SSA_UnfilteredCfg::NodeReachingDefTable>& localDefTable)
{
    set<SSA_UnfilteredCfg::VarName> usedNames;

    foreach(const SSA_UnfilteredCfg::CFGNodeToVarNamesMap::value_type& nodeVarsPair, defs)
    {
        map<CFGNode, SSA_UnfilteredCfg::NodeReachingDefTable>::const_iterator allDefsIter =
                localDefTable.find(nodeVarsPair.first);

        foreach(const SSA_UnfilteredCfg::NodeReachingDefTable::value_type& varDefPair, allDefsIter->second)
        {
            usedNames.insert(varDefPair.first);
        }
    }

    foreach(const SSA_UnfilteredCfg::ASTNodeToVarRefsMap::value_type& nodeVarRefsPair, uses)
    {

        foreach(SgVarRefExp* varRef, nodeVarRefsPair.second)
        {
            const SSA_UnfilteredCfg::VarName& usedVarName = SSA_UnfilteredCfg::getVarName(varRef);
            ROSE_ASSERT(usedVarName != SSA_UnfilteredCfg::emptyName);

            //If we have a use for p.x.y, insert uses for p and p.x as well as p.x.y
            for (size_t i = 1; i <= usedVarName.size(); ++i)
            {
                SSA_UnfilteredCfg::VarName var(usedVarName.begin(), usedVarName.begin() + i);

                usedNames.insert(var);
            }
        }
    }

    return usedNames;
}

void SSA_UnfilteredCfg::run()
{
    localDefTable.clear();
    reachingDefTable.clear();
    outgoingDefTable.clear();
    astNodeToUses.clear();

#ifdef DISPLAY_TIMINGS
    timer time;
#endif
    if (getDebug())
        cout << "Running UniqueNameTraversal...\n";
    ssa_private::UniqueNameTraversal uniqueTrav(SageInterface::querySubTree<SgInitializedName > (project, V_SgInitializedName));
    uniqueTrav.traverse(project);

    //    //BEGIN DEBUGGING CODE
    //    vector<SgNode*> allNodes = SageInterface::querySubTree<SgNode>(project, V_SgNode);
    //    foreach (SgNode* node, allNodes)
    //    {
    //        const VarName& varName = getVarName(node);
    //    
    //        if (varName != emptyName)
    //        {
    //            if (varnameToString(varName).substr(0, 11) == "__builtin__")
    //                continue;
    //            
    //            if (node->get_file_info()->get_line() == 0)
    //                continue;
    //            
    //            printf("%d: Node %s - %s\n", node->get_file_info()->get_line(), node->class_name().c_str(), 
    //                    varnameToString(varName).c_str());
    //        }
    //    }
    //    
    //    return;

    if (getDebug())
        cout << "Finished UniqueNameTraversal." << endl;
#ifdef DISPLAY_TIMINGS
    printf("-- Timing: UniqueNameTraversal took %.2f seconds.\n", time.elapsed());
    fflush(stdout);
    time.restart();
#endif

    //Get a list of all the functions that we'll process
    boost::unordered_set<SgFunctionDefinition*> interestingFunctions;
    vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition > (project, V_SgFunctionDefinition);

    FunctionFilter functionFilter;

    foreach(SgFunctionDefinition* f, funcs)
    {
        if (functionFilter(f->get_declaration()))
            interestingFunctions.insert(f);
    }

    foreach(SgFunctionDefinition* func, interestingFunctions)
    {
        if (getDebug())
            cout << "Running DefsAndUsesTraversal on function: " << SageInterface::get_name(func) << func << endl;

        CFGNodeToVarNamesMap functionDefs;
        ASTNodeToVarRefsMap functionUses;
        DefsAndUsesTraversal::CollectDefsAndUses(func->get_declaration(), functionDefs, functionUses);

        if (getDebug())
            cout << "Finished DefsAndUsesTraversal..." << endl;

        //Insert the new definitions into the local defs table

        foreach(const CFGNodeToVarNamesMap::value_type& nodeVarsPair, functionDefs)
        {
            const CFGNode& definingNode = nodeVarsPair.first;
            NodeReachingDefTable& nodeLocalDefs = localDefTable[definingNode];

            foreach(const VarName& definedVar, nodeVarsPair.second)
            {
                ReachingDefPtr originalDef = ReachingDefPtr(new ReachingDef(definingNode, ReachingDef::ORIGINAL_DEF));
                nodeLocalDefs.insert(make_pair(definedVar, originalDef));
            }
        }

        //Insert new uses into the uses table
        astNodeToUses.insert(functionUses.begin(), functionUses.end());

        //Expand any member variable definition to also define its parents at the same node. E.g. p.x defines p
        expandParentMemberDefinitions(functionDefs);

        //Insert defs for any variable that is an extension of a defined variable and is used.
        //E.g. if p is defined, a definition for p.x will be inserted if p.x is used anywhere in the function
        set<VarName> referencedVarNames = getAllVarsUsedOrDefined(functionDefs, functionUses, localDefTable);
        insertDefsForChildMemberUses(functionDefs, referencedVarNames);

        //Insert definitions at the SgFunctionDefinition for external variables whose values flow inside the function
        insertDefsForExternalVariables(func, referencedVarNames);

        //Insert phi functions at join points
        vector<CFGNode> functionCfgNodesPostorder = getCfgNodesInPostorder(func);
        insertPhiFunctions(func, functionCfgNodesPostorder);

        //Renumber all instantiated ReachingDef objects
        renumberAllDefinitions(func, functionCfgNodesPostorder);

        if (getDebug())
        {
            printf("\nUses table for %s():\n", func->get_declaration()->get_name().str());

            foreach(const ASTNodeToVarRefsMap::value_type& nodeVarsPair, functionUses)
            {
                printf("%s @ %d: ", nodeVarsPair.first->class_name().c_str(), nodeVarsPair.first->get_file_info()->get_line());

                foreach(SgVarRefExp* varRef, nodeVarsPair.second)
                {
                    printf("%s ", varnameToString(getVarName(varRef)).c_str());
                }
                printf("\n");
            }
        }

        if (getDebug())
            cout << "Running DefUse Data Flow on function: " << SageInterface::get_name(func) << func << endl;
        runDefUseDataFlow(func);

        if (getDebugExtra())
        {
            ofstream file((func->get_declaration()->get_name() + "_unfiltered.dot").str());
            printToDOT(func, file);
        }
    }
}

void SSA_UnfilteredCfg::expandParentMemberDefinitions(const CFGNodeToVarNamesMap& defs)
{

    foreach(const CFGNodeToVarNamesMap::value_type& cfgNodeVarsPair, defs)
    {
        const CFGNode& definingNode = cfgNodeVarsPair.first;
        const set<VarName>& definedVarsAtNode = cfgNodeVarsPair.second;

        NodeReachingDefTable& nodeLocalDefs = localDefTable[definingNode];

        foreach(const VarName& definedVar, definedVarsAtNode)
        {
            //We are dealing with a multi-part variable, loop the entry and expand it
            //Start at one so we don't get the same defs in the original and expanded defs
            for (unsigned int i = 1; i < definedVar.size(); i++)
            {
                //Create a new varName vector that goes from beginning to end - i
                VarName newName;
                newName.assign(definedVar.begin(), definedVar.end() - i);

                //Only insert the new definition if it does not already exist in the original def table
                if (definedVarsAtNode.count(newName) == 0)
                {
                    ROSE_ASSERT(nodeLocalDefs.count(newName) == 0);
                    ReachingDefPtr expandedDef = ReachingDefPtr(new ReachingDef(definingNode, ReachingDef::EXPANDED_DEF));
                    nodeLocalDefs.insert(make_pair(newName, expandedDef));
                }
            }
        }
    }
}

void SSA_UnfilteredCfg::runDefUseDataFlow(SgFunctionDefinition* func)
{
    //Keep track of visited nodes
    set<CFGNode> visited;

    set<CFGNode> worklist;

    CFGNode current = func->cfgForBeginning();
    worklist.insert(current);

    while (!worklist.empty())
    {
        if (getDebugExtra())
            cout << "-------------------------------------------------------------------------" << endl;
        //Get the node to work on
        current = *worklist.begin();
        worklist.erase(worklist.begin());

        //Propagate defs to the current node
        bool changed = propagateDefs(current);

        //For every edge, add it to the worklist if it is not seen or something has changed

        reverse_foreach(const CFGEdge& edge, current.outEdges())
        {
            CFGNode nextNode = edge.target();

            //Insert the child in the worklist if the parent is changed or it hasn't been visited yet
            if (changed || visited.count(nextNode) == 0)
            {
                //Add the node to the worklist
                bool insertedNew = worklist.insert(nextNode).second;
                if (insertedNew && getDebugExtra())
                {
                    if (changed)
                        cout << "Defs Changed: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
                    else
                        cout << "Next unvisited: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
                }
            }
        }

        //Mark the current node as seen
        visited.insert(current);
    }
}

bool SSA_UnfilteredCfg::propagateDefs(const CFGNode& cfgNode)
{
    if (getDebugExtra())
        printf("propagateDefs(%s)\n", cfgNode.toStringForDebugging().c_str());

    //This updates the IN table with the reaching defs from previous nodes
    updateIncomingPropagatedDefs(cfgNode);

    //Create a staging OUT table. At the end, we will check if this table
    //Was the same as the currently available one, to decide if any changes have occurred
    //We initialize the OUT table to the IN table
    NodeReachingDefTable stagingOutDefsTable = reachingDefTable[cfgNode];

    if (getDebugExtra())
    {
        printf("Incoming defs:\n");
        printNodeDefTable(stagingOutDefsTable);
    }

    //Now overwrite any local definitions
    CFGNodeToDefTableMap::const_iterator localDefIter = localDefTable.find(cfgNode);
    if (localDefIter != localDefTable.end())
    {

        foreach(const NodeReachingDefTable::value_type& varDefPair, localDefIter->second)
        {
            const VarName& definedVar = varDefPair.first;
            ReachingDefPtr localDef = varDefPair.second;

            stagingOutDefsTable[definedVar] = localDef;
        }
    }

    //Compare old to new OUT tables
    NodeReachingDefTable& existingoutDefTable = outgoingDefTable[cfgNode];
    bool changed = (existingoutDefTable != stagingOutDefsTable);
    if (changed)
    {
        existingoutDefTable = stagingOutDefsTable;
    }

    if (getDebugExtra() && changed)
    {
        printf("\nOutgoing defs changed!:\n");
        printNodeDefTable(outgoingDefTable[cfgNode]);
    }

    return changed;
}

void SSA_UnfilteredCfg::updateIncomingPropagatedDefs(const CFGNode& cfgNode)
{
    //Get the previous edges in the CFG for this node
    vector<CFGEdge> inEdges = cfgNode.inEdges();

    NodeReachingDefTable& incomingDefTable = reachingDefTable[cfgNode];

    //Iterate all of the incoming edges
    for (unsigned int i = 0; i < inEdges.size(); i++)
    {
        const CFGNode& prev = inEdges[i].source();

        const NodeReachingDefTable& previousDefs = outgoingDefTable[prev];

        //Merge all the previous defs into the IN table of the current node

        foreach(const NodeReachingDefTable::value_type& varDefPair, previousDefs)
        {
            const VarName& var = varDefPair.first;
            const ReachingDefPtr previousDef = varDefPair.second;

            //Here we don't propagate defs for variables that went out of scope
            //(built-in vars are body-scoped but we inserted the def at the SgFunctionDefinition node, so we make an exception)
            //if (!isVarInScope(var, cfgNode.getNode()) && !isBuiltinVar(var))
            //    continue;

            //If this is the first time this def has propagated to this node, just copy it over
            NodeReachingDefTable::const_iterator existingDefIter = incomingDefTable.find(var);
            if (existingDefIter == incomingDefTable.end())
            {
                incomingDefTable.insert(make_pair(var, previousDef));
            }
            else
            {
                ReachingDefPtr existingDef = existingDefIter->second;

                if (existingDef->isPhiFunction() && existingDef->getDefinitionNode() == cfgNode)
                {
                    //There is a phi node here. We update the phi function to point to the previous reaching definition
                    existingDef->addJoinedDef(previousDef, inEdges[i]);
                }
                else
                {
                    //If there is no phi node, and we get a new definition, it better be the same as the one previously
                    //propagated.
                    if (!(*previousDef == *existingDef))
                    {
                        printf("ERROR: At node %s, two different definitions reach for variable %s\n",
                                cfgNode.toStringForDebugging().c_str(), varnameToString(var).c_str());
                        ROSE_ASSERT(false);
                    }
                }
            }
        }
    }
}

void SSA_UnfilteredCfg::insertDefsForChildMemberUses(const CFGNodeToVarNamesMap& defs, const set<VarName>& usedNames)
{
    //Map each varName to all used names for which it is a prefix
    map<VarName, set<VarName> > nameToChildNames;

    foreach(const VarName& rootName, usedNames)
    {

        foreach(const VarName& childName, usedNames)
        {
            if (childName.size() <= rootName.size())
                continue;

            if (isPrefixOfName(childName, rootName))
            {
                nameToChildNames[rootName].insert(childName);
            }
        }
    }

    //Go through each definition and make sure all child names are also defined at each definition point

    foreach(const CFGNodeToVarNamesMap::value_type& nodeVarsPair, defs)
    {
        const CFGNode& definingNode = nodeVarsPair.first;
        NodeReachingDefTable& nodeLocalDefs = localDefTable[definingNode];

        foreach(const VarName& definedVarName, nodeVarsPair.second)
        {
            //Find all the children of this variable that should also be defined here
            map<VarName, set<VarName> >::iterator childVars = nameToChildNames.find(definedVarName);
            if (childVars == nameToChildNames.end())
                continue;

            //Go over all the child names and define them here also

            foreach(const VarName& childName, childVars->second)
            {
                ROSE_ASSERT(childName.size() > definedVarName.size());
                for (size_t i = 0; i < (childName.size() - definedVarName.size()); i++)
                {
                    //Create a new varName vector that goes from beginning to end - i
                    VarName newName;
                    newName.assign(childName.begin(), childName.end() - i);

                    if (nodeLocalDefs.count(newName) == 0)
                    {
                        ReachingDefPtr expandedDef = ReachingDefPtr(new ReachingDef(definingNode, ReachingDef::EXPANDED_DEF));

                        nodeLocalDefs.insert(make_pair(newName, expandedDef));
                    }
                }
            }
        }
    }
}

/** Insert defs for functions that are declared outside the function scope. */
void SSA_UnfilteredCfg::insertDefsForExternalVariables(SgFunctionDefinition* function, const set<VarName>& usedNames)
{
    //The function definition should have 4 indices in the CFG. We insert defs for external variables
    //At the very first one
    ROSE_ASSERT(function->cfgIndexForEnd() == 3);
    CFGNode functionEntryNode = function->cfgForBeginning();

    NodeReachingDefTable& functionEntryDefs = localDefTable[functionEntryNode];

    //Iterate over each used variable and check it it is declared outside of the function scope

    foreach(const VarName& usedVar, usedNames)
    {
        ROSE_ASSERT(!usedVar.empty());
        VarName rootName;
        rootName.assign(1, usedVar[0]);

        SgScopeStatement* varScope = SageInterface::getScope(rootName[0]);
        SgScopeStatement* functionScope = function;

        //If it is a local variable, there should be a def somewhere inside the function
        if (varScope == functionScope || SageInterface::isAncestor(functionScope, varScope))
        {
            //We still need to insert defs for compiler-generated variables (e.g. __func__), since they don't have defs in the AST
            if (!isBuiltinVar(rootName))
                continue;
        }
        else if (isSgGlobal(varScope))
        {
            //Handle the case of declaring "extern int x" inside the function
            //Then, x has global scope but it actually has a definition inside the function so we don't need to insert one
            if (SageInterface::isAncestor(function, rootName[0]))
            {
                //When else could a var be declared inside a function and be global?
                SgVariableDeclaration* varDecl = isSgVariableDeclaration(rootName[0]->get_parent());
                ROSE_ASSERT(varDecl != NULL);
                ROSE_ASSERT(varDecl->get_declarationModifier().get_storageModifier().isExtern());
                continue;
            }
        }

        //Are there any other types of external vars?
        ROSE_ASSERT(isBuiltinVar(rootName) || isSgClassDefinition(varScope) || isSgNamespaceDefinitionStatement(varScope)
                || isSgGlobal(varScope) || isSgDeclarationScope(varScope));

        //The variable is not in local scope; we need to insert a def for it at the function definition
        for (size_t i = 0; i < usedVar.size(); i++)
        {
            //Create a new varName vector that goes from beginning to end - i
            VarName newName(usedVar.begin(), usedVar.end() - i);

            if (functionEntryDefs.count(newName) > 0)
                continue;

            ReachingDefPtr def = ReachingDefPtr(new ReachingDef(functionEntryNode, ReachingDef::EXTERNAL_DEF));
            functionEntryDefs.insert(make_pair(newName, def));
        }
    }
}

void SSA_UnfilteredCfg::insertPhiFunctions(SgFunctionDefinition* function, const std::vector<CFGNode>& cfgNodesInPostOrder)
{
    if (getDebug())
        printf("Inserting phi nodes in function %s...\n", function->get_declaration()->get_name().str());
    ROSE_ASSERT(function != NULL);

    //First, find all the places where each name is defined
    map<VarName, vector<CFGNode> > nameToDefNodesMap;

    foreach(const CFGNode& cfgNode, cfgNodesInPostOrder)
    {
        CFGNodeToDefTableMap::const_iterator defTableIter = localDefTable.find(cfgNode);
        if (defTableIter == localDefTable.end())
            continue;

        const NodeReachingDefTable& definitionsAtNode = defTableIter->second;

        foreach(const NodeReachingDefTable::value_type& varDefPair, definitionsAtNode)
        {
            nameToDefNodesMap[varDefPair.first].push_back(cfgNode);
        }
    }

    //Build an iterated dominance frontier for this function
    map<CFGNode, CFGNode> iPostDominatorMap;
    map<CFGNode, set<CFGNode> > domFrontiers = ssa_private::calculateDominanceFrontiers<CFGNode, CFGEdge > (function, NULL, &iPostDominatorMap);

    //Find the phi function locations for each variable
    map<VarName, vector<CFGNode> >::const_iterator nameToDefNodesIter = nameToDefNodesMap.begin();
    for (; nameToDefNodesIter != nameToDefNodesMap.end(); ++nameToDefNodesIter)
    {
        const VarName& var = nameToDefNodesIter->first;
        const vector<CFGNode>& definitionPoints = nameToDefNodesIter->second;

        ROSE_ASSERT(!definitionPoints.empty() && "We have a variable that is not defined anywhere!");

        //Calculate the iterated dominance frontier
        set<CFGNode> phiNodes = ssa_private::calculateIteratedDominanceFrontier(domFrontiers, definitionPoints);

        if (getDebug())
            printf("Variable %s has phi nodes inserted at\n", varnameToString(var).c_str());

        foreach(const CFGNode& phiNode, phiNodes)
        {
            //We don't want to insert phi defs for functions that have gone out of scope
            //if (!isVarInScope(var, phiNode.getNode()))
            //    continue;
            ReachingDefPtr phiDef = ReachingDefPtr(new ReachingDef(phiNode, ReachingDef::PHI_FUNCTION));
            reachingDefTable[phiNode].insert(make_pair(var, phiDef));
        }
    }
}

void SSA_UnfilteredCfg::renumberAllDefinitions(SgFunctionDefinition* func, const vector<CFGNode>& cfgNodesInPostOrder)
{
    //Map from each name to the next index. Not in map means 0
    map<VarName, int> nameToNextIndexMap;

    //We process nodes in reverse postorder; this provides a natural numbering for definitions

    reverse_foreach(const CFGNode& cfgNode, cfgNodesInPostOrder)
    {
        CFGNodeToDefTableMap::const_iterator reachingDefsIter = reachingDefTable.find(cfgNode);
        if (reachingDefsIter != reachingDefTable.end())
        {
            //Iterate over all the phi functions inserted at this node.

            foreach(const NodeReachingDefTable::value_type& varDefPair, reachingDefsIter->second)
            {
                const VarName& definedVar = varDefPair.first;
                ReachingDefPtr reachingDef = varDefPair.second;

                //At this point, only phi functions should be in the reaching defs table. Everything else is local defs
                ROSE_ASSERT(reachingDef->isPhiFunction());

                //Give an index to the variable
                int index = 0;
                if (nameToNextIndexMap.count(definedVar) > 0)
                {
                    index = nameToNextIndexMap[definedVar];
                }
                nameToNextIndexMap[definedVar] = index + 1;

                reachingDef->setRenamingNumber(index);
            }
        }

        //Number the real (non-phi) definitions
        CFGNodeToDefTableMap::const_iterator localDefIter = localDefTable.find(cfgNode);
        if (localDefIter != localDefTable.end())
        {

            foreach(const NodeReachingDefTable::value_type& varDefPair, localDefIter->second)
            {
                const VarName& definedVar = varDefPair.first;
                ReachingDefPtr reachingDef = varDefPair.second;

                //At this point, only phi functions should be in the reaching defs table. Everything else is local defs
                ROSE_ASSERT(!reachingDef->isPhiFunction());

                //Give an index to the variable
                int index = 0;
                if (nameToNextIndexMap.count(definedVar) > 0)
                {
                    index = nameToNextIndexMap[definedVar];
                }
                nameToNextIndexMap[definedVar] = index + 1;

                reachingDef->setRenamingNumber(index);
            }
        }
    }
}

/*static*/
vector<CFGNode> SSA_UnfilteredCfg::getCfgNodesInPostorder(SgFunctionDefinition* func)
{

    struct RecursiveDFS
    {

        static void depthFirstSearch(const CFGNode& cfgNode, set<CFGNode>& visited, vector<CFGNode>& result)
        {
            //First, make sure this node hasn't been visited yet
            if (visited.count(cfgNode) != 0)
                return;

            visited.insert(cfgNode);

            //Now, visit all the node's successors

            reverse_foreach(const CFGEdge& outEdge, cfgNode.outEdges())
            {
                depthFirstSearch(outEdge.target(), visited, result);
            }

            //Add this node to the postorder list
            result.push_back(cfgNode);
        }
    };

    ROSE_ASSERT(func != NULL);

    set<CFGNode> visited;
    vector<CFGNode> results;
    CFGNode entry = func->cfgForBeginning();

    RecursiveDFS::depthFirstSearch(entry, visited, results);

    return results;
}
