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
			// If the variable is a local variable of the given function (except parameters), we will set its
			// version to NULL.
			if (SageInterface::isAncestor(
					backstroke_util::getFunctionBody(func_decl),
					name_to_num.first[0]->get_declaration()))
			{
				//cout << VariableRenaming::keyToString(name_to_num.first) << endl;
				table_[name_to_num.first];
			}
			else
				table_[name_to_num.first].insert(num_to_node.first);
		}
	}
}

/** Returns the version of the variable, or an empty set if the variable is not in the table. */
set<int> VariableVersionTable::getVersion(VarName varName) const
{
	if (table_.count(varName) == 0)
	{
		set<int> result;
		return result;
	}

	return table_.find(varName)->second;
}

void VariableVersionTable::print() const
{
	foreach(const TableType::value_type& var_idx, table_)
	{
		cout << VariableRenaming::keyToString(var_idx.first) << " : ";
		foreach(int i, var_idx.second)
			cout << i << ' ';
		cout << endl;
	}
}


bool VariableVersionTable::checkVersionForUse(SgExpression* exp) const
{
	// First, get all variables in the given expression.
	vector<SgExpression*> vars = getAllVariables(exp);

	foreach(SgExpression* var, vars)
	{
		// When checking a USE var's version, we search its reaching def's version.
		VarName name = getVarName(var);
		VariableRenaming::NumNodeRenameEntry defs = var_renaming_->getReachingDefsAtNodeForName(var, name);

		ROSE_ASSERT(!defs.empty());
		ROSE_ASSERT(table_.count(name) > 0);

		set<int> reaching_def_ver;

		// Get the version of its reaching def.
		foreach(VariableRenaming::NumNodeRenameEntry::value_type& num_to_node, defs)
			reaching_def_ver.insert(num_to_node.first);

		// Compare the current version of this variable and its reaching def's version.
		if (table_.find(name)->second != reaching_def_ver)
			return false;
	}
	return true;
}

bool VariableVersionTable::checkVersionForDef(SgExpression* exp) const
{
	// First, get all variables in the given expression.
	vector<SgExpression*> vars = getAllVariables(exp);

	foreach(SgExpression* var, vars)
	{
		// When checking a USE var's version, we search its reaching def's version.
		VarName name = getVarName(var);
		VariableRenaming::NumNodeRenameEntry defs = var_renaming_->getDefsAtNodeForName(var->get_parent(), name);

		ROSE_ASSERT(!defs.empty());
		ROSE_ASSERT(table_.count(name) > 0);

		set<int> reaching_def_ver;

		// Get the version of its reaching def.
		foreach(VariableRenaming::NumNodeRenameEntry::value_type& num_to_node, defs)
			reaching_def_ver.insert(num_to_node.first);

		// Compare the current version of this variable and its reaching def's version.
		if (table_.find(name)->second != reaching_def_ver)
			return false;
	}
	return true;
}

#if 0

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
		VarName name = getVarName(lhs_var);
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

		VarName name = getVarName(var);
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
		VarName name = getVarName(var);
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
		VarName name = getVarName(var);
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

#endif

void VariableVersionTable::setLastVersion(SgInitializedName* init_name)
{
	//FIXME: This does not set the versions of all the expanded variables
	VarName name;
	name.push_back(init_name);
	SgFunctionDefinition* enclosing_func = SageInterface::getEnclosingFunctionDefinition(init_name->get_declaration());
	VariableRenaming::NumNodeRenameEntry num_table = var_renaming_->getReachingDefsAtFunctionEndForName(enclosing_func, name);

	table_[name].clear();
	foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, num_table)
	{
		table_[name].insert(num_to_node.first);
	}
}

