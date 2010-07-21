/* 
 * File:   VariableRenaming.h
 * Author: frye5
 *
 * Created on June 15, 2010, 4:12 PM
 */

#ifndef SSAANALYSIS_H
#define	SSAANALYSIS_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <ostream>
#include <fstream>
#include <sstream>
#include <boost/foreach.hpp>
#include "filteredCFG.h"

/** Class holding a unique name for a variable. Is attached to varRefs as a persistant attribute.
 */
class VarUniqueName : public AstAttribute
{
private:

    /** The vector of initializedNames that uniquely identifies this VarRef.
     *  The node which this name is attached to should be the last in the list.
     */
    std::vector<SgInitializedName*> key;

    bool usesThis;

public:
    /** Constructs the attribute with an empty key.
     */
    VarUniqueName():key(),usesThis(false){}

    /** Constructs the attribute with value thisNode.
     *
     * The key will consist of only the current node.
     *
     * @param thisNode The node to use for the key.
     */
    VarUniqueName(SgInitializedName* thisNode):usesThis(false)
    {
        key.push_back(thisNode);
    }

    /** Constructs the attribute using the prefix vector and thisNode.
     *
     * The key will first be copied from the prefix value, and then the thisNode
     * value will be appended.
     *
     * @param prefix The prefix of the new name.
     * @param thisNode The node to append to the end of the new name.
     */
    VarUniqueName(const std::vector<SgInitializedName*>& prefix, SgInitializedName* thisNode):usesThis(false)
    {
        key.assign(prefix.begin(),prefix.end());
        key.push_back(thisNode);
    }

    /** Copy the attribute.
     *
     * @param other The attribute to copy from.
     */
    VarUniqueName(const VarUniqueName& other):usesThis(false)
    {
        key.assign(other.key.begin(), other.key.end());
    }

    VarUniqueName* copy()
    {
        VarUniqueName* newName = new VarUniqueName(*this);
        return newName;
    }

    /** Get a constant reference to the name.
     *
     * @return Constant Reference to the name.
     */
    std::vector<SgInitializedName*>& getKey() { return key; }

    /** Set the value of the name.
     *
     * @param newKey The new name to use.
     */
    void setKey(const std::vector<SgInitializedName*>& newKey) { key.assign(newKey.begin(),newKey.end()); }

    bool getUsesThis() { return usesThis; }
    void setUsesThis(bool uses) { usesThis = uses; }

    /** Get the string representing this uniqueName
     *
     * @return The name string.
     */
    std::string getNameString()
    {
        std::string name = "";
        std::vector<SgInitializedName*>::iterator iter;
        if(usesThis)
            name += "this->";
        for(iter = key.begin(); iter != key.end(); ++iter)
        {
            if(iter != key.begin())
            {
                name += ":";
            }
            name += (*iter)->get_name().getString();
        }

        return name;
    }
};

/** Struct containing a filtering function to determine what CFG nodes
 * are interesting during the DefUse traversal.
 */
struct IsDefUseFilter
{
    /** Determines if the provided CFG node should be traversed during DefUse.
     *
     * @param cfgn The node in question.
     * @return Whether it should be traversed.
     */
    bool operator() (CFGNode cfgn) const
    {
        SgNode *node = cfgn.getNode();

        //If it is the last node in a function call, keep it
        if(isSgFunctionCallExp(node) && cfgn == node->cfgForEnd())
            return true;
        
        //Remove all non-interesting nodes
        if (!cfgn.isInteresting())
            return false;

        //Remove all non-beginning nodes for initNames
        if (isSgInitializedName(node) && cfgn != node->cfgForBeginning())
            return false;

        return true; 
    }
};

/** Class that defines an VariableRenaming of a program
 *
 * Contains all the functionality to implement variable renaming on a given program.
 * For this class, we do not actually transform the AST directly, rather
 * we perform the analysis and add attributes to the AST nodes so that later
 * optimizations can access the results of this analysis while still preserving
 * the original AST.
 */
