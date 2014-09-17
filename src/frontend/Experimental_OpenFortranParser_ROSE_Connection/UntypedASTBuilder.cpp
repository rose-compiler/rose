#define COMPILED_WITH_ROSE 1

#if COMPILED_WITH_ROSE
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"
#endif

#include "UntypedASTBuilder.hpp"
#include "OFPExpr.hpp"
#include <assert.h>
#include <stdio.h>

// DQ (9/14/2014): This file contains the function that are called by the OFP file:
//    src/3rdPartyLibraries/experimental-fortran-parser/aterm_traversal/ofp_traverse_productions.cpp
// These function use OFP class defined in:
//    src/3rdPartyLibraries/experimental-fortran-parser/aterm_traversal/traverse.hpp
// These functions (in this file) must implement the calls to the ROSE build functions
// to build up the ROSE AST for Untyped Nodes.
//
// To support this we need to:
//   1) build the SageBuilder build functions to build the ROSE Untyped IR nodes.
//   2) Then we have to modify these functions to call those ROSE build API functions.
//   3) Once we have the Untyped AST using ROSE Untyped IR nodes, then we can worry about the  
//      translation of these IR nodes to the usual ROSE IR nodes to define a proper ROSE AST 
//      (defining a new Fortran frontend for ROSE).

#define UNPARSER_AVAILABLE 0
#define OFP_BUILD_DEBUG

extern OFP::Unparser* unparser;

