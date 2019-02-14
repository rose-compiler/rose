//Author: George Vulov <georgevulov@hotmail.com>
//Based on work by Justin Frye <jafrye@tamu.edu>

// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

#include "CallGraph.h"
#include "staticSingleAssignment.h"
#include "sageInterface.h"
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <fstream>
#include <stack>
#include <boost/timer.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>
#include "uniqueNameTraversal.h"
#include "defsAndUsesTraversal.h"
#include "iteratedDominanceFrontier.h"
#include "controlDependence.h"

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

//#define DISPLAY_TIMINGS

using namespace std;
using namespace ssa_private;
using namespace boost;

//Initializations of the static attribute tags
StaticSingleAssignment::VarName StaticSingleAssignment::emptyName;

bool StaticSingleAssignment::isBuiltinVar(const VarName& var)
{
    string name = var[0]->get_name().getString();
    if (name == "__func__" ||
            name == "__FUNCTION__" ||
            name == "__PRETTY_FUNCTION__")
        return true;

    return false;
}

bool StaticSingleAssignment::isVarInScope(const VarName& var, SgNode* astNode)
{
    SgScopeStatement* accessingScope = SageInterface::getScope(astNode);
    ROSE_ASSERT(var.size() > 0 && accessingScope != NULL);
    SgScopeStatement* varScope = SageInterface::getScope(var[0]);

    //Work around a ROSE bug that sets incorrect scopes for built-in variables.
    if (isBuiltinVar(var))
    {
        return SageInterface::isAncestor(accessingScope, var[0]);
    }

    if (varScope == accessingScope || SageInterface::isAncestor(varScope, accessingScope))
    {
        //FIXME: In a basic block, the definition of the variable might come AFTER the node in question
        //We should return false in this case.

        //Special case: a variable cannot be accessed in its own assign initializer
        //This is important for loops where a variable is redefined on every iteration
        //E.g. while (int a = 3) {}
        if (SageInterface::isAncestor(var[0], astNode))
            return false;

        return true;
    }

    if (isSgNamespaceDefinitionStatement(varScope) || isSgGlobal(varScope))
    {
        //Variables defined in a namespace or in global scope are always accessible if they're fully qualified
        return true;
    }

    if (isSgInitializedName(astNode) && isSgCtorInitializerList(astNode->get_parent()))
    {
        //Work around a SageInterface::getScope peculiarity
        //SageInterface::getScope returns class scope for the initialized names in the constructor initializer list,
        //because these are class-scoped variables. However, those initializers can actually access function parameters
        SgFunctionDeclaration* funcDeclaration = isSgFunctionDeclaration(astNode->get_parent()->get_parent());
        ROSE_ASSERT(funcDeclaration != NULL);
        SgFunctionParameterList* parameters = funcDeclaration->get_parameterList();
        const SgInitializedNamePtrList& paramList = parameters->get_args();
        //If the variable in question in the parameter list, it can be reached by the constructor initializers
        if (find(paramList.begin(), paramList.end(), var[0]) != paramList.end())
        {
            return true;
        }
    }

    if (SgClassDefinition * varClassScope = isSgClassDefinition(varScope))
    {
        //If the variable is static & public, it's accessible
        SgVariableDeclaration* varDeclaration = isSgVariableDeclaration(var[0]->get_parent());
        if (varDeclaration != NULL) //variable declaration is null inside constructor initializer list
        {
            if (varDeclaration->get_declarationModifier().get_storageModifier().isStatic() &&
                    varDeclaration->get_declarationModifier().get_accessModifier().isPublic())
            {
                return true;
            }
        }

        //If the variable is accessed by a friend function, then it is available. Check if the function
        //looking to access the var is a friend
        SgFunctionDeclaration* accessingFunction = SageInterface::getEnclosingFunctionDeclaration(astNode, true);
        ROSE_ASSERT(accessingFunction != NULL);
        SgName accessingFunctionName = accessingFunction->get_mangled_name();

        //We'll look at all functions declared inside the variables class and see if any of them is the accessing function
        //and is declared a friend

        foreach(SgDeclarationStatement* varClassMember, varClassScope->get_members())
        {
            if (!isSgFunctionDeclaration(varClassMember))
                continue;

            SgFunctionDeclaration* nestedFunction = isSgFunctionDeclaration(varClassMember);

            if (!nestedFunction->get_declarationModifier().isFriend())
                continue;

            if (nestedFunction->get_mangled_name() != accessingFunctionName)
                continue;

            //The accessing function is a friend, so the variable is in scope
            return true;
        }

        //The variable is a class member; see if the accessing function is a member function of the same class
        SgMemberFunctionDeclaration* memFunction = isSgMemberFunctionDeclaration(accessingFunction);
        if (memFunction == NULL)
            return false;

        SgScopeStatement* funcScope = memFunction->get_class_scope();
        ROSE_ASSERT(funcScope != NULL);

        //If they are members of the same class, we're done
        if (funcScope == varClassScope)
        {
            return true;
        }

        SgDeclarationStatement * decl = NULL;
        SgClassDefinition* funcClassScope = isSgClassDefinition(funcScope);
        SgDeclarationScope* funcNonrealScope = isSgDeclarationScope(funcScope);
        if (funcClassScope != NULL) {
          decl = funcClassScope->get_declaration();
        } else if (funcNonrealScope != NULL) {
          decl = isSgDeclarationStatement(funcClassScope->get_parent());
        } else {
          ROSE_ASSERT(false);
        }

        //The two are not from the same class. Let's see if there is a friend class declaration

        foreach(SgDeclarationStatement* varClassMember, varClassScope->get_members())
        {
            SgClassDeclaration* nestedDeclaration = isSgClassDeclaration(varClassMember);
            if (nestedDeclaration == NULL)
                continue;

            if (nestedDeclaration->get_declarationModifier().isFriend())
            {
                //The variable's class has friend class. Check if the member function in question is in that friend
                if (nestedDeclaration->get_firstNondefiningDeclaration() ==
                        decl->get_firstNondefiningDeclaration())
                {
                    return true;
                }
            }
        }

        //The variable is not in the same class and there is no friend class declaration, but we need to check the inheritance tree
        //We do a search of the inheritance tree; this will terminate because it's a DAG
        set<SgBaseClass*> worklist;
        if (funcClassScope != NULL) {
          worklist.insert(funcClassScope->get_inheritances().begin(), funcClassScope->get_inheritances().end());
        }

        while (!worklist.empty())
        {
            SgBaseClass* baseClass = *worklist.begin();
            worklist.erase(worklist.begin());

            //Get the class definition so we can get its base classes
            SgClassDeclaration* definingDeclaration = isSgClassDeclaration(baseClass->get_base_class()->get_definingDeclaration());
            if (definingDeclaration == NULL)
                continue;

            SgClassDefinition* baseClassDefinition = isSgClassDefinition(definingDeclaration->get_definition());
            ROSE_ASSERT(baseClassDefinition != NULL);

            foreach(SgBaseClass* grandparentClass, baseClassDefinition->get_inheritances())
            {
                worklist.insert(grandparentClass);
            }

            //Check if this base class matches the var scope
            if (baseClassDefinition == varClassScope)
            {
                //Check that the variable is public or protected
                const SgAccessModifier& access = varDeclaration->get_declarationModifier().get_accessModifier();
                return (access.isPublic() || access.isProtected());
            }
        }
    }

    return false;
}

