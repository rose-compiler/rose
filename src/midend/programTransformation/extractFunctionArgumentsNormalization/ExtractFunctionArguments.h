#pragma once

// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

#include "functionEvaluationOrderTraversal.h"

//class FunctionCallInfo;

//! This normalization makes sure each function call argument is a side-effect free expression of only one variable.
//! To accomplish this, temporary variables are declared and the arguments of a function are evaluated before the function itself.
//! Note: Normalization is not performed if it is unsafe. E,g.: while(Foo(Bar()) {}  is not equal to t = Bar(); while(Foo(t) {}
//! Note that only SgFunctionCall arguments are normalized. For example the arguments of the constructor initializer
//! MyObject o(foo(), bar()) will not be extracted. 
class ExtractFunctionArguments
{
public:

    /** Performs the function argument extraction on all function calls in the given subtree of the AST. */
    /** It does not do transofrmations in places where it is not safe. If you pass doUnsafeNormalization= true, we will normalize all call sites ignoring the safety (Suggested by Markus Schordan) */
    void NormalizeTree(SgNode* tree, bool doUnsafeNormalization = false);

    // returns a vector of newly introduced temporaries
    std::vector<SgVariableDeclaration*> GetTemporariesIntroduced();
    
    /*
     IsNormalized: Given a subtree, returns true if every argument of every function call is a trivial arguemnt.
     */    
    bool IsNormalized(SgNode* tree);
    
    /*
     IsNormalizable: Given a subtree, returns true if every argument of every function call is a either trivial arguemnt or
     is present in a SAFE place from where lifting is possible.
     */    
    bool IsNormalizable(SgNode* tree);

    
private:

    std::vector<SgVariableDeclaration*> temporariesIntroduced;
    std::pair< std::vector<FunctionCallInfo>, std::vector<FunctionCallInfo> > functionCalls;
    
    /** Given the expression which is the argument to a function call, returns true if that
     * expression should be pulled out into a temporary variable on a separate line.
     * E.g. if the expression contains a function call, it needs to be normalized, while if it
     * is a constant, there is no need to change it. */
    bool FunctionArgumentNeedsNormalization(SgExpression* argument);

    /** Returns true if any of the arguments of the given function call will need to
     * be extracted. */
    bool FunctionArgsNeedNormalization(SgExprListExp* functionArgs);

    /** Returns true if any function calls in the given subtree will need to be
     * instrumented. (to extract function arguments). */
    bool SubtreeNeedsNormalization(SgNode* top);

    /** Given the information about a function call (obtained through a traversal), extract its arguments
     * into temporary variables where it is necessary. */
    void RewriteFunctionCallArguments(const FunctionCallInfo& functionCallInfo);

    /** Insert a new statement in the specified location. The actual insertion can occur either before or after the location
     * depending on the insertion mode. */
    void InsertStatement(SgStatement* newStatement, SgStatement* location, const FunctionCallInfo& insertionMode);

    bool FunctionArgumentCanBeNormalized(SgExpression* argument);
    
    
    /* Given the expression which is the argument to a function call, returns true if that
     expression is trivial. Trivial expressions are those which are simple variable references or constants.
     */
    bool IsFunctionArgumentTrivial(SgExpression* argument);

    /* Given a vector of function call sites returns true if every argument of every function call is a trivial expression
     (IsFunctionArgumentTrivial). Such functions don't need to be normalized.
     */
    bool AreAllFunctionCallsTrivial(std::vector<FunctionCallInfo> functions);
    
    /* Given a vector of function call sites, returns true if every argument
     at every function call site is either trivial (IsFunctionArgumentTrivial) or can be normalized (FunctionArgumentCanBeNormalized).
     */
    bool AreAllFunctionCallsNormalizable(std::vector<FunctionCallInfo> functions);
};