class VariableRenaming
{
private:
    /** The project to perform SSA Analysis on.
     */
    SgProject* project;
    bool DEBUG_MODE;
    bool DEBUG_MODE_EXTRA;

public:
    /** Vector of SgNode*
     */
    typedef std::vector<SgNode*> NodeVec;
    /** A compound variable name as used by the variable renaming.
     */
    typedef std::vector<SgInitializedName*> VarName;
    /** An entry in the rename table mapping a name to a nodeVec.
     */
    typedef std::map<VarName, NodeVec> TableEntry;
    /** A table storing the name->node mappings for every node in the program.
     */
    typedef std::map<SgNode*, TableEntry> DefUseTable;
    /** A table mapping a name to a single node.
     */
    typedef std::map<VarName, SgNode*> FirstDefTable;
    /** A list of names.
     */
    typedef std::vector<VarName> GlobalTable;
    /** A vector of SgInitializedName*
     */
    typedef std::vector<SgInitializedName*> InitNameVec;
    /** A filtered CFGNode that is used for DefUse traversal.
     */
    typedef FilteredCFGNode<IsDefUseFilter> cfgNode;
    /** A filtered CFGEdge that is used for DefUse traversal.
     */
    typedef FilteredCFGEdge<IsDefUseFilter> cfgEdge;
    /** A vector of cfgNodes.
     */
    typedef std::vector<cfgNode> cfgNodeVec;
    /** A vector of cfgEdges.
     */
    typedef std::vector<cfgEdge> cfgEdgeVec;
    /** An entry in the rename table that maps a node to a number.
     */
    typedef std::map<SgNode*, int> NodeNumRenameEntry;
    /** A table that maps a name to it's node->number renamings.
     */
    typedef std::map<VarName, NodeNumRenameEntry> NodeNumRenameTable;
    /** An entry in the rename table that maps a number to a node.
     */
    typedef std::map<int, SgNode*> NumNodeRenameEntry;
    /** A table that maps a name to it's number->node renamings.
     */
    typedef std::map<VarName, NumNodeRenameEntry> NumNodeRenameTable;


private:
    //Private member variables

    /** This is the table of variable definition locations that is generated by
     * the VarDefUseTraversal. It is later used to populate the actual def/use table.
     */
    DefUseTable originalDefTable;

    /** This is the table of definitions that is expanded from the original table.
     * It is used to populate the actual def/use table.
     */
    DefUseTable expandedDefTable;

    /** This is the table that is populated with all the def information for all the variables
     * at all the nodes. It is populated during the runDefUse function, and is done
     * with the steady-state dataflow algorithm.
     */
    DefUseTable defTable;

    /** This is the table that is populated with all the use information for all the variables
     * at all the nodes. It is populated during the runDefUse function, and is done
     * with the steady-state dataflow algorithm.
     */
    DefUseTable useTable;

    /** This is the table that is populated with all the use information for all the variables
     * at all the nodes. It is populated during the VarDefUse Traversal and contains the varRefs
     * to the actual uses of the variables.
     */
    DefUseTable useLocTable;

    /** Holds a list of the locations that a particular name is first
     * defined.
     *
     * This helps when backwards-inserting definitions of member variables,
     * so that we can simply insert the definition at the first definition.
     */
    FirstDefTable firstDefList;

    /** This holds the mapping between variables and the nodes where they are renumbered.
     * Given a name and a node, we can get the number of the name that is defined at that node.
     * Nodes which do not define a name are not in the table. 
     */
    NodeNumRenameTable nodeRenameTable;

    /** This holds the mapping between variables and the nodes where they are renumbered.
     * Given a name and a number, we can get the node where that number is defined.
     * Nodes which do not define a name are not in the table.
     */
    NumNodeRenameTable numRenameTable;

    /** A list of all the global varibales in the program.
     */
    GlobalTable globalVarList;

public:
    VariableRenaming(SgProject* proj): project(proj), DEBUG_MODE(true), DEBUG_MODE_EXTRA(true){}

    ~VariableRenaming(){}
    
    void run();

    bool getDebug() const { return DEBUG_MODE; }
    bool getDebugExtra() const { return DEBUG_MODE_EXTRA; }
    
private:
    void runDefUse(SgFunctionDefinition* func);
    bool defUse(cfgNode node, bool *memberRefInserted, NodeVec &changedNodes);

    /** Add an entry to the renumbering table for the given var and node.
     *
     * This will place a new entry into the renaming table that renumbers
     * var at node. This new definition will have the next available numbering
     * of var. If the var @ node combination already exists, the number will
     * be returned.
     *
     * @param var The variable to renumber.
     * @param node The node that defines the new number.
     * @return The renumbering assigned to ver @ node.
     */
    int addRenameNumberForNode(const VarName& var, SgNode* node);
    