void StaticSingleAssignment::run(bool interprocedural, bool treatPointersAsStructures)
{
    originalDefTable.clear();
    expandedDefTable.clear();
    reachingDefsTable.clear();
    localUsesTable.clear();
    useTable.clear();
    ssaLocalDefTable.clear();

#ifdef DISPLAY_TIMINGS
    timer time;
#endif
    if (getDebug())
        cout << "Running UniqueNameTraversal...\n";
    UniqueNameTraversal uniqueTrav(
        SageInterface::querySubTree<SgInitializedName > (project, V_SgInitializedName), treatPointersAsStructures);
    uniqueTrav.traverse(project);
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
#ifdef DISPLAY_TIMINGS
    printf("-- Timing: Creating list of functions took %.2f seconds.\n", time.elapsed());
    fflush(stdout);
    time.restart();
#endif

    DefsAndUsesTraversal defUseTrav(this, treatPointersAsStructures);

    //Generate all local information before doing interprocedural analysis. This is so we know
    //what variables are directly modified in each function body before we do interprocedural propagation

    foreach(SgFunctionDefinition* func, interestingFunctions)
    {
        if (getDebug())
            cout << "Running DefsAndUsesTraversal on function: " << SageInterface::get_name(func) << func << endl;

        defUseTrav.traverse(func->get_declaration());

        if (getDebug())
            cout << "Finished DefsAndUsesTraversal..." << endl;

        //Expand any member variable definition to also define its parents at the same node
        expandParentMemberDefinitions(func->get_declaration());

        //Expand any member variable uses to also use the parent variables (e.g. a.x also uses a)
        expandParentMemberUses(func->get_declaration());

        insertDefsForChildMemberUses(func->get_declaration());
    }

#ifdef DISPLAY_TIMINGS
    printf("-- Timing: Inserting all local defs for %" PRIuPTR " functions took %.2f seconds.\n",
            interestingFunctions.size(), time.elapsed());
    fflush(stdout);
    time.restart();
#endif

    //Interprocedural iterations. We iterate on the call graph until all interprocedural defs are propagated
    if (interprocedural)
    {
        interproceduralDefPropagation(interestingFunctions);
    }

#ifdef DISPLAY_TIMINGS
    printf("-- Timing: Interprocedural propagation took %.2f seconds.\n", time.elapsed());
    fflush(stdout);
    time.restart();
#endif

    //Now we have all local information, including interprocedural defs. Propagate the defs along control-flow

    foreach(SgFunctionDefinition* func, interestingFunctions)
    {
        vector<FilteredCfgNode> functionCfgNodesPostorder = getCfgNodesInPostorder(func);

        //Insert definitions at the SgFunctionDefinition for external variables whose values flow inside the function
        insertDefsForExternalVariables(func->get_declaration());

        //Create all ReachingDef objects:
        //Create ReachingDef objects for all original definitions
        populateLocalDefsTable(func->get_declaration());
        //Insert phi functions at join points
        multimap< FilteredCfgNode, pair<FilteredCfgNode, FilteredCfgEdge> > controlDependencies =
                insertPhiFunctions(func, functionCfgNodesPostorder);

        //Renumber all instantiated ReachingDef objects
        renumberAllDefinitions(func, functionCfgNodesPostorder);

        if (getDebug())
            cout << "Running DefUse Data Flow on function: " << SageInterface::get_name(func) << func << endl;
        runDefUseDataFlow(func);

        //We have all the propagated defs, now update the use table
        buildUseTable(functionCfgNodesPostorder);

        //Annotate phi functions with dependencies
        //annotatePhiNodeWithConditions(func, controlDependencies);
    }
}

