#include "ExtractFunctionArguments.h"
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <functionEvaluationOrderTraversal.h>
#include "SingleStatementToBlockNormalization.h"

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;


/** Perform the function argument extraction on all function calls in the given subtree of the AST.
 * Returns true on sucess, false on failure (unsupported code). */
void ExtractFunctionArguments::NormalizeTree(SgNode* tree)
{
    // First Normalize each single statememnt body into a block body
    // This transformation is necessary to provide scope to newly created temporaries
    // Note: if this transofrmation is already done, it is benign to rerun it
    // and this would act as a NOP transformation.
    
    SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
    singleStatementToBlockNormalizer.Normalize(tree);
    
    //Get all functions in function evaluation order
    vector<FunctionCallInfo> functionCalls = FunctionEvaluationOrderTraversal::GetFunctionCalls(tree);
    
    foreach(const FunctionCallInfo& functionCallInfo, functionCalls)
    {
        RewriteFunctionCallArguments(functionCallInfo);
    }
}

/** Given the information about a function call (obtained through a traversal), extract its arguments
 * into temporary variables where it is necessary.
 * Returns true on success, false on failure (unsupported code). */
void ExtractFunctionArguments::RewriteFunctionCallArguments(const FunctionCallInfo& functionCallInfo)
{
    SgFunctionCallExp* functionCall = functionCallInfo.functionCall;
    
    // Force the function call to NOT use operator syntax
    functionCall->set_uses_operator_syntax(false);
    
    SgExprListExp* functionArgs = functionCall->get_args();
    ROSE_ASSERT(functionArgs != NULL);
    
    SgExpressionPtrList argumentList = functionArgs->get_expressions();
    
    // We also normalize the caller if the function called is a member function.
    if (SgBinaryOp * binExp = isSgBinaryOp(functionCall->get_function())) {
        argumentList.push_back(binExp->get_lhs_operand());
    }
    
    //Go over all the function arguments, pull them out
    
    foreach(SgExpression* arg, argumentList)
    {
        //No need to pull out parameters that are not complex expressions and
        //thus don't have side effects
        if (!FunctionArgumentNeedsNormalization(arg))
            continue;

        //Build a declaration for the temporary variable
        SgScopeStatement* scope = functionCallInfo.tempVarDeclarationLocation->get_scope();
        ROSE_ASSERT(scope != NULL );
        SgVariableDeclaration* tempVarDeclaration;
        SgExpression* tempVarReference;
        tie(tempVarDeclaration, tempVarReference) = SageInterface::createTempVariableAndReferenceForExpression(arg, scope);
        
        // createTempVariableOrReferenceForExpression does not set the parent if the scope stack is empty. Hence set it manually to the currect scope.
        tempVarDeclaration->set_parent(scope);
#if 0
        {
            std::cout<<"\n"<<functionCall->get_file_info()->get_filenameString () << ":" << functionCall->get_file_info()->get_line () << ":" << functionCall->get_file_info()->get_col ();
            std::cout<<"\n Name = "<< tempVarDeclaration->get_mangled_name().getString()<< ":type :"<<arg->get_type()->class_name() << "Expr class :"<<arg->class_name();
        }
#endif
        ROSE_ASSERT(tempVarDeclaration != NULL );
        ROSE_ASSERT(tempVarDeclaration->get_definition(0) != NULL);
        ROSE_ASSERT(isSgVariableDefinition(tempVarDeclaration->get_definition()) != NULL);
        
        //Insert the temporary variable declaration
        InsertStatement(tempVarDeclaration, functionCallInfo.tempVarDeclarationLocation, functionCallInfo);
        
        //Replace the argument with the new temporary variable
        SageInterface::replaceExpression(arg, tempVarReference);
    }
}

/** Returns true if the given expression refers to a variable. This could include using the
 * dot and arrow operator to access member variables. A comma op counts as a variable references
 * if all its members are variable references (not just the last expression in the list). */
bool isVariableReference(SgExpression* expression)
{
    if (isSgVarRefExp(expression))
    {
        return true;
    }
    else if (isSgThisExp(expression))
    {
        return true;
    }
    else if (isSgDotExp(expression))
    {
        SgDotExp* dotExpression = isSgDotExp(expression);
        return isVariableReference(dotExpression->get_lhs_operand()) &&
        isVariableReference(dotExpression->get_rhs_operand());
    }
    else if (isSgArrowExp(expression))
    {
        SgArrowExp* arrowExpression = isSgArrowExp(expression);
        return isVariableReference(arrowExpression->get_lhs_operand()) &&
        isVariableReference(arrowExpression->get_rhs_operand());
    }
    else if (isSgCommaOpExp(expression))
    {
        //Comma op where both the lhs and th rhs are variable references.
        //The lhs would be semantically meaningless since it doesn't have any side effects
        SgCommaOpExp* commaOp = isSgCommaOpExp(expression);
        return isVariableReference(commaOp->get_lhs_operand()) &&
        isVariableReference(commaOp->get_rhs_operand());
    }
    else if (isSgPointerDerefExp(expression) || isSgCastExp(expression) || isSgAddressOfOp(expression))
    {
        return isVariableReference(isSgUnaryOp(expression)->get_operand());
    }
    else
    {
        return false;
    }
}

/** Given the expression which is the argument to a function call, returns true if that
 * expression should be pulled out into a temporary variable on a separate line.
 * E.g. if the expression contains a function call, it needs to be normalized, while if it
 * is a constant, there is no need to change it. */

bool ExtractFunctionArguments::FunctionArgumentNeedsNormalization(SgExpression* argument)
{

    while ((isSgPointerDerefExp(argument) || isSgCastExp(argument) || isSgAddressOfOp(argument)))
    {
        argument = isSgUnaryOp(argument)->get_operand();
    }
    
    SgArrowExp* arrowExp = isSgArrowExp(argument);
    if (arrowExp && isSgThisExp(arrowExp->get_lhs_operand()))
        return false;
    
    //For right now, move everything but a constant value or an explicit variable access
    // Don't include SgConstructorInitializer since it will be called even on the temporary, so avoid double copy.
    if (isVariableReference(argument) || isSgValueExp(argument) || isSgFunctionRefExp(argument)
        || isSgMemberFunctionRefExp(argument) || isSgConstructorInitializer(argument))
        return false;

    // Unknow Template type expressions can't be normalized.
    if (isSgTypeUnknown(argument->get_type()) || isSgMemberFunctionType(argument->get_type())) {
        //printf("\n Skipping over SgTypeUnknown/SgMemberFunctionType  expr");
        return false;
    }

    return true;
}


/** Returns true if any of the arguments of the given function call will need to
 * be extracted. */
bool ExtractFunctionArguments::FunctionArgsNeedNormalization(SgExprListExp* functionArgs)
{
    ROSE_ASSERT(functionArgs != NULL);
    SgExpressionPtrList& argumentList = functionArgs->get_expressions();
    
    foreach(SgExpression* functionArgument, argumentList)
    {
        if (FunctionArgumentNeedsNormalization(functionArgument))
            return true;
    }
    return false;
}

/** Returns true if any function calls in the given subtree will need to be
 * instrumented. (to extract function arguments). */
bool ExtractFunctionArguments::SubtreeNeedsNormalization(SgNode* top)
{
    ROSE_ASSERT(top != NULL);
    Rose_STL_Container<SgNode*> functionCalls = NodeQuery::querySubTree(top, V_SgFunctionCallExp);
    for (Rose_STL_Container<SgNode*>::const_iterator iter = functionCalls.begin(); iter != functionCalls.end(); iter++)
    {
        SgExpression* functionCall = isSgFunctionCallExp(*iter);
        ROSE_ASSERT(functionCall != NULL);
        if (FunctionArgumentNeedsNormalization(functionCall))
            return true;
    }
    
    return false;
}

/** Insert a new statement in the specified location. The actual insertion can occur either before or after the location
 * depending on the insertion mode. */
void ExtractFunctionArguments::InsertStatement(SgStatement* newStatement, SgStatement* location, const FunctionCallInfo& insertionMode)
{
    switch (insertionMode.tempVarDeclarationInsertionMode)
    {
        case FunctionCallInfo::INSERT_BEFORE:
            SageInterface::insertStatementBefore(location, newStatement);
            break;
        case FunctionCallInfo::APPEND_SCOPE:
        {
            SgScopeStatement* scopeStatement = isSgScopeStatement(location);
            if (scopeStatement == NULL)
            {
                //scopeStatement = isSgScopeStatement(SageInterface::ensureBasicBlockAsParent(location));
                if (SageInterface::isBodyStatement(location)) // if the location is a single body statement (not a basic block) at this point
                    scopeStatement = SageInterface::makeSingleStatementBodyToBlock (location);
                else
                    scopeStatement = isSgScopeStatement(location->get_parent());
            }
            ROSE_ASSERT(scopeStatement != NULL);
            
            SageInterface::appendStatement(newStatement, scopeStatement);
            break;
        }
        case FunctionCallInfo::INVALID:
        default:
            ROSE_ASSERT(false);
    }
}

/** Traverses the subtree of the given AST node and finds all function calls in
 * function-evaluation order. */
/*static*/std::vector<FunctionCallInfo> FunctionEvaluationOrderTraversal::GetFunctionCalls(SgNode* root)
{
    FunctionEvaluationOrderTraversal t;
    FunctionCallInheritedAttribute rootAttribute;
    t.traverse(root, rootAttribute);
    
    return t.functionCalls;
}