void VariableVersionTable::reverseVersion(SgNode* node)
{
	// Note all expanded nodes are reversed here. For example, for m->a, both m->a
	// and m are reversed.
	VarName name = getVarName(node);
	ROSE_ASSERT(name != VariableRenaming::emptyName);

	while (!name.empty())
	{
		ROSE_ASSERT(table_.count(name) > 0);
		ROSE_ASSERT(name != VariableRenaming::emptyName);
		VariableRenaming::NumNodeRenameEntry num_table = var_renaming_->getReachingDefsAtNodeForName(node, name);

		table_[name].clear();

		foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, num_table)
		{
			table_[name].insert(num_to_node.first);
		}

		name.pop_back();
	}
}

void VariableVersionTable::reverseVersionAtStatementStart(const std::vector<SgExpression*>& vars, SgStatement* stmt)
{
	VariableRenaming::NumNodeRenameTable var_versions = var_renaming_->getReachingDefsAtStatementStart(stmt);

	foreach (SgExpression* var, vars)
	{
		VarName name = getVarName(var);

		cout << VariableRenaming::keyToString(name) << endl;

		ROSE_ASSERT(name != VariableRenaming::emptyName);
		ROSE_ASSERT(var_versions.count(name) > 0);
		ROSE_ASSERT(table_.count(name) > 0);

		set<int> new_version;
		foreach (const VariableRenaming::NumNodeRenameEntry::value_type& num_node, var_versions[name])
			new_version.insert(num_node.first);
		table_[name].swap(new_version);
	}
}

void VariableVersionTable::setNullVersion(SgInitializedName* name)
{
	VarName var_name(1, name);
	ROSE_ASSERT(table_.find(var_name) != table_.end());
	table_[var_name].clear();
}

void VariableVersionTable::setNullVersion(SgNode* node)
{
	VarName varName = getVarName(node);
	ROSE_ASSERT(varName != VariableRenaming::emptyName);
	ROSE_ASSERT(table_.find(varName) != table_.end());
	table_[varName].clear();
}

/** This function gets two variable version tables for true/false bodies in an if statement.
 * Since currently there is no fi function in implementation, this is a workaround to get the
 * correct vartable at the end of each body. At the end of if statement, for each variable,
 * check the def node for its each version. If that version is defined in true body, remove
 * this version in var table of the false body, and if thie def's enclosing if body is true body,
 * remove other versions in var table of the true body. And vice versa.  */