    /** Locate all global varibales and add them to the table.
     */
    void findGlobalVars();

    /** Called to merge the defs from previous nodes in the CFG to this one.
     *
     * This will merge the def tables from all previous CFG nodes, merge in the
     * defs at this node, and update this node's table if needed. If it locates a def that
     * is not in the table, it will attempt to find the location where that member
     * was first defined and insert a definition there. It will then set the outParameter
     * to indicate that it back-inserted a def.
     *
     * @param curNode The node to merge defs onto.
     * @param memberRefInserted Reference that indicates whether the function back-inserted a definition.
     * @return Whether the defs were different from those already on the node.
     */
    bool mergeDefs(cfgNode curNode, bool *memberRefInserted);

    /** Called to update the uses on the current node.
     *
     * This will update the uses at this node to point to the defs that were propogated
     * from previous nodes. If it locates a use of a def that did not exist, it
     * will attempt to locate the definition site of the base variable and insert a def
     * there. It will then set the outParameter to indicate that it back-inserted a def.
     *
     * @param curNode The node to resolve uses on.
     * @param memberRefInserted Reference that indicates whether the function back-inserted a definition.
     * @return Whether the uses were different from those already on the node.
     */
    bool resolveUses(cfgNode curNode, bool *memberRefInserted, NodeVec &changedNodes);

    /** Trace backwards in the cfg one step and return an aggregate of all previous defs.
     *
     * @param curNode Node to traverse backwards from.
     * @param results TableEntry reference where results are stored.
     */
    void aggregatePreviousDefs(cfgNode curNode, TableEntry& results);

    /** Inserts definition points for all global variables.
     *
     * This will insert definitions for all global variables at 2 places.
     * 1. At the entry points of all functionDefinitions.
     * 2. At every function call expression.
     */
    void insertGlobalVarDefinitions();

    /** Expand all member definitions (chained names) to define every name in the chain.
     *
     * When a member of a struct/class is referenced, this will insert definitions
     * for every member referenced to access the currently referenced one.
     *
     * ex.   Obj o;         //Declare o of type Obj
     *       o.a.b = 5;     //Def for o.a.b
     *
     * In the second line, this function will insert the following:
     *
     *       o.a.b = 5;     //Def for o.a.b, o.a, o
     *
     * @param curNode The node to expand the definitions on.
     * @return Whether or not any new defs were inserted.
     */
    bool expandMemberDefinitions(cfgNode curNode);

    /** Insert defs for member uses (chained names) that do not have an explicit def.
     *
     * When a member of a struct/class is used and that member does not have a propogated
     * def on the current node, this will find the closest definition of a member
     * in the ref chain and insert a definition for this use at that member's def.
     *
     * ex. Obj o;         //Declare o of type o
     *     o.a = 5;       //Def for o.a
     *     int i = o.b.x; //Def for i, use for o.b.x
     *     int j = o.a.x; //Def for j, use for o.a.x
     *
     * This function will insert the following:
     *     Obj o;         //Declare o of type o, Def for o.a, def for o.b, def for o.b.x
     *     o.a = 5;       //Def for o.a, use for o, use for o.a, def for o.a.x
     *     int i = o.b.x; //Def for i, use for o.b.x
     *     int j = o.a.x; //Def for j, use for o.a.x
     *
     * @param curNode The node to expand the uses on.
     * @param name The variableName to expand the uses for.
     * @return Whether any new defs were inserted.
     */
    bool insertExpandedDefsForUse(cfgNode curNode, VarName name, NodeVec &changedNodes);

    /** Expand all member uses (chained names) to explicitly use every name in the chain.
     *
     * When a member of a struct/class is used, this will insert uses for every
     * member referenced to access the currently used one.
     *
     * ex.   Obj o;         //Declare o of type Obj
     *       int i;         //Declare i of type int
     *       i = o.a.b;     //Def for i, use for o.a.b
     *
     * In the third line, this function will insert the following:
     *
     *       i = o.a.b;     //Def for i, use for o.a.b, o.a, o
     *
     * @param curNode
     * @return Whether any new uses were inserted.
     */
    bool expandMemberUses(cfgNode curNode);

    void printToDOT(SgSourceFile* file, std::ofstream &outFile);
    void printToFilteredDOT(SgSourceFile* file, std::ofstream &outFile);

    void printUses(TableEntry& table);
    void printDefs(TableEntry& table);
    
public:
    //External static helper functions/variables
    /** Tag to use to retrieve unique naming key from node.
     */
    static std::string varKeyTag;

