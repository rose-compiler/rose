#include <functionEvaluationOrderTraversal.h>

/** Visits AST nodes in pre-order */
FunctionCallInheritedAttribute FunctionEvaluationOrderTraversal::evaluateInheritedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute)
{
    FunctionCallInheritedAttribute result = parentAttribute;
    SgForStatement* parentForLoop = isSgForStatement(parentAttribute.currentLoop);
    SgWhileStmt* parentWhileLoop = isSgWhileStmt(parentAttribute.currentLoop);
    SgDoWhileStmt* parentDoWhileLoop = isSgDoWhileStmt(parentAttribute.currentLoop);

    if (isSgForStatement(astNode))
        result.currentLoop = isSgForStatement(astNode);
    else if (isSgWhileStmt(astNode))
        result.currentLoop = isSgWhileStmt(astNode);
    else if (isSgDoWhileStmt(astNode))
        result.currentLoop = isSgDoWhileStmt(astNode);
    else if (isSgForInitStatement(astNode))
    {
        ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
        result.loopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_INIT;
        ROSE_ASSERT(isSgForStatement(result.currentLoop));
    }
    else if (parentForLoop != NULL && parentForLoop->get_test() == astNode)
    {
        ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
        result.loopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_TEST;
    }
    else if (parentForLoop != NULL && parentForLoop->get_increment() == astNode)
    {
        ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
        result.loopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT;
    }
    else if (parentWhileLoop != NULL && parentWhileLoop->get_condition() == astNode)
    {
        ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
        result.loopStatus = FunctionCallInheritedAttribute::INSIDE_WHILE_CONDITION;
    }
    else if (parentDoWhileLoop != NULL && parentDoWhileLoop->get_condition() == astNode)
    {
        ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
        result.loopStatus = FunctionCallInheritedAttribute::INSIDE_DO_WHILE_CONDITION;
    }

    //We can't insert variables before an expression statement that appears inside if(), switch, throw, etc.
    if (isSgExprStatement(astNode) && !isSgBasicBlock(astNode->get_parent()))
    {
        //We can't insert a variable declaration at these locations. Use the parent statement
    }
    else if (isSgStatement(astNode))
        result.lastStatement = isSgStatement(astNode);

    return result;
}

/** Visits AST nodes in post-order. This is function-evaluation order. */
bool FunctionEvaluationOrderTraversal::evaluateSynthesizedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute, SynthesizedAttributesList)
{
    SgFunctionCallExp* functionCall = isSgFunctionCallExp(astNode);
    if (functionCall == NULL)
        return false; //dummy return value

    FunctionCallInfo functionCallInfo(functionCall);

    //Handle for loops (being inside the body of a for loop doesn't need special handling)
    if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INIT
 || parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_TEST
            || parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT )
    {
        SgForStatement* forLoop = isSgForStatement(parentAttribute.currentLoop);
        ROSE_ASSERT(forLoop != NULL);
        //Temporary variables should be declared before the loop
        functionCallInfo.tempVarDeclarationLocation = forLoop;
        functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
    }
    else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_WHILE_CONDITION)
    {
        SgWhileStmt* whileLoop = isSgWhileStmt(parentAttribute.currentLoop);
        ROSE_ASSERT(whileLoop != NULL);
        //Temporary variables should be declared before the loop
        functionCallInfo.tempVarDeclarationLocation = whileLoop;
        functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
    }
    else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_DO_WHILE_CONDITION)
    {
        SgDoWhileStmt* doWhileLoop = isSgDoWhileStmt(parentAttribute.currentLoop);
        ROSE_ASSERT(doWhileLoop);
        //Temporary variables should be declared before the loop
        functionCallInfo.tempVarDeclarationLocation = doWhileLoop;
        functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
    } 
    else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP)
    {
        //Assume we're in a basic block. Then just insert right before the current statement
        ROSE_ASSERT(parentAttribute.loopStatus = FunctionCallInheritedAttribute::NOT_IN_LOOP);
        functionCallInfo.tempVarDeclarationLocation = parentAttribute.lastStatement;
        functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
    }
    else
    {
        //Unhandled condition?!
        ROSE_ASSERT(false);
    }

    functionCalls.push_back(functionCallInfo);
    return false; //dummy return value
}
