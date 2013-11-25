/*
 * AOSModuleCheck.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: Kamal Sharma
 */

#include "AOSModuleCheck.hpp"


/**
 * Array of Struct Check Constructor
 * @param args Command line arguments
 */
AOSModuleCheck::AOSModuleCheck(Rose_STL_Container<string> &args, Meta *meta)
: isVisit(false)
{
	handleModuleOptions(args);
	setMeta(meta);
}


/**
 * Processes parameters passed to a function
 * @param functionDeclaration
 */
void AOSModuleCheck::checkParameterTypes(SgFunctionDeclaration *functionDeclaration) {
	PrintUtil::printFunctionStart("AOSModuleCheck::checkParameterTypes");

	SgBasicBlock *functionBody =
			functionDeclaration->get_definition()->get_body();
	ROSE_ASSERT(functionBody);

	SgFunctionParameterList *parameters =
			functionDeclaration->get_parameterList();
	ROSE_ASSERT (parameters);

	SgInitializedNamePtrList & parameterList = parameters->get_args();
	foreach(SgInitializedName * initializedName, parameterList)
	{
		SgType *paramType = initializedName->get_type();
		string type = TransformationSupport::getTypeName(paramType->stripType());

		trace << " Function Parameter :" << initializedName->get_name().str()
				<< " Type: " << type << endl;

		Field *metaField = meta->getField(initializedName->get_name().str());
		if (metaField == NULL)
			continue;
		trace << " Meta Field found ... type: " << endl;
		metaField->print();

		if(metaField->getTypeString() != type)
		{
			traceerror << " Meta Field " << initializedName->get_name() << " found in function parameter " << functionDeclaration->get_name() << " but type doesn't conform to meta file" << endl;
			exit(1);
		}
	}

	PrintUtil::printFunctionEnd("AOSModuleCheck::checkParameterTypes");
}

bool AOSModuleCheck::checkExtendedVariable(SgInitializedName *var) {
	if(!extendedTypes)
		return false;

	// Check if meta field exist
	Field *metaField = meta->getField(var->get_name().str());
	if (metaField == NULL)
		return false;

	// Check if pointer type
	SgType *type = isSgType(var->get_type());
	if(!isSgPointerType(type))
		return false;

	// Check if variable matches meta type
	if(metaField->getTypeString().compare(type->stripType()->unparseToString().c_str()) == 0)
		return true;

	return false;
}

/**
 * Processes Allocations within a function
 * e.g. new int => AllocateInt
 * This function call is necessary for runtime allocation
 * @param functionDeclaration
 */
void AOSModuleCheck::checkAllocation(SgFunctionDeclaration *functionDeclaration) {

	if (!extendedTypes)
		return;

	PrintUtil::printFunctionStart("AOSModuleCheck::processAllocation");

	SgBasicBlock *functionBody =
			functionDeclaration->get_definition()->get_body();
	ROSE_ASSERT(functionBody);

	// Look for any 'new' operators associated with variables */
	Rose_STL_Container<SgNewExp*> newExpressionList =
	querySubTree<SgNewExp>(functionBody, V_SgNewExp);
	foreach(SgNewExp * newExp, newExpressionList)
	{
		SgAssignInitializer *assignInitializer = isSgAssignInitializer(
				newExp->get_parent());
		if (assignInitializer == NULL)
			continue;

		SgInitializedName *variable = isSgInitializedName(
				assignInitializer->get_parent());
		if (variable == NULL)
			continue;

		SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(
				variable->get_parent());
		if (variableDeclaration == NULL)
			continue;

		trace << " Variable Name: " << variable->get_name() << " Type:" << TransformationSupport::getTypeName(variable->get_type()->stripType()) << endl;

		// Get Field Pointer for variable name
		Field *metaField = meta->getField(variable->get_name().str());
		if (metaField == NULL)
			continue;

		trace << " Meta Field found " << metaField->getTypeString() << endl;

		// check for type mismatch
		if(TransformationSupport::getTypeName(variable->get_type()->stripType()) != metaField->getTypeString())
		{
			traceerror << " Error :: Meta Type : "<< metaField->getTypeString() << " and allocation type : " << variable->get_type()->stripType() << " mismatch. " << endl;
			exit(1);
		}

		// type matches
		if (variable->get_type()->stripType()
				!= newExp->get_specified_type()->stripType())
			continue;
	}

	PrintUtil::printFunctionEnd("AOSModuleCheck::processAllocation");
}