    /** This represents the initializedName for the 'this' keyword.
     *
     * This will allow the this pointer to be versioned inside member functions.
     */
    static SgInitializedName* thisDecl;

    static VarName emptyName;

    static NumNodeRenameTable emptyRenameTable;

    static NumNodeRenameEntry emptyRenameEntry;


    /*
     *  Printing functions.
     */

    /** Print the CFG with any UniqueNames and Def/Use information visible.
     *
     * @param fileName The filename to save graph as. Filenames will be prepended.
     */
    void toDOT(const std::string fileName);
    
    /** Print the CFG with any UniqueNames and Def/Use information visible.
     *
     * This will only print the nodes that are of interest to the filter function
     * used by the def/use traversal.
     *
     * @param fileName The filename to save graph as. Filenames will be prepended.
     */
    void toFilteredDOT(const std::string fileName);

    /** Get a string representation of a varName.
     *
     * @param vec varName to get string for.
     * @return String for given varName.
     */
    static std::string keyToString(VarName vec);
    
    void printDefs(SgNode* node);

    void printOriginalDefs(SgNode* node);

    void printOriginalDefTable();

    void printUses(SgNode* node);

    void printRenameTable();

    void printRenameTable(const VarName& var);

    void printRenameTable(const NodeNumRenameTable& table);

    void printRenameTable(const NumNodeRenameTable& table);

    void printRenameEntry(const NodeNumRenameEntry& entry);
    
    void printRenameEntry(const NumNodeRenameEntry& entry);



    /*
     *   Def/Use Table Access Functions
     */

    /** Get the table of definitions for every node.
     *
     * @return Definition table.
     */
    DefUseTable& getDefTable(){ return originalDefTable; }

    /** Get the defTable containing the propogated definition information.
     *
     * @return Def table.
     */
    DefUseTable& getPropDefTable(){ return defTable; }

    /** Get the table of uses for every node.
     *
     * @return Use Table.
     */
    DefUseTable& getUseTable(){ return useTable; }

    /** Get the listing of global variables.
     *
     * @return Global Var List.
     */
    GlobalTable& getGlobalVarList(){ return globalVarList; }



    /*
     *   Rename Table Access Functions
     */

    /** Get the rename number for the given variable and the given node.
     *
     * This will return the number of the given variable as it is defined on the given
     * node. If the provided node does not define the variable, the function will
     * return -1.
     *
     * @param var The variable to get the renumbering for.
     * @param node The defining node to get the renumbering at.
     * @return The number of var @ node, or -1 if node does not define var.
     */
    int getRenameNumberForNode(const VarName& var, SgNode* node) const;

    /** Get the node that defines the given number of the given variable.
     *
     * This will return the node that defines the 'num' value of var.
     * It will be the defining node for the variable renumbered with num of the variable
     * var. If the provided number does not exist for var, it will return NULL.
     *
     * @param var The variable to get the defining node for.
     * @param num The renumbering of the defining node to get.
     * @return The defining node of var:num, or NULL if the renumbering does not exist.
     */
    SgNode* getNodeForRenameNumber(const VarName& var, int num) const;

    /** Get the number of the last rename of the given variable.
     *
     * This will return the number of the last renaming of the given variable.
     * If the given variable has no renamings, it will return -1.
     *
     * @param var The variable to get the last renaming for.
     * @return The highest renaming number, or -1 if var is not renamed.
     */
    int getMaxRenameNumberForName(const VarName& var) const;



    /*
     *   Variable Renaming Access Functions
     */

    /** Retrieve a list of nodes that use the var:num specified.
     *
     * This will retrieve a list of nodes that use the specified var:num combo.
     * ex.  int i = s2.y1;  //Search for s:y1 will yield varRef for y1, as well as
     *                      //the DotExpr and the AssignOp
     * 
     * @param var The variable name to find.
     * @param num The revision of the variable to find.
     * @return A vector containing the usage nodes of the variable. Empty vector otherwise.
     */
    NodeVec getAllUsesForDef(const VarName& var, int num);

