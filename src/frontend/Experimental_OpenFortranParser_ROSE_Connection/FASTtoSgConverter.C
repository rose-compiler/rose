#include "sage3basic.h"

#include "FASTtoSgConverter.h"

#define DEBUG_FAST_CONVERTER 0

FASTtoSgConverter::FASTtoSgConverter(SgSourceFile* source)
{
   SgUntypedDeclarationStatementList* sg_decls = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            sg_stmts = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  sg_funcs = new SgUntypedFunctionDeclarationList();
   SgUntypedGlobalScope*          global_scope = new SgUntypedGlobalScope("",SgToken::FORTRAN_UNKNOWN,sg_decls,sg_stmts,sg_funcs);

   pSourceFile = source;
   ROSE_ASSERT(pSourceFile != NULL);

   pUntypedFile = new SgUntypedFile(global_scope);
   ROSE_ASSERT(pUntypedFile != NULL);

// DQ (2/25/2013): Set the default for source position generation to be consistent with other languages (e.g. C/C++).
   SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
}

FASTtoSgConverter::~FASTtoSgConverter()
{
   delete pUntypedFile;
}

void FASTtoSgConverter::convert_scope_lists(FAST::Scope* function_scope, SgUntypedDeclarationStatementList* sg_decls,
                                                                         SgUntypedStatementList* sg_stmts,
                                                                         SgUntypedFunctionDeclarationList* sg_funcs)
{
   std::vector<FAST::Statement*>::iterator it;
   std::vector<FAST::Statement*> & decls = function_scope->get_declaration_list();
   std::vector<FAST::Statement*> & stmts = function_scope->get_statement_list();
   std::vector<FAST::Statement*> & funcs = function_scope->get_function_list();

   for (it = decls.begin(); it != decls.end(); ++it) {
      SgUntypedDeclarationStatement* sg_decl = NULL;
      FAST::Statement* stmt = *it;

      if (dynamic_cast<FAST::ImplicitStmt*>(stmt)) {
         // TODO - SgToken::FORTRAN_IMPLICIT CASE
         sg_decl = new SgUntypedImplicitDeclaration(stmt->getLabel(), SgToken::FORTRAN_IMPLICIT_NONE);
      }

      if (dynamic_cast<FAST::TypeDeclarationStmt*>(stmt)) {
         std::string type_name = "INTEGER";
         std::string char_length_string = "";
         SgUntypedType::type_enum type_enum_id = SgUntypedType::e_int;
         SgUntypedExpression* type_kind = NULL;
         SgUntypedExpression* char_length_expression = NULL;
         bool has_kind = false;
         bool is_literal = false;
         bool is_class = false;
         bool is_intrinsic = true;
         bool is_constant = false;
         bool is_user_defined = false;
         bool char_length_is_string = false;

         FAST::TypeDeclarationStmt* decl = dynamic_cast<FAST::TypeDeclarationStmt*>(stmt);
         std::vector<FAST::AttrSpec*>   & attrs = decl->getAttrSpecList();
         std::vector<FAST::EntityDecl*> & vars  = decl->getEntityDeclList();

         SgUntypedType* sg_type = new SgUntypedType(type_name, type_kind, has_kind, is_literal,
                                                    is_class, is_intrinsic, is_constant, is_user_defined,
                                                    char_length_expression, char_length_string, char_length_is_string,
                                                    type_enum_id);

         SgUntypedInitializedNameList* sg_vars = new SgUntypedInitializedNameList();
         for (std::vector<FAST::EntityDecl*>::iterator it = vars.begin(); it != vars.end(); ++it) {
            FAST::EntityDecl* entity_decl = *it;
            SgUntypedInitializedName* sg_var = new SgUntypedInitializedName(sg_type, entity_decl->getName());
            setSourcePosition(sg_var, entity_decl->getPosInfo());
            sg_vars->get_name_list().push_back(sg_var);
         }

         sg_decl = new SgUntypedVariableDeclaration(decl->getLabel(),SgToken::FORTRAN_TYPE,sg_type,sg_vars);
      }

      ROSE_ASSERT(sg_decl != NULL);
#if DEBUG_FAST_CONVERTER
  FAST::PosInfo pos = stmt->getPosInfo();
  std::cout << "setSourcePosition: " << pos.getStartLine() << " " <<  pos.getStartCol();
  std::cout <<                   " " << pos.getEndLine()   << " " <<  pos.getEndCol() << std::endl;
#endif
      setSourcePosition(sg_decl, stmt->getPosInfo());
      sg_decls->get_decl_list().push_back(sg_decl);
   }

   for (it = stmts.begin(); it != stmts.end(); ++it) {
      SgUntypedStatement* sg_stmt = NULL;
      FAST::Statement* stmt = *it;

      ROSE_ASSERT(sg_stmt != NULL);
      setSourcePosition(sg_stmt, stmt->getPosInfo());
      sg_stmts->get_stmt_list().push_back(sg_stmt);
   }

   for (it = funcs.begin(); it != funcs.end(); ++it) {
      SgUntypedFunctionDeclaration* sg_decl = NULL;
      FAST::Statement* stmt = *it;

      ROSE_ASSERT(sg_decl != NULL);
      setSourcePosition(sg_decl, stmt->getPosInfo());
      sg_funcs->get_func_list().push_back(sg_decl);
   }

}

