#ifndef VARIABLEDECLARATIONHANDLER_H
#define	VARIABLEDECLARATIONHANDLER_H

#include "handlerTypes.h"

//! This handler handles reversing the side effects of variable declarations. Declarations are inserted
//! unmodified in the forward code. For example, 'int a = i++' is reversed as 'i--;'.
//! Installing this handler is always necessary.
class VariableDeclarationHandler : public StatementReversalHandler
{
public:
    VariableDeclarationHandler() { name_ = "Variable Declaration Handler"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

#endif	/* VARIABLEDECLARATIONHANDLER_H */

