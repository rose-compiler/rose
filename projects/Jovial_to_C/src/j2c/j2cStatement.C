#include "j2c.h"
#include <algorithm>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Jovial_to_C;

extern vector<SgStatement*> statementList;
extern vector<SgNode*>      removeList;

/******************************************************************************************************************/
/*
  Rename the output filename to .C file
  Replace the output file name to rose_j2c_*.C"
 */
/******************************************************************************************************************/
void Jovial_to_C::translateFileName(SgFile* sourceFile)
{
   string outputFilename = sourceFile->get_sourceFileNameWithoutPath();
   size_t found;

   if (SgProject::get_verbose() > 2)
     {
        cout << "found file name: " << sourceFile->get_sourceFileNameWithoutPath() << endl;
     }

// First look for *.jov
   found = outputFilename.find(".jov");

   if (found == string::npos)
     {
     // File extension not .jov so look for .cpl
        found = outputFilename.find(".cpl");
     }

   if (found == string::npos)
     {
        std::cerr << "Jovial_to_C::translateFileName: file name doesn't end in .jov or cpl \n";
        ROSE_ASSERT(found != string::npos);
     }
    
   outputFilename.replace(found, 4, ".c");
   outputFilename = "rose_j2c_" + outputFilename;
   if (SgProject::get_verbose() > 2)
     {
        cout << "New output name: " << outputFilename  << endl;
     }

// set the output filename
   sourceFile->set_unparse_output_filename(outputFilename);
   sourceFile->set_inputLanguage (SgFile::e_C_language);
   sourceFile->set_outputLanguage(SgFile::e_C_language);
   sourceFile->set_C_only(true);
}


/******************************************************************************************************************/
/*
  Translate SgJovialCompoolStatement in Jovial into C.  For now (at least) all declarations in the compool module
  will go into global scope.
*/
/******************************************************************************************************************/
void Jovial_to_C::translateJovialCompoolStatement(SgJovialCompoolStatement* compoolStatement)
{
// Get scopeStatement from the SgJovialCompoolStatement
   SgScopeStatement* scopeStatement = compoolStatement->get_scope();
   ROSE_ASSERT(scopeStatement);

// The compool scope should be the global scope
   SgGlobal* global = isSgGlobal(scopeStatement);
   ROSE_ASSERT(global);
   ROSE_ASSERT(scopeStatement == global);

#if 0
   cout << ".x.  translate compool module: scope is " << scopeStatement << ": global scope is " << global << endl;
   cout << ".x.                      # declarations " << scopeStatement->getDeclarationList().size() << endl;
#endif

   SgSymbolTable* symbolTable = scopeStatement->get_symbol_table();
   ROSE_ASSERT(symbolTable);

// There shouldn't be a symbol (why not, should turn a compool module into a Fortran module thing)
   SgSymbol* compoolSymbol = scopeStatement->lookup_symbol(compoolStatement->get_name());
   ROSE_ASSERT(compoolSymbol == NULL);

#if 0
   cout << ".x.  translate compool module: compool object for name " << compoolStatement->get_name() << " is " << compoolStatement << endl;
   cout << ".x.                          : compool symbol for name " << compoolStatement->get_name() << " is " << compoolSymbol << endl;

// cout << ".x.                          : compool definition is   " << compoolStatement->get_definition() << endl;
   cout << ".x.                          : compool parent is       " << compoolStatement->get_parent() << endl;
   cout << ".x.                          : compool scope  is       " << compoolStatement->get_declarationScope() << endl;
   cout << ".x.                          : compool scope finfo     " << compoolStatement->get_file_info() << endl;
#endif

// parent of the compool statement is the global scope
   statementList.push_back(compoolStatement);
//   removeList.push_back(compoolStatement);

#if 0
// Setup the C function declaration.
   removeList.push_back(cFunctionDeclaration->get_definition());
   functionDefinition->set_parent(cFunctionDeclaration);
   cFunctionDeclaration->set_definition(functionDefinition);
   compoolStatement->set_definition(NULL);

// Replace the SgJovialCompoolStatement with SgFunctionDeclaration.
   replaceStatement(compoolStatement,cFunctionDeclaration,true);
   cFunctionDeclaration->set_decoratorList(decoratorList);
// cFunctionDeclaration->set_startOfConstruct(functionDefinition->get_startOfConstruct());
// cFunctionDeclaration->set_endOfConstruct(functionDefinition->get_endOfConstruct());
// cFunctionDeclaration->get_file_info()->set_physical_filename(cFunctionDeclaration->get_file_info()->get_filenameString());

#endif
}  // End of Jovial_to_C::translateJovialCompoolStatement