std::pair<VariableVersionTable, VariableVersionTable>
VariableVersionTable::getVarTablesForIfBodies(SgBasicBlock* true_body, SgBasicBlock* false_body) const
{
	VariableVersionTable true_body_var_table = *this;
	VariableVersionTable false_body_var_table = *this;

	VariableRenaming::NumNodeRenameTable true_body_defs = var_renaming_->getReachingDefsAtScopeEnd(true_body);
	VariableRenaming::NumNodeRenameTable false_body_defs = var_renaming_->getReachingDefsAtScopeEnd(false_body);

#if 0
	cout << "Current VVT:\n";
	print();
#endif

	foreach (const TableType::value_type& var_version, table_)
	{
		const VarName& var_name = var_version.first;

		// Get the correct versions for var_name for true body.
		if (true_body_defs.count(var_name) > 0)
		{
			set<int> true_body_versions;
			// Get version for var_name from reaching defs at scope end.
			foreach (const VariableRenaming::NumNodeRenameTable::mapped_type::value_type& num_node,
				true_body_defs[var_name])
				true_body_versions.insert(num_node.first);
		
			set<int> new_versions;
			std::set_intersection(true_body_var_table.table_[var_name].begin(),
					true_body_var_table.table_[var_name].end(),
					true_body_versions.begin(), true_body_versions.end(),
					inserter(new_versions, new_versions.begin()));
			true_body_var_table.table_[var_name].swap(new_versions);
		}
		else
		{
			//true_body_var_table.table_.erase(var_name);
			true_body_var_table.table_[var_name].clear();
		}

		// Get the correct versions for var_name for false body.
		if (false_body_defs.count(var_name) > 0)
		{
			set<int> false_body_versions;
			// Get version for var_name from reaching defs at scope end.
			foreach (const VariableRenaming::NumNodeRenameTable::mapped_type::value_type& num_node,
				false_body_defs[var_name])
				false_body_versions.insert(num_node.first);

			set<int> new_versions;
			std::set_intersection(false_body_var_table.table_[var_name].begin(),
					false_body_var_table.table_[var_name].end(),
					false_body_versions.begin(), false_body_versions.end(),
					inserter(new_versions, new_versions.begin()));
			false_body_var_table.table_[var_name].swap(new_versions);
		}
		else
		{
			//false_body_var_table.table_.erase(var_name);
			false_body_var_table.table_[var_name].clear();
		}

#if 0


		foreach (int version, var_version.second)
		{
			SgNode* def_node = var_renaming_->getNodeForRenameNumber(var_version.first, version);
			SgStatement* if_body = backstroke_util::getEnclosingIfBody(def_node);
			if (if_body == true_body)
			{
				VariableRenaming::NumNodeRenameEntry num_node_entry =
						var_renaming_->getReachingDefsAtNodeForName(true_body, var_name);
				foreach (const VariableRenaming::NumNodeRenameEntry::value_type& num_node, num_node_entry)
				{
					true_body_var_table.table_[var_name].erase(num_node.first);
				}

				//true_body_var_table.table_[var_version.first].clear();
				//true_body_var_table.table_[var_version.first].insert(version);
			}
			else if (if_body == false_body)
			{
				VariableRenaming::NumNodeRenameEntry num_node_entry =
						var_renaming_->getReachingDefsAtNodeForName(false_body, var_name);
				foreach (const VariableRenaming::NumNodeRenameEntry::value_type& num_node, num_node_entry)
				{
					false_body_var_table.table_[var_name].erase(num_node.first);
				}
				//false_body_var_table.table_[var_version.first].clear();
				//false_body_var_table.table_[var_version.first].insert(version);
			}

			if (SageInterface::isAncestor(true_body, def_node))
			{
				false_body_var_table.table_[var_name].erase(version);
			}
			else if (SageInterface::isAncestor(false_body, def_node))
			{
				true_body_var_table.table_[var_name].erase(version);
			}
		}
#endif
	}

#if 0
	cout << "True body VVT:\n";
	true_body_var_table.print();
	cout << "False body VVT:\n";
	false_body_var_table.print();
#endif

	return make_pair(true_body_var_table, false_body_var_table);
}

VariableVersionTable VariableVersionTable::getVarTablesForLoopBody(SgBasicBlock* loop_body) const
{
	VariableVersionTable loop_body_var_table = *this;

	VariableRenaming::NumNodeRenameTable loop_body_defs = var_renaming_->getReachingDefsAtScopeEnd(loop_body);


	foreach (const TableType::value_type& var_version, table_)
	{
		const VarName& var_name = var_version.first;

		// Get the correct versions for var_name for true body.
		if (loop_body_defs.count(var_name) > 0)
		{
			set<int> loop_body_versions;
			// Get version for var_name from reaching defs at scope end.
			foreach (const VariableRenaming::NumNodeRenameTable::mapped_type::value_type& num_node,
				loop_body_defs[var_name])
				loop_body_versions.insert(num_node.first);

			set<int> new_versions;
			std::set_intersection(loop_body_var_table.table_[var_name].begin(),
					loop_body_var_table.table_[var_name].end(),
					loop_body_versions.begin(), loop_body_versions.end(),
					inserter(new_versions, new_versions.begin()));
			loop_body_var_table.table_[var_name].swap(new_versions);
		}
		else
		{
			//true_body_var_table.table_.erase(var_name);
			loop_body_var_table.table_[var_name].clear();
		}
	}




#if 0

	foreach (const TableType::value_type& var_version, table_)
	{
		const VarName& var_name = var_version.first;
		// For each version of var, if the def of this version is inside of loop body,
		foreach (int version, var_version.second)
		{
			SgNode* def_node = var_renaming_->getNodeForRenameNumber(var_version.first, version);
			SgStatement* enclosing_body = backstroke_util::getEnclosingLoopBody(def_node);
			if (enclosing_body == loop_body)
			{
				VariableRenaming::NumNodeRenameEntry num_node_entry =
						var_renaming_->getReachingDefsAtNodeForName(loop_body, var_name);
				foreach (const VariableRenaming::NumNodeRenameEntry::value_type& num_node, num_node_entry)
				{
					//cout << "$^$^%$^$" << num_node.first << endl;
					loop_body_var_table.table_[var_name].erase(num_node.first);
				}
			}
		}
	}

#endif

#if 0
	cout << "\nLoop body VVT:\n";
	loop_body_var_table.print();
	cout << endl;
#endif


	return loop_body_var_table;
}