void StaticSingleAssignment::expandParentMemberDefinitions(SgFunctionDeclaration* function)
{

    class ExpandDefsTraversal : public AstSimpleProcessing
    {
    public:
        StaticSingleAssignment* ssa;

        void visit(SgNode* node)
        {
            if (ssa->originalDefTable.count(node) == 0)
                return;

            //We want to iterate the vars defined on this node, and expand them

            foreach(const VarName& definedVar, ssa->originalDefTable[node])
            {
                if (getDebugExtra())
                {
                    cout << "Checking [" << varnameToString(definedVar) << "]" << endl;
                }

                //Check if the variableName has multiple parts
                if (definedVar.size() == 1)
                {
                    continue;
                }

                //We are dealing with a multi-part variable, loop the entry and expand it
                //Start at one so we don't get the same defs in the original and expanded defs
                for (unsigned int i = 1; i < definedVar.size(); i++)
                {
                    //Create a new varName vector that goes from beginning to end - i
                    VarName newName;
                    newName.assign(definedVar.begin(), definedVar.end() - i);

                    if (getDebugExtra())
                    {
                        cout << "Testing for presence of [" << varnameToString(newName) << "]" << endl;
                    }

                    //Only insert the new definition if it does not already exist in the original def table
                    if (ssa->originalDefTable[node].count(newName) == 0)
                    {
                        //Insert the new name as being defined here.
                        ssa->expandedDefTable[node].insert(newName);

                        if (getDebugExtra())
                        {
                            cout << "Inserted new name [" << varnameToString(newName) << "] into defs." << endl;
                        }
                    }
                }
            }
        }
    };

    ExpandDefsTraversal trav;
    trav.ssa = this;
    trav.traverse(function, preorder);
}