namespace OFP {

UntypedASTBuilder::UntypedASTBuilder()
{
}

UntypedASTBuilder::~UntypedASTBuilder()
{
}

//========================================================================================
// R201 program
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_Program(Program * program)
{
#ifdef TODO_ROSE
   Sg_File_Info * start = NULL;
   SgUntypedGlobalScope * scope = NULL;
   SgUntypedDeclarationStatement * decl = NULL;
   SgUntypedDeclarationList * declList = NULL;

   // set up the global program scope
   //
   scope = new SgUntypedGlobalScope(start);
   scope->set_declaration_list(new SgUntypedDeclarationList(start));  
   scope->set_statement_list(new SgUntypedStatementList(start));  
   scope->set_function_list(new SgUntypedFunctionDeclarationList(start));  

   SgUntypedFile * file = new SgUntypedFile(start);
   file->set_scope(scope);
   declList = file->get_scope()->get_declaration_list();

   // StartCommentBlock (ignore for now)
   //

   // ProgramUnit
   //
   std::vector<ProgramUnit*>* unitList = program->getProgramUnitList();
   for (int i = 0; i < unitList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(unitList->at(i)->getPayload());  assert(decl);
      declList->get_decl_list().push_back(decl);
   }

   program->setPayload(file);
   
#ifdef OFP_BUILD_DEBUG
   printf("build_Program: ..................... %lu\n", declList->get_decl_list().size());
#endif
#else
   printf ("UntypedASTBuilder::build_Program(): calling ROSE build functions \n");

   SgUntypedDeclarationList* declaration_list      = NULL;
   SgUntypedStatementList* statement_list          = NULL;
   SgUntypedFunctionDeclarationList* function_list = NULL;

   SgUntypedGlobalScope* globalScope = SageBuilder::buildUntypedGlobalScope(declaration_list,statement_list,function_list);

   SgUntypedFile* file = SageBuilder::buildUntypedFile(globalScope);

   program->setPayload(file);
#endif
}

//========================================================================================
// R204 specification-part
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_InitialSpecPart(InitialSpecPart * initialSpecPart)
{
#ifdef TODO_ROSE
   SgUntypedDeclarationStatement * decl = NULL;
   SgUntypedDeclarationList * sgDeclList = new SgUntypedDeclarationList(NULL);

   std::vector<Statement*>* stmtList = initialSpecPart->getStatementList();
   for (int i = 0; i < stmtList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(stmtList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }

   initialSpecPart->setPayload(sgDeclList);

#ifdef OFP_BUILD_DEBUG
   printf("build_InitialSpecPart: ........... %lu\n", sgDeclList->get_decl_list().size());
#endif
#endif
}

void UntypedASTBuilder::build_SpecificationPart(SpecificationPart * specificationPart)
{
   SgUntypedDeclarationStatement * decl = NULL;
   SgUntypedDeclarationList * sgDeclList = new SgUntypedDeclarationList(NULL);

   std::vector<UseStmt*>* useList = specificationPart->getUseStmtList();
   for (unsigned int i = 0; i < useList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(useList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }

   std::vector<ImportStmt*>* importList = specificationPart->getImportStmtList();
   for (unsigned int i = 0; i < importList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(importList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }

   ImplicitPart* implicitPart = specificationPart->getImplicitPart();
   if (implicitPart) {
      SgUntypedDeclarationList * implList;
      implList = dynamic_cast<SgUntypedDeclarationList*>(implicitPart->getPayload());  assert(implList);
      for (unsigned int i = 0; i < implList->get_decl_list().size(); i++) {
         decl = dynamic_cast<SgUntypedDeclarationStatement*>(implList->get_decl_list().at(i));  assert(decl);
         sgDeclList->get_decl_list().push_back(decl);
      }
   }

   std::vector<DeclarationConstruct*>* declList = specificationPart->getDeclarationConstructList();
   for (unsigned int i = 0; i < declList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(declList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }

   specificationPart->setPayload(sgDeclList);
   
#ifdef OFP_BUILD_DEBUG
   printf("build_SpecificationPart: ........... %lu\n", sgDeclList->get_decl_list().size());
#endif
}

//========================================================================================
// R205 implicit-part
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ImplicitPart(ImplicitPart* implicitPart)
{
#if 0
   SgUntypedDeclarationStatement * decl = NULL;

   SgUntypedDeclarationList * sgDeclList = new SgUntypedDeclarationList(NULL);
   std::vector<ImplicitPartStmt*>* ipartList = implicitPart->getImplicitPartStmtList();

   for (unsigned int i = 0; i < ipartList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(ipartList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }
   decl = dynamic_cast<SgUntypedDeclarationStatement*>(implicitPart->getImplicitStmt()->getPayload());  assert(decl);
   sgDeclList->get_decl_list().push_back(decl);

   implicitPart->setPayload(sgDeclList);

#ifdef OFP_BUILD_DEBUG
   printf("build_ImplicitPart: ................ %lu\n", sgDeclList->get_decl_list().size());
#endif
#endif
}

//========================================================================================
// R208 execution-part
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_SpecAndExecPart(SpecAndExecPart * specAndExecPart)
{
#ifdef TODO_ROSE
   SgUntypedStatement * stmt = NULL;

   SgUntypedStatementList * sgStmtList = new SgUntypedStatementList(NULL);
   std::vector<Statement*>* ofpStmtList = specAndExecPart->getStatementList();

   for (int i = 0; i < ofpStmtList->size(); i++) {
      stmt = dynamic_cast<SgUntypedStatement*>(ofpStmtList->at(i)->getPayload());  assert(stmt);
      sgStmtList->get_stmt_list().push_back(stmt);
   }
   specAndExecPart->setPayload(sgStmtList);

#ifdef OFP_BUILD_DEBUG
   printf("build_SpecAndExecPart: ............... %lu\n", sgStmtList->get_stmt_list().size());
#endif
#endif
}

void UntypedASTBuilder::build_ExecutionPart(ExecutionPart * executionPart)
{
   SgUntypedStatement * stmt = NULL;

   SgUntypedStatementList * sgStmtList = new SgUntypedStatementList(NULL);
   std::vector<ExecutionPartConstruct*>* execList = executionPart->getExecutionPartConstructList();

   for (unsigned int i = 0; i < execList->size(); i++) {
      stmt = dynamic_cast<SgUntypedStatement*>(execList->at(i)->getPayload());  assert(stmt);
      sgStmtList->get_stmt_list().push_back(stmt);
   }
   executionPart->setPayload(sgStmtList);

#ifdef OFP_BUILD_DEBUG
   printf("build_ExecutionPart: ............... %lu\n", sgStmtList->get_stmt_list().size());
#endif
}

//========================================================================================
// R210 internal-subprogram-part
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_InternalSubprogramPart(InternalSubprogramPart* internalSubprogramPart)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedStatement * stmt = NULL;
   SgUntypedFunctionDeclaration * fdecl = NULL;

   ContainsStmt * contains = internalSubprogramPart->getContainsStmt();  assert(contains);

   stmt = dynamic_cast<SgUntypedStatement*>(contains->getPayload());  assert(stmt);
   start = stmt->get_startOfConstruct();

   SgUntypedScope * scope = new SgUntypedScope(start);
   scope->set_declaration_list(new SgUntypedDeclarationList(start));
   scope->set_statement_list(new SgUntypedStatementList(start));  
   scope->set_function_list(new SgUntypedFunctionDeclarationList(start));  

   scope->get_statement_list()->get_stmt_list().push_back(stmt);

   std::vector<InternalSubprogram*>* isubList = internalSubprogramPart->getInternalSubprogramList();
   for (unsigned int i = 0; i < isubList->size(); i++) {
      fdecl = dynamic_cast<SgUntypedFunctionDeclaration*>(isubList->at(i)->getPayload());  assert(fdecl);
      scope->get_function_list()->get_func_list().push_back(fdecl);
   }
   internalSubprogramPart->setPayload(scope);

#ifdef OFP_BUILD_DEBUG
   printf("build_InternalSubprogramPart: ...... %lu\n", scope->get_function_list()->get_func_list().size());
#endif
#endif
}

//========================================================================================  
// R403 declaration-type-spec                                                              
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_DeclarationTypeSpec(DeclarationTypeSpec * declarationTypeSpec)
{
#if 0
   std::string name = "";
   Sg_File_Info * start = NULL;
   SgUntypedType * node = NULL;
   SgToken::ROSE_Fortran_Keywords keyword;

   bool checkKind = true;
   bool checkChar = false;
   IntrinsicTypeSpec * intrinsicTypeSpec = declarationTypeSpec->getIntrinsicTypeSpec();

   //TODO-CER- handle other possibilities
   assert(intrinsicTypeSpec);

   switch (declarationTypeSpec->getOptionType()) {
    //TODO-DQ- new keyword FORTRAN_TYPE is needed
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_T_STAR: keyword = SgToken::FORTRAN_TYPE;   break;
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_C_STAR: keyword = SgToken::FORTRAN_CLASS;  break;
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_C_DTS:
       keyword = SgToken::FORTRAN_CLASS;
       break;
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_T_DTS:
       keyword = SgToken::FORTRAN_TYPE;
       break;
     case OFP::DeclarationTypeSpec::IntrinsicType:
       switch (intrinsicTypeSpec->getOptionType()) {
         case OFP::IntrinsicTypeSpec::INTEGER:
            name = "INTEGER";  keyword = SgToken::FORTRAN_INTEGER;         break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_REAL:
            name = "REAL";     keyword = SgToken::FORTRAN_REAL;            break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_CMPLX:
            name = "COMPLEX";  keyword = SgToken::FORTRAN_COMPLEX;         break;
         case OFP::IntrinsicTypeSpec::LOGICAL:
            name = "LOGICAL";  keyword = SgToken::FORTRAN_LOGICAL;         break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_DBL_PREC:
            name = "DOUBLE PRECISION";
            checkKind = false; keyword = SgToken::FORTRAN_DOUBLEPRECISION; break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_DBL_CMPLX:
           //TODO-DQ- new keyword FORTRAN_DOUBLECOMPLEX is needed (I think this is nonstandard)
            name = "DOUBLE COMPLEX";
            checkKind = false;  keyword = SgToken::FORTRAN_DOUBLE_COMPLEX;  break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_CHAR:
            name = "CHARACTER";
            checkChar = true;   keyword = SgToken::FORTRAN_CHARACTER;       break;
       }
       break;
   }

   node = new SgUntypedType(start, name);
   //TODO-DQ-2014.3.7 there should be an enum for the type
   //node->set_statement_enum(keyword);

   if (intrinsicTypeSpec) {
      if (checkKind) {
         KindSelector* kindSelector = intrinsicTypeSpec->getKindSelector();
         //TODO-DQ- need a kind parameter (scalar integer constant expression)
         //TODO-CER- node->set_kind();
      }
      if (checkChar) {
         //TODO-DQ- need a length parameter (scalar integer expression or "*" or ":")
         //TODO-CER- node->set_kind();
         //TODO-CER- node->set_char_length();
         CharSelector* charSelector = intrinsicTypeSpec->getCharSelector();
      }
   }
   
   declarationTypeSpec->setPayload(node);
#endif
}
 
//========================================================================================
// R407 int-literal-constant
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_IntLiteralConstant(IntLiteralConstant * intLiteralConstant)
{
#if 0
   OFP::KindParam * kindParam = intLiteralConstant->getKindParam();

   SgUntypedValueExpression * expr = dynamic_cast<SgUntypedValueExpression*>(intLiteralConstant->getPayload());
   SgUntypedType * type = expr->get_type();
   type->set_is_literal(true);
   type->set_is_constant(true);
   //TODO-DQ-2014.3.7 ok for a type to have a keyword
   //type->set_keyword(SgToken::FORTRAN_INTEGER);

   if (kindParam) {
      SgUntypedValueExpression* kind = dynamic_cast<SgUntypedValueExpression*>(kindParam->getPayload());
      type->set_type_kind(kind);
   }

#ifdef OFP_BUILD_DEBUG
   printf("build_IntLiteralConstant ........... ");
#if UNPARSER_AVAILABLE
   unparser->unparseExpr(expr);  printf("\n");
#endif
#endif
#endif
}

//========================================================================================
// R501 type-declaration-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_TypeDeclarationStmt(TypeDeclarationStmt * typeDeclarationStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedType * type  = NULL;
   SgUntypedVariableDeclaration * stmt = NULL;
   SgUntypedInitializedNameList * parameters = NULL;

   // DeclarationTypeSpec 
   //
   type = isSgUntypedType(typeDeclarationStmt->getDeclarationTypeSpec()->getPayload());
   start = type->get_startOfConstruct();

   stmt = new SgUntypedVariableDeclaration(start, type);
#if UNPARSER_AVAILABLE
   stmt->set_has_unparse(true);
#endif

   if (typeDeclarationStmt->getLabel()) stmt->set_label_string(typeDeclarationStmt->getLabel()->getValue());
   stmt->set_parameters(new SgUntypedInitializedNameList(start));

#ifdef OFP_BUILD_DEBUG
   printf("build_TypeDeclarationStmt: .........\n");
#endif

   // OptAttrSpecList
   //TODO-CER- implement
   //

   // EntityDeclList
   //
   std::vector<EntityDecl*>* declList = typeDeclarationStmt->getEntityDeclList()->getEntityDeclList();

   //TODO-CER-2014.3.7 should this be variables or parameters?
#ifdef OFP_BUILD_DEBUG
   printf("               parameters: ......... ");
#endif
   parameters = stmt->get_parameters();
   for (unsigned int i = 0; i < declList->size(); i++) {
      std::string name = declList->at(i)->getObjectName()->getIdent()->getName();
      parameters->get_name_list().push_back(new SgUntypedInitializedName(start, type, name));
#ifdef OFP_BUILD_DEBUG
      printf("%s ", name.c_str());
#endif
   }
#ifdef OFP_BUILD_DEBUG
   printf("\n");
#endif

   typeDeclarationStmt->setPayload(stmt);
#endif
}

//========================================================================================  
// R503 entity-decl                                                                         
//----------------------------------------------------------------------------------------  
void UntypedASTBuilder::build_EntityDecl(EntityDecl * entityDecl)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedType * type = NULL;
   SgUntypedInitializedName * node = NULL;

   std::string name = entityDecl->getObjectName()->getIdent()->getName();

   node = new SgUntypedInitializedName(start, type, name);

   //TODO ArraySpec;
   //TODO CoarraySpec;
   //TODO CharLength;
   //TODO Initialization;

   entityDecl->setPayload(node);
#endif
}

//========================================================================================
// R560 implicit-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ImplicitStmt(ImplicitStmt * implicitStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedImplicitDeclaration * stmt = NULL;

   switch (implicitStmt->getOptionType()) {
     case ImplicitStmt::ImplicitStmt_NONE:
        stmt = new SgUntypedImplicitDeclaration(start);
#if UNPARSER_AVAILABLE
        stmt->set_has_unparse(true);
#endif
        stmt->set_statement_enum(SgToken::FORTRAN_IMPLICIT_NONE);
        if (implicitStmt->getLabel()) stmt->set_label_string(implicitStmt->getLabel()->getValue());

#ifdef OFP_BUILD_DEBUG
        printf("build_ImplicitStmt: NONE ...........\n");
#endif
        break;
     default:
        //TODO-CER- fix this cons call
        //stmt = new SgUntypedImplicitStatement(start, false);
#ifdef OFP_BUILD_DEBUG
        printf("build_ImplicitStmt: ISL  ...........\n");
#endif
        break;
   }

   implicitStmt->setPayload(stmt);
#endif
}

//========================================================================================
// R611 data-ref
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_DataRef(DataRef * dataRef)
{
#if 0
   // TODO - handle PartRef list instead of just grabbing first element
   dataRef->inheritPayload(dataRef->getPartRefList()->front());

#ifdef OFP_BUILD_DEBUG
   printf("build_DataRef: ..................... ");
#if UNPARSER_AVAILABLE
   unparser->unparseExpr(dynamic_cast<SgUntypedExpression*>(dataRef->getPayload()));  printf("\n");
#endif
#endif
#endif
}

//========================================================================================
// R612 part-ref
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_PartRef(PartRef * partRef)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedReferenceExpression* expr = NULL;

   //TODO-CER-2014.3.14 handle SectionSubscriptList and ImageSelector

   expr = new SgUntypedReferenceExpression(start, SgToken::FORTRAN_UNKNOWN, partRef->getPartName()->getIdent()->getValue().c_str());
   partRef->setPayload(expr);

#ifdef OFP_BUILD_DEBUG
   printf("build_PartRef: ..................... ");
#if UNPARSER_AVAILABLE
   unparser->unparseExpr(dynamic_cast<SgUntypedExpression*>(partRef->getPayload()));  printf("\n");
#endif
#endif
#endif
}

//========================================================================================
// R732 assignment-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_AssignmentStmt(AssignmentStmt * assignmentStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedAssignmentStatement * stmt = NULL;