void AOSModuleCheck::checkActualFormalParameters(SgFunctionDeclaration *functionDeclaration)
{
	PrintUtil::printFunctionStart("AOSModuleCheck::checkActualFormalParameters");

	SgBasicBlock *functionBody =
				functionDeclaration->get_definition()->get_body();
	ROSE_ASSERT(functionBody);

	// Loop through all the function call arguments and
	// compare it with the function declaration parameters
	// If either of them is a meta field but not both then
	// throw an error
	Rose_STL_Container<SgFunctionCallExp*> functionCallList =
	querySubTree<SgFunctionCallExp>(functionBody, V_SgFunctionCallExp);
	foreach(SgFunctionCallExp * functionCall, functionCallList)
	{
		int paramNo = -1;
		trace << "Function Calls " << functionCall->unparseToString() << endl;

		SgFunctionDeclaration *functionCallDecl = functionCall->getAssociatedFunctionDeclaration();
		if(functionCallDecl==NULL)
			continue;
		SgFunctionParameterList *parameters =
				functionCallDecl->get_parameterList();
		ROSE_ASSERT (parameters);
		SgInitializedNamePtrList & parameterList = parameters->get_args();

		SgExprListExp *exprList = functionCall->get_args();
		SgExpressionPtrList & expressionList = exprList->get_expressions();
		foreach(SgExpression *exp, expressionList)
		{
			paramNo++;
			trace << "Expression " << exp->unparseToString() << endl;
			SgVarRefExp * varRef = isSgVarRefExp(exp);
			if (varRef == NULL)
				continue;
			string variableName = varRef->get_symbol()->get_name().str();

			bool isFunctionCallParamMeta = false;
			// Get Field Pointer for variable name
			Field *metaFieldCall = meta->getField(variableName);
			if (metaFieldCall != NULL)
			{
				trace << " Meta Field found " << endl;
				isFunctionCallParamMeta = true;
			}

			SgInitializedName *initializedName = parameterList.at(paramNo);
			ROSE_ASSERT (initializedName);
			SgType *paramType = initializedName->get_type();
			string type = TransformationSupport::getTypeName(paramType->stripType());

			trace << " Function Parameter :" << initializedName->get_name().str()
					<< " Type: " << type << endl;

			bool isFunctionDeclParamMeta = false;
			Field *metaFieldParam = meta->getField(initializedName->get_name().str());
			if (metaFieldParam != NULL)
			{
				trace << " Meta Field found " << endl;
				isFunctionDeclParamMeta = true;
			}

			if(isFunctionDeclParamMeta != isFunctionCallParamMeta)
			{
				traceerror << " Error: " << functionCall->unparseToString() << " parameter : " <<
						variableName << " and declaration parameter : " << initializedName->get_name().str()
						<< " mismatch. " << endl;
				exit(1);
			}
		}
	}

	PrintUtil::printFunctionEnd("AOSModuleCheck::checkActualFormalParameters");
}

void AOSModuleCheck::checkViewSpace(SgFunctionDeclaration *functionDeclaration)
{
	PrintUtil::printFunctionStart("AOSModuleCheck::checkViewSpace");

	SgBasicBlock *functionBody =
				functionDeclaration->get_definition()->get_body();
	ROSE_ASSERT(functionBody);

	// Loop through all the statements
	Rose_STL_Container<SgStatement*> statementList =
	querySubTree<SgStatement>(functionBody, V_SgStatement);
	foreach(SgStatement *statement, statementList)
	{
		Rose_STL_Container<SgAssignOp*> assignOpList =
					querySubTree<SgAssignOp>(statement, V_SgAssignOp);
		trace << " assignOpList: " << assignOpList.size() << endl;
		if(assignOpList.size() == 0)
			continue;

		string viewName = "";
		Rose_STL_Container<SgVarRefExp*> varRefList =
			querySubTree<SgVarRefExp>(statement, V_SgVarRefExp);
		foreach(SgVarRefExp * varRef, varRefList)
		{
			string variableName = varRef->get_symbol()->get_name().str();

			// Get Field Pointer for variable name
			Field *metaField = meta->getField(variableName);
			if (metaField == NULL)
				continue;

			trace << " Meta Field found " << endl;

			if(viewName == "")
			{
				viewName = metaField->getFieldBlock()->getView()->getViewName();
				continue;
			}

			if(metaField->getFieldBlock()->getView()->getViewName() != viewName)
			{
				int stmtNo = getEnclosingStatement(varRef)->get_file_info()->get_line();
				traceerror << " Warning : Views " << viewName << " and " <<
						metaField->getFieldBlock()->getView()->getViewName() <<
						" mistmatch on Source Line " << stmtNo << endl;
			}
		}
	}

	PrintUtil::printFunctionEnd("AOSModuleCheck::checkViewSpace");
}

/**
 * Process the Project by visiting each function in
 * source files
 * @param project SageProject
 */
void AOSModuleCheck::process(SgProject *project) {

	// For each source file in the project
	SgFilePtrList & fileList = project->get_fileList();
	foreach(SgFile * file, fileList)
	{
		SgSourceFile * sourceFile = isSgSourceFile(file);
		ROSE_ASSERT(sourceFile);

		Rose_STL_Container<SgFunctionDeclaration*> fileFunctionList =
		querySubTree<SgFunctionDeclaration>(sourceFile, V_SgFunctionDeclaration);

		functionList.clear();
		foreach(SgFunctionDeclaration * functionDeclaration, fileFunctionList)
		{
			// Check for function definition
			SgFunctionDefinition *functionDefintion =
					functionDeclaration->get_definition();
			if (functionDefintion == NULL)
				continue;

			// Ignore functions in system headers, Can keep them to test robustness
			if (functionDefintion->get_file_info()->get_filename()
					!= sourceFile->get_file_info()->get_filename())
				continue;

			functionList.push_back(functionDeclaration);

			// Now, process this function
			PrintUtil::printHeader(functionDeclaration->get_name());

			checkActualFormalParameters(functionDeclaration);
			checkParameterTypes (functionDeclaration);
			checkAllocation(functionDeclaration);
			checkViewSpace(functionDeclaration);
		}
	}

}

/**
 * AOSModuleCheck visitor
 * @param project ROSE Sage Project
 */
void AOSModuleCheck::visit(SgProject *project) {

	PrintUtil::printStart("AOSModuleCheck");

	if (isVisit)
		process(project);

	PrintUtil::printEnd("AOSModuleCheck");
}

/**
 * Private method to handle different options
 * @param args
 */
void AOSModuleCheck::handleModuleOptions(Rose_STL_Container<string> &args)
{
	if ((CommandlineProcessing::isOption(args, "-module:", "aos", false))
			&& (CommandlineProcessing::isOption(args, "-check", "*", true))) {
		isVisit = true;
	}

	if (CommandlineProcessing::isOption(args, "-extend", "*", false)) {
		extendedTypes = true;
	}
}