void StaticSingleAssignment::expandParentMemberUses(SgFunctionDeclaration* function)
{

    class ExpandUsesTraversal : public AstSimpleProcessing
    {
    public:
        StaticSingleAssignment* ssa;

        void visit(SgNode* node)
        {
            if (ssa->localUsesTable.count(node) == 0)
                return;

            //We want to iterate the vars used on this node, and expand them

            foreach(const VarName& usedVar, ssa->localUsesTable[node])
            {
                if (getDebugExtra())
                {
                    cout << "Checking [" << varnameToString(usedVar) << "]" << endl;
                }

                //Check if the variableName has multiple parts
                if (usedVar.size() == 1)
                {
                    continue;
                }

                //We are dealing with a multi-part variable, loop the entry and expand it
                //Start at one so we don't reinsert same use
                for (unsigned int i = 1; i < usedVar.size(); i++)
                {
                    //Create a new varName vector that goes from beginning to end - i
                    VarName newName;
                    newName.assign(usedVar.begin(), usedVar.end() - i);

                    if (getDebugExtra())
                    {
                        cout << "Testing for presence of [" << varnameToString(newName) << "]" << endl;
                    }

                    //Only insert the new definition if it does not already exist
                    if (ssa->localUsesTable[node].count(newName) == 0)
                    {
                        //Insert the new name as being used here.
                        ssa->localUsesTable[node].insert(newName);

                        if (getDebugExtra())
                        {
                            cout << "Inserted new name [" << varnameToString(newName) << "] into uses." << endl;
                        }
                    }
                }
            }
        }
    };

    ExpandUsesTraversal trav;
    trav.ssa = this;
    trav.traverse(function, preorder);
}

void StaticSingleAssignment::runDefUseDataFlow(SgFunctionDefinition* func)
{
    if (getDebug())
        printOriginalDefTable();
    //Keep track of visited nodes
    boost::unordered_set<SgNode*> visited;

    set<FilteredCfgNode> worklist;

    FilteredCfgNode current= FilteredCfgNode(func->cfgForBeginning());
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

        reverse_foreach(const FilteredCfgEdge& edge, current.outEdges())
        {
            FilteredCfgNode nextNode = edge.target();

            //Insert the child in the worklist if the parent is changed or it hasn't been visited yet
            if (changed || visited.count(nextNode.getNode()) == 0)
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
        visited.insert(current.getNode());
    }
}

bool StaticSingleAssignment::propagateDefs(FilteredCfgNode cfgNode)
{
    SgNode* node = cfgNode.getNode();

    //This updates the IN table with the reaching defs from previous nodes
    updateIncomingPropagatedDefs(cfgNode);

    //Special Case: the OUT table at the function definition node actually denotes definitions at the function entry
    //So, if we're propagating to the *end* of the function, we shouldn't update the OUT table
    if (isSgFunctionDefinition(node) && cfgNode == FilteredCfgNode(node->cfgForEnd()))
    {
        return false;
    }

    //Create a staging OUT table. At the end, we will check if this table
    //Was the same as the currently available one, to decide if any changes have occurred
    //We initialize the OUT table to the IN table
    NodeReachingDefTable outDefsTable = reachingDefsTable[node].first;

    //Special case: the IN table of the function definition node actually denotes
    //definitions reaching the *end* of the function. So, start with an empty table to prevent definitions
    //from the bottom of the function from propagating to the top.
    if (isSgFunctionDefinition(node) && cfgNode == FilteredCfgNode(node->cfgForBeginning()))
    {
        outDefsTable.clear();
    }

    //Now overwrite any local definitions:
    if (ssaLocalDefTable.count(node) > 0)
    {

        foreach(const NodeReachingDefTable::value_type& varDefPair, ssaLocalDefTable[node])
        {
            const VarName& definedVar = varDefPair.first;
            ReachingDefPtr localDef = varDefPair.second;

            outDefsTable[definedVar] = localDef;
        }
    }

    //Compare old to new OUT tables
    bool changed = (reachingDefsTable[node].second != outDefsTable);
    if (changed)
    {
        reachingDefsTable[node].second = outDefsTable;
    }

    return changed;
}

