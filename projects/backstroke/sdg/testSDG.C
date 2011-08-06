#include "newCFG.h"
#include "newCDG.h"
#include "newDDG.h"
#include "newPDG.h"

using namespace SystemDependenceGraph;

bool filterCfgNode(const VirtualCFG::CFGNode& cfgNode)
{
    if (!cfgNode.isInteresting())
        return false;
    
    
    SgNode* astNode = cfgNode.getNode();
    
    if (SgExpression* expr = isSgExpression(astNode))
    {
        SgNode* parent = expr->get_parent();
        if (isSgExpression(parent) ||
                isSgReturnStmt(parent))
            return false;
    }
    
    if (isSgFunctionDefinition(astNode))
        return true;
    
    if (isSgScopeStatement(astNode))
        return false;
    
    switch (astNode->variantT())
    {
    case V_SgExprStatement:
    case V_SgInitializedName:
    case V_SgCaseOptionStmt:
    case V_SgDefaultOptionStmt:
    case V_SgFunctionParameterList:
    case V_SgBreakStmt:
    case V_SgContinueStmt:
        return false;
    default:
        break;
    }
    
    return true;
}

int main(int argc, char *argv[])
{
    // Build the AST used by ROSE
    SgProject* project = frontend(argc,argv);
    SgSourceFile* sourceFile = isSgSourceFile((*project)[0]);

    // Process all function definition bodies for static control flow graph generation
    Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
    {
        SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
        ROSE_ASSERT (proc != NULL);

        if (!proc->get_file_info()->isSameFile(sourceFile))
            continue;

        //ControlFlowGraph cfg(proc, InterestingCFGNodeFilter());
        ControlFlowGraph cfg(proc, filterCfgNode);
        cfg.toDot("CFG.dot");
        
        ControlDependenceGraph cdg(cfg);
        cdg.toDot("CDG.dot");
        
        DataDependenceGraph ddg(cfg);
        ddg.toDot("DDG.dot");
        
        ProgramDependenceGraph pdg(cfg);
        pdg.toDot("PDG.dot");
        
        break;
    }
}
