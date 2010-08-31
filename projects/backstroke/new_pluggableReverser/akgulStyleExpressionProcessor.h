#pragma once

#include <rose.h>
#include <boost/tuple/tuple.hpp>
#include "utilities/types.h"
#include "VariableRenaming.h"
#include <map>
#include <set>
#include <pluggableReverser/eventProcessor.h>

class AkgulStyleExpressionProcessor : public ExpressionProcessor
{
public:

	/** Attempts to reverse an expression. If the reversal fails,
          * this function returns an empty vector. */
	virtual std::vector<ExpressionReversal> process(SgExpression* exp, const VariableVersionTable& var_table,
                                                        bool isReverseValueUsed);

	/** Returns the variable name referred by the expression. Also returns
	  * the AST expression for referring to that variable (using the variable renaming analysis).
	  * Handles comma ops correctly. */
	static std::pair<VariableRenaming::VarName, SgExpression*> getReferredVariable(SgExpression* exp);

private:

	
        /** Reverses an assignment op. Returns true on success and false on failure.
         * @param reverseExpressions a list of expressions, to be executed in the specified order */
        bool handleAssignOp(SgAssignOp* varRef, const VariableVersionTable& availableVariables, SgExpression*& reverseExpressions);

	std::multimap<int, SgExpression*> collectUsesForVariable(VariableRenaming::VarName name, SgNode* node);
};

/** The redefine technique re-executes the reaching definition to obtain the value of a variable. */
class RedefineValueRestorer : public VariableValueRestorer
{
        /**
         * Given a variable and a version, returns an expression evaluating to the value of the variable
         * at the given version.
         *
         * @param variable name of the variable to be restored
         * @param availableVariables variables whos values are currently available
         * @return definitions the version of the variable which should be restored
         */
        virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
		VariableRenaming::NumNodeRenameEntry definitions);

private:

	static std::vector<SgExpression*> findVarReferences(VariableRenaming::VarName var, SgNode* root);
	
	/** Returns true if an expression calls any functions or modifies any variables. */
	static bool isModifyingExpression(SgExpression* expr, VariableRenaming* variableRenamingAnalysis);
};

class ExtractFromUseRestorer : public VariableValueRestorer
{
        /**
         * Given a variable and a version, returns an expression evaluating to the value of the variable
         * at the given version.
         *
         * @param variable name of the variable to be restored
         * @param availableVariables variables whos values are currently available
         * @return definitions the version of the variable which should be restored
         */
        virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
		VariableRenaming::NumNodeRenameEntry definitions);
};