   SgUntypedExpression* lhs = dynamic_cast<SgUntypedExpression*>(assignmentStmt->getVariable()->payload);
   SgUntypedExpression* rhs = dynamic_cast<SgUntypedExpression*>(assignmentStmt->getExpr()->payload);
   assert(lhs);  assert(rhs);

   stmt = new SgUntypedAssignmentStatement(start, lhs, rhs);
   stmt->set_statement_enum(SgToken::FORTRAN_ASSIGN);
   if (assignmentStmt->getLabel()) stmt->set_label_string(assignmentStmt->getLabel()->getValue());

   assignmentStmt->setPayload(stmt);
#endif
}

//========================================================================================
// R1101 main-program
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_MainProgram(MainProgram * mainProgram)
{
#ifdef TODO_ROSE
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = NULL;
   SgUntypedDeclarationList* sgDeclList = NULL;
   SgUntypedStatementList* sgStmtList = NULL;
   SgUntypedProgramHeaderDeclaration * program = NULL;

   // ProgramStmt
   //
   if (mainProgram->getProgramStmt()) {
      program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(mainProgram->getProgramStmt()->getPayload());  assert(program);
   }
   else {
      // no optional ProgramStmt
      program = new SgUntypedProgramHeaderDeclaration(NULL, "");
      program->set_statement_enum(SgToken::FORTRAN_PROGRAM);
   }
   program->set_scope(new SgUntypedFunctionScope(start));
#if UNPARSER_AVAILABLE
   program->set_has_unparse(true);
#endif

#ifdef OFP_BUILD_DEBUG
   printf("build_MainProgram label: ........... %s\n", program->get_label_string().c_str());
   printf("             begin name: ........... %s\n", program->get_name().c_str());
#endif

   // InitialSpecPart
   //
   InitialSpecPart * specPart = mainProgram->getInitialSpecPart();
   sgDeclList = dynamic_cast<SgUntypedDeclarationList*>(specPart->givePayload());  assert(sgDeclList);
   program->get_scope()->set_declaration_list(sgDeclList);

#ifdef OFP_BUILD_DEBUG
   printf("         spec_list_size: ........... %lu\n", sgDeclList->get_decl_list().size());
#endif

   // SpecAndExecPart
   //
   SpecAndExecPart * execPart = mainProgram->getSpecAndExecPart();
   sgStmtList = dynamic_cast<SgUntypedStatementList*>(execPart->givePayload());  assert(sgStmtList);
   program->get_scope()->set_statement_list(sgStmtList);

#ifdef OFP_BUILD_DEBUG
   printf("         exec_list_size: ........... %lu\n", sgStmtList->get_stmt_list().size());
#endif

   // InternalSubprogramPart
   //
   InternalSubprogramPart * isubPart = mainProgram->getInternalSubprogramPart();
   if (isubPart) {
      SgUntypedScope * isubScope = dynamic_cast<SgUntypedScope*>(isubPart->givePayload());  assert(isubScope);
      SgUntypedStatementList * sgStmtList = isubScope->get_statement_list();
      SgUntypedFunctionDeclarationList * sgFuncList = isubScope->get_function_list();

      // has one contains stmt
      SgUntypedStatement * contains = sgStmtList->get_stmt_list().front();
      program->get_scope()->get_statement_list()->get_stmt_list().push_back(contains);
      program->get_scope()->set_function_list(sgFuncList);
      sgStmtList->get_stmt_list().clear();
   }
   else {
      program->get_scope()->set_function_list(new SgUntypedFunctionDeclarationList(NULL));
   }

   // EndProgramStmt
   //
   stmt = dynamic_cast<SgUntypedNamedStatement*>(mainProgram->getEndProgramStmt()->getPayload());  assert(stmt);
   program->set_end_statement(stmt);

#ifdef OFP_BUILD_DEBUG
   printf("              end label: ........... %s\n", stmt->get_label_string().c_str());
   printf("              end  name: ........... %s\n", stmt->get_statement_name().c_str());
#endif

   mainProgram->setPayload(program);
#else
   printf ("UntypedASTBuilder::build_MainProgram(): commented out! \n");
#endif
}
   
