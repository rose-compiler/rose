/*
 * AOSModule.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: Kamal Sharma
 */

#include "AOSModule.hpp"

/*
 * Algorithm Overview:
 * 		Visit Each Source File [process]
 * 			Visit Each Function [process]
 * 				process Parameters [processParameters]
 * 				process Allocation [processAllocation] ; //new int[10]=>AllocateInt(...)
 * 				process Variables References [processReferences]
 */

/**
 * Array of Struct Constructor
 * @param args Command line arguments
 */
AOSModule::AOSModule(Rose_STL_Container<string> &args, Meta *meta)
: isVisit(false)
{
	handleModuleOptions(args);
	setMeta(meta);
	structNoMap = new map<FieldBlock*,int>();
}

/**
 * Checks if building struct is fine
 * @param field
 * @return true - building struct is fine
 */
bool AOSModule::isBuildStruct(Field *field) {

	// Check if this field block has only one field
	if (field->getFieldBlock()->isSingleFieldBlock())
		return false;

	// Check if struct is already built
	map<FieldBlock*, int>::iterator structMapIter = structNoMap->find(
			field->getFieldBlock());
	if (structMapIter != structNoMap->end())
		return false;

	return true;
}

/**
 * Returns struct number from local data structure
 * @param field
 * @return struct number
 */
int AOSModule::getStructNo(Field *field) {

	map<FieldBlock*, int>::iterator structMapIter = structNoMap->find(
			field->getFieldBlock());

	if (structMapIter == structNoMap->end())
	{
		traceerror << " Struct doesn't exist " << endl;
		exit(1);
	}

	return (*structMapIter).second;
}

void AOSModule::buildStructAtStart(Field *metaField, SgBasicBlock *block) {

	// Build the struct fb<n> {...};
	trace << " Building Struct " << endl;
	int structNo = structNoMap->size();
	StructUtil::buildStructDeclarationAtStart(metaField, structNo,
			block);

	// Insert field in structMap
	structNoMap->insert(
			pair<FieldBlock*, int>(metaField->getFieldBlock(),
					structNo));

	sourceCodeStat += metaField->getFieldBlock()->getSize() + 6;
}

/**
 * Processes parameters passed to a function
 * @param functionDeclaration
 */
void AOSModule::processParameters(SgFunctionDeclaration *functionDeclaration) {
	PrintUtil::printFunctionStart("AOSModule::processParameters");

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
		string type = TransformationSupport::getTypeName(paramType);

		trace << " Function Parameter :" << initializedName->get_name()
				<< " Type: " << type << endl;

		Field *metaField = meta->getField(initializedName->get_name().str());
		if (metaField == NULL)
			continue;
		trace << " Meta Field found " << endl;

		if(!checkExtendedVariable(initializedName))
			continue;

		// Set the appropriate type as there may be
		// parameters with const keyword which causes exception
		// in creating structs at runtime
		initializedName->set_type(buildParameterType(metaField));

		// if struct exist continue
		if (!isBuildStruct(metaField))
			continue;

		// Build a structure at the start for function
		buildStructAtStart(metaField, functionBody);
	}

	PrintUtil::printFunctionEnd("AOSModule::processParameters");
}

bool AOSModule::checkExtendedVariable(SgInitializedName *var) {
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
 * Internal Method to build Allocation String
 * @param field
 * @return allocation name
 */
string AOSModule::buildAllocationType(Field *field) {

	string allocateName;

	switch (field->getType()) {
	case FieldTypes::INT:
		allocateName = "AllocateInt";
		break;
	case FieldTypes::DOUBLE:
		allocateName = "AllocateDouble";
		break;
	case FieldTypes::FLOAT:
		allocateName = "AllocateFloat";
		break;
	default:
		trace << " Field Error Type " << endl;
		exit(1);
	}

	return allocateName;
}

SgType* AOSModule::buildParameterType(Field *field) {

	SgType *type;

	switch (field->getType()) {
	case FieldTypes::INT:
		type = new SgPointerType(new SgTypeInt());
		break;
	case FieldTypes::DOUBLE:
		type = new SgPointerType(new SgTypeDouble());
		break;
	case FieldTypes::FLOAT:
		type = new SgPointerType(new SgTypeFloat());
		break;
	default:
		trace << " Field Error Type " << endl;
		exit(1);
	}

	return type;
}

/**
 * Processes Allocations within a function
 * e.g. new int => AllocateInt
 * This function call is necessary for runtime allocation
 * @param functionDeclaration
 */
void AOSModule::processAllocation(SgFunctionDeclaration *functionDeclaration) {

	if (!extendedTypes)
		return;

	PrintUtil::printFunctionStart("AOSModule::processAllocation");

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

		trace << " Variable Name: " << variable->get_name() << endl;

		// Get Field Pointer for variable name
		Field *metaField = meta->getField(variable->get_name().str());
		if (metaField == NULL)
			continue;

		trace << " Meta Field found " << endl;

		// type matches
		if (variable->get_type()->stripType()
				!= newExp->get_specified_type()->stripType())
			continue;

		// Build the Allocate Function Call
		string allocationName = buildAllocationType(metaField);
		SgPointerType *ptr = isSgPointerType(newExp->get_type());
		ROSE_ASSERT(ptr);
		SgArrayType *arrayPtr = isSgArrayType(ptr->get_base_type());
		ROSE_ASSERT(arrayPtr);
		// argument list
		SgExprListExp *parameterList = buildExprListExp(buildIntVal(0),
				buildStringVal(variable->get_name().str()),
				arrayPtr->get_index());
		SgFunctionCallExp* newCallExp = buildFunctionCallExp(allocationName,
				variable->get_type(), parameterList, getScope(variable));

		// Replace the RHS
		assignInitializer->set_operand(isSgExpression(newCallExp));

		sourceCodeStat += 1;

		trace << " FuncCallExp :" << newCallExp->unparseToString() << endl;

		// Build Structure if necessary
		if (!isBuildStruct(metaField))
			continue;

		// Build the struct fb<n> {...};
		trace << " Building Struct " << endl;
		int structNo = structNoMap->size();
		StructUtil::buildStructDeclarationAfter(metaField, structNo,
				getEnclosingStatement(assignInitializer));

		// Insert field in structMap
		structNoMap->insert(
				pair<FieldBlock*, int>(metaField->getFieldBlock(),
						structNo));

		sourceCodeStat += metaField->getFieldBlock()->getSize() + 6;
	}

	PrintUtil::printFunctionEnd("AOSModule::processAllocation");
}