void StaticSingleAssignment::updateIncomingPropagatedDefs(FilteredCfgNode cfgNode)
{
    //Get the previous edges in the CFG for this node
    vector<FilteredCfgEdge> inEdges = cfgNode.inEdges();
    SgNode* astNode = cfgNode.getNode();

    NodeReachingDefTable& incomingDefTable = reachingDefsTable[astNode].first;

    //Iterate all of the incoming edges
    for (unsigned int i = 0; i < inEdges.size(); i++)
    {
        SgNode* prev = inEdges[i].source().getNode();

        const NodeReachingDefTable& previousDefs = reachingDefsTable[prev].second;

        //Merge all the previous defs into the IN table of the current node

        foreach(const NodeReachingDefTable::value_type& varDefPair, previousDefs)
        {
            const VarName& var = varDefPair.first;
            const ReachingDefPtr previousDef = varDefPair.second;

            //Here we don't propagate defs for variables that went out of scope
            //(built-in vars are body-scoped but we inserted the def at the SgFunctionDefinition node, so we make an exception)
            if (!isVarInScope(var, astNode) && !isBuiltinVar(var))
                continue;

            //If this is the first time this def has propagated to this node, just copy it over
            if (incomingDefTable.count(var) == 0)
            {
                incomingDefTable[var] = previousDef;
            }
            else
            {
                ReachingDefPtr existingDef = incomingDefTable[var];

                if (existingDef->isPhiFunction() && existingDef->getDefinitionNode() == astNode)
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
                        printf("ERROR: At node %s@%d, two different definitions reach for variable %s\n",
                                astNode->class_name().c_str(), astNode->get_file_info()->get_line(), varnameToString(var).c_str());
                        ROSE_ASSERT(false);
                    }
                }
            }
        }
    }
}

void StaticSingleAssignment::buildUseTable(const vector<FilteredCfgNode>& cfgNodes)
{

    foreach(const FilteredCfgNode& cfgNode, cfgNodes)
    {
        SgNode* node = cfgNode.getNode();

        if (localUsesTable.count(node) == 0)
            continue;

        foreach(const VarName& usedVar, localUsesTable[node])
        {
            //Check the defs that are active at the current node to find the reaching definition
            //We want to check if there is a definition entry for this use at the current node
            if (reachingDefsTable[node].first.count(usedVar) > 0)
            {
                useTable[node][usedVar] = reachingDefsTable[node].first[usedVar];
            }
            else
            {
                // There are no defs for this use at this node, this shouldn't happen
                printf("Error: Found use for the name '%s', but no reaching defs!\n", varnameToString(usedVar).c_str());
                printf("Node is %s:%d in %s\n", node->class_name().c_str(), node->get_file_info()->get_line(),
                        node->get_file_info()->get_filename());
                continue; // FIXME ROSE-1392
                ROSE_ASSERT(false);
            }
        }
    }

    if (getDebug())
    {
        printf("Local uses table:\n");
        printLocalDefUseTable(localUsesTable);
    }
}

/** Returns a set of all the variables names that have uses in the subtree. */
set<StaticSingleAssignment::VarName> StaticSingleAssignment::getVarsUsedInSubtree(SgNode* root) const
{

    class CollectUsesVarsTraversal : public AstSimpleProcessing
    {
    public:
        const StaticSingleAssignment* ssa;

        //All the varNames that have uses in the function
        set<VarName> usedNames;

        void visit(SgNode* node)
        {
            LocalDefUseTable::const_iterator useEntry = ssa->localUsesTable.find(node);
            if (useEntry != ssa->localUsesTable.end())
            {
                usedNames.insert(useEntry->second.begin(), useEntry->second.end());
            }

            LocalDefUseTable::const_iterator defEntry = ssa->originalDefTable.find(node);
            if (defEntry != ssa->originalDefTable.end())
            {
                usedNames.insert(defEntry->second.begin(), defEntry->second.end());
            }
        }
    };

    CollectUsesVarsTraversal usesTrav;
    usesTrav.ssa = this;
    usesTrav.traverse(root, preorder);
    return usesTrav.usedNames;
}

void StaticSingleAssignment::insertDefsForChildMemberUses(SgFunctionDeclaration* function)
{
    ROSE_ASSERT(function->get_definition() != NULL);

    set<VarName> usedNames = getVarsUsedInSubtree(function);

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

    //Now that we have all the used names, we iterate the definitions.
    //If there is a definition and a child of it is used, we have to insert a definition for the child also

    class InsertExpandedDefsTraversal : public AstSimpleProcessing
    {
    public:
        StaticSingleAssignment* ssa;
        map<VarName, set<VarName> >* nameToChildNames;

        void visit(SgNode* node)
        {
            LocalDefUseTable::const_iterator childDefs = ssa->originalDefTable.find(node);

            if (childDefs == ssa->originalDefTable.end())
                return;

            foreach(const VarName& definedVar, childDefs->second)
            {
                map<VarName, set<VarName> >::iterator childVars = nameToChildNames->find(definedVar);
                if (childVars == nameToChildNames->end())
                    continue;

                //Go over all the child names and define them here also

                foreach(const VarName& childName, childVars->second)
                {
                    ROSE_ASSERT(childName.size() > definedVar.size());
                    for (size_t i = 0; i < (childName.size() - definedVar.size()); i++)
                    {
                        //Create a new varName vector that goes from beginning to end - i
                        VarName newName;
                        newName.assign(childName.begin(), childName.end() - i);

                        if (ssa->expandedDefTable[node].count(newName) == 0 && ssa->originalDefTable[node].count(newName) == 0)
                        {
                            ssa->expandedDefTable[node].insert(newName);
                        }
                    }
                }
            }
        }
    };

    InsertExpandedDefsTraversal trav;
    trav.ssa = this;
    trav.nameToChildNames = &nameToChildNames;
    trav.traverse(function, preorder);
}