//========================================================================================
// R1102 program-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ProgramStmt(ProgramStmt * programStmt)
{
   Sg_File_Info * start = NULL;
   SgUntypedProgramHeaderDeclaration * program = NULL;

   program = new SgUntypedProgramHeaderDeclaration(start, programStmt->getProgramName()->getIdent()->getName());
   program->set_statement_enum(SgToken::FORTRAN_PROGRAM);

   if (programStmt->getLabel()) program->set_label_string(programStmt->getLabel()->getValue());

   programStmt->setPayload(program);
}

//========================================================================================
// R1103 end-program-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_EndProgramStmt(EndProgramStmt * endProgramStmt)
{
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = new SgUntypedNamedStatement(start);
   stmt->set_statement_enum(SgToken::FORTRAN_END_PROGRAM);

   if (endProgramStmt->getLabel())       stmt->set_label_string  (endProgramStmt->getLabel()->getValue());
   if (endProgramStmt->getProgramName()) stmt->set_statement_name(endProgramStmt->getProgramName()->getIdent()->getValue());

   endProgramStmt->setPayload(stmt);
}

//========================================================================================
// R1104 module
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_Module(Module* module)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * endStmt = NULL;
   SgUntypedDeclarationList * sgDeclList = NULL;
   SgUntypedModuleDeclaration * sgModule = NULL;

   // ModuleStmt
   //
   sgModule = dynamic_cast<SgUntypedModuleDeclaration*>(module->getModuleStmt()->getPayload());  assert(sgModule);
   start = sgModule->get_startOfConstruct();
   sgModule->set_scope(new SgUntypedModuleScope(start));
