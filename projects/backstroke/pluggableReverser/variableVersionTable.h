#ifndef BACKSTROKE_VARIABLEVERSIONTABLE_H
#define	BACKSTROKE_VARIABLEVERSIONTABLE_H

#include <rose.h>
#include <VariableRenaming.h>
#include <boost/shared_ptr.hpp>

class VariableVersionTable
{
	std::map<VariableRenaming::VarName, std::set<int> > table_;
	VariableRenaming* var_renaming_;

	friend bool operator ==(const VariableVersionTable& t1, const VariableVersionTable& t2);

public:

	VariableVersionTable() : var_renaming_(NULL) { }
	VariableVersionTable(SgFunctionDeclaration* func_decl, VariableRenaming* var_renaming);

	/** Returns the version of the variable, or an empty set if the variable is not in the table. */
	std::set<int> getVersion(VariableRenaming::VarName varName) const;

	/*! Check if the given variables with their version numbers exist in the current table.
	 * Note that we can check one or two expressions in urary and binary operation cases. */
	bool checkLhsVersion(SgNode* node) const;
	bool checkRhsVersion(SgNode* node) const;
	bool checkVersion(SgExpression* lhs, SgExpression* rhs = NULL) const;

	// Check if all varibles in the given expression have the same version in the current version table.
    // This is only for expressions which are USE not DEF. For example, for a = b, only b can use this
    // check, but a cannot.
    bool checkVersionForUse(SgExpression* exp) const;

	/** Regress the version of the given variable. Call this function once the expression or
	  statement containing the given variable is reversed successfully. */
	void reverseVersion(SgNode* node);

	/** Remove a variable from the current table. */
	void removeVariable(SgNode* node)
	{
		table_.erase(VariableRenaming::getVarName(node));
	}

	/** Intersect this variable version table to another one. For each variable inside, we set its
     new version which is the common indices from those two tables. */
    void intersect(const VariableVersionTable& var_table);

	/** If a local variable is not restored at the begining of the reverse basic block, set its
	  version to NULL. */
	void setNullVersion(SgInitializedName* name);
	void setNullVersion(SgNode* node);

	/** If the given node is using its first definition. It's useful to decide whether to reverse the value or not. */
	//FIXME I don't like this name!
	bool isUsingFirstDefinition(SgNode* node) const;

	/** If the given node is using its first use. It's useful to decide whether to remove a variable from variable version table. */
	//FIXME I don't like this name!
	bool isUsingFirstUse(SgNode* node) const;

	/** Returns true if a variable is at the specified version.
	 * @param varName name of the variable to look up
	 * @param version version that the variable should have (list of possible definitions). */
	bool matchesVersion(VariableRenaming::VarName varName, VariableRenaming::NumNodeRenameEntry version) const;

	static std::vector<SgExpression*> getAllVariables(SgNode* node);

	void print() const;
};

inline bool operator ==(const VariableVersionTable& t1, const VariableVersionTable& t2)
{
	return t1.table_ == t2.table_;
}

typedef boost::shared_ptr<VariableVersionTable> VariableVersionTablePtr;

#endif	/* BACKSTROKE_VARIABLEVERSIONTABLE_H */