/** Insert defs for functions that are declared outside the function scope. */
void StaticSingleAssignment::insertDefsForExternalVariables(SgFunctionDeclaration* function)
{
    ROSE_ASSERT(function->get_definition() != NULL);

    set<VarName> usedNames = getVarsUsedInSubtree(function);

    set<VarName>& originalVarsAtFunctionEntry = originalDefTable[function->get_definition()];
    set<VarName>& expandedVarsAtFunctionEntry = expandedDefTable[function->get_definition()];

    //Iterate over each used variable and check it it is declared outside of the function scope

    foreach(const VarName& usedVar, usedNames)
    {
        VarName rootName;
        rootName.assign(1, usedVar[0]);

        SgScopeStatement* varScope = SageInterface::getScope(rootName[0]);
        SgScopeStatement* functionScope = function->get_definition();

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
            if (SageInterface::isAncestor(function->get_definition(), rootName[0]))
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
                || isSgGlobal(varScope));

        //The variable is not in local scope; we need to insert a def for it at the function definition
        for (size_t i = 0; i < usedVar.size(); i++)
        {
            //Create a new varName vector that goes from beginning to end - i
            VarName newName;
            newName.assign(usedVar.begin(), usedVar.end() - i);
            originalVarsAtFunctionEntry.insert(newName);
            ROSE_ASSERT(expandedVarsAtFunctionEntry.count(newName) == 0);
        }
    }
}

multimap< StaticSingleAssignment::FilteredCfgNode, pair<StaticSingleAssignment::FilteredCfgNode, StaticSingleAssignment::FilteredCfgEdge> >
StaticSingleAssignment::insertPhiFunctions(SgFunctionDefinition* function, const std::vector<FilteredCfgNode>& cfgNodesInPostOrder)
{
    if (getDebug())
        printf("Inserting phi nodes in function %s...\n", function->get_declaration()->get_name().str());
    ROSE_ASSERT(function != NULL);

    //First, find all the places where each name is defined
    map<VarName, vector<FilteredCfgNode> > nameToDefNodesMap;

    foreach(const FilteredCfgNode& cfgNode, cfgNodesInPostOrder)
    {
        SgNode* node = cfgNode.getNode();

        //Don't visit the sgFunctionDefinition node twice
        if (isSgFunctionDefinition(node) && cfgNode != FilteredCfgNode(node->cfgForBeginning()))
            continue;

        //Check the definitions at this node and add them to the map
        LocalDefUseTable::const_iterator defEntry = originalDefTable.find(node);
        if (defEntry != originalDefTable.end())
        {

            foreach(const VarName& definedVar, defEntry->second)
            {
                nameToDefNodesMap[definedVar].push_back(cfgNode);
            }
        }

        defEntry = expandedDefTable.find(node);
        if (defEntry != expandedDefTable.end())
        {

            foreach(const VarName& definedVar, defEntry->second)
            {
                nameToDefNodesMap[definedVar].push_back(cfgNode);
            }
        }
    }

    //Build an iterated dominance frontier for this function
    map<FilteredCfgNode, FilteredCfgNode> iPostDominatorMap;
    map<FilteredCfgNode, set<FilteredCfgNode> > domFrontiers =
            calculateDominanceFrontiers<FilteredCfgNode, FilteredCfgEdge > (function, NULL, &iPostDominatorMap);

    //Calculate control dependencies (for annotating the phi functions)
    multimap< FilteredCfgNode, pair<FilteredCfgNode, FilteredCfgEdge> > controlDependencies =
            calculateControlDependence<FilteredCfgNode, FilteredCfgEdge > (function, iPostDominatorMap);

    //Find the phi function locations for each variable
    VarName var;
    vector<FilteredCfgNode> definitionPoints;

    foreach(tie(var, definitionPoints), nameToDefNodesMap)
    {
        ROSE_ASSERT(!definitionPoints.empty() && "We have a variable that is not defined anywhere!");

        //Calculate the iterated dominance frontier
        set<FilteredCfgNode> phiNodes = calculateIteratedDominanceFrontier(domFrontiers, definitionPoints);

        if (getDebug())
            printf("Variable %s has phi nodes inserted at\n", varnameToString(var).c_str());

        foreach(FilteredCfgNode phiNode, phiNodes)
        {
            SgNode* node = phiNode.getNode();
            ROSE_ASSERT(reachingDefsTable[node].first.count(var) == 0);

            //We don't want to insert phi defs for functions that have gone out of scope
            if (!isVarInScope(var, node))
                continue;

            reachingDefsTable[node].first[var] = ReachingDefPtr(new ReachingDef(node, ReachingDef::PHI_FUNCTION));

            if (getDebug())
                printf("\t\t%s\n", phiNode.toStringForDebugging().c_str());
        }
    }

    return controlDependencies;
}