#if UNPARSER_AVAILABLE
   sgModule->set_has_unparse(true);
#endif

#ifdef OFP_BUILD_DEBUG
   printf("build_Module label: ................ %s\n", sgModule->get_label_string().c_str());
#if 0
// DQ (9/10/2014): Need to add name data member for SgUntypedModuleDeclaration.
   printf("             begin name: ........... %s\n", sgModule->get_name().c_str());
#endif
#endif

   // SpecificationPart
   //
   SpecificationPart * specPart = module->getSpecificationPart();
   sgDeclList = dynamic_cast<SgUntypedDeclarationList*>(specPart->givePayload());  assert(sgDeclList);
   sgModule->get_scope()->set_declaration_list(sgDeclList);

#ifdef OFP_BUILD_DEBUG
   printf("         spec_list_size: ........... %lu\n", sgDeclList->get_decl_list().size());
#endif

   // ModuleSubprogramPart
   //
   ModuleSubprogramPart * msubPart = module->getModuleSubprogramPart();
   if (msubPart) {
      SgUntypedScope * msubScope = dynamic_cast<SgUntypedScope*>(msubPart->givePayload());  assert(msubScope);
      sgModule->get_scope()->set_statement_list(msubScope->get_statement_list());
      sgModule->get_scope()->set_function_list (msubScope->get_function_list());
   }
   else {
      sgModule->get_scope()->set_statement_list(new SgUntypedStatementList(NULL));
      sgModule->get_scope()->set_function_list (new SgUntypedFunctionDeclarationList(NULL));
   }

   // EndModuleStmt
   //
   endStmt = dynamic_cast<SgUntypedNamedStatement*>(module->getEndModuleStmt()->getPayload());  assert(endStmt);
   sgModule->set_end_statement(endStmt);

#ifdef OFP_BUILD_DEBUG
   printf("              end label: ........... %s\n", endStmt->get_label_string().c_str());
   printf("              end  name: ........... %s\n", endStmt->get_statement_name().c_str());
#endif

   module->setPayload(sgModule);
#endif
}

//========================================================================================
// R1105 module-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ModuleStmt(ModuleStmt* moduleStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedModuleDeclaration * module = NULL;

#if 0
// DQ (9/10/2014): This failes to compile so we have commented it out.
   module = new SgUntypedModuleDeclaration(start, moduleStmt->getModuleName()->getIdent()->getName());
   module->set_statement_enum(SgToken::FORTRAN_MODULE);

   if (moduleStmt->getLabel()) module->set_label_string(moduleStmt->getLabel()->getValue());
#endif

   moduleStmt->setPayload(module);
#endif
}

//========================================================================================
// R1106 end-module-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_EndModuleStmt(EndModuleStmt* endModuleStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = new SgUntypedNamedStatement(start);
   stmt->set_statement_enum(SgToken::FORTRAN_END_MODULE);

   if (endModuleStmt->getLabel())       stmt->set_label_string  (endModuleStmt->getLabel()->getValue());
   if (endModuleStmt->getModuleName()) stmt->set_statement_name(endModuleStmt->getModuleName()->getIdent()->getValue());

   endModuleStmt->setPayload(stmt);
#endif
}

//========================================================================================
// R1107 module-subprogram-part
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ModuleSubprogramPart(ModuleSubprogramPart* moduleSubprogramPart)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedStatement * stmt = NULL;
   SgUntypedFunctionDeclaration * fdecl = NULL;

   ContainsStmt * contains = moduleSubprogramPart->getContainsStmt();  assert(contains);

   stmt = dynamic_cast<SgUntypedStatement*>(contains->getPayload());  assert(stmt);
   start = stmt->get_startOfConstruct();

   SgUntypedScope * scope = new SgUntypedScope(start);
   scope->set_declaration_list(new SgUntypedDeclarationList(start));
   scope->set_statement_list(new SgUntypedStatementList(start));  
   scope->set_function_list(new SgUntypedFunctionDeclarationList(start));  

   scope->get_statement_list()->get_stmt_list().push_back(stmt);

   std::vector<ModuleSubprogram*>* msubList = moduleSubprogramPart->getModuleSubprogramList();
   for (int i = 0; i < msubList->size(); i++) {
      fdecl = dynamic_cast<SgUntypedFunctionDeclaration*>(msubList->at(i)->getPayload());  assert(fdecl);
      scope->get_function_list()->get_func_list().push_back(fdecl);
   }
   moduleSubprogramPart->setPayload(scope);

#ifdef OFP_BUILD_DEBUG
   printf("build_ModuleSubprogramPart: ...... %lu\n", scope->get_function_list()->get_func_list().size());
#endif
#endif
}

