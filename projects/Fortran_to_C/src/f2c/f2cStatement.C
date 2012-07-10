#include "f2cStatement.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;

/* 
   Rename the output filename to .C file
   Replace the output file name to rose_f2c_*.C"
*/
void Fortran_to_C::translateFileName(SgProject* project)
{
    Rose_STL_Container<SgNode*> sourceFileList = NodeQuery::querySubTree (project,V_SgFile);
    for (Rose_STL_Container<SgNode*>::iterator i = sourceFileList.begin(); i != sourceFileList.end(); i++)
    {
        SgFile* sourceFile = isSgFile(*i);
        string outputFilename = sourceFile->get_sourceFileNameWithoutPath();
        size_t found;

        if (SgProject::get_verbose() > 2)
        {
            std::cout << "find file name: " << sourceFile->get_sourceFileNameWithoutPath()  << std::endl;
        }
        // Search for *.F or *.f, both are valid for Fortran.
        found = outputFilename.find(".F");
        if (found == string::npos) {
            found = outputFilename.find(".f");
            ROSE_ASSERT(found != string::npos);
        }
        
        outputFilename.replace(found, 2, ".C");
        outputFilename = "rose_f2c_" + outputFilename;
        if (SgProject::get_verbose() > 2)
        {
            std::cout << "New output name: " << outputFilename  << std::endl;
        }
        // set the output filename
        sourceFile->set_unparse_output_filename(outputFilename);
        ROSE_ASSERT(sourceFile->get_unparse_output_filename().empty() == false);
    }
}

/* 
   Translate SgProgramHeaderStatement in Fortran into SgFunctionDeclaration in C.
   The main subroutine in Fortran will become main function in C.
*/
void Fortran_to_C::translateProgramHeaderStatement(SgProject* project)
{
    Rose_STL_Container<SgNode*> ProgramHeaderStatementList = NodeQuery::querySubTree (project,V_SgProgramHeaderStatement);
    for (Rose_STL_Container<SgNode*>::iterator i = ProgramHeaderStatementList.begin(); i != ProgramHeaderStatementList.end(); i++)
    {
        SgProgramHeaderStatement* ProgramHeaderStatement = isSgProgramHeaderStatement(*i);
        // Get scopeStatement from SgProgramHeaderStatement
        SgScopeStatement* scopeStatement = ProgramHeaderStatement->get_scope();
        ROSE_ASSERT(scopeStatement);

        // Get ParameterList and DecoratorList
        SgFunctionParameterList* functionParameterList = deepCopy(ProgramHeaderStatement->get_parameterList());
        SgExprListExp* decoratorList = deepCopy(ProgramHeaderStatement->get_decoratorList());

        // Copy FunctionDefinition from Fortran ProgramHeaderStatement, and delete old FunctionDefinition
        SgFunctionDefinition* fortranFunctionDefinition = ProgramHeaderStatement->get_definition();
        SgFunctionDefinition* CfunctionDefinition = deepCopy(fortranFunctionDefinition);
        ROSE_ASSERT(CfunctionDefinition);
        fortranFunctionDefinition->set_parent(NULL);
        delete(fortranFunctionDefinition);

        // Get fileInfo from SgProgramHeaderStatement
        Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
        SgBasicBlock* basicBlock = CfunctionDefinition->get_body();
        ROSE_ASSERT(basicBlock);

        // The main function return type is int
        SgType* mainType = SgTypeInt::createType();

        // Create SgFunctionDeclaration for C main function. 
        SgFunctionDeclaration* cMainFunction = buildDefiningFunctionDeclaration("main", mainType,functionParameterList,scopeStatement,decoratorList);
        ROSE_ASSERT(cMainFunction);

        // Setup the C function declaration.
        CfunctionDefinition->set_declaration(cMainFunction);
        cMainFunction->set_definition(CfunctionDefinition);

        // Add return statement to the end of main function.  Return 0 for main funciton.
        SgIntVal* returnVal = buildIntVal(0);
        ROSE_ASSERT(returnVal);
        SgReturnStmt* returnStmt = buildReturnStmt(returnVal);
        ROSE_ASSERT(returnStmt);
        returnVal->set_parent(returnStmt);
        basicBlock->get_statements().insert(basicBlock->get_statements().end(),returnStmt);
        returnStmt->set_parent(basicBlock);

        // The return value becomes the end of Construct.
        returnVal->set_endOfConstruct(fileInfo);

        // Replace the SgProgramHeaderStatement with SgFunctionDeclaration.
        replaceStatement(ProgramHeaderStatement,cMainFunction,true);

        // Remove the original symbol from symbol table and scopeStatement
        SgSymbol* symbol = scopeStatement->lookup_symbol(ProgramHeaderStatement->get_name());
        symbol->set_parent(NULL);
        scopeStatement->remove_symbol(symbol);
        delete(symbol);

        // Deep delete the original Fortran SgProgramHeaderStatement
        deepDelete(ProgramHeaderStatement);
    }
}  // End of Fortran_to_C::translateProgramHeaderStatement