void AOSModule::processReferences(SgFunctionDeclaration *functionDeclaration) {
	PrintUtil::printFunctionStart("AOSModule::processReferences");

	vector<int> statementNoList;
	statementNoList.clear();

	SgBasicBlock *functionBody =
			functionDeclaration->get_definition()->get_body();
	ROSE_ASSERT(functionBody);

	Rose_STL_Container<SgVarRefExp*> varRefList =
	querySubTree<SgVarRefExp>(functionBody, V_SgVarRefExp);
	foreach(SgVarRefExp * varRef, varRefList)
	{
		string variableName = varRef->get_symbol()->get_name().str();

		// Get Field Pointer for variable name
		Field *metaField = meta->getField(variableName);
		if (metaField == NULL)
			continue;

		trace << " Meta Field found " << endl;

		// Check if this field block has only one field
		if (metaField->getFieldBlock()->isSingleFieldBlock())
			continue;

		/* Verify this is an array operation */
		SgPntrArrRefExp *arrRef = isSgPntrArrRefExp(varRef->get_parent());
		if (arrRef != NULL) {

			// Verify if the variable appear in lhs i.e. x[i] should be
			// processed and i[x] should not be
			if(arrRef->get_lhs_operand() != varRef)
				continue;

			trace << "Processing array reference " << arrRef->unparseToString() << endl;

			// if struct doesn't exist, build it
			if (isBuildStruct(metaField)) {
				// Build a structure at the start for function
				buildStructAtStart(metaField, functionBody);
			}

			// Update Source Code Statistics
			int stmtNo = getEnclosingStatement(arrRef)->get_file_info()->get_line();
			if(find (statementNoList.begin(), statementNoList.end(), stmtNo) != statementNoList.end())
			{
				statementNoList.push_back(stmtNo);
				sourceCodeStat +=1;
			}

			// Build a structure Reference to replace current expression
			SgExpression *structExp = StructUtil::buildStructReference(
					metaField, getStructNo(metaField), getScope(arrRef),
					arrRef->get_rhs_operand());
			replaceExpression(arrRef, structExp, true);
		}
	}

	PrintUtil::printFunctionEnd("AOSModule::processReferences");
}

/**
 * Process the Project by visiting each function in
 * source files
 * @param project SageProject
 */
void AOSModule::process(SgProject *project) {

	//Initialize Variables
	sourceCodeStat = 0;

	// For each source file in the project
	SgFilePtrList & fileList = project->get_fileList();
	foreach(SgFile * file, fileList)
	{
		SgSourceFile * sourceFile = isSgSourceFile(file);
		ROSE_ASSERT(sourceFile);

		Rose_STL_Container<SgFunctionDeclaration*> functionList =
		querySubTree<SgFunctionDeclaration>(sourceFile, V_SgFunctionDeclaration);

		foreach(SgFunctionDeclaration * functionDeclaration, functionList)
		{
			// Clear the Internal Data Structures
			structNoMap->clear();

			// Check for function definition
			SgFunctionDefinition *functionDefintion =
					functionDeclaration->get_definition();
			if (functionDefintion == NULL)
				continue;

			// Ignore functions in system headers, Can keep them to test robustness
			if (functionDefintion->get_file_info()->get_filename()
					!= sourceFile->get_file_info()->get_filename())
				continue;

			// Now, process this function
			PrintUtil::printHeader(functionDeclaration->get_name());

			processParameters (functionDeclaration);
			processAllocation(functionDeclaration);
			processReferences(functionDeclaration);
		}

	}

	trace << " Source Code Changes : " << sourceCodeStat << endl;
}

/**
 * AOSModule visitor
 * @param project ROSE Sage Project
 */
void AOSModule::visit(SgProject *project) {

	PrintUtil::printStart("AOSModule");

	if (isVisit)
		process(project);

	PrintUtil::printEnd("AOSModule");
}

/**
 * Private method to handle different options
 * @param args
 */
void AOSModule::handleModuleOptions(Rose_STL_Container<string> &args)
{
	if (CommandlineProcessing::isOption(args, "-module:", "aos",
					true)) {
		isVisit = true;
	}

	if (CommandlineProcessing::isOption(args, "-extend", "*",
					true)) {
		extendedTypes = true;
	}
}
