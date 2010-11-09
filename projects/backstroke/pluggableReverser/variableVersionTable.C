#include "variableVersionTable.h"
#include <VariableRenaming.h>
#include <utilities/utilities.h>
#include <utilities/cppDefinesAndNamespaces.h>

using namespace std;
using namespace BackstrokeUtility;

VariableVersionTable::VariableVersionTable(SgFunctionDeclaration* func_decl, VariableRenaming* var_renaming)
: var_renaming_(var_renaming)
{
	SgFunctionDefinition* func_def = isSgFunctionDeclaration(func_decl->get_definingDeclaration())->get_definition();
	VariableRenaming::NumNodeRenameTable num_node_table = var_renaming_->getReachingDefsAtFunctionEnd(func_def);
	//cout << num_node_table.size() << endl;
	foreach(VariableRenaming::NumNodeRenameTable::value_type name_to_num, num_node_table)
	{
		foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, name_to_num.second)
		{
			// If the variable is a local variable of the given function (except parameters), we will set its
			// version to NULL.
			if (SageInterface::isAncestor(
					BackstrokeUtility::getFunctionBody(func_decl),
					name_to_num.first[0]->get_declaration()))
			{
				//cout << VariableRenaming::keyToString(name_to_num.first) << endl;
				table_[name_to_num.first];
				table_[name_to_num.first].insert(num_to_node.first);
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

void VariableVersionTable::reverseVersionAtStatementStart(SgStatement* stmt)
{

	VariableRenaming::NumNodeRenameTable var_versions = var_renaming_->getReachingDefsAtStatementStart(stmt);

	table_.clear();
	foreach(VariableRenaming::NumNodeRenameTable::value_type name_to_num, var_versions)
	{
		//boost::insert(table_[name_to_num.first], );
		foreach(VariableRenaming::NumNodeRenameEntry::value_type num_to_node, name_to_num.second)
		{
			table_[name_to_num.first].insert(num_to_node.first);
		}
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

	return table_.find(varName)->second == indices;
}