/******************************************************************************************************************/
/*
  Translate SgJovialTableStatement in Jovial into C.  A table will be translated to a C struct.
*/
/******************************************************************************************************************/
void Jovial_to_C::translateJovialTableStatement(SgJovialTableStatement* tableStatement)
{
// Get scopeStatement from SgJovialTableStatement
   SgScopeStatement* scopeStatement = tableStatement->get_scope();
   ROSE_ASSERT(scopeStatement);

// Reuse the class definition
   SgClassDefinition* tableDefinition = tableStatement->get_definition();

// Remove original class symbol.  Keep the new function symbol with name of "main"
   SgClassSymbol* classSymbol = isSgClassSymbol(scopeStatement->lookup_symbol(tableStatement->get_name()));
   ROSE_ASSERT(classSymbol);

#if 0
   SgSymbolTable* globalSymbolTable = isSgSymbolTable(classSymbol->get_parent());
   globalSymbolTable->remove(functionSymbol);
   functionSymbol->set_parent(NULL);
   delete(functionSymbol);
#endif

// Create SgClassDeclaration for the C struct.
   std::string old_name = tableStatement->get_name();
   std::string new_name = tableStatement->get_name();
   boost::replace_all(new_name, "'", "_");

// TODO - with a new name we can just build a new type declaration with a new symbol
//      - need to actually figure out how to replace symbol AND type in case of references to type
// ------------ THIS MUST BE FIXED --------------------
   ROSE_ASSERT(new_name != old_name);

// cout << ".x. will build class declaration with new type name " << new_name << endl;

   SgClassDeclaration* cStructDeclaration = buildStructDeclaration(new_name, scopeStatement);
   ROSE_ASSERT(cStructDeclaration);
   cStructDeclaration->set_class_type(SgClassDeclaration::e_struct);

// cout << ".x. built new struct declaration " << cStructDeclaration << ": " << cStructDeclaration->get_name() << endl;

// Replace the definition in the C struct/class declaration with the definition from the table definition
   removeList.push_back(cStructDeclaration->get_definition());  // blow away the new definition
   tableDefinition->set_parent(cStructDeclaration);
   cStructDeclaration->set_definition(tableDefinition);
   tableStatement->set_definition(NULL);

// Replace the SgJovialTableStatement with the C struct declaration (SgClassDeclaration).
   replaceStatement(tableStatement,cStructDeclaration,true);

// Add to removal list (why statementList and not removeList)
   statementList.push_back(tableStatement);
   tableStatement->set_parent(NULL);

}  // End of Jovial_to_C::translateJovialTableStatement


/******************************************************************************************************************/
/*
  Translate SgProgramHeaderStatement in Jovial into SgFunctionDeclaration in C.
  The main program in Jovial will become main function in C.
*/
/******************************************************************************************************************/
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
  
// Add header file: #include <stdlib.h>
   std::string include_target("<stdlib.h>");
   attachIncludeDeclaration(cFunctionDeclaration, include_target);

   programHeaderStatement->set_parent(NULL);
}  // End of Jovial_to_C::translateProgramHeaderStatement


