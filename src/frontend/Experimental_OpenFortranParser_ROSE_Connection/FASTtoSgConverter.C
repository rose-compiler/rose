#include "sage3basic.h"

#include "FASTtoSgConverter.h"

#define PRINT_FAST_CONVERTER 0


FASTtoSgConverter::FASTtoSgConverter()
{
   SgUntypedDeclarationStatementList* sg_decls = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            sg_stmts = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  sg_funcs = new SgUntypedFunctionDeclarationList();
   SgUntypedGlobalScope*          global_scope = new SgUntypedGlobalScope("",SgToken::FORTRAN_UNKNOWN,sg_decls,sg_stmts,sg_funcs);

   pUntypedFile = new SgUntypedFile(global_scope);
   ROSE_ASSERT(pUntypedFile != NULL);
}

FASTtoSgConverter::~FASTtoSgConverter()
{
   delete pUntypedFile;
}

void FASTtoSgConverter::convert_scope_lists(FAST::Scope* function_scope, SgUntypedDeclarationStatementList* sg_decls,
                                                                         SgUntypedStatementList* sg_stmts,
                                                                         SgUntypedFunctionDeclarationList* sg_funcs)
{
   ROSE_ASSERT(function_scope->get_declaration_list().size() == 0);
   ROSE_ASSERT(function_scope->get_statement_list()  .size() == 0);
   ROSE_ASSERT(function_scope->get_function_list()   .size() == 0);
}

void FASTtoSgConverter::convert_MainProgram(FAST::MainProgram* main_program)
{
#if PRINT_FAST_CONVERTER
   printf("FASTtoSgConverter::convert_MainProgram \n");
#endif

   SgUntypedProgramHeaderDeclaration* sg_program;

   std::string label;
   std::string name;
   SgToken::ROSE_Fortran_Keywords keyword;

   SgUntypedExpression* sg_null_expr = new SgUntypedExpression(SgToken::FORTRAN_NULL);

   SgUntypedType* sg_type = new SgUntypedType("void",sg_null_expr/*type_kind*/,false,false,false,true/*is_intrinsic*/,false,false,
                                                     sg_null_expr/*char_length*/,"",false/*char_length_is_string*/,SgUntypedType::e_void);

   FAST::ProgramStmt*    programStmt    = main_program->getProgramStmt();
   FAST::Scope*          programScope   = main_program->getScope();
// FAST::ContainsStmt*   containsStmt   = main_program->getContainsStmt();
   FAST::EndProgramStmt* endProgramStmt = main_program->getEndProgramStmt();

// convert EndProgramStmt first as constructor arguments are readily available
   label   = endProgramStmt->getLabel();
   keyword = SgToken::FORTRAN_END_PROGRAM;
   name    = endProgramStmt->getName();
   SgUntypedNamedStatement* sg_end_stmt = new SgUntypedNamedStatement(label,keyword,name);

// convert ProgramStmt
   label   = programStmt->getLabel();
   keyword = SgToken::FORTRAN_PROGRAM;
   name    = programStmt->getName();

// convert arguments
   SgUntypedInitializedNameList*      sg_params = new SgUntypedInitializedNameList();

// convert scope
   SgUntypedDeclarationStatementList* sg_decls  = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            sg_stmts  = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  sg_funcs  = new SgUntypedFunctionDeclarationList();

   convert_scope_lists(programScope, sg_decls, sg_stmts, sg_funcs);

   SgUntypedFunctionScope* sg_function_scope = new SgUntypedFunctionScope(label,keyword,sg_decls,sg_stmts,sg_funcs);

// convert MainProgram
   sg_program = new SgUntypedProgramHeaderDeclaration(label,keyword,name,sg_params,sg_type,sg_function_scope,sg_end_stmt);

// add program to the global scope
   get_scope()->get_function_list()->get_func_list().push_back(sg_program);
   ROSE_ASSERT(get_scope()->get_function_list()->get_func_list().size() > 0);
}
