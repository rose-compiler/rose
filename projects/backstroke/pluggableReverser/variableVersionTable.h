#ifndef BACKSTROKE_VARIABLEVERSIONTABLE_H
#define	BACKSTROKE_VARIABLEVERSIONTABLE_H

#include <rose.h>
#include <VariableRenaming.h>
#include <boost/shared_ptr.hpp>

class VariableVersionTable
{
	typedef std::map<VariableRenaming::VarName, std::set<int> > TableType;
	
	TableType table_;
	VariableRenaming* var_renaming_;

	friend bool operator ==(const VariableVersionTable& t1, const VariableVersionTable& t2);

public:

	VariableVersionTable() : var_renaming_(NULL) { }
	VariableVersionTable(SgFunctionDeclaration* func_decl, VariableRenaming* var_renaming);

	// TODO: we may avoid to provide this interface.
	const TableType& getTable() const
	{ return table_; }

	/** Returns the version of the variable, or an empty set if the variable is not in the table. */
	std::set<int> getVersion(VariableRenaming::VarName varName) const;

#if 0
	/** Check if the given variables with their version numbers exist in the current table.
	* Note that we can check one or two expressions in urary and binary operation cases. */
	bool checkLhsVersion(SgNode* node) const;
	bool checkRhsVersion(SgNode* node) const;
	bool checkVersion(SgExpression* lhs, SgExpression* rhs = NULL) const;
#endif

	//! Check if all varibles in the given expression have the same version in the current version table.
	//! This is only for expressions which are USE not DEF. For example, for a = b, only b can use this
	//! check, but a cannot. 
	bool checkVersionForUse(SgExpression* exp) const;

	//! Check if all varibles in the given expression have the same version in the current version table.
	//! This is only for expressions which are DEF not USE. For example, for a = b, only a can use this
	//! check, but b cannot.
	bool checkVersionForDef(SgExpression* exp) const;

	//! This is a combination of two functions above, which checks two expressions for def and use separately.
	//! For example, for a += b, both a and b are checked.
	bool checkVersionForDefUse(SgExpression* lhs_exp, SgExpression* rhs_exp) const
	{ return checkVersionForDef(lhs_exp) && checkVersionForUse(rhs_exp); }

	//! Set the version of the given variable to its last version in its scope. This method is for local
	//! variables.
	void setLastVersion(SgInitializedName* init_name);

	//! Regress the version of the given variable. Call this function once the expression or
	//! statement containing the given variable is reversed successfully. 
	void reverseVersion(SgNode* node);

	/** Remove a variable from the current table. */
	void removeVariable(SgNode* node)
	{ table_.erase(VariableRenaming::getVarName(node)); }


	//! This function gets two variable version tables for true/false bodies in an if statement.
	//! Since currently there is no fi function in implementation, this is a workaround to get the
	//! correct vartable at the end of each body. At the end of if statement, for each variable,
	//! check the def node for its each version. If that version is defined in true body, remove
	//! this version in var table of the false body, and if thie def's enclosing if body is true body,
	//! remove the versions killed by this def in var table of the true body. And vice versa.
	std::pair<VariableVersionTable, VariableVersionTable>
	getVarTablesForIfBodies(SgStatement* true_body, SgStatement* false_body) const;

	//! This function gets the variable version tables for the loop body in an for/while/do-while statement.
	//! Since currently there is no fi function in implementation, this is a workaround to get the
	//! correct vartable at the end of the body.
	VariableVersionTable getVarTablesForLoopBody(SgStatement* loop_body) const;

	/** Intersect this variable version table to another one. For each variable inside, we set its
	* new version which is the common indices from those two tables. */
	void intersect(const VariableVersionTable& var_table);

	/** Set union this variable version table to another one. For each variable inside, we set its
	* new version which is the all indices from those two tables. */
	void setUnion(const VariableVersionTable& var_table);

	/** If a local variable is not restored at the begining of the reverse basic block, set its
	* version to NULL. */
	void setNullVersion(SgInitializedName* name);
	void setNullVersion(SgNode* node);

	/** If the given node is using its first definition. It's useful to decide whether to reverse the value or not. */
	//FIXME I don't like this name!
	bool isUsingFirstDef(SgNode* node) const;

	/** If the given node is using its first use. It's useful to decide whether to remove a variable from variable version table. */
	//FIXME I don't like this name!
	bool isUsingFirstUse(SgNode* node) const;

	/** Returns true if a variable is at the specified version.
	* @param varName name of the variable to look up
	* @param version version that the variable should have (list of possible definitions). */
	bool matchesVersion(VariableRenaming::VarName varName, VariableRenaming::NumNodeRenameEntry version) const;

	void print() const;
};

inline bool operator ==(const VariableVersionTable& t1, const VariableVersionTable& t2)
{
	return t1.table_ == t2.table_;
}

typedef boost::shared_ptr<VariableVersionTable> VariableVersionTablePtr;

#endif	/* BACKSTROKE_VARIABLEVERSIONTABLE_H */