void StaticSingleAssignment::populateLocalDefsTable(SgFunctionDeclaration* function)
{
    ROSE_ASSERT(function->get_definition() != NULL);

    struct InsertDefs : public AstSimpleProcessing
    {
        StaticSingleAssignment* ssa;

        void visit(SgNode * node)
        {
            //Short circuit to prevent creating empty entries in the local def table when we don't need them
            if ((ssa->originalDefTable.count(node) == 0 || ssa->originalDefTable[node].empty()) &&
                    (ssa->expandedDefTable.count(node) == 0 || ssa->expandedDefTable[node].empty()))
            {
                return;
            }

            //This is the table of local definitions at the current node
            NodeReachingDefTable& localDefs = ssa->ssaLocalDefTable[node];

            if (ssa->originalDefTable.count(node) > 0)
            {

                foreach(const VarName& definedVar, ssa->originalDefTable[node])
                {
                    localDefs[definedVar] = ReachingDefPtr(new ReachingDef(node, ReachingDef::ORIGINAL_DEF));
                }
            }

            if (ssa->expandedDefTable.count(node) > 0)
            {

                foreach(const VarName& definedVar, ssa->expandedDefTable[node])
                {
                    localDefs[definedVar] = ReachingDefPtr(new ReachingDef(node, ReachingDef::EXPANDED_DEF));
                }
            }
        }
    };

    InsertDefs trav;
    trav.ssa = this;
    trav.traverse(function, preorder);
}