    /** Retrieve a list of nodes of type T that use the var:num specified.
     *
     * This will retrieve a list of nodes of type T that use the specified var:num combo.
     * ex.  int i = s2.y1;  //Search for s:y1,AssignOp will yield the AssignOp
     *
     * @param var The variable name to find.
     * @param num The revision of the variable to find.
     * @return A vector containing the usage nodes of the variable. Empty vector otherwise.
     */
    template<typename T> inline std::vector<T*> getAllUsesForDef(const VarName& var, int num)
    {
        NodeVec vec = getAllUsesForDef(var, num);
        std::vector<T*> res;
        T* temp = NULL;

        BOOST_FOREACH(NodeVec::value_type& val, vec)
        {
            temp = dynamic_cast<T*>(val);
            if(temp != NULL)
            {
                //Push back if it casts correctly.
                res.push_back(temp);
            }
        }

        return res;
    }

    /** Get name:num mappings for all reaching definitions of all variables at the node.
     *
     * @param node The node to retrieve reaching definitions for.
     * @return A table mapping VarName->(num, defNode). Empty table otherwise.
     */
    NumNodeRenameTable getReachingDefsAtNode(SgNode* node);

    /** Get name:num mapping for all reaching definitions of the given variable at the node.
     *
     * @param node The node to retrieve reaching definitions for.
     * @param var The variable to retrieve definitions of.
     * @return A table of (num, defNode) for the given variable. Empty table otherwise.
     */
    NumNodeRenameEntry getReachingDefsAtNodeForName(SgNode* node, const VarName& var);

    /** Get the final versions if all variables at the end of the given function.
     *
     * @param node The function to get variables for.
     * @return A table of VarName->(num, defNode) for all variables at the end of the function. Empty table otherwise.
     */
    NumNodeRenameTable getReachingDefsAtFunctionEnd(SgFunctionDefinition* node);

    /** Get the versions of a variable at the end of the given function.
     *
     * @param node The function definition to get definitions for.
     * @param var The varName to get definitions for.
     * @return A table of (num, defNode) for the given variable. Empty table otherwise.
     */
    NumNodeRenameEntry getReachingDefsAtFunctionEndForName(SgFunctionDefinition* node, const VarName& var);

    /** Get name:num mappings for all uses at this node.
     *
     * @param node The node to get uses for.
     * @return A table mapping VarName->(num, defNode) for every varName used at node. Empty table otherwise.
     */
    NumNodeRenameTable getUsesAtNode(SgNode* node);

    /** Get name:num mapping for use of the given variable at this node.
     *
     * @param node The node to get uses for.
     * @param var The varName to get the uses for.
     * @return  A table of (num, defNode) for the given varName used at node. Empty table otherwise.
     */
    NumNodeRenameEntry getUsesAtNodeForName(SgNode* node, const VarName& var);

    /** Get name:num mapping for all defs at the given node.
     *
     * This will return the combination of original and expanded defs on this node.
     *
     * ex. s.x = 5;  //This will return s.x and s  (s.x is original & s is expanded)
     *
     * @param node The node to get defs for.
     * @return A table mapping VarName->(num, defNode) for every varName defined at node. Empty table otherwise.
     */
    NumNodeRenameTable getDefsAtNode(SgNode* node);

    /** Get name:num mapping for def of the specified variable at the given node.
     *
     * This will return the combination of original and expanded defs of the given variable on this node.
     *
     * ex. s.x = 5;   //(s is expanded & s.x is original)
     *     Looking for s will return this node, even though s is an expanded definition.
     *
     * @param node The node to get defs for.
     * @param var The variable to get defs for.
     * @return A table mapping VarName->(num, defNode) for every varName defined at node. Empty table otherwise.
     */
    NumNodeRenameEntry getDefsAtNodeForName(SgNode* node, const VarName& var);

    /** Get name:num mapping for all original defs at the given node.
     *
     * This will return the original defs on this node.
     *
     * ex. s.x = 5;  //This will return s.x  (s.x is original & s is expanded)
     *
     * @param node The node to get defs for.
     * @return A table mapping VarName->(num, defNode) for every varName originally defined at node. Empty table otherwise.
     */
    NumNodeRenameTable getOriginalDefsAtNode(SgNode* node);

    /** Get name:num mapping for def of the specified variable at the given node.
     *
     * This will return the combination of original defs of the given variable on this node.
     *
     * ex. s.x = 5;   //(s is expanded & s.x is original)
     *     Looking for s.x will return this node, s will return empty
     *
     * @param node The node to get defs for.
     * @param var The variable to get defs for.
     * @return A table mapping VarName->(num, defNode) for every varName defined at node. Empty table otherwise.
     */
    NumNodeRenameEntry getOriginalDefsAtNodeForName(SgNode* node, const VarName& var);

