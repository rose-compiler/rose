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
    VariableVersionTable(SgFunctionDeclaration* func_decl, VariableRenaming* var_renaming);

    //! Once the value of a variable is restored or reversed, set its index to the previous one.
    void backToPreviousIndex(const VarName& var);

    //! Get the unique variable name from a AST node (could be a varref, dot or arrow operator).
    static VarName getVarName(SgNode* node);

    //! Check if the given variable with its version number exists in the current table.
    bool checkVersion(SgNode* node) const;

    /** Regress the version of the given variable. Call this function once the expression or
     * statement containing the given variable is reversed successfully. */
    void reverseVersion(SgNode* node);

    static bool isEmptyVarName(const VarName& var) { return var.empty(); }

    //! Check if the current index of a variable is the same as the given one.
    bool variableHasIndex(const std::vector<int>& index);
    bool variableHasIndex(int index);

    void print() const;

};


#endif	/* BACKSTROKE_VARIABLEVERSIONTABLE_H */

