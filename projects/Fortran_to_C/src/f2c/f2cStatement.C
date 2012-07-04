#include "f2cStatement.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace AstFromString;

// Rename the output filename to .C file
void f2cFile(SgFile* file){
     // Replace the output file name to *_f2c.C"
     string outputFilename = file->get_sourceFileNameWithoutPath();
     size_t found;
     std::cout << "find file name: " << file->get_sourceFileNameWithoutPath()  << std::endl;
     found = outputFilename.find(".F");
     if (found == string::npos) {
       found = outputFilename.find(".f");
       ROSE_ASSERT(found != string::npos);
     }
     outputFilename.replace(found, 2, ".C");
     outputFilename = "rose_f2c_" + outputFilename;
     std::cout << "New output name: " << outputFilename  << std::endl;

     // set the output filename
     file->set_unparse_output_filename(outputFilename);
     ROSE_ASSERT(file->get_unparse_output_filename().empty() == false);
}

// Translate SgProgramHeaderStatement in Fortran into SgFunctionDeclaration in C.
// The main subroutine in Fortran will become main function in C.
void f2cProgramHeaderStatement(SgProgramHeaderStatement* ProgramHeaderStatement){
     // remove the SgProgramHeaderStatement symbol from the symbol table
     SgScopeStatement* scope = ProgramHeaderStatement->get_scope();
     SgSymbolTable* symtable = scope->get_symbol_table();
     ROSE_ASSERT(symtable);
     SgSymbol* sym = symtable->find(ProgramHeaderStatement);
     ROSE_ASSERT(sym);
     symtable->remove(sym);
    
     // Reuse FunctionDefinition from Fortran ProgramHeaderStatement
     SgFunctionDefinition* funcDef = ProgramHeaderStatement->get_definition();
     //Sg_File_Info* fileInfo = ProgramHeaderStatement->get_file_info();
     Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();

     // The main function return type is int
     SgFunctionType* functionType = new SgFunctionType(SgTypeInt::createType());       
     // Create SgFunctionDeclaration for C main function. 
     SgFunctionDeclaration* func = new SgFunctionDeclaration("main", functionType, NULL);
     // Setup the C function declaration.
     funcDef->set_declaration(func);
     func->set_definition(funcDef);
     func->set_scope(scope);
     func->set_file_info(fileInfo);
     func->set_parameterList(ProgramHeaderStatement->get_parameterList());
     func->set_decoratorList(ProgramHeaderStatement->get_decoratorList());
     // Replace the SgProgramHeaderStatement with SgFunctionDeclaration.
     replaceStatement(ProgramHeaderStatement,func,true);
     // Add return statement to the end of main subroutine.
     SgIntVal* returnVal = new SgIntVal(fileInfo,0,"");
     SgReturnStmt* returnStmt = new SgReturnStmt(fileInfo, returnVal);
     ROSE_ASSERT(returnStmt);
     SgBasicBlock* basicblock = funcDef->get_body();
     ROSE_ASSERT(basicblock);
     basicblock->get_statements().insert(basicblock->get_statements().end(),returnStmt);
     returnStmt->set_parent(basicblock);
     // The return value becomes the end of Construct.
     returnVal->set_endOfConstruct(fileInfo);
}

// Translate SgProcedureHeaderStatement in Fortran into SgFunctionDeclaration in C.
// The subroutine in Fortran will become function in C.

void f2cProcedureHeaderStatement(SgProcedureHeaderStatement* ProcedureHeaderStatement){
     // remove the SgProcedureHeaderStatement symbol from the symbol table
     SgScopeStatement* scope = ProcedureHeaderStatement->get_scope();
     SgSymbolTable* symtable = scope->get_symbol_table();
     ROSE_ASSERT(symtable);
     SgSymbol* sym = symtable->find(ProcedureHeaderStatement);
     ROSE_ASSERT(sym);
     symtable->remove(sym);
     SgName functionName = ProcedureHeaderStatement->get_name();
     SgInitializedName* returnVar = ProcedureHeaderStatement->get_result_name();
     ROSE_ASSERT(returnVar);
//     deleteAST(returnVar);
    
     // Reuse FunctionDefinition from Fortran ProcedureHeaderStatement
     SgFunctionDefinition* funcDef = ProcedureHeaderStatement->get_definition();
     //Sg_File_Info* fileInfo = ProcedureHeaderStatement->get_file_info();
     Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();

     SgFunctionType* functionType = ProcedureHeaderStatement->get_type(); 
     // Create SgFunctionDeclaration for C function. 
     SgFunctionDeclaration* func = new SgFunctionDeclaration(functionName, functionType, NULL);
     // Setup the C function declaration.
     funcDef->set_declaration(func);
     func->set_definition(funcDef);
     func->set_scope(scope);
     func->set_file_info(fileInfo);
     func->set_parameterList(ProcedureHeaderStatement->get_parameterList());
     func->set_decoratorList(ProcedureHeaderStatement->get_decoratorList());
     // Replace the SgProcedureHeaderStatement with SgFunctionDeclaration.
     SgBasicBlock* basicblock = funcDef->get_body();
     ROSE_ASSERT(basicblock);

     SgVariableSymbol* variableSymbol = new SgVariableSymbol(returnVar);
     SgVariableDeclaration* variableDeclaration = new SgVariableDeclaration(fileInfo, returnVar->get_name(), returnVar->get_type());
     variableDeclaration->set_firstNondefiningDeclaration(variableDeclaration);
     basicblock->get_statements().insert(basicblock->get_statements().end(),variableDeclaration);
     basicblock->insert_symbol(returnVar->get_name(),variableSymbol);

     replaceStatement(ProcedureHeaderStatement,func,true);

     // The return variable name for Fortran function is same as the function name
     SgVarRefExp* VarRefExp = new SgVarRefExp(fileInfo,variableSymbol);
     // Add return statement to the end of main subroutine.
     SgReturnStmt* returnStmt = new SgReturnStmt(fileInfo, VarRefExp);
     ROSE_ASSERT(returnStmt);
     basicblock->get_statements().insert(basicblock->get_statements().end(),returnStmt);
     returnStmt->set_parent(basicblock);
     // The return value becomes the end of Construct.
     VarRefExp->set_endOfConstruct(fileInfo);
}
