#include "variableVersionTable.h"
#include <VariableRenaming.h>
#include <utilities/Utilities.h>
#include <utilities/CPPDefinesAndNamespaces.h>

using namespace backstroke_util;

VariableVersionTable::VariableVersionTable(SgFunctionDeclaration* func_decl, VariableRenaming* var_renaming)
: var_renaming_(var_renaming)
{
	SgFunctionDefinition* func_def = isSgFunctionDeclaration(func_decl->get_definingDeclaration())->get_definition();
	VariableRenaming::NumNodeRenameTable num_node_table = var_renaming_->getReachingDefsAtFunctionEnd(func_def);

	foreach(VariableRenaming::NumNodeRenameTable::value_type name_to_num, num_node_table)
	{

		foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, name_to_num.second)
		{
			table_[name_to_num.first].insert(num_to_node.first);
		}
	}
}

/** Returns the version of the variable, or an empty set if the variable is not in the table. */
set<int> VariableVersionTable::getVersion(VariableRenaming::VarName varName) const
{
	if (table_.count(varName) == 0)
	{
		set<int> result;
		return result;
	}

	return table_.at(varName);
}

void VariableVersionTable::print() const
{
	typedef pair<VariableRenaming::VarName, std::set<int> > VarIdxPair;

	foreach(VarIdxPair var_idx, table_)
	{
		foreach(SgInitializedName* name, var_idx.first)
		cout << name->get_name().str() << ' ';
		foreach(int i, var_idx.second)
		cout << i << ' ';
		cout << endl;
	}
}

/** This function get all variables in an AST node. Note that for a variable a.b or a->b,
	only a.b or a->b is returned, not a or b. */
vector<SgExpression*> VariableVersionTable::getAllVariables(SgNode* node)
{
	vector<SgExpression*> vars;

	vector<SgExpression*> exps = querySubTree<SgExpression > (node);

	//ROSE_ASSERT(!exps.empty());

	foreach(SgExpression* exp, exps)
	{
		SgExpression* cand = NULL;
		if (isSgVarRefExp(exp))
			cand = exp;
		else if (isSgDotExp(exp) && isSgVarRefExp(isSgDotExp(exp)->get_rhs_operand()))
			cand = exp;
		else if (isSgArrowExp(exp) && isSgVarRefExp(isSgArrowExp(exp)->get_rhs_operand()))
			cand = exp;

		if (cand != NULL &&
				isSgDotExp(cand->get_parent()) == NULL &&
				isSgArrowExp(cand->get_parent()) == NULL)
		{
			vars.push_back(cand);
		}
	}

	return vars;
}

bool VariableVersionTable::checkVersion(SgExpression* lhs, SgExpression* rhs) const
{
	// In this function, for expression a.b or a->b ,we only care about the version of a.b or a->b,
	// but not the version of a or b.

	// First, get all variables in lhs expression.
	// Note that a expression containing several variables can be passed in.
	vector<SgExpression*> lhs_vars = getAllVariables(lhs);

	// Currently, the lhs operand of an assignment should contain only one variable.
	// For array case, like a[i] = 0, we cannot handle it now.
	ROSE_ASSERT(lhs_vars.size() == 1);

	SgExpression* lhs_var = lhs_vars[0];

	//foreach (SgExpression* var, lhs_vars)
	{
		VariableRenaming::VarName name = VariableRenaming::getVarName(lhs_var);
		VariableRenaming::NumNodeRenameEntry defs = var_renaming_->getDefsAtNodeForName(lhs_var->get_parent(), name);

		ROSE_ASSERT(!defs.empty());
		ROSE_ASSERT(table_.count(name) > 0);

		foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, defs)
		{
			int num = num_to_node.first;
			if (table_.find(name)->second.count(num) == 0)
				return false;
		}
		//int num = var_renaming_->getRenameNumberForNode(name, var);
	}

	// For unary operation ++ and --, rhs is NULL and we can just return true at this point.
	if (rhs == NULL)
		return true;

	vector<SgExpression*> rhs_vars = getAllVariables(rhs);

	foreach(SgExpression* var, rhs_vars)
	{
		// Here we check if the variable is the same as the lhs variable. If true, we don't have
		// to check its version. For example,
		//        a(2) = a(1) + b(1)
		// where the versions of a in lhs and rhs operand are different.
		if (backstroke_util::areSameVariable(lhs_var, var))
			continue;

		VariableRenaming::VarName name = VariableRenaming::getVarName(var);
		VariableRenaming::NumNodeRenameEntry defs = var_renaming_->getReachingDefsAtNodeForName(var, name);

		ROSE_ASSERT(!defs.empty());
		ROSE_ASSERT(table_.count(name) > 0);

		foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, defs)
		{
			int num = num_to_node.first;
			if (table_.find(name)->second.count(num) == 0)
				return false;
		}
		//int num = var_renaming_->getRenameNumberForNode(name, var);
	}
	return true;
}

