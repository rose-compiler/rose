#include "j2c.h"
#include <algorithm>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Jovial_to_C;

extern vector<SgStatement*> statementList;
extern vector<SgNode*>      removeList;

/******************************************************************************************************************************/
/*
  Rename the output filename to .C file
  Replace the output file name to rose_j2c_*.C"
 */
/******************************************************************************************************************************/
void Jovial_to_C::translateFileName(SgFile* sourceFile)
{
   string outputFilename = sourceFile->get_sourceFileNameWithoutPath();
   size_t found;

   if (SgProject::get_verbose() > 2)
     {
        std::cout << "found file name: " << sourceFile->get_sourceFileNameWithoutPath() << std::endl;
     }
// Search for *.jov (for now)
   found = outputFilename.find(".jov");
   ROSE_ASSERT(found != string::npos);
    
   outputFilename.replace(found, 4, ".c");
   outputFilename = "rose_j2c_" + outputFilename;
   if (SgProject::get_verbose() > 2)
     {
        std::cout << "New output name: " << outputFilename  << std::endl;
     }

// set the output filename
   sourceFile->set_unparse_output_filename(outputFilename);
   sourceFile->set_inputLanguage (SgFile::e_C_language);
   sourceFile->set_outputLanguage(SgFile::e_C_language);
   sourceFile->set_C_only(true);
}


/******************************************************************************************************************************/
/* 
  Translate SgProgramHeaderStatement in Jovial into SgFunctionDeclaration in C.
  The main program in Jovial will become main function in C.
*/
/******************************************************************************************************************************/
void Jovial_to_C::translateProgramHeaderStatement(SgProgramHeaderStatement* programHeaderStatement)
{
// Get scopeStatement from SgProgramHeaderStatement
   SgScopeStatement* scopeStatement = programHeaderStatement->get_scope();
   ROSE_ASSERT(scopeStatement);
  
// Get ParameterList and DecoratorList
   SgFunctionParameterList* functionParameterList = buildFunctionParameterList(); 
   SgExprListExp* decoratorList = deepCopy(programHeaderStatement->get_decoratorList());
  
// Reuse FunctionDefinition from Fortran programHeaderStatement
   SgFunctionDefinition* functionDefinition = programHeaderStatement->get_definition();
  
// Get basicBlock from SgProgramHeaderStatement
   SgBasicBlock* basicBlock = functionDefinition->get_body();
   ROSE_ASSERT(basicBlock);

   SgSymbolTable* symbolTable = basicBlock->get_symbol_table();
   ROSE_ASSERT(symbolTable);
  
// The main function return type is int
   SgType* mainType = SgTypeInt::createType();
  
// Remove original function symbol.  Keep the new function symbol with name of "main"
   SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(scopeStatement->lookup_symbol(programHeaderStatement->get_name()));
   SgSymbolTable* globalSymbolTable = isSgSymbolTable(functionSymbol->get_parent());
   globalSymbolTable->remove(functionSymbol);
   functionSymbol->set_parent(NULL);
   delete(functionSymbol);
  
// Create SgFunctionDeclaration for C main function. Name must be "main".
   SgFunctionDeclaration* cFunctionDeclaration = buildDefiningFunctionDeclaration("main",
                                                                                  mainType,
                                                                                  functionParameterList,
                                                                                  scopeStatement);
  
// Setup the C function declaration.
   removeList.push_back(cFunctionDeclaration->get_definition());
   functionDefinition->set_parent(cFunctionDeclaration);
   cFunctionDeclaration->set_definition(functionDefinition);
   programHeaderStatement->set_definition(NULL);
 
// Replace the SgProgramHeaderStatement with SgFunctionDeclaration.
   replaceStatement(programHeaderStatement,cFunctionDeclaration,true);
   cFunctionDeclaration->set_decoratorList(decoratorList);
// cFunctionDeclaration->set_startOfConstruct(functionDefinition->get_startOfConstruct());
// cFunctionDeclaration->set_endOfConstruct(functionDefinition->get_endOfConstruct());
// cFunctionDeclaration->get_file_info()->set_physical_filename(cFunctionDeclaration->get_file_info()->get_filenameString()); 
  
   programHeaderStatement->set_parent(NULL);
}  // End of Jovial_to_C::translateProgramHeaderStatement
