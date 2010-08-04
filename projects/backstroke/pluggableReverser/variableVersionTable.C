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
    VariableRenaming::NumNodeRenameTable num_node_table =
            var_renaming_->getReachingDefsAtFunctionEnd(func_def);

    //var_renaming_->printRenameTable(num_node_table);

    foreach (VariableRenaming::NumNodeRenameTable::value_type name_to_num, num_node_table)
    {
        foreach (VariableRenaming::NumNodeRenameEntry::value_type num_to_node, name_to_num.second)
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

void VariableVersionTable::print(const VarName& name)
{
    foreach (SgInitializedName* n, name)
        cout << n->get_name().str() << ' ';
    cout << endl;
}


/** This function get all variables in an AST node. Note that for a variable a.b or a->b,
    only a.b or a->b is returned, not a or b. */
vector<SgExpression*> getAllVariables(SgNode* node)
{
    vector<SgExpression*> vars;

    vector<SgExpression*> exps = querySubTree<SgExpression>(node);

    //ROSE_ASSERT(!exps.empty());

    foreach (SgExpression* exp, exps)
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

bool VariableVersionTable::checkRhsVersion(SgNode* node) const
{
    // In this function, for expression a.b or a->b ,we only care about the version of a.b or a->b,
    // but not the version of a or b.
    
    // First, get all variables in the given AST node.
    // Note that a expression contains several variables can be passed in.
    vector<SgExpression*> vars = getAllVariables(node);

    //ROSE_ASSERT(!vars.empty());

    foreach (SgExpression* var, vars)
    {
        VarName name = getVarName(var);
        VariableRenaming::NumNodeRenameEntry defs = var_renaming_->getReachingDefsAtNodeForName(var, name);

        ROSE_ASSERT(!defs.empty());
        
        foreach (VariableRenaming::NumNodeRenameEntry::value_type num_to_node, defs)
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

    foreach (SgExpression* var, vars)
    {
        VarName name = getVarName(var);
        VariableRenaming::NumNodeRenameEntry defs = var_renaming_->getDefsAtNodeForName(var->get_parent(), name);

        ROSE_ASSERT(!defs.empty());

        foreach (VariableRenaming::NumNodeRenameEntry::value_type num_to_node, defs)
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
    
    VarName name = getVarName(node);
    VariableRenaming::NumNodeRenameEntry num_table =
            var_renaming_->getReachingDefsAtNodeForName(node, name);

    table_[name].clear();
    foreach (VariableRenaming::NumNodeRenameEntry::value_type num_to_node, num_table)
    {
        table_[name].insert(num_to_node.first);
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
    VarName var_name = getVarName(node);
    ROSE_ASSERT(table_.find(var_name) != table_.end());
    table_[var_name].clear();
}

bool VariableVersionTable::isUsingFirstDefinition(SgNode* node) const
{
    VariableRenaming::NumNodeRenameEntry num_table =
            var_renaming_->getReachingDefsAtNodeForName(node, getVarName(node));

    // The first definition has the number 1
    if (num_table.size() == 1 && num_table.count(1) > 0)
        return true;
    return false;
}

//void VariableVersionTable::setPreviousVersion(SgNode* node)
//{
//    NumNodeRenameTable table = var_renaming_->getOriginalDefsAtNode(node);
//    ROSE_ASSERT(table.count(getName(node)) > 0);
//
//}

