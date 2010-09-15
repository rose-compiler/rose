#ifndef REDEFINEVALUERESTORER_H
#define	REDEFINEVALUERESTORER_H

#include "handlerTypes.h"

/** The redefine technique re-executes the reaching definition to obtain the value of a variable. */
class RedefineValueRestorer : public VariableValueRestorer
{
	/**
	* Given a variable and a version, returns an expression evaluating to the value of the variable
	* at the given version.
	*
	* @param variable name of the variable to be restored
	* @param availableVariables variables whose values are currently available
	* @param definitions the version of the variable which should be restored
	* @return expessions that when evaluated will produce the desired version of the variable
	*/
	virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable,
			const VariableVersionTable& availableVariables, VariableRenaming::NumNodeRenameEntry definitions);

private:

	//! Handle +=, -=, */, etc
	std::vector<SgExpression*> handleUseAssignDefinition(VariableRenaming::VarName variable,
			const VariableVersionTable& availableVariables, SgNode* reachingDefinition);

	//! The reaching definition is ++ or --
	std::vector<SgExpression*> handleIncrementDecrementDefinition(VariableRenaming::VarName variable,
			const VariableVersionTable& availableVariables, SgNode* reachingDefinition);

	//! The reaching definition is an assign op or an initialized name.
	//! An initialized name with a SgAssignInitializer is pretty much identical to an assign op
	std::vector<SgExpression*> handleAssignOp(VariableRenaming::VarName variable,
			const VariableVersionTable& availableVariables, SgNode* reachingDefinition);
};

#endif	/* REDEFINEVALUERESTORER_H */

