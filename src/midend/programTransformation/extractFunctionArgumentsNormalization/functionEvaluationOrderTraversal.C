#include "sage3basic.h"                                 // every librose .C file must start with this
#include <functionEvaluationOrderTraversal.h>


bool IsStatusSafe(int status){
    if (status == FunctionCallInheritedAttribute::IN_SAFE_PLACE)
        return true;
    if (status == FunctionCallInheritedAttribute::INSIDE_FOR_INIT)
        return true;
    return false;
}

/** Visits AST nodes in pre-order */
FunctionCallInheritedAttribute FunctionEvaluationOrderTraversal::evaluateInheritedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute)
{
    FunctionCallInheritedAttribute result = parentAttribute;
    SgForStatement* parentForLoop = isSgForStatement(parentAttribute.currentScope);
    SgWhileStmt* parentWhileLoop = isSgWhileStmt(parentAttribute.currentScope);
    SgDoWhileStmt* parentDoWhileLoop = isSgDoWhileStmt(parentAttribute.currentScope);
    
    SgConditionalExp* parentSgConditionalExp = astNode->get_parent() ? isSgConditionalExp(astNode->get_parent()) : NULL;
    SgAndOp* parentAndOp =  astNode->get_parent() ?isSgAndOp(astNode->get_parent()) : NULL;
    SgOrOp* parentOrOp =  astNode->get_parent() ? isSgOrOp(astNode->get_parent()) : NULL;

    if (isSgForStatement(astNode))
        result.currentScope = isSgForStatement(astNode);
    else if (isSgWhileStmt(astNode))
        result.currentScope = isSgWhileStmt(astNode);
    else if (isSgDoWhileStmt(astNode))
        result.currentScope = isSgDoWhileStmt(astNode);
    //else if (isSgConditionalExp(astNode))
    //    result.currentScope = isSgConditionalExp(astNode);
    //else if (isSgAndOp(astNode))
    //    result.currentScope = isSgAndOp(astNode);
    //else if (isSgOrOp(astNode))
    //    result.currentScope = isSgOrOp(astNode);
    else if (isSgForInitStatement(astNode)) {
        ROSE_ASSERT(result.scopeStatus == FunctionCallInheritedAttribute::IN_SAFE_PLACE);
        result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_FOR_INIT;
        ROSE_ASSERT(isSgForStatement(result.currentScope));
    } else if (parentForLoop != NULL && parentForLoop->get_test() == astNode) {
        ROSE_ASSERT(result.scopeStatus == FunctionCallInheritedAttribute::IN_SAFE_PLACE);
        result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_FOR_TEST;
    } else if (parentForLoop != NULL && parentForLoop->get_increment() == astNode) {
        ROSE_ASSERT(result.scopeStatus == FunctionCallInheritedAttribute::IN_SAFE_PLACE);
        result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT;
    } else if (parentWhileLoop != NULL && parentWhileLoop->get_condition() == astNode) {
        ROSE_ASSERT(result.scopeStatus == FunctionCallInheritedAttribute::IN_SAFE_PLACE);
        result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_WHILE_CONDITION;
    } else if (parentDoWhileLoop != NULL && parentDoWhileLoop->get_condition() == astNode) {
        ROSE_ASSERT(result.scopeStatus == FunctionCallInheritedAttribute::IN_SAFE_PLACE);
        result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_DO_WHILE_CONDITION;
    } else if( parentSgConditionalExp != NULL && parentSgConditionalExp->get_true_exp() == astNode) {
        // if the scope status was safe, turn it into unsafe
        if (IsStatusSafe(result.scopeStatus))
            result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_CONDITIONAL_EXP_TRUE_ARM;
    } else if(parentSgConditionalExp != NULL && parentSgConditionalExp->get_false_exp() == astNode) {
        // if the scope status was safe, turn it into unsafe
        if (IsStatusSafe(result.scopeStatus))
            result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_CONDITIONAL_EXP_FALSE_ARM;
    } else if( parentOrOp != NULL && parentOrOp->get_rhs_operand () == astNode) {
        // if the scope status was safe, turn it into unsafe
        if (IsStatusSafe(result.scopeStatus))
            result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_SHORT_CIRCUIT_EXP_RHS;
    } else if( parentAndOp != NULL && parentAndOp->get_rhs_operand () == astNode)  {
        // if the scope status was safe, turn it into unsafe
        if (IsStatusSafe(result.scopeStatus))
            result.scopeStatus = FunctionCallInheritedAttribute::INSIDE_SHORT_CIRCUIT_EXP_RHS;
    }
    
    //We can't insert variables before an expression statement that appears inside if(), switch, throw, etc.
    if (isSgExprStatement(astNode) && !isSgBasicBlock(astNode->get_parent())) {
        //We can't insert a variable declaration at these locations. Use the parent statement
    } else if (isSgStatement(astNode)) {
        result.lastStatement = isSgStatement(astNode);
    }

    return result;
}

