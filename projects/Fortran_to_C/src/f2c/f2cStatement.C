#include "f2cStatement.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;

/* 
   Rename the output filename to .C file
   Replace the output file name to rose_f2c_*.C"
*/
void Fortran_to_C::transFileName(SgFile* file)
{
     string outputFilename = file->get_sourceFileNameWithoutPath();
     size_t found;

     if (SgProject::get_verbose() > 2)
     {
       std::cout << "find file name: " << file->get_sourceFileNameWithoutPath()  << std::endl;
     }
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
     file->set_unparse_output_filename(outputFilename);
     ROSE_ASSERT(file->get_unparse_output_filename().empty() == false);
}

/* 
   Translate SgProgramHeaderStatement in Fortran into SgFunctionDeclaration in C.
   The main subroutine in Fortran will become main function in C.
*/
void Fortran_to_C::transProgramHeaderStatement(SgProgramHeaderStatement* ProgramHeaderStatement)
{
     // Get scopeStatement from SgProgramHeaderStatement
     SgScopeStatement* scopeStatement = ProgramHeaderStatement->get_scope();
     ROSE_ASSERT(scopeStatement);
     // Get symbolTable from SgProgramHeaderStatement
     SgSymbolTable* symbolTable = scopeStatement->get_symbol_table();
     ROSE_ASSERT(symbolTable);
     // Reuse FunctionDefinition from Fortran ProgramHeaderStatement
     SgFunctionDefinition* functionDefinition = deepCopy(ProgramHeaderStatement->get_definition());
     ROSE_ASSERT(functionDefinition);
     // Get fileInfo from SgProgramHeaderStatement
     Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
     SgBasicBlock* basicBlock = functionDefinition->get_body();
     ROSE_ASSERT(basicBlock);
     // Get ParameterList and DecoratorList
     SgFunctionParameterList* functionParameterList = deepCopy(ProgramHeaderStatement->get_parameterList());
     SgExprListExp* decoratorList = deepCopy(ProgramHeaderStatement->get_decoratorList());


     // remove the SgProgramHeaderStatement symbol from the symbol table
     SgSymbol* sym = symbolTable->find(ProgramHeaderStatement);
     ROSE_ASSERT(sym);
     symbolTable->remove(sym);
    

     // The main function return type is int
     SgType* mainType = SgTypeInt::createType();

     // Create SgFunctionDeclaration for C main function. 
     SgFunctionDeclaration* cMainFunction = buildDefiningFunctionDeclaration("main", mainType,functionParameterList,scopeStatement,decoratorList);
     //SgFunctionDeclaration* cMainFunction = new SgFunctionDeclaration("main", mainFunctionType, functionDefinition);

     // Setup the C function declaration.
     functionDefinition->set_declaration(cMainFunction);
     cMainFunction->set_definition(functionDefinition);

     // Add return statement to the end of main function.  Return 0 for main funciton.
     SgIntVal* returnVal = buildIntVal(0);
     SgReturnStmt* returnStmt = buildReturnStmt(returnVal);
     returnVal->set_parent(returnStmt);
     ROSE_ASSERT(returnStmt);
     basicBlock->get_statements().insert(basicBlock->get_statements().end(),returnStmt);
     returnStmt->set_parent(basicBlock);

     // The return value becomes the end of Construct.
     returnVal->set_endOfConstruct(fileInfo);

     // Replace the SgProgramHeaderStatement with SgFunctionDeclaration.
     replaceStatement(ProgramHeaderStatement,cMainFunction,true);
}

/* 
   Translate SgProcedureHeaderStatement in Fortran into SgFunctionDeclaration in C.
   The subroutine in Fortran will become function in C.
*/
void Fortran_to_C::transProcedureHeaderStatement(SgProcedureHeaderStatement* ProcedureHeaderStatement)
{
     // We only handles Fortran function and Fortran subroutine
     ROSE_ASSERT(ProcedureHeaderStatement->isFunction() || ProcedureHeaderStatement->isSubroutine());

     // remove the SgProcedureHeaderStatement symbol from the symbol table
     SgScopeStatement* scopeStatement = ProcedureHeaderStatement->get_scope();
     ROSE_ASSERT(scopeStatement);

     // Get symbolTable from SgProgramHeaderStatement
     SgSymbolTable* symbolTable = scopeStatement->get_symbol_table();
     ROSE_ASSERT(symbolTable);

     // Reuse FunctionDefinition from Fortran ProcedureHeaderStatement
     SgFunctionDefinition* functionDefinition = deepCopy(ProcedureHeaderStatement->get_definition());
     Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
     // Get basicBlock from SgProcedureHeaderStatement
     SgBasicBlock* basicBlock = functionDefinition->get_body();
     ROSE_ASSERT(basicBlock);

     // Get ParameterList and DecoratorList
     SgFunctionParameterList* functionParameterList = deepCopy(ProcedureHeaderStatement->get_parameterList());
     SgExprListExp* decoratorList = deepCopy(ProcedureHeaderStatement->get_decoratorList());

     // Get the function name from Fortran
     SgName functionName = ProcedureHeaderStatement->get_name();

     // Get the return variable from Fortran, name is same as function name.
     SgInitializedName* fortranReturnVar = ProcedureHeaderStatement->get_result_name();
     ROSE_ASSERT(fortranReturnVar);

     // remove the ProcedureHeaderStatement symbol from the symbol table
     SgSymbol* sym = symbolTable->find(ProcedureHeaderStatement);
     ROSE_ASSERT(sym);
     symbolTable->remove(sym);
   

     /* 
        Get the return function type from Fortran.
        Subroutine has only void return type. 
     */
     SgType* functionType = ProcedureHeaderStatement->get_type()->get_return_type(); 
     
     // Create SgFunctionDeclaration for C function. 
     SgFunctionDeclaration* functionDeclaration = buildDefiningFunctionDeclaration(functionName,functionType,functionParameterList,scopeStatement,decoratorList);


     // Setup the C function declaration.
     functionDefinition->set_declaration(functionDeclaration);
     functionDeclaration->set_definition(functionDefinition);

     if(ProcedureHeaderStatement->isFunction())
     {
         // Create C return variable, based on the Fortran return variable.
         const SgName fortranFnctionName = fortranReturnVar->get_name();
         //SgInitializedName* cReturnVar = buildInitializedName(fortranFnctionName,fortranReturnVar->get_type());
         SgVariableDeclaration* variableDeclaration = buildVariableDeclaration(fortranFnctionName, fortranReturnVar->get_type(), NULL, scopeStatement);
         variableDeclaration->set_firstNondefiningDeclaration(variableDeclaration);

         // patch the required information for new variable declaration
         fixVariableDeclaration(variableDeclaration,scopeStatement); 

         // Insert return variable declaration into beginning of basic block
         basicBlock->get_statements().insert(basicBlock->get_statements().begin(),variableDeclaration);
    
         // The return variable name for Fortran function is same as the function name
         //SgVarRefExp* VarRefExp = buildVarRefExp(cReturnVar,scopeStatement);

         // Add return statement to the end of C function.
         //SgReturnStmt* returnStmt = buildReturnStmt(VarRefExp);
         //ROSE_ASSERT(returnStmt);

         //basicBlock->get_statements().insert(basicBlock->get_statements().end(),returnStmt);
         //returnStmt->set_parent(basicBlock);
    
         // The return value becomes the end of Construct.
         //VarRefExp->set_endOfConstruct(fileInfo);
     }
     // Replace the SgProcedureHeaderStatement with SgFunctionDeclaration.
     replaceStatement(ProcedureHeaderStatement,functionDeclaration,true);
     
}
