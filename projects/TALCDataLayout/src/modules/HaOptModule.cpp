/*
 * HaOptModule.cpp
 *
 *  Created on: Oct 29, 2012
 *      Modified Code: Kamal Sharma
 *      Original Code: Jeff Keasler
 */

#include "HaOptModule.hpp"
#include "TALCAttribute.cpp"

/**
 * Hardware Optimizer constructor
 * @param args Command Line arguments
 * @param meta Meta file handler
 */
HaOptModule::HaOptModule(Rose_STL_Container<string> &args, Meta *meta)
: isVisit(false), compiler(CompilerTypes::UNKNOWN)
{
	handleModuleOptions(args);
	setMeta(meta);
}

/**
 * Internal Method to invoke pragma processing
 */
void HaOptModule::processPragma(SgProject *project) {
	HAOPTUtil::DoPragma(project, compiler);
}

/**
 * Internal Method to process variable declarations
 * This parses the TALC attribute to see if the struct
 * was created by AOSModule or tries to check if Meta
 * exist for a given variable
 * @param functionDeclaration
 */
void HaOptModule::processVariableDecls(
		SgFunctionDeclaration *functionDeclaration) {
	PrintUtil::printFunctionStart("HaOptModule::processVariableDecls");

	SgBasicBlock *functionBody =
			functionDeclaration->get_definition()->get_body();
	ROSE_ASSERT(functionBody);

	Rose_STL_Container<SgVariableDeclaration*> varDeclList =
			querySubTree<SgVariableDeclaration>(functionBody, V_SgVariableDeclaration);
	foreach(SgVariableDeclaration * varDecl, varDeclList)
	{
		bool isTALCStructDecl = false;
		// Check if TALC Attribute exist
		AstAttribute *astattribute = varDecl->getAttribute("TALCAttribute");
		TALCAttribute *talcAttribute =
				dynamic_cast<TALCAttribute*>(astattribute);
		if (talcAttribute != NULL) {
			trace << " TALC Attribute is not NULL " << endl;
			isTALCStructDecl = talcAttribute->isStructDecl();
		}

		SgInitializedNamePtrList &parameterList = varDecl->get_variables();
		foreach(SgInitializedName * initializedName, parameterList)
		{
			trace << " Init Name : " << initializedName->get_name().str()
							<< endl;

			if (isTALCStructDecl) {
				HAOPTUtil::DoStructDecl(initializedName, applyRestrict);
				continue;
			}

			// Check if Meta Field exists
			Field *metaField = meta->getField(
					initializedName->get_name().str());
			if (metaField == NULL)
				continue;
			trace << " Meta Field found " << endl;

			HAOPTUtil::DoPointer(initializedName, varDecl, compiler,
					applyRestrict)
			|| HAOPTUtil::DoArray(initializedName, varDecl, compiler);
		}
	}

	PrintUtil::printFunctionEnd("HaOptModule::processVariableDecls");
}

/**
 * Internal Method to handle function parameters
 * @param functionDeclaration
 */
void HaOptModule::processParameters(
		SgFunctionDeclaration *functionDeclaration) {
	PrintUtil::printFunctionStart("HaOptModule::processParameters");

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

		// Check if Meta Field exists
		Field *metaField = meta->getField(initializedName->get_name().str());
		if (metaField == NULL)
			continue;
		trace << " Meta Field found " << endl;

		HAOPTUtil::DoPointer(initializedName, functionBody, compiler,
				applyRestrict);
	}

	PrintUtil::printFunctionEnd("HaOptModule::processParameters");
}

/**
 * Internal method to process functions
 * @param project
 */
void HaOptModule::process(SgProject *project) {

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
			processVariableDecls(functionDeclaration);
		}
	}
	processPragma(project);
}

/**
 * Hardware Optimization visitor
 * @param project
 */
void HaOptModule::visit(SgProject *project) {

	PrintUtil::printStart("HaOptModule");

	if (isVisit) {
		if (compiler == CompilerTypes::UNKNOWN) {
			traceerror<< " Unknown Compiler Type " << endl;
			exit(1);
		}
		process(project);
	}

	PrintUtil::printEnd("HaOptModule");
}

/**
 * Private method to handle different options
 * @param args
 */
void HaOptModule::handleModuleOptions(Rose_STL_Container<string> &args)
{
	if (CommandlineProcessing::isOption(args, "-module:", "ha",
			true)) {
		isVisit = true;
	}

	if ( CommandlineProcessing::isOption(
			args, "-ha:", "(r|restrict)", true) )
	{
		applyRestrict = true;
	}

	if ( CommandlineProcessing::isOption(
			args, "-ha:", "(g|gcc)", true) )
	{
		compiler = CompilerTypes::GCC;
	}

	if ( CommandlineProcessing::isOption(
			args, "-ha:", "(i|icc)", true) )
	{
		compiler = CompilerTypes::ICC;
	}

	if ( CommandlineProcessing::isOption(
			args, "-ha:", "(x|xlc)", true) )
	{
		compiler = CompilerTypes::XLC;
	}

	if ( CommandlineProcessing::isOption(
			args, "-ha:", "(p|pgi)", true) )
	{
		compiler = CompilerTypes::PGI;
	}

}
