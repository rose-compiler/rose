/* 
 * File:   cfgNodeFilter.C
 * Author: Cong Hou [hou_cong@gatech.edu]
 */

#include "cfgNodeFilter.h"

namespace SDG
{

bool filterCfgNode(const VirtualCFG::CFGNode& cfgNode)
{
    if (!cfgNode.isInteresting())
        return false;
    
    
    SgNode* astNode = cfgNode.getNode();
    
    if (SgExpression* expr = isSgExpression(astNode))
    {
        if (isSgFunctionCallExp(expr))
            return true;
        
        SgNode* parent = expr->get_parent();
        if (isSgConditionalExp(parent)
                || isSgExprListExp(parent))
            return true;

        if (isSgExpression(parent))
            return false;
    }
    
    if (isSgFunctionDefinition(astNode))
        return true;
    
    if (isSgScopeStatement(astNode))
        return false;
    
    // Keep function parameters.
    if (isSgInitializedName(astNode) 
            && isSgFunctionParameterList(astNode->get_parent()))
        return true;
    
    switch (astNode->variantT())
    {
    case V_SgExprStatement:
    case V_SgInitializedName:
    case V_SgCaseOptionStmt:
    case V_SgDefaultOptionStmt:
    case V_SgFunctionParameterList:
    case V_SgBreakStmt:
    case V_SgContinueStmt:
    case V_SgReturnStmt:
        return false;
    default:
        break;
    }
    return true;
}

bool filterCFGNodesByKeepingStmt(const VirtualCFG::CFGNode& cfgNode)
{
    if (!cfgNode.isInteresting())
        return false;
    
    
    SgNode* astNode = cfgNode.getNode();
    
    if (SgExpression* expr = isSgExpression(astNode))
    {
        if (isSgFunctionCallExp(expr))
            return true;
        
        SgNode* parent = expr->get_parent();
        if (isSgConditionalExp(parent)
                || isSgExprListExp(parent))
            return true;

        //if (isSgExpression(parent))
        return false;
    }
    
    if (isSgFunctionDefinition(astNode))
        return true;
    
    if (isSgScopeStatement(astNode))
        return false;
    
    // Keep function parameters.
    if (isSgInitializedName(astNode) 
            && isSgFunctionParameterList(astNode->get_parent()))
        return true;
    
    switch (astNode->variantT())
    {
    case V_SgInitializedName:
    case V_SgCaseOptionStmt:
    case V_SgDefaultOptionStmt:
    case V_SgFunctionParameterList:
    case V_SgBreakStmt:
    case V_SgContinueStmt:
    case V_SgClassDeclaration:
    case V_SgEnumDeclaration:
        return false;
    default:
        break;
    }
    return true;  
}

}