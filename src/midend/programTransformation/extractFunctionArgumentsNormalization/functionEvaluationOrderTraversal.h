#pragma once

// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "sage3basic.h"

struct FunctionCallInheritedAttribute
{
    /** The innermost scope inside of which this AST node resides. It is either a for-loop,
         a do-loop,  a while-loop or a conditioanl expression. */
    SgNode* currentScope;

    /** The last statement encountered before the current node in the AST. */
    SgStatement* lastStatement;

    /** Is the current node inside a for loop or conditional expresion structure (not the body). */
    enum
    {
        INSIDE_FOR_INIT, INSIDE_FOR_TEST, INSIDE_FOR_INCREMENT, INSIDE_WHILE_CONDITION,
        INSIDE_DO_WHILE_CONDITION, IN_SAFE_PLACE, INSIDE_CONDITIONAL_EXP_TRUE_ARM, INSIDE_CONDITIONAL_EXP_FALSE_ARM, INSIDE_SHORT_CIRCUIT_EXP_RHS
    }
    scopeStatus;

    /** Default constructor. Initializes everything to NULL. */
    FunctionCallInheritedAttribute() : currentScope(NULL), lastStatement(NULL), scopeStatus(IN_SAFE_PLACE) { }
};

/** Stores a function call expression, along with associated information about its context. */
struct FunctionCallInfo
{
    /** The function call expression. */
    SgFunctionCallExp* functionCall;

    /** When a variable is created to replace one of the arguments of this function, where should it be inserted?
     * The declaration of the variable will occur right before this statement. */
    SgStatement* tempVarDeclarationLocation;

    /** How a statement should be inserted.   */
    enum InsertionMode
    {
        /** Insert right before the given statement. */
        INSERT_BEFORE,
        /** Insert at the bottom of the scope defined by the given statement. */
        APPEND_SCOPE,
        INVALID
    };

    /** How to insert the temporary variable declaration. */
    InsertionMode tempVarDeclarationInsertionMode;

    FunctionCallInfo(SgFunctionCallExp * function) :
    functionCall(function),
    tempVarDeclarationLocation(NULL),
    tempVarDeclarationInsertionMode(INVALID) { }
};


//! Traverses a given AST and finds all function calls in the order in which they're evaluated
//! Also, for each function we find where to put declarations of temporary variables so that they're accessible at the function call

class FunctionEvaluationOrderTraversal : public AstTopDownBottomUpProcessing<FunctionCallInheritedAttribute, bool>
{
public:
    /** Traverses the subtree of the given AST node and finds all function calls in
     * function-evaluation order. */
    static std::pair< std::vector<FunctionCallInfo>, std::vector<FunctionCallInfo> > GetFunctionCalls(SgNode* root);

    /** Visits AST nodes in pre-order */
    FunctionCallInheritedAttribute evaluateInheritedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute);

    /** Visits AST nodes in post-order. This is function-evaluation order. */
    bool evaluateSynthesizedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute, SynthesizedAttributesList);

    /// Returns true if the function call has no side effects.
    virtual bool IsFunctionCallSideEffectFree(SgFunctionCallExp* functionCall);

private:

    //! Private constructor. Use the static method to access the functionality of this class.

    FunctionEvaluationOrderTraversal() { }

    /** All the function calls seen so far that can be normalized. */
    std::vector<FunctionCallInfo> normalizableFunctionCalls;
    /** All the function calls seen so far that can't be normalized. */
    std::vector<FunctionCallInfo> nonNormalizableFunctionCalls;
};

