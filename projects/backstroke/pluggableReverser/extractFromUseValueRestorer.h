#ifndef EXTRACTFROMUSEVALUERESTORER_H
#define	EXTRACTFROMUSEVALUERESTORER_H

#include "handlerTypes.h"

class ExtractFromUseValueRestorer : public VariableValueRestorer
{
public:
	/**
	* Given a variable and a version, returns an expression evaluating to the value of the variable
	* at the given version.
	*
	* @param variable name of the variable to be restored
	* @param availableVariables variables whos values are currently available
	* @param definitions the version of the variable which should be restored
	* @return expessions that when evaluated will produce the desired version of the variable
	*/
	virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable,
			const VariableVersionTable& availableVariables, VariableRenaming::NumNodeRenameEntry definitions);

private:

	//! Given a variable used in an expression, and the value of that expresion, restore the value of the variable.
	//! @param variable the variable whose value should be extracted
	//! @param availableVariables versions of all the variables currently defined
	//! @param expression expression in which the variable is used
	//! @param expressionValue expression that evaluates to the value of the use expression, given the current variable versions
	SgExpression* restoreVariableFromExpression(VariableRenaming::VarName variable,
			const VariableVersionTable& availableVariables, SgExpression* expression, SgExpression* expressionValue);
};

#endif	/* EXTRACTFROMUSEVALUERESTORER_H */