    /** Get name:num mapping for all expanded defs at the given node.
     *
     * This will return the expanded defs on this node.
     *
     * ex. s.x = 5;  //This will return s  (s.x is original & s is expanded)
     *
     * @param node The node to get defs for.
     * @return A table mapping VarName->(num, defNode) for every varName defined via expansion at node. Empty table otherwise.
     */
    NumNodeRenameTable getExpandedDefsAtNode(SgNode* node);

    /** Get name:num mapping for def of the specified variable at the given node.
     *
     * This will return the combination of expanded defs of the given variable on this node.
     *
     * ex. s.x = 5;   //(s is expanded & s.x is original)
     *     Looking for s will return this node, s.x will return empty
     *
     * @param node The node to get defs for.
     * @param var The variable to get defs for.
     * @return A table mapping VarName->(num, defNode) for every varName defined at node. Empty table otherwise.
     */
    NumNodeRenameEntry getExpandedDefsAtNodeForName(SgNode* node, const VarName& var);

    /** Get all definitions for the subtree rooted at this node.
     *
     * @param node The root of the subtree to get definitions for.
     * @return The table mapping VarName->(num, node) for every definition.
     */
    NumNodeRenameTable getDefsForSubtree(SgNode* node);

    /** Get an AST fragment containing the appropriate varRefs and Dot/Arrow ops to access the given variable.
     *
     * @param var The variable to construct access for.
     * @param scope The scope within which to construct the access.
     * @return An expression that access the given variable in the given scope.
     */
    SgExpression* buildVariableReference(const varName& var, SgScopeStatement* scope = NULL);



    /*
     *   Static Utility Functions
     */

    /** Find if the given prefix is a prefix of the given name.
     *
     * This will return whether the given name has the given prefix inside it.
     *
     * ex. a.b.c has prefix a.b, but not a.c
     *
     * @param name The name to search.
     * @param prefix The prefix to search for.
     * @return Whether or not the prefix is in this name.
     */
    static bool isPrefixOfName(VarName name, VarName prefix);

    /** Get the uniqueName attribute for the given node.
     *
     * @param node Node to get the attribute from.
     * @return The attribute, or NULL.
     */
    static VarUniqueName* getUniqueName(SgNode* node);

    /** Get the variable name of the given node.
     *
     * @param node The node to get the name for.
     * @return The name, or empty name.
     */
    static VarName getVarName(SgNode* node);

    /** Gets whether or not the initializedName is from a library.
     *
     * This method checks if the variable is compiler generated, and if its
     * filename has "/include/" in it. If so, it will return true. Otherwise, it returns
     * false.
     *
     * @param initName The SgInitializedName* to check.
     * @return true if initName is from a library, false if otherwise.
     */
    static bool isFromLibrary(SgInitializedName* initName);

private:

    /** Attribute that describes the variables modified by a given expression.
     */
    class VarRefSynthAttr
    {
    private:
        /** Stores all of the varRefs from the subtree.
         */
        std::vector<SgNode*> refs;

    public:
        /** Create the attribute with no refs.
         */
        VarRefSynthAttr():refs(){}

        /** Create the attribute with thisNode.
         *
         * @param thisNode The node to add to the list of refs.
         */
        VarRefSynthAttr(SgNode* thisNode)
        {
            refs.push_back(thisNode);
        }

        /** Create the attribute with the subtree and thisNode.
         *
         * @param subtree The list of nodes to copy to this attribute.
         * @param thisNode The node to append to the list of refs.
         */
        VarRefSynthAttr(const std::vector<SgNode*>& subtree, SgNode* thisNode)
        {
            refs.assign(subtree.begin(), subtree.end());
            refs.push_back(thisNode);
        }

        /** Create the attribute with the provided refs.
         *
         * @param subtree The refs to use for this attribute.
         */
        VarRefSynthAttr(const std::vector<SgNode*>& subtree)
        {
            refs.assign(subtree.begin(), subtree.end());
        }

        /** Get the references for this node and below.
         *
         * @return A constant reference to the ref list.
         */
        const std::vector<SgNode*>& getRefs() { return refs; }

        /** Set the references for this node and below.
        *
        * @param newRefs A constant reference to the refs to copy to this node.
        */
        void setRefs(const std::vector<SgNode*>& newRefs) { refs.assign(newRefs.begin(), newRefs.end()); }
    };