void VariableVersionTable::intersect(const VariableVersionTable& var_table)
{
	ROSE_ASSERT(var_table.table_.size() == this->table_.size());
	
	for (TableType::iterator it = table_.begin(); it != table_.end(); ++it)
	{
		ROSE_ASSERT(var_table.table_.find(it->first) != var_table.table_.end());

		// Make the intersection of those two sets of versions.
		const set<int>& ver1 = var_table.table_.find(it->first)->second;
		const set<int>& ver2 = it->second;
		set<int> intersection;

		std::set_intersection(ver1.begin(), ver1.end(),
				ver2.begin(), ver2.end(), inserter(intersection, intersection.begin()));
		it->second.swap(intersection);

#if 0
		if (var_table.table_.find(it->first)->second != it->second)
			it->second.clear();
#endif
	}
}

void VariableVersionTable::setUnion(const VariableVersionTable& var_table)
{
	ROSE_ASSERT(var_table.table_.size() == this->table_.size());

	for (TableType::iterator it = table_.begin(); it != table_.end(); ++it)
	{
		ROSE_ASSERT(var_table.table_.find(it->first) != var_table.table_.end());

		// Make the intersection of those two sets of versions.
		const set<int>& ver1 = var_table.table_.find(it->first)->second;
		const set<int>& ver2 = it->second;
		set<int> result;

		std::set_union(ver1.begin(), ver1.end(),
				ver2.begin(), ver2.end(), inserter(result, result.begin()));
		it->second.swap(result);

#if 0
		if (var_table.table_.find(it->first)->second != it->second)
			it->second.clear();
#endif
	}
}

bool VariableVersionTable::isUsingFirstDef(SgNode* node) const
{
	VarName varName = getVarName(node);
	ROSE_ASSERT(varName != VariableRenaming::emptyName);
	VariableRenaming::NumNodeRenameEntry num_table = var_renaming_->getReachingDefsAtNodeForName(node, varName);

	// The first definition has the number 1
	// This is also true for branch case.

	// The size of the num_table does not have to be 1. Considering the following case:
	// void event(model* m)
	// {
	//    if (...) m->i = 1;
	//    m->i++; // Its reaching def has indices 1,2 not 1
	// }
	if (/*num_table.size() == 1 && */num_table.count(1) > 0)
		return true;
	return false;
}

bool VariableVersionTable::isUsingFirstUse(SgNode* node) const
{
	VarName varName = getVarName(node);
	ROSE_ASSERT(varName != VariableRenaming::emptyName);
	VariableRenaming::NumNodeRenameEntry num_table = var_renaming_->getUsesAtNodeForName(node, varName);

	// The first definition has the number 1
	// FIXME This may not be true for branch case!
	if (num_table.size() == 1 && num_table.count(1) > 0)
		return isUsingFirstDef(node);
	return false;
}

/** Returns true if a variable is at the specified version.
 * @param varName name of the variable to look up
 * @param version version that the variable should have (list of possible definitions). */
bool VariableVersionTable::matchesVersion(VarName varName, VariableRenaming::NumNodeRenameEntry version) const
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

#if	0
	printf("%s:\nIndices found: ", VariableRenaming::keyToString(varName).c_str());

	foreach(int v, table_.find(varName)->second)
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

	bool result = table_.find(varName)->second == indices;
	printf("Result is %s\n\n", result ? "true" : "false");
#endif

	return table_.find(varName)->second == indices;
}

