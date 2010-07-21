#ifndef BACKSTROKE_VARIABLEVERSIONTABLE_H
#define	BACKSTROKE_VARIABLEVERSIONTABLE_H

#include <rose.h>


class VariableRenaming;

class VariableVersionTable
{
    typedef std::vector<SgInitializedName*> VarName;
    
    std::map<VarName, std::set<int> > table_;
    VariableRenaming* var_renaming_;

public:
    VariableVersionTable() : var_renaming_(NULL) {}
    VariableVersionTable(SgFunctionDeclaration* func_decl, VariableRenaming* var_renaming);

//    //! Once the value of a variable is restored or reversed, set its index to the previous one.
//    void backToPreviousIndex(const VarName& var);

    //! Get the unique variable name from a AST node (could be a varref, dot or arrow operator).
    static VarName getVarName(SgNode* node);

    //! Check if the given variable with its version number exists in the current table.
    bool checkVersion(SgNode* node) const;

    /** Regress the version of the given variable. Call this function once the expression or
      statement containing the given variable is reversed successfully. */
    void reverseVersion(SgNode* node);

    /** If a local variable is not restored at the begining of the reverse basic block, set its
      version to NULL. */
    void setNullVersion(SgInitializedName* name);

//    /** Set the version with the same name of the node to the previous version of the given node.
//      This function if mainly for store and restore mechanism. */
//    void setPreviousVersion(SgNode* node);

    static bool isEmptyVarName(const VarName& var) { return var.empty(); }

    //! Check if the current index of a variable is the same as the given one.
    bool variableHasIndex(const std::vector<int>& index);
    bool variableHasIndex(int index);

    void print() const;
    
    //! Print a VarName object.
    static void print(const VarName& name);
};


#endif	/* BACKSTROKE_VARIABLEVERSIONTABLE_H */