void FASTtoSgConverter::convert_MainProgram(FAST::MainProgram* main_program)
{
#if DEBUG_FAST_CONVERTER
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
   FAST::ContainsStmt*   containsStmt   = main_program->getContainsStmt();
   FAST::EndProgramStmt* endProgramStmt = main_program->getEndProgramStmt();

// convert EndProgramStmt first as constructor arguments are readily available
   label   = endProgramStmt->getLabel();
   keyword = SgToken::FORTRAN_END_PROGRAM;
   name    = endProgramStmt->getName();
   SgUntypedNamedStatement* sg_end_stmt = new SgUntypedNamedStatement(label,keyword,name);

   bool hasImplicitProgramStmt = false;
   if (programStmt == NULL) {
   // need to create an implicit ProgramStmt
      hasImplicitProgramStmt = true;
      FAST::PosInfo pos(1,1,1,1);
      programStmt = new FAST::ProgramStmt("", "rose_fortran_main", "", pos);
   }

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

// set source positions
   setSourcePosition(sg_program,     programStmt->getPosInfo());
   setSourcePosition(sg_end_stmt, endProgramStmt->getPosInfo());

// add program to the global scope
   get_scope()->get_function_list()->get_func_list().push_back(sg_program);
   ROSE_ASSERT(get_scope()->get_function_list()->get_func_list().size() > 0);

   if (hasImplicitProgramStmt) {
      delete programStmt;
   }
}

void
FASTtoSgConverter::setSourcePosition( SgLocatedNode* locatedNode, FAST::PosInfo & pos )
{
     ROSE_ASSERT(locatedNode != NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);

     std::string filename = getCurrentFilename();

#if DEBUG_FAST_CONVERTER
     std::cout << "setSourcePosition: " << pos.getStartLine() << " " <<  pos.getStartCol();
     std::cout <<                   " " << pos.getEndLine()   << " " <<  pos.getEndCol() << std::endl;
#endif

     locatedNode->set_startOfConstruct(new Sg_File_Info(filename, pos.getStartLine(), pos.getStartCol()));
     locatedNode->get_startOfConstruct()->set_parent(locatedNode);

     locatedNode->set_endOfConstruct(new Sg_File_Info(filename, pos.getEndLine(), pos.getEndCol()));
     locatedNode->get_endOfConstruct()->set_parent(locatedNode);

     SageInterface::setSourcePosition(locatedNode);
}

void
FASTtoSgConverter::setSourcePosition( SgLocatedNode* locatedNode, FAST::PosInfo & startPos, FAST::PosInfo & endPos )
{
     ROSE_ASSERT(locatedNode != NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);

     std::string filename = getCurrentFilename();

#if DEBUG_FAST_CONVERTER
     std::cout << "setSourcePosition: " << startPos.getStartLine() << " " <<  startPos.getStartCol();
     std::cout <<                   " " <<   endPos.getEndLine()   << " " <<    endPos.getEndCol() << std::endl;
#endif

     locatedNode->set_startOfConstruct(new Sg_File_Info(filename, startPos.getStartLine(), startPos.getStartCol()));
     locatedNode->get_startOfConstruct()->set_parent(locatedNode);

     locatedNode->set_endOfConstruct(new Sg_File_Info(filename, endPos.getEndLine(), endPos.getEndCol()));
     locatedNode->get_endOfConstruct()->set_parent(locatedNode);

     SageInterface::setSourcePosition(locatedNode);
}