//========================================================================================
// R1227 function-subprogram
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_FunctionSubprogram(FunctionSubprogram* functionSubprogram)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = NULL;
   SgUntypedDeclarationList* sgDeclList = NULL;
   SgUntypedStatementList* sgStmtList = NULL;
   SgUntypedFunctionDeclaration * function = NULL;

   // FunctionStmt
   //
   function = dynamic_cast<SgUntypedFunctionDeclaration*>(functionSubprogram->getFunctionStmt()->getPayload());
   assert(function);
   start = function->get_startOfConstruct();
   function->set_scope(new SgUntypedFunctionScope(start));
#if UNPARSER_AVAILABLE
   function->set_has_unparse(true);
#endif

#ifdef OFP_BUILD_DEBUG
   printf("build_Function    label: ........... %s\n", function->get_label_string().c_str());
   printf("             begin name: ........... %s\n", function->get_name().c_str());
#endif

   // InitialSpecPart
   //
   InitialSpecPart * specPart = functionSubprogram->getInitialSpecPart();
   sgDeclList = dynamic_cast<SgUntypedDeclarationList*>(specPart->givePayload());  assert(sgDeclList);
   function->get_scope()->set_declaration_list(sgDeclList);

#ifdef OFP_BUILD_DEBUG
   printf("         spec_list_size: ........... %lu\n", sgDeclList->get_decl_list().size());
#endif

   // SpecAndExecPart
   //
   SpecAndExecPart * execPart = functionSubprogram->getSpecAndExecPart();
   sgStmtList = dynamic_cast<SgUntypedStatementList*>(execPart->givePayload());  assert(sgStmtList);
   function->get_scope()->set_statement_list(sgStmtList);

#ifdef OFP_BUILD_DEBUG
   printf("         exec_list_size: ........... %lu\n", sgStmtList->get_stmt_list().size());
#endif

   // InternalSubprogramPart
   //
   InternalSubprogramPart * isubPart = functionSubprogram->getInternalSubprogramPart();
   if (isubPart) {
      SgUntypedFunctionDeclarationList* sgFuncList;
      sgFuncList = dynamic_cast<SgUntypedFunctionDeclarationList*>(isubPart->givePayload());  assert(sgFuncList);
      function->get_scope()->set_function_list(sgFuncList);
   }
   else {
      function->get_scope()->set_function_list(new SgUntypedFunctionDeclarationList(NULL));
   }

   // EndFunctionStmt
   //
   stmt = dynamic_cast<SgUntypedNamedStatement*>(functionSubprogram->getEndFunctionStmt()->getPayload());  assert(stmt);
   function->set_end_statement(stmt);

#ifdef OFP_BUILD_DEBUG
   printf("              end label: ........... %s\n", stmt->get_label_string().c_str());
   printf("              end  name: ........... %s\n", stmt->get_statement_name().c_str());
#endif

   functionSubprogram->setPayload(function);
#endif
}

//========================================================================================
// R1228 function-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_FunctionStmt(FunctionStmt* functionStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedFunctionDeclaration * function = NULL;

   function = new SgUntypedFunctionDeclaration(start, functionStmt->getFunctionName()->getIdent()->getName());
   function->set_statement_enum(SgToken::FORTRAN_FUNCTION);

   if (functionStmt->getLabel()) function->set_label_string(functionStmt->getLabel()->getValue());

   //TODO-CER-2014.4.11 - handle prefix, arguments, language-binding

   functionStmt->setPayload(function);
#endif
}

//========================================================================================
// R1232 end-function-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_EndFunctionStmt(EndFunctionStmt* endFunctionStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = new SgUntypedNamedStatement(start);
   stmt->set_statement_enum(SgToken::FORTRAN_END_FUNCTION);

   if (endFunctionStmt->getLabel()) {
      stmt->set_label_string  (endFunctionStmt->getLabel()->getValue());
   }
   if (endFunctionStmt->getFunctionName()) {
      stmt->set_statement_name(endFunctionStmt->getFunctionName()->getIdent()->getValue());
   }

   endFunctionStmt->setPayload(stmt);
#endif
}

//========================================================================================
// R1233 subroutine-subprogram
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_SubroutineSubprogram(SubroutineSubprogram * subroutineSubprogram)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = NULL;
   SgUntypedDeclarationList* sgDeclList = NULL;
   SgUntypedStatementList* sgStmtList = NULL;
   SgUntypedSubroutineDeclaration * subroutine = NULL;

   // SubroutineStmt
   //
   subroutine = dynamic_cast<SgUntypedSubroutineDeclaration*>(subroutineSubprogram->getSubroutineStmt()->getPayload());
   assert(subroutine);
   start = subroutine->get_startOfConstruct();
   subroutine->set_scope(new SgUntypedFunctionScope(start));

#ifdef OFP_BUILD_DEBUG
   printf("build_Subroutine  label: ........... %s\n", subroutine->get_label_string().c_str());
   printf("             begin name: ........... %s\n", subroutine->get_name().c_str());
#endif

   // InitialSpecPart
   //
   InitialSpecPart * specPart = subroutineSubprogram->getInitialSpecPart();
   sgDeclList = dynamic_cast<SgUntypedDeclarationList*>(specPart->givePayload());  assert(sgDeclList);
   subroutine->get_scope()->set_declaration_list(sgDeclList);

#ifdef OFP_BUILD_DEBUG
   printf("         spec_list_size: ........... %lu\n", sgDeclList->get_decl_list().size());
#endif

   // SpecAndExecPart
   //
   SpecAndExecPart * execPart = subroutineSubprogram->getSpecAndExecPart();
   sgStmtList = dynamic_cast<SgUntypedStatementList*>(execPart->givePayload());  assert(sgStmtList);
   subroutine->get_scope()->set_statement_list(sgStmtList);

#ifdef OFP_BUILD_DEBUG
   printf("         exec_list_size: ........... %lu\n", sgStmtList->get_stmt_list().size());
