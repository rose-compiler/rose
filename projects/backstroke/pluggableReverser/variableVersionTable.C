#include "variableVersionTable.h"
#include <VariableRenaming.h>
#include <utilities/Utilities.h>
#include <utilities/CPPDefinesAndNamespaces.h>

using namespace backstroke_util;

VariableVersionTable::VariableVersionTable(
        SgFunctionDeclaration* func_decl,
        VariableRenaming* var_renaming)
    : var_renaming_(var_renaming)
{
    SgFunctionDefinition* func_def = 
            isSgFunctionDeclaration(func_decl->get_definingDeclaration())->get_definition();
    VariableRenaming::numNodeRenameTable num_node_table =
            var_renaming_->getReachingDefsAtFunctionEnd(func_def);

    foreach (VariableRenaming::numNodeRenameTable::value_type name_to_num, num_node_table)
    {
        foreach (VariableRenaming::numNodeRenameEntry::value_type num_to_node, name_to_num.second)
        {
            table_[name_to_num.first].insert(num_to_node.first);
        }
    }
}

VariableVersionTable::VarName VariableVersionTable::getVarName(SgNode* node)
{
    return VariableRenaming::getVarName(node);
}

void VariableVersionTable::print() const
{
    typedef pair<VarName, std::set<int> > VarIdxPair;
    foreach (VarIdxPair var_idx, table_)
    {
        foreach (SgInitializedName* name, var_idx.first)
            cout << name->get_name().str() << ' ';
        foreach (int i, var_idx.second)
            cout << i << ' ';
        cout << endl;
    }
}

bool VariableVersionTable::checkVersion(SgNode* node) const
{
    VarName name = getVarName(node);
    int num = var_renaming_->getRenameNumberForNode(name, node);
    return table_.find(name)->second.count(num) > 0;
}

void VariableVersionTable::reverseVersion(SgNode* node)
{
    VariableRenaming::numNodeRenameTable node_table =
            var_renaming_->getReachingDefsAtNode(node);

    foreach (VariableRenaming::numNodeRenameTable::value_type name_to_num, node_table)
    {
        table_[name_to_num.first].clear();
        foreach (VariableRenaming::numNodeRenameEntry::value_type num_to_node, name_to_num.second)
        {
            table_[name_to_num.first].insert(num_to_node.first);
        }
    }
}

