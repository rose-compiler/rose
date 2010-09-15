#ifndef EXTRACTFROMUSEVALUERESTORER_H
#define	EXTRACTFROMUSEVALUERESTORER_H

#include "handlerTypes.h"

class ExtractFromUseValueRestorer : public VariableValueRestorer
{
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
};

#endif	/* EXTRACTFROMUSEVALUERESTORER_H */