bool VariableVersionTable::checkRhsVersion(SgNode* node) const
{
	// In this function, for expression a.b or a->b ,we only care about the version of a.b or a->b,
	// but not the version of a or b.

	// First, get all variables in the given AST node.
	// Note that a expression contains several variables can be passed in.
	vector<SgExpression*> vars = getAllVariables(node);

	//ROSE_ASSERT(!vars.empty());

	foreach(SgExpression* var, vars)
	{
		VariableRenaming::VarName name = VariableRenaming::getVarName(var);
		VariableRenaming::NumNodeRenameEntry defs = var_renaming_->getReachingDefsAtNodeForName(var, name);

		ROSE_ASSERT(!defs.empty());

		foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, defs)
		{
			int num = num_to_node.first;
			if (table_.find(name)->second.count(num) == 0)
				return false;
		}
		//int num = var_renaming_->getRenameNumberForNode(name, var);
	}
	return true;
}

bool VariableVersionTable::checkLhsVersion(SgNode* node) const
{
	// In this function, for expression a.b or a->b ,we only care about the version of a.b or a->b,
	// but not the version of a or b.

	// First, get all variables in the given AST node.
	// Note that a expression contains several variables can be passed in.
	vector<SgExpression*> vars = getAllVariables(node);

	//ROSE_ASSERT(!vars.empty());

	foreach(SgExpression* var, vars)
	{
		VariableRenaming::VarName name = VariableRenaming::getVarName(var);
		VariableRenaming::NumNodeRenameEntry defs = var_renaming_->getDefsAtNodeForName(var->get_parent(), name);

		ROSE_ASSERT(!defs.empty());

		foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, defs)
		{
			int num = num_to_node.first;
			if (table_.find(name)->second.count(num) == 0)
				return false;
		}
		//int num = var_renaming_->getRenameNumberForNode(name, var);
	}
	return true;
}

void VariableVersionTable::reverseVersion(SgNode* node)
{
	// Note only the original variable (not the expanded one) is reversed here.
	VariableRenaming::VarName name = VariableRenaming::getVarName(node);
	ROSE_ASSERT(name != VariableRenaming::emptyName);
	VariableRenaming::NumNodeRenameEntry num_table = var_renaming_->getReachingDefsAtNodeForName(node, name);

	table_[name].clear();

	foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, num_table)
	{
		table_[name].insert(num_to_node.first);
	}
}

void VariableVersionTable::setNullVersion(SgInitializedName* name)
{
	VariableRenaming::VarName var_name(1, name);
	ROSE_ASSERT(table_.find(var_name) != table_.end());
	table_[var_name].clear();
}

void VariableVersionTable::setNullVersion(SgNode* node)
{
	VariableRenaming::VarName varName = VariableRenaming::getVarName(node);
	ROSE_ASSERT(varName != VariableRenaming::emptyName);
	ROSE_ASSERT(table_.find(varName) != table_.end());
	table_[varName].clear();
}

bool VariableVersionTable::isUsingFirstDefinition(SgNode* node) const
{
	VariableRenaming::VarName varName = VariableRenaming::getVarName(node);
	ROSE_ASSERT(varName != VariableRenaming::emptyName);
	VariableRenaming::NumNodeRenameEntry num_table = var_renaming_->getReachingDefsAtNodeForName(node, varName);

	// The first definition has the number 1
	// This is also true for branch case.
	if (num_table.size() == 1 && num_table.count(1) > 0)
		return true;
	return false;
}

bool VariableVersionTable::isUsingFirstUse(SgNode* node) const
{
	VariableRenaming::VarName varName = VariableRenaming::getVarName(node);
	ROSE_ASSERT(varName != VariableRenaming::emptyName);
	VariableRenaming::NumNodeRenameEntry num_table = var_renaming_->getUsesAtNodeForName(node, varName);

	// The first definition has the number 1
	// FIXME This may not be true for branch case!
	if (num_table.size() == 1 && num_table.count(1) > 0)
		return isUsingFirstDefinition(node);
	return false;
}

/** Returns true if a variable is at the specified version.
 * @param varName name of the variable to look up
 * @param version version that the variable should have (list of possible definitions). */
bool VariableVersionTable::matchesVersion(VariableRenaming::VarName varName, VariableRenaming::NumNodeRenameEntry version) const
{
	//If the variable is not in the table, the versions can't match.
	if (table_.count(varName) == 0)
	{
		return false;
	}

	set<int> indices;
	pair<int, SgNode*> versionDefPair;

	foreach(versionDefPair, version)
	{
		indices.insert(versionDefPair.first);
	}

	printf("%s:\nIndices found: ", VariableRenaming::keyToString(varName).c_str());

	foreach(int v, table_.at(varName))
	{
		printf("%d ", v);
	}
	printf("\n");

	printf("Indices required: ");

	foreach(int v, indices)
	{
		printf("%d ", v);
	}
	printf("\n");

	bool result = table_.at(varName) == indices;
	printf("Result is %s\n\n", result ? "true" : "false");

	return table_.at(varName) == indices;
}