#endif

   // InternalSubprogramPart
   //
   InternalSubprogramPart * isubPart = subroutineSubprogram->getInternalSubprogramPart();
   if (isubPart) {
      SgUntypedFunctionDeclarationList* sgFuncList;
      sgFuncList = dynamic_cast<SgUntypedFunctionDeclarationList*>(isubPart->givePayload());  assert(sgFuncList);
      subroutine->get_scope()->set_function_list(sgFuncList);
   }
   else {
      subroutine->get_scope()->set_function_list(new SgUntypedFunctionDeclarationList(NULL));  
   }

   // EndSubroutineStmt
   //
   stmt = dynamic_cast<SgUntypedNamedStatement*>(subroutineSubprogram->getEndSubroutineStmt()->getPayload());  assert(stmt);
   subroutine->set_end_statement(stmt);

#ifdef OFP_BUILD_DEBUG
   printf("              end label: ........... %s\n", stmt->get_label_string().c_str());
   printf("              end  name: ........... %s\n", stmt->get_statement_name().c_str());
#endif

   subroutineSubprogram->setPayload(subroutine);
#endif
}

//========================================================================================
// R1234 subroutine-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_SubroutineStmt(SubroutineStmt * subroutineStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedSubroutineDeclaration * subroutine = NULL;

   subroutine = new SgUntypedSubroutineDeclaration(start, subroutineStmt->getSubroutineName()->getIdent()->getName());
   subroutine->set_statement_enum(SgToken::FORTRAN_SUBROUTINE);

   if (subroutineStmt->getLabel()) subroutine->set_label_string(subroutineStmt->getLabel()->getValue());

   //TODO-CER-2014.3.24 - handle prefix, arguments, language-binding

   subroutineStmt->setPayload(subroutine);
#endif
}

//========================================================================================
// R1236 end-subroutine-stmt
//----------------------------------------------------------------------------------------
void  UntypedASTBuilder::build_EndSubroutineStmt(EndSubroutineStmt * endSubroutineStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = new SgUntypedNamedStatement(start);
   stmt->set_statement_enum(SgToken::FORTRAN_END_SUBROUTINE);

   if (endSubroutineStmt->getLabel()) {
      stmt->set_label_string  (endSubroutineStmt->getLabel()->getValue());
   }
   if (endSubroutineStmt->getSubroutineName()) {
      stmt->set_statement_name(endSubroutineStmt->getSubroutineName()->getIdent()->getValue());
   }

   endSubroutineStmt->setPayload(stmt);
#endif
}

//========================================================================================
// R1237 separate-module-subprogram
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_SeparateModuleSubprogram(SeparateModuleSubprogram* separateModuleSubprogram)
{
#if 0
// typedef SgUntypedFunctionDeclaration SgUntypedMpSubprogramDeclaration;

   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = NULL;
   SgUntypedDeclarationList* sgDeclList = NULL;
   SgUntypedStatementList* sgStmtList = NULL;
   //TODO-DQ-2014.4.15 - need SgUntypedMpSubprogramDeclaration
   SgUntypedMpSubprogramDeclaration * subprogram = NULL;

   // MpSubprogramStmt
   //
   subprogram = dynamic_cast<SgUntypedMpSubprogramDeclaration*>(separateModuleSubprogram->getMpSubprogramStmt()->getPayload());
   assert(subprogram);
   start = subprogram->get_startOfConstruct();
   subprogram->set_scope(new SgUntypedFunctionScope(start));

#ifdef OFP_BUILD_DEBUG
   printf("build_Subprogram  label: ........... %s\n", subprogram->get_label_string().c_str());
   printf("             begin name: ........... %s\n", subprogram->get_name().c_str());
#endif

   // InitialSpecPart
   //
   InitialSpecPart * specPart = separateModuleSubprogram->getInitialSpecPart();
   sgDeclList = dynamic_cast<SgUntypedDeclarationList*>(specPart->givePayload());  assert(sgDeclList);
   subprogram->get_scope()->set_declaration_list(sgDeclList);

#ifdef OFP_BUILD_DEBUG
   printf("         spec_list_size: ........... %lu\n", sgDeclList->get_decl_list().size());
#endif

   // SpecAndExecPart
   //
   SpecAndExecPart * execPart = separateModuleSubprogram->getSpecAndExecPart();
   sgStmtList = dynamic_cast<SgUntypedStatementList*>(execPart->givePayload());  assert(sgStmtList);
   subprogram->get_scope()->set_statement_list(sgStmtList);

#ifdef OFP_BUILD_DEBUG
   printf("         exec_list_size: ........... %lu\n", sgStmtList->get_stmt_list().size());
#endif

   // InternalSubprogramPart
   //
   InternalSubprogramPart * isubPart = separateModuleSubprogram->getInternalSubprogramPart();
   if (isubPart) {
      SgUntypedFunctionDeclarationList* sgFuncList;
      sgFuncList = dynamic_cast<SgUntypedFunctionDeclarationList*>(isubPart->givePayload());  assert(sgFuncList);
      subprogram->get_scope()->set_function_list(sgFuncList);
   }
   else {
      subprogram->get_scope()->set_function_list(new SgUntypedFunctionDeclarationList(NULL));  
   }

   // EndMpSubprogramStmt
   //
   stmt = dynamic_cast<SgUntypedNamedStatement*>(separateModuleSubprogram->getEndMpSubprogramStmt()->getPayload());  assert(stmt);
   subprogram->set_end_statement(stmt);

#ifdef OFP_BUILD_DEBUG
   printf("              end label: ........... %s\n", stmt->get_label_string().c_str());
   printf("              end  name: ........... %s\n", stmt->get_statement_name().c_str());
#endif

   separateModuleSubprogram->setPayload(subprogram);
#endif
}

