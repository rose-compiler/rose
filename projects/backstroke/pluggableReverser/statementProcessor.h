#ifndef REVERSE_COMPUTATION_STMT_PROCESSOR_H
#define REVERSE_COMPUTATION_STMT_PROCESSOR_H

#include <rose.h>
#include "eventProcessor.h"

class BasicStatementProcessor : public StatementProcessor
{
    StatementReversalVec processReturnStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    StatementReversalVec processExprStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    StatementReversalVec processVariableDeclaration(SgStatement* stmt, const VariableVersionTable& var_table);

    StatementReversalVec processBasicBlock(SgStatement* stmt, const VariableVersionTable& var_table);


public:
    virtual StatementReversalVec process(SgStatement* stmt, const VariableVersionTable& var_table);
};


/** Handles return statements. The reverse of a return statement is a no-op. */
class ReturnStatementProcessor : public StatementProcessor
{
	public:
		virtual StatementReversalVec process(SgStatement* stmt, const VariableVersionTable& var_table);
};

#endif