    /** Attribute that describes the variables modified by a given expression.
     */
    class VarDefUseSynthAttr
    {
    private:
        /** Stores all of the varRefs that are defined in the current subtree.
         */
        std::vector<SgNode*> defs;

        /** Stores all the varRefs that are used in the current subTree.
         */
        std::vector<SgNode*> uses;

    public:
        /** Create the attribute with no refs.
         */
        VarDefUseSynthAttr():defs(), uses(){}

        /** Create the attribute with specified def/use.
         *
         * @param defNode The node to add to the list of defs, or NULL
         * @param useNode The node to add to the list of uses, or NULL
         */
        VarDefUseSynthAttr(SgNode* defNode, SgNode* useNode)
        {
            if(defNode)
                defs.push_back(defNode);

            if(useNode)
                uses.push_back(useNode);
        }

        /** Create the attribute with the list of defs and the use.
         *
         * @param defTree The vector of defs to add, or an empty vector.
         * @param useNode The node to add to the list of uses, or NULL.
         */
        VarDefUseSynthAttr(const std::vector<SgNode*>& defTree, SgNode* useNode)
        {
            if(defTree.size() > 0)
                defs.assign(defTree.begin(), defTree.end());

            if(useNode)
                uses.push_back(useNode);
        }

        /** Create the attribute with the def and list of uses.
         *
         * @param defNode The node to add to the list of defs, or NULL.
         * @param useTree The vector of uses to add, or an empty vector.
         */
        VarDefUseSynthAttr(SgNode* defNode, const std::vector<SgNode*>& useTree)
        {
            if(useTree.size() > 0)
                uses.assign(useTree.begin(), useTree.end());

            if(defNode)
                defs.push_back(defNode);
        }

        /** Create the attribute with the provided uses and defs.
         *
         * @param defTree The defs to use in this node, or empty vector.
         * @param useTree The uses to use in this node, or empty vector.
         */
        VarDefUseSynthAttr(const std::vector<SgNode*>& defTree, const std::vector<SgNode*>& useTree)
        {

            if(defTree.size() > 0)
                defs.assign(defTree.begin(), defTree.end());

            if(useTree.size() > 0)
                uses.assign(useTree.begin(), useTree.end());
        }

        /** Get the references for this node and below.
         *
         * @return A constant reference to the ref list.
         */
        std::vector<SgNode*>& getDefs() { return defs; }

        /** Set the defs for this node and below.
        *
        * @param newDefs A constant reference to the defs to copy to this node.
        */
        void setDefs(const std::vector<SgNode*>& newDefs) { defs.assign(newDefs.begin(), newDefs.end()); }

         /** Get the uses for this node and below.
         *
         * @return A constant reference to the use list.
         */
        std::vector<SgNode*>& getUses() { return uses; }

        /** Set the uses for this node and below.
        *
        * @param newUses A constant reference to the uses to copy to this node.
        */
        void setUses(const std::vector<SgNode*>& newUses) { uses.assign(newUses.begin(), newUses.end()); }
    };

    class VarDefUseTraversal : public AstBottomUpProcessing<VariableRenaming::VarDefUseSynthAttr>
    {
        VariableRenaming* varRename;
    public:
        VarDefUseTraversal(VariableRenaming* varRenaming):varRename(varRenaming){}

        /** Called to evaluate the synthesized attribute on every node.
         *
         * This function will handle passing all variables that are defined and used by a given operation.
         *
         * @param node The node being evaluated.
         * @param attr The attributes from the child nodes.
         * @return The attribute at this node.
         */
        virtual VariableRenaming::VarDefUseSynthAttr evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs);
    };

    /** Class to traverse the AST and assign unique names to every varRef.
     */
    class UniqueNameTraversal : public AstBottomUpProcessing<VariableRenaming::VarRefSynthAttr>
    {
        VariableRenaming* varRename;
    public:
        UniqueNameTraversal(VariableRenaming* varRenaming):varRename(varRenaming){}

        /** Called to evaluate the synthesized attribute on every node.
         *
         * This function will handle passing all variables that are referenced by a given expression.
         *
         * @param node The node being evaluated.
         * @param attrs The attributes from the child nodes.
         * @return The attribute at this node.
         */
        virtual VariableRenaming::VarRefSynthAttr evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs);
    };
    
};

#endif	/* SSAANALYSIS_H */

