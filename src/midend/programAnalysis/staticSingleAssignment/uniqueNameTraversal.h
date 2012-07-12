#pragma once
#include <rose.h>
#include <vector>

namespace ssa_private
{

/** Class holding a unique name for a variable. Is attached to varRefs as a persistant attribute.
 * This is used to assign absolute names to VarRefExp nodes during VariableRenaming.
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
    VarUniqueName() : key(), usesThis(false)
    {
    }

    /** Constructs the attribute with value thisNode.
     *
     * The key will consist of only the current node.
     *
     * @param thisNode The node to use for the key.
     */
    VarUniqueName(SgInitializedName* thisNode) : usesThis(false)
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
    VarUniqueName(const std::vector<SgInitializedName*>& prefix, SgInitializedName* thisNode) : usesThis(false)
    {
        key.assign(prefix.begin(), prefix.end());
        key.push_back(thisNode);
    }

    /** Copy the attribute.
     *
     * @param other The attribute to copy from.
     */
    VarUniqueName(const VarUniqueName& other) : AstAttribute(other), usesThis(false)
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
    const std::vector<SgInitializedName*>& getKey()
    {
        return key;
    }

    /** Set the value of the name.
     *
     * @param newKey The new name to use.
     */
    void setKey(const std::vector<SgInitializedName*>& newKey)
    {
        key.assign(newKey.begin(), newKey.end());
    }

    bool getUsesThis()
    {
        return usesThis;
    }

    void setUsesThis(bool uses)
    {
        usesThis = uses;
    }

    /** Get the string representing this uniqueName
     *
     * @return The name string.
     */
    std::string getNameString()
    {
        std::string name = "";
        std::vector<SgInitializedName*>::iterator iter;
        if (usesThis)
            name += "this->";
        for (iter = key.begin(); iter != key.end(); ++iter)
        {
            if (iter != key.begin())
            {
                name += ":";
            }
            name += (*iter)->get_name().getString();
        }

        return name;
    }
};

/** Attribute that describes the variables modified by a given expression.  */
class VariableReferenceSet
{
    //! The current variable reference expression. It can be any valid node to which a var name is attached,
    //! such as SgVarRefExp, SgInitializedName, or SgDotExp. 
    //! It is NULL if the AST node currently traversed does not denote a variable
    SgNode* currentVar;

public:

    //! Default constructor

    VariableReferenceSet() : currentVar(NULL)
    {
    }

    VariableReferenceSet(SgNode* var)
    {
        currentVar = var;
    }

    SgNode* getCurrentVar()
    {
        return currentVar;
    }
};

/** Class to traverse the AST and assign unique names to every varRef. */
class UniqueNameTraversal : public AstBottomUpProcessing<VariableReferenceSet>
{
    /** All the initialized names in the project. */
    std::vector<SgInitializedName*> allInitNames;

    /** Finds initialized names that are "fake" (refer to p_prev_decl_item in the SgInitializedName docs)
     * and replaces them with the true declaration. */
    SgInitializedName* resolveTemporaryInitNames(SgInitializedName* name);

    //! If true, expressions such as p->x will be chained together in a variable name p.x
    //! Turning on this option reduces safety of analyses. 
    const bool treatPointersAsStructs;

    //! If true, expressions such as (a, b).x will be named as b.x. This allows detecting more defs,
    //! but if the left-hand side of a comma has modifying side effects it may reduce the safety of an analysis.
    const bool propagateNamesThroughComma;

public:

    /** Tag to use to retrieve unique naming key from node.  */
    static std::string varKeyTag;

    /** A compound variable name as used by the variable renaming.  */
    typedef std::vector<SgInitializedName*> VarName;

    //! @param treatPointersAsStructs If true, expressions such as p->x will be chained together in a variable name p.x
    //! @param propagateNamesThroughComma If true, expressions such as (a, b).x will be named as b.x.

    UniqueNameTraversal(const std::vector<SgInitializedName*>& allNames,
            bool treatPointersAsStructs = true, bool propagateNamesThroughComma = true) : allInitNames(allNames),
    treatPointersAsStructs(treatPointersAsStructs), propagateNamesThroughComma(propagateNamesThroughComma)
    {
    }

    /** Called to evaluate the synthesized attribute on every node.
     *
     * This function will handle passing all variables that are referenced by a given expression.
     *
     * @param node The node being evaluated.
     * @param attrs The attributes from the child nodes.
     * @return The attribute at this node.
     */
    virtual VariableReferenceSet evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs);
};

} //namespace ssa_private