void StaticSingleAssignment::renumberAllDefinitions(SgFunctionDefinition* func, const vector<FilteredCfgNode>& cfgNodesInPostOrder)
{
    //Map from each name to the next index. Not in map means 0
    map<VarName, int> nameToNextIndexMap;

    //The SgFunctionDefinition node is special. reachingDefs INTO the function definition node are actually
    //The definitions that reach the *end* of the function
    //reachingDefs OUT of the function definition node are the ones that come externally into the function
    FilteredCfgNode functionStartNode = FilteredCfgNode(func->cfgForBeginning());
    FilteredCfgNode functionEndNode = FilteredCfgNode(func->cfgForEnd());

    //We process nodes in reverse postorder; this provides a natural numbering for definitions

    reverse_foreach(const FilteredCfgNode& cfgNode, cfgNodesInPostOrder)
    {
        SgNode* astNode = cfgNode.getNode();

        //Iterate over all the phi functions inserted at this node. We skip the SgFunctionDefinition entry node,
        //since those phi functions actually belong to the bottom of the CFG
        if (cfgNode != functionStartNode)
        {

            foreach(NodeReachingDefTable::value_type& varDefPair, reachingDefsTable[astNode].first)
            {
                const VarName& definedVar = varDefPair.first;
                ReachingDefPtr reachingDef = varDefPair.second;

                if (!reachingDef->isPhiFunction())
                    continue;

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

        //Local defs at the function end actually occur at the very beginning of the function
        if (cfgNode != functionEndNode)
        {
            //Iterate over all the local definitions at the node

            foreach(NodeReachingDefTable::value_type& varDefPair, ssaLocalDefTable[astNode])
            {
                const VarName& definedVar = varDefPair.first;
                ReachingDefPtr reachingDef = varDefPair.second;

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
vector<StaticSingleAssignment::FilteredCfgNode> StaticSingleAssignment::getCfgNodesInPostorder(SgFunctionDefinition* func)
{

    struct RecursiveDFS
    {

        static void depthFirstSearch(StaticSingleAssignment::FilteredCfgNode cfgNode,
                set<FilteredCfgNode>& visited, vector<StaticSingleAssignment::FilteredCfgNode>& result)
        {
            //First, make sure this node hasn't been visited yet
            if (visited.count(cfgNode) != 0)
                return;

            visited.insert(cfgNode);

            //Now, visit all the node's successors

            reverse_foreach(const FilteredCfgEdge outEdge, cfgNode.outEdges())
            {
                depthFirstSearch(outEdge.target(), visited, result);
            }

            //Add this node to the postorder list
            result.push_back(cfgNode);
        }
    };

    ROSE_ASSERT(func != NULL);

    set<FilteredCfgNode> visited;
    vector<FilteredCfgNode> results;
    FilteredCfgNode entry = func->cfgForBeginning();

    RecursiveDFS::depthFirstSearch(entry, visited, results);

    return results;
}


  // DQ (1/7/2018): Move this from the header file to the source code file (staticSingleAssignmentCalculation.C).
    /**
     * @param dominatorTree map from each node in the dom tree to its childrenn
     * @param iDominatorMap map from each node to its immediate dominator. */
template<class CfgNodeT, class CfgEdgeT>
multimap< CfgNodeT, pair<CfgNodeT, CfgEdgeT> >
ssa_private::calculateControlDependence(SgFunctionDefinition* function, const map<CfgNodeT, CfgNodeT>& iPostDominatorMap)
    {
        //Map from each node to the nodes it's control dependent on (and corresponding edges)
        multimap< CfgNodeT, pair<CfgNodeT, CfgEdgeT> > controlDepdendences;

        //Let's iterate the control flow graph and stop every time we hit an edge with a condition
        set<CfgNodeT> visited;
        set<CfgNodeT> worklist;

        CfgNodeT sourceNode = function->cfgForBeginning();
        worklist.insert(sourceNode);

        while (!worklist.empty())
        {
            //Get the node to work on
            sourceNode = *worklist.begin();
            worklist.erase(worklist.begin());
            visited.insert(sourceNode);

            //For every edge, add it to the worklist 

            BOOST_FOREACH(const CfgEdgeT& edge, sourceNode.outEdges())
            {
                CfgNodeT targetNode = edge.target();

                //Insert the child in the worklist if the it hasn't been visited yet
                if (visited.count(targetNode) == 0)
                {
                    worklist.insert(targetNode);
                }

                //Check if we need to process this edge in control dependence calculation
                if (edge.condition() == VirtualCFG::eckUnconditional)
                    continue;

                //We traverse from nextNode up in the postdominator tree until we reach the parent of currNode.
                CfgNodeT parent;
                typename map<CfgNodeT, CfgNodeT>::const_iterator parentIter = iPostDominatorMap.find(sourceNode);

             // DQ (1/7/2018): Note clear how to fix this. The correct handling of compound statements appears to be a problem for this code.
             // This failes for test2001_14.C in the ss testing.
                if (parentIter == iPostDominatorMap.end())
                   {
                  // printf ("sourceNode = %p \n",sourceNode);
                     printf ("Error: function = %p = %s \n",function,function->class_name().c_str());
                     function->get_file_info()->display("Called from ssa_private::calculateControlDependence: debug");
                  // ROSE_ASSERT(false);
                  // return controlDepdendences;
                   }
#if 0
                printf ("Testing: parentIter != iPostDominatorMap.end() \n");
#endif
                ROSE_ASSERT(parentIter != iPostDominatorMap.end());

                parent = parentIter->second;

                //This is the node that we'll be marking as control dependent
                CfgNodeT currNode = targetNode;

                while (true)
                {
                    //If we reach the parent of the source, stop
                    if (currNode == parent)
                    {
                        break;
                    }

                    //Add a control dependence from the source to the new node
                    controlDepdendences.insert(make_pair(currNode, make_pair(sourceNode, edge)));

                    if (StaticSingleAssignment::getDebugExtra())
                    {
                        printf("%s is control-dependent on %s - %s \n", currNode.toStringForDebugging().c_str(),
                                sourceNode.toStringForDebugging().c_str(), edge.condition() == VirtualCFG::eckTrue ? "true" : "false");
                    }

                    //Move to the parent of the current node
                    parentIter = iPostDominatorMap.find(currNode);
                    ROSE_ASSERT(parentIter != iPostDominatorMap.end());
                    currNode = parentIter->second;
                }
            }
        }

        return controlDepdendences;
    }
