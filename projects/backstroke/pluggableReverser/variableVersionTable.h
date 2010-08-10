#ifndef BACKSTROKE_VARIABLEVERSIONTABLE_H
#define	BACKSTROKE_VARIABLEVERSIONTABLE_H

#include <rose.h>
#include <VariableRenaming.h>

class VariableVersionTable
{
    typedef VariableRenaming::VarName VarName;
    
    std::map<VarName, std::set<int> > table_;
    VariableRenaming* var_renaming_;

    friend bool operator ==(const VariableVersionTable& t1, const VariableVersionTable& t2);

public:
    VariableVersionTable() : var_renaming_(NULL) {}
    VariableVersionTable(SgFunctionDeclaration* func_decl, VariableRenaming* var_renaming);

//    //! Once the value of a variable is restored or reversed, set its index to the previous one.
//    void backToPreviousIndex(const VarName& var);

    //! Get the unique variable name from a AST node (could be a varref, dot or arrow operator).
    static VarName getVarName(SgNode* node);

    /*! Check if the given variables with their version numbers exist in the current table.
     * Note that we can check one or two expressions in urary and binary operation cases. */
    bool checkLhsVersion(SgNode* node) const;
    bool checkRhsVersion(SgNode* node) const;
    bool checkVersion(SgExpression* lhs, SgExpression* rhs = NULL) const;

    /** Regress the version of the given variable. Call this function once the expression or
      statement containing the given variable is reversed successfully. */
    void reverseVersion(SgNode* node);

    /** Remove a variable from the current table. */
    void removeVariable(SgNode* node)
    { 
        ROSE_ASSERT(table_.count(getVarName(node)) > 0);
        table_.erase(getVarName(node)); 
    }

    /** If a local variable is not restored at the begining of the reverse basic block, set its
      version to NULL. */
    void setNullVersion(SgInitializedName* name);
    void setNullVersion(SgNode* node);

    /** If the given node is using its first definition. It's useful to decide whether to reverse the value or not. */
    //FIXME I don't like this name!
    bool isUsingFirstDefinition(SgNode* node) const;

//    /** Set the version with the same name of the node to the previous version of the given node.
//      This function if mainly for store and restore mechanism. */
//    void setPreviousVersion(SgNode* node);

    static bool isEmptyVarName(const VarName& var) { return var.empty(); }

    //! Check if the current index of a variable is the same as the given one.
    bool variableHasIndex(const std::vector<int>& index) const;
    bool variableHasIndex(int index) const;

    void print() const;
    
    //! Print a VarName object.
    static void print(const VarName& name);
};

inline bool operator ==(const VariableVersionTable& t1, const VariableVersionTable& t2)
{
    return t1.table_ == t2.table_;
}


#endif	/* BACKSTROKE_VARIABLEVERSIONTABLE_H */