/***************************************************************************************************************
  Translate SgStopOrPauseStatement in Jovial into exit function call (for now) in C.
****************************************************************************************************************/
void Jovial_to_C::translateStopOrPauseStatement(SgStopOrPauseStatement* stopOrPauseStmt)
{
   std::vector<SgExpression*> c_exit_args;

   SgExpression* c_exit_expr = stopOrPauseStmt->get_code();
   ROSE_ASSERT(c_exit_expr);

   switch (stopOrPauseStmt->get_stop_or_pause())
     {
       case SgStopOrPauseStatement::e_abort:
          c_exit_expr = buildIntVal(-2);
          break;
       case SgStopOrPauseStatement::e_exit:
          c_exit_expr = buildIntVal(-1);
          break;
       case SgStopOrPauseStatement::e_stop:
          break;
       default:
          cout << "Jovial_to_C::translateStopOrPauseStatement: enum = "
               << stopOrPauseStmt->get_stop_or_pause()
               << " not handled \n";
          ROSE_ASSERT(false);
     }

   c_exit_args.push_back(c_exit_expr);

// Replace the SgStopOrPauseStatement with call to exit
   SgType* c_return_type = buildIntType();
   SgExprListExp* c_arg_list = buildExprListExp(c_exit_args);
   SgScopeStatement* scope = SageInterface::getEnclosingScope(stopOrPauseStmt);
   SgExprStatement* c_exit_call_stmt = buildFunctionCallStmt("exit", c_return_type, c_arg_list, scope);
   replaceStatement(stopOrPauseStmt, c_exit_call_stmt, true);

   stopOrPauseStmt->set_parent(NULL);

}  // End of Jovial_to_C::translateStopOrPauseStatement


/***************************************************************************************************************
  Translate SgInitializedName by replacing "'" with "_" in names.
****************************************************************************************************************/
void Jovial_to_C::translateInitializedName(SgInitializedName* name)
{
   std::string old_name = name->get_name();
   std::string new_name = name->get_name();
   boost::replace_all(new_name, "'", "_");
   if (new_name != old_name)
      {
         SgName sg_new_name(new_name);
         SageInterface::set_name(name, sg_new_name);

         if (SgProject::get_verbose() > 2)
            {
               cout << ".x. initialized names differ \n";
               cout << ".x. old name is " << old_name << endl;
               cout << ".x. new name is " << new_name << endl;
            }
      }
}  // End of Jovial_to_C::translateInitializedName


/******************************************************************************************************************/
/*
  Translate SgJovialDefineDeclaration in Jovial into a C #define preprocessing statement
*/
/******************************************************************************************************************/
PreprocessingInfo* Jovial_to_C::translateJovialDefineDeclaration(SgJovialDefineDeclaration* defineStatement)
{
   std::string define_line = "#define " + defineStatement->get_define_string();
   PreprocessingInfo::DirectiveType directive_type = PreprocessingInfo::CpreprocessorDefineDeclaration;
   PreprocessingInfo::RelativePositionType position = PreprocessingInfo::before;

   boost::replace_all(define_line, "\"", "");

   PreprocessingInfo* directive = (PreprocessingInfo*) NULL;

   directive = new PreprocessingInfo(directive_type, define_line, "transformation-generated", 0, 0, 0, position);
   ROSE_ASSERT(directive);

// Maybe need to set source position
#if 0
   SgNullStatement* null_stmt = SageBuilder::buildNullStatement();
   ROSE_ASSERT(null_stmt);

   //   replaceStatement(defineStatement, null_stmt, true);
   //   defineStatement->set_parent(NULL);

   SageInterface::prependStatement(null_stmt);

   null_stmt->addToAttachedPreprocessingInfo(directive);
#endif

   statementList.push_back(defineStatement);
//   removeList.push_back(defineStatement);

   return directive;

}  // End of Jovial_to_C::translateJovialDefineDeclaration


PreprocessingInfo* Jovial_to_C::attachIncludeDeclaration(SgLocatedNode* target, const std::string & include_target,
                                                         PreprocessingInfo::RelativePositionType position /* =PreprocessingInfo::before*/)
{
   PreprocessingInfo* result = NULL;
   std::string include_line = "#include " + include_target;
   PreprocessingInfo::DirectiveType directive_type = PreprocessingInfo::CpreprocessorIncludeDeclaration;

   ROSE_ASSERT(target != NULL);

   result = new PreprocessingInfo(directive_type, include_line, "transformation-generated", 0, 0, 0, position);
   ROSE_ASSERT(result);

   target->addToAttachedPreprocessingInfo(result);

   return result;
}