/* 
   Translate SgProcedureHeaderStatement in Fortran into SgFunctionDeclaration in C.
   The subroutine in Fortran will become function in C.
*/
void Fortran_to_C::translateProcedureHeaderStatement(SgProject* project)
{

    Rose_STL_Container<SgNode*> procedureHeaderStatementList = NodeQuery::querySubTree (project,V_SgProcedureHeaderStatement);
    for (Rose_STL_Container<SgNode*>::iterator i = procedureHeaderStatementList.begin(); i != procedureHeaderStatementList.end(); i++)
    {
        SgProcedureHeaderStatement* ProcedureHeaderStatement = isSgProcedureHeaderStatement(*i);

        // We only handles Fortran function and Fortran subroutine
        ROSE_ASSERT(ProcedureHeaderStatement->isFunction() || ProcedureHeaderStatement->isSubroutine());

        // Get scopeStatement from SgProcedureHeaderStatement
        SgScopeStatement* scopeStatement = ProcedureHeaderStatement->get_scope();
        ROSE_ASSERT(scopeStatement);

        // Get ParameterList and DecoratorList
        SgFunctionParameterList* functionParameterList = deepCopy(ProcedureHeaderStatement->get_parameterList());
        SgExprListExp* decoratorList = deepCopy(ProcedureHeaderStatement->get_decoratorList());
        
        // Get the return variable from Fortran, name is same as function name.
        SgInitializedName* fortranReturnVar = ProcedureHeaderStatement->get_result_name();
        ROSE_ASSERT(fortranReturnVar);

        // Reuse FunctionDefinition from Fortran ProcedureHeaderStatement
        SgFunctionDefinition* fortranFunctionDefinition = ProcedureHeaderStatement->get_definition();
        SgFunctionDefinition* CfunctionDefinition = deepCopy(fortranFunctionDefinition);
        delete(fortranFunctionDefinition); 

        Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
        
        // Get basicBlock from SgProcedureHeaderStatement
        SgBasicBlock* basicBlock = CfunctionDefinition->get_body();
        ROSE_ASSERT(basicBlock);

        // Get the function name from Fortran
        SgName functionName = ProcedureHeaderStatement->get_name();

        /* 
           Get the return function type from Fortran.
           Subroutine has only void return type. 
        */
        SgType* functionType = ProcedureHeaderStatement->get_type()->get_return_type(); 
        
        // Create SgFunctionDeclaration for C function. 
        SgFunctionDeclaration* functionDeclaration = buildDefiningFunctionDeclaration(functionName,functionType,functionParameterList,scopeStatement,decoratorList);

        // Setup the C function declaration.
        CfunctionDefinition->set_declaration(functionDeclaration);
        functionDeclaration->set_definition(CfunctionDefinition);

        // If it is a Fortran function, then translator needs to declare its return variable and insert return statement at the end.
        if(ProcedureHeaderStatement->isFunction())
        {
            // Create C return variable, based on the Fortran return variable name and type.
            const SgName fortranReturnVarName = fortranReturnVar->get_name();
            SgVariableDeclaration* variableDeclaration = buildVariableDeclaration(fortranReturnVarName, fortranReturnVar->get_type(), NULL, basicBlock);
            ROSE_ASSERT(variableDeclaration);
            variableDeclaration->set_firstNondefiningDeclaration(variableDeclaration);
    
            // patch the required information for new variable declaration
            fixVariableDeclaration(variableDeclaration,basicBlock); 
    
            // Insert return variable declaration into beginning of basic block
            basicBlock->get_statements().insert(basicBlock->get_statements().begin(),variableDeclaration);
       
            // Build VarRefExp for the return statement.  The return varaible has same name as the Fortran function.
            SgVarRefExp* VarRefExp = buildVarRefExp(fortranReturnVarName,basicBlock);
            ROSE_ASSERT(VarRefExp);
    
            // Add return statement to the end of C function.
            SgReturnStmt* returnStmt = buildReturnStmt(VarRefExp);
            ROSE_ASSERT(returnStmt);
            basicBlock->get_statements().insert(basicBlock->get_statements().end(),returnStmt);
            returnStmt->set_parent(basicBlock);
       
            // The return value becomes the end of Construct.
            VarRefExp->set_endOfConstruct(fileInfo);
        }
        // Replace the SgProcedureHeaderStatement with SgFunctionDeclaration.
        replaceStatement(ProcedureHeaderStatement,functionDeclaration,true);

        // Remove the original symbol from symbol table and scopeStatement
        SgSymbol* symbol = scopeStatement->lookup_symbol(ProcedureHeaderStatement->get_name());
        symbol->set_parent(NULL);
        scopeStatement->remove_symbol(symbol);
        delete(symbol);

        // Deep delete the original Fortran ProcedureHeaderStatement.
        deepDelete(ProcedureHeaderStatement);
    }
}  // End of Fortran_to_C::translateProcedureHeaderStatement
