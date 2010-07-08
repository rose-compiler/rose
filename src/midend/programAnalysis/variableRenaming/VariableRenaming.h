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
#include "DefUseAnalysis.h"
#include <map>
#include <vector>
#include <algorithm>
#include <ostream>
#include <fstream>
#include <sstream>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

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
      SgNode *n = cfgn.getNode();
       //Remove all non-interesting nodes
      if (!cfgn.isInteresting() && !(isSgFunctionCallExp(cfgn.getNode()) && cfgn.getIndex() >= 2))
        return false;
      //Remove all non-beginning nodes for initNames
      if (isSgInitializedName(n) && cfgn.getIndex()>0)
        return false;
       return true;
    }
};

/** Class that defines an VariableRenaming of a program
 *
 * Contains all the functionality to implement SSA on a given program.
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
    //Type definitions for the various tables.
    typedef std::vector<SgNode*> nodeVec;
    typedef std::vector<SgInitializedName*> varName;
    typedef std::map<varName, nodeVec> tableEntry;
    typedef std::map<SgNode*, tableEntry> defUseTable;
    typedef std::vector<varName> globalTable;
    typedef std::vector<SgInitializedName*> initNameVec;
    typedef FilteredCFGNode<IsDefUseFilter> cfgNode;
    typedef FilteredCFGEdge<IsDefUseFilter> cfgEdge;
    typedef std::vector<cfgNode> cfgNodeVec;
    typedef std::vector<cfgEdge> cfgEdgeVec;
    typedef std::map<SgNode*, int> nodeNumRenameEntry;
    typedef std::map<varName, nodeNumRenameEntry> nodeNumRenameTable;
    typedef std::map<int, SgNode*> numNodeRenameEntry;
    typedef std::map<varName, numNodeRenameEntry> numNodeRenameTable;


private:
    //Private member variables

    /** This is the table of variable definition locations that is generated by
     * the VarDefUseTraversal. It is later used to populate the actual def/use table.
     */
    defUseTable originalDefTable;

    /** This is the table that is populated with all the def information for all the variables
     * at all the nodes. It is populated during the runDefUse function, and is done
     * with the steady-state dataflow algorithm.
     */
    defUseTable defTable;

    /** This is the table that is populated with all the use information for all the variables
     * at all the nodes. It is populated during the runDefUse function, and is done
     * with the steady-state dataflow algorithm.
     */
    defUseTable useTable;

    /** This holds the mapping between variables and the nodes where they are renumbered.
     * Given a name and a node, we can get the number of the name that is defined at that node.
     * Nodes which do not define a name are not in the table. 
     */
    nodeNumRenameTable nodeRenameTable;

    /** This holds the mapping between variables and the nodes where they are renumbered.
     * Given a name and a number, we can get the node where that number is defined.
     * Nodes which do not define a name are not in the table.
     */
    numNodeRenameTable numRenameTable;

    /** A list of all the global varibales in the program.
     */
    globalTable globalVarList;

public:
    VariableRenaming(SgProject* proj): project(proj), DEBUG_MODE(true), DEBUG_MODE_EXTRA(true){}

    ~VariableRenaming(){}
    
    void run();

    bool getDebug() { return DEBUG_MODE; }
    bool getDebugExtra() { return DEBUG_MODE_EXTRA; }

    /** Get the table of definitions for every node.
     *
     * @return Definition table.
     */
    defUseTable& getDefTable() { return originalDefTable; }

    /**
     * 
     * @return
     */
    defUseTable& getPropDefTable() { return defTable; }
    defUseTable& getUseTable() { return useTable; }
    globalTable& getGlobalVarList() { return globalVarList; }

    //Manipulation functions for the rename table.
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
    int getRenameNumberForNode(const varName& var, SgNode* node);

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
    SgNode* getNodeForRenameNumber(const varName& var, int num);

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
    int addRenameNumberForNode(const varName& var, SgNode* node);

private:
    void runDefUse(SgFunctionDefinition* func);
    bool defUse(cfgNode node, bool *copied);

    /** Locate all global varibales and add them to the table.
     */
    void findGlobalVars();

    bool mergeDefs(cfgNode curNode);
    bool resolveUses(cfgNode curNode);

    /** Trace backwards in the cfg one step and return an aggregate of all previous defs.
     *
     * @param curNode Node to traverse backwards from.
     * @param results TableEntry reference where results are stored.
     */
    void aggregatePreviousDefs(cfgNode curNode, tableEntry& results);

    void printToDOT(SgSourceFile* file, std::ofstream &outFile);
    void printToFilteredDOT(SgSourceFile* file, std::ofstream &outFile);
    
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

    std::string keyToString(varName vec);
    
    void printDefs(SgNode* node);

    void printDefs(tableEntry& table);

    void printUses(SgNode* node);

    void printUses(tableEntry& table);

    void printRenameTable();

    VarUniqueName* getUniqueName(SgNode* node);

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

    /** Create a renaming object and attribute with the given information, and attach it to target.
     *
     * This is used to attach rename information to a variable use. This will create
     * a rename attribute using the information in the renameTable. For example:
     * 1: int x;
     * 2: x = 5;
     * 3: int y = x;
     * To rename the x in line 3, def = x in line 2, decl = x in line 1, target = x in line 3.
     * Transformed:
     * 1: int x1;
     * 2: x2 = 5;
     * 3: int y1 = x2;
     *
     * This function will not change the renameTable.
     *
     * @param func The function that contains the target.
     * @param def The node where this variable was last defined (through def/use).
     * @param decl The node where this varible was declared.
     * @param target The SgNode to attach the attribute to.
     */
    void attachVarUse(SgFunctionDefinition* func, SgNode* def, SgInitializedName* decl, SgNode* target);

    /** Create a renaming for a variable declaration and attach it to that declaration.
     *
     * This creates a renaming for the initial declaration of a variable and
     * inserts it into the rename table and attaches it to the SgNode.
     *
     * This will insert this declaration into the renameTable, and will generate
     * a ROSE_ASSERT if the declaration is not the first one in the table.
     *
     * @param func The function that contains the definition.
     * @param dec The node where the variable is declared.
     */
    void attachVarDec(SgFunctionDefinition* func, SgInitializedName* decl);

    /** Create a renaming for a variable definition and attach it to that definition.
     *
     * This is used to attach rename information to a variable definition. This will create
     * a rename attribute using the information in the renameTable. For example:
     * 1: int x;
     * 2: x = 5;
     * To rename the x in line 2, def = x in line 2, decl = x in line 1.
     * Transformed:
     * 1: int x1;
     * 2: x2 = 5;
     *
     * The renameTable is updated with the new definition.
     *
     * @param func The function that contains the definition.
     * @param def The node where the variable is redefined.
     * @param decl The node where the variable is initialy declared.
     */
    void attachVarDef(SgFunctionDefinition* func, SgNode* def, SgInitializedName* decl);

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
    public:
        UniqueNameTraversal(){}

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