//========================================================================================
// R1238 mp-subprogram-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_MpSubprogramStmt(MpSubprogramStmt* mpSubprogramStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedMpSubprogramDeclaration * subprogram = NULL;

   subprogram = new SgUntypedMpSubprogramDeclaration(start, mpSubprogramStmt->getProcedureName()->getIdent()->getName());
   subprogram->set_statement_enum(SgToken::FORTRAN_MP_SUBPROGRAM);

   if (mpSubprogramStmt->getLabel()) subprogram->set_label_string(mpSubprogramStmt->getLabel()->getValue());

   mpSubprogramStmt->setPayload(subprogram);
#endif
}

//========================================================================================
// R1239 end-mp-subprogram-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_EndMpSubprogramStmt(EndMpSubprogramStmt* endMpSubprogramStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = new SgUntypedNamedStatement(start);
   stmt->set_statement_enum(SgToken::FORTRAN_END_MP_SUBPROGRAM);

   if (endMpSubprogramStmt->getLabel()) {
      stmt->set_label_string  (endMpSubprogramStmt->getLabel()->getValue());
   }
   if (endMpSubprogramStmt->getProcedureName()) {
      stmt->set_statement_name(endMpSubprogramStmt->getProcedureName()->getIdent()->getValue());
   }

   endMpSubprogramStmt->setPayload(stmt);
#endif
}

//========================================================================================
// R1242 contains-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ContainsStmt(ContainsStmt* containsStmt)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedStatement * stmt = new SgUntypedStatement(start);
   stmt->set_statement_enum(SgToken::FORTRAN_CONTAINS);
#if UNPARSER_AVAILABLE
   stmt->set_has_unparse(true);
#endif

   if (containsStmt->getLabel()) {
      stmt->set_label_string  (containsStmt->getLabel()->getValue());
   }

   containsStmt->setPayload(stmt);
#endif
}

//========================================================================================
// Binary operators
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_BinaryOp(Expr * expr, SgToken::ROSE_Fortran_Operators op, std::string name)
{
#if 0
   Sg_File_Info * start = NULL;
   SgUntypedBinaryOperator * binop = NULL;

   SgUntypedExpression * lhs = dynamic_cast<SgUntypedExpression*>(expr->getExpr1()->getPayload());
   SgUntypedExpression * rhs = dynamic_cast<SgUntypedExpression*>(expr->getExpr2()->getPayload());
   assert(rhs);  assert(lhs);

#ifdef OFP_BUILD_DEBUG
#if UNPARSER_AVAILABLE
   unparser->unparseExpr(lhs);
   printf(" ");
   unparser->unparseExpr(rhs);
   printf("\n");
#endif
#endif

   //TODO-DQ-2014.3.7 I don't think a Fortran enum should be in constructor
   binop = new SgUntypedBinaryOperator(start, SgToken::FORTRAN_UNKNOWN, op, name, lhs, rhs); 
   expr->setPayload(binop);
#endif
}

//========================================================================================
// Binary operators
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_Dop(Dop* dop)
{
}

void UntypedASTBuilder::build_HexConstant(HexConstant* hexConstant)
{
}

void UntypedASTBuilder::build_OctalConstant(OctalConstant* octalConstant)
{
}

void UntypedASTBuilder::build_BinaryConstant(BinaryConstant* binaryConstant)
{
}

void UntypedASTBuilder::build_Rcon(Rcon* rcon)
{
}

void UntypedASTBuilder::build_Scon(Scon* scon)
{
}

void UntypedASTBuilder::build_Icon(Icon* icon)
{
#if 0
   //TODO-CER-2014.3.7 set (or request) SgToken::FORTRAN_INTEGER);
   SgUntypedType*             type = new SgUntypedType(NULL, "INTEGER");
   SgUntypedValueExpression* value = new SgUntypedValueExpression(NULL, SgToken::FORTRAN_INTEGER, icon->getValue(), type);
   icon->setPayload(value);

#ifdef OFP_BUILD_DEBUG
   printf("build_Icon: ........................ ");
#if UNPARSER_AVAILABLE
   unparser->unparseExpr(dynamic_cast<SgUntypedExpression*>(icon->getPayload()));  printf("\n");
#endif
#endif

#if 0
   printf ("Error: In UntypedASTBuilder::build_Icon(): Built a SgUntypedValueExpression (need to call SageBuilder API) \n");
   ROSE_ASSERT(false);
#else
   printf ("WARNING: In UntypedASTBuilder::build_Icon(): Built a SgUntypedValueExpression (need to call SageBuilder API) \n");
#endif
#endif
}

void UntypedASTBuilder::build_Ident(Ident* ident)
{
#if 0
   SgUntypedValueExpression* value = new SgUntypedValueExpression(NULL, SgToken::FORTRAN_UNKNOWN, ident->getValue(), NULL);
   ident->setPayload(value);

#ifdef OFP_BUILD_DEBUG
   printf("build_Ident: ....................... ");
#if UNPARSER_AVAILABLE
   unparser->unparseExpr(dynamic_cast<SgUntypedExpression*>(ident->getPayload()));  printf("\n");
#endif
#endif

#if 0
   printf ("Error: In UntypedASTBuilder::build_Ident(): Built a SgUntypedValueExpression (need to call SageBuilder API) \n");
   ROSE_ASSERT(false);
#else
   printf ("WARNING: In UntypedASTBuilder::build_Ident(): Built a SgUntypedValueExpression (need to call SageBuilder API) \n");
#endif
#endif
}

void UntypedASTBuilder::build_Letter(Letter* letter)
{
}

void UntypedASTBuilder::build_LblRef(LblRef* lblRef)
{
}

void UntypedASTBuilder::build_StartCommentBlock(StartCommentBlock* startCommentBlock)
{
}

void UntypedASTBuilder::build_EOS(EOS* eos)
{
}

}; // namespace OFP