// Returns false in the absence of any analysis information
bool FunctionEvaluationOrderTraversal::IsFunctionCallSideEffectFree(SgFunctionCallExp* functionCall) {
    return false;
}


/** Visits AST nodes in post-order. This is function-evaluation order. */
bool FunctionEvaluationOrderTraversal::evaluateSynthesizedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute, SynthesizedAttributesList)
{
    SgFunctionCallExp* functionCall = isSgFunctionCallExp(astNode);
    if (functionCall == NULL)
        return false; //dummy return value
    
    FunctionCallInfo functionCallInfo(functionCall);

    // Can't lift function call arguments from the following:
    // 1. For loop test and increment
    // 2. While loop test
    // 3. Do-While loop test
    // 4. Either arms of ternary op
    // 5. RHS of any short circuit expression
    // An alternative is to use comma operators and use assignemnt op as done by the original author. 
    // for(;foo(bar());) ==> T i; for(;i=bar();foo(i);)
    // But using assignement op is not always safe and it requires us to always have a default constructor
    // There is also an issue when the return type is a reference and we'll have to use & op to get a pointer
    // but if & op is overloaded we may not get the pointer.
    // Taking all these in view, I am simpling not lifting such expressions.
    
    if (parentAttribute.scopeStatus == FunctionCallInheritedAttribute::INSIDE_FOR_TEST ||
        parentAttribute.scopeStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT ||
        parentAttribute.scopeStatus == FunctionCallInheritedAttribute::INSIDE_WHILE_CONDITION ||
        parentAttribute.scopeStatus == FunctionCallInheritedAttribute::INSIDE_DO_WHILE_CONDITION) {

        // ***** FOR UNSAFE TRANSFORMATION *******
        //Temporary variables should be declared before the stmt
        ROSE_ASSERT(isSgStatement(parentAttribute.currentScope));
        functionCallInfo.tempVarDeclarationLocation = isSgStatement(parentAttribute.currentScope);
        functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;

        nonNormalizableFunctionCalls.push_back(functionCallInfo);
        return false;
    }

    // In future, if the function call is assured to be side effect free, then we can lift it from short circuit and conditional expressions
    if(parentAttribute.scopeStatus == FunctionCallInheritedAttribute::INSIDE_CONDITIONAL_EXP_TRUE_ARM ||
       parentAttribute.scopeStatus == FunctionCallInheritedAttribute::INSIDE_CONDITIONAL_EXP_FALSE_ARM ||
       parentAttribute.scopeStatus == FunctionCallInheritedAttribute::INSIDE_SHORT_CIRCUIT_EXP_RHS){
        
        if(!IsFunctionCallSideEffectFree(functionCall)) {
            
            // ***** FOR UNSAFE TRANSFORMATION *******
            //Temporary variables should be declared before the stmt
            ROSE_ASSERT(parentAttribute.lastStatement);
            functionCallInfo.tempVarDeclarationLocation = parentAttribute.lastStatement;
            functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;

            nonNormalizableFunctionCalls.push_back(functionCallInfo);
            return false;
        }
    }
    
    //Handle for loops (being inside the body of a for loop doesn't need special handling)
    if (parentAttribute.scopeStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INIT)
    {
        SgForStatement* forLoop = isSgForStatement(parentAttribute.currentScope);
        ROSE_ASSERT(forLoop != NULL);
        //Temporary variables should be declared before the loop
        functionCallInfo.tempVarDeclarationLocation = forLoop;
        functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
    }
    else if (parentAttribute.scopeStatus == FunctionCallInheritedAttribute::IN_SAFE_PLACE)
    {
        //Assume we're in a basic block. Then just insert right before the current statement
        ROSE_ASSERT(parentAttribute.scopeStatus = FunctionCallInheritedAttribute::IN_SAFE_PLACE);
        functionCallInfo.tempVarDeclarationLocation = parentAttribute.lastStatement;
        functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
    }
    else
    {
        //Unhandled condition?!
        ROSE_ASSERT(false);
    }

    normalizableFunctionCalls.push_back(functionCallInfo);
    return false; //dummy return value
}
