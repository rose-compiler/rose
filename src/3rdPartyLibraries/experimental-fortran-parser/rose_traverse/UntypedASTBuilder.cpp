#include "UntypedASTBuilder.hpp"
#include "OFPExpr.h"
#include <assert.h>
#include <stdio.h>

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
   Sg_File_Info * start = NULL;
   SgUntypedFunctionDeclaration * decl = NULL;
   SgUntypedDeclarationList * declList = NULL;

   SgUntypedFile * file = new SgUntypedFile(start);
   file->set_scope(new SgUntypedGlobalScope(start));
   declList = file->get_scope()->get_declaration_list();

   // StartCommentBlock (ignore for now)
   //

   // ProgramUnit
   //
   std::vector<ProgramUnit*>* unitList = program->getProgramUnitList();
   for (int i = 0; i < unitList->size(); i++) {
      decl = dynamic_cast<SgUntypedFunctionDeclaration*>(unitList->at(i)->getPayload());  assert(decl);
      declList->get_decl_list().push_back(decl);
   }

   program->setPayload(file);
   
   printf("build_Program: ..................... %lu\n", declList->get_decl_list().size());

}

//========================================================================================
// R204 specification-part
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_SpecificationPart(SpecificationPart * specificationPart)
{
   SgUntypedDeclarationStatement * decl = NULL;
   SgUntypedDeclarationList * sgDeclList = new SgUntypedDeclarationList(NULL);

   std::vector<UseStmt*>* useList = specificationPart->getUseStmtList();
   for (int i = 0; i < useList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(useList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }

   std::vector<ImportStmt*>* importList = specificationPart->getImportStmtList();
   for (int i = 0; i < importList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(importList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }

   ImplicitPart* implicitPart = specificationPart->getImplicitPart();
   if (implicitPart) {
      SgUntypedDeclarationList * implList;
      implList = dynamic_cast<SgUntypedDeclarationList*>(implicitPart->getPayload());  assert(implList);
      for (int i = 0; i < implList->get_decl_list().size(); i++) {
         decl = dynamic_cast<SgUntypedDeclarationStatement*>(implList->get_decl_list().at(i));  assert(decl);
         sgDeclList->get_decl_list().push_back(decl);
      }
   }

   std::vector<DeclarationConstruct*>* declList = specificationPart->getDeclarationConstructList();
   for (int i = 0; i < declList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(declList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }

   specificationPart->setPayload(sgDeclList);
   
   printf("build_SpecificationPart: ........... %lu\n", sgDeclList->get_decl_list().size());

}

//========================================================================================
// R205 implicit-part
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ImplicitPart(ImplicitPart* implicitPart)
{
   SgUntypedDeclarationStatement * decl = NULL;

   SgUntypedDeclarationList * sgDeclList = new SgUntypedDeclarationList(NULL);
   std::vector<ImplicitPartStmt*>* ipartList = implicitPart->getImplicitPartStmtList();

   for (int i = 0; i < ipartList->size(); i++) {
      decl = dynamic_cast<SgUntypedDeclarationStatement*>(ipartList->at(i)->getPayload());  assert(decl);
      sgDeclList->get_decl_list().push_back(decl);
   }
   decl = dynamic_cast<SgUntypedDeclarationStatement*>(implicitPart->getImplicitStmt()->getPayload());  assert(decl);
   sgDeclList->get_decl_list().push_back(decl);

   implicitPart->setPayload(sgDeclList);

   printf("build_ImplicitPart: .............. %lu\n", sgDeclList->get_decl_list().size());
}

//========================================================================================
// R208 execution-part
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ExecutionPart(ExecutionPart * executionPart)
{
   SgUntypedStatement * stmt = NULL;

   SgUntypedStatementList * sgStmtList = new SgUntypedStatementList(NULL);
   std::vector<ExecutionPartConstruct*>* execList = executionPart->getExecutionPartConstructList();

   for (int i = 0; i < execList->size(); i++) {
      stmt = dynamic_cast<SgUntypedStatement*>(execList->at(i)->getPayload());  assert(stmt);
      sgStmtList->get_stmt_list().push_back(stmt);
   }
   executionPart->setPayload(sgStmtList);

   printf("build_ExecutionPart: ............... %lu\n", sgStmtList->get_stmt_list().size());
}

//========================================================================================  
// R403 declaration-type-spec                                                              
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_DeclarationTypeSpec(DeclarationTypeSpec * declarationTypeSpec)
{
   Sg_File_Info * start = NULL;
   SgUntypedType * node = NULL;
   SgToken::ROSE_Fortran_Keywords keyword;

   bool checkKind = true;
   bool checkChar = false;
   IntrinsicTypeSpec * intrinsicTypeSpec = declarationTypeSpec->getIntrinsicTypeSpec();

   //TODO-CER- handle other possibilities
   assert(intrinsicTypeSpec);

   switch (intrinsicTypeSpec->getOptionType()) {
    //TODO-DQ- new keyword FORTRAN_TYPE is needed
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_T_STAR: keyword = SgToken::FORTRAN_TYPE;   break;
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_C_STAR: keyword = SgToken::FORTRAN_CLASS;  break;
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_C_DTS:
       keyword = SgToken::FORTRAN_CLASS;
       break;
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_T_DTS:
       keyword = SgToken::FORTRAN_TYPE;
       break;
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_T_ITS:
       keyword = SgToken::FORTRAN_TYPE;
       break;
     case OFP::DeclarationTypeSpec::DeclarationTypeSpec_ITS:
       switch (intrinsicTypeSpec->getOptionType()) {
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_INT:      keyword = SgToken::FORTRAN_INTEGER;         break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_REAL:     keyword = SgToken::FORTRAN_REAL;            break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_CMPLX:    keyword = SgToken::FORTRAN_COMPLEX;         break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_LOGICAL:  keyword = SgToken::FORTRAN_LOGICAL;         break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_DBL_PREC:
           checkKind = false;                                     keyword = SgToken::FORTRAN_DOUBLEPRECISION; break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_DBL_CMPLX:
           //TODO-DQ- new keyword FORTRAN_DOUBLECOMPLEX is needed (I think this is nonstandard)
           checkKind = false;                                     keyword = SgToken::FORTRAN_DOUBLE_COMPLEX;  break;
         case OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_CHAR:
           checkChar = true;                                      keyword = SgToken::FORTRAN_CHARACTER;       break;
       }
       intrinsicTypeSpec = declarationTypeSpec->getIntrinsicTypeSpec();
       break;
   }

   //TODO-CER- FIXME by setting type name string
   node = new SgUntypedType(start, "INTEGER");
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
}
 
//========================================================================================
// R407 int-literal-constant
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_IntLiteralConstant(IntLiteralConstant * intLiteralConstant)
{
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

   printf("ROSE IntLiteralConstant ............. ");
   unparser->unparseExpr(expr);  printf("\n");
}

//========================================================================================
// R501 type-declaration-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_TypeDeclarationStmt(TypeDeclarationStmt * typeDeclarationStmt)
{
   Sg_File_Info * start = NULL;
   SgUntypedType * type  = NULL;
   SgUntypedVariableDeclaration * stmt = NULL;
   SgUntypedInitializedNameList * parameters = NULL;

   // DeclarationTypeSpec 
   //
   type = isSgUntypedType(typeDeclarationStmt->getDeclarationTypeSpec()->getPayload());
   start = type->get_startOfConstruct();

   stmt = new SgUntypedVariableDeclaration(start, type);
   if (typeDeclarationStmt->getLabel()) stmt->set_label_string(typeDeclarationStmt->getLabel()->getValue());

   printf("build_TypeDeclarationStmt: .........\n");

   // OptAttrSpecList
   //TODO-CER- implement
   //

   // EntityDeclList
   //
   std::vector<EntityDecl*>* declList = typeDeclarationStmt->getEntityDeclList()->getEntityDeclList();

#ifdef NOT_NEEDED
   if (declSpec->getOptionType() == DeclarationTypeSpec::DeclarationTypeSpec_ITS) {
      IntrinsicTypeSpec* ispec = declSpec->getIntrinsicTypeSpec();
      if (ispec->getOptionType() == IntrinsicTypeSpec::IntrinsicTypeSpec_INT) {
         type = new SgUntypedType(start, false); 
         //TODO - set intrinsic type (FORTRAN_INTEGER)
         printf("                type_spec: ......... INTEGER\n");
      }
   }
#endif

   //TODO-CER-2014.3.7 should this be variables or parameters?
   printf("                variables: ......... ");
   parameters = stmt->get_parameters();
   for (int i = 0; i < declList->size(); i++) {
      std::string name = declList->at(i)->getObjectName()->getIdent()->getName();
      parameters->get_name_list().push_back(new SgUntypedInitializedName(start, type, name));
      printf("%s ", name.c_str());
   }
   printf("\n");

   typeDeclarationStmt->setPayload(stmt);
}

//========================================================================================  
// R503 entity-decl                                                                         
//----------------------------------------------------------------------------------------  
void UntypedASTBuilder::build_EntityDecl(EntityDecl * entityDecl)
{
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
}

//========================================================================================
// R560 implicit-stmt
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_ImplicitStmt(ImplicitStmt * implicitStmt)
{
   Sg_File_Info * start = NULL;
   SgUntypedImplicitDeclaration * stmt = NULL;

   switch (implicitStmt->getOptionType()) {
     case ImplicitStmt::ImplicitStmt_NONE:
        stmt = new SgUntypedImplicitDeclaration(start);
        stmt->set_statement_enum(SgToken::FORTRAN_IMPLICIT_NONE);
        if (implicitStmt->getLabel()) stmt->set_label_string(implicitStmt->getLabel()->getValue());

        printf("build_ImplicitStmt: NONE .........\n");
        break;
     case ImplicitStmt::ImplicitStmt_ISL:
        //TODO-CER- fix this cons call
        //stmt = new SgUntypedImplicitStatement(start, false);
        printf("build_ImplicitStmt: ISL  .........\n");
        break;
   }

   implicitStmt->setPayload(stmt);
}

//========================================================================================
// R1101 main-program
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_MainProgram(MainProgram * mainProgram)
{
   Sg_File_Info * start = NULL;
   SgUntypedNamedStatement * stmt = NULL;
   SgUntypedDeclarationList* sgDeclList = NULL;
   SgUntypedStatementList* sgStmtList = NULL;
   SgUntypedProgramHeaderDeclaration * program = NULL;

   // ProgramStmt
   //
   program = dynamic_cast<SgUntypedProgramHeaderDeclaration*>(mainProgram->getProgramStmt()->getPayload());  assert(program);

   printf("build_MainProgram label: ........... %s\n", program->get_label_string().c_str());
   printf("             begin name: ........... %s\n", program->get_name().c_str());

   // SpecificationPart
   //
   SpecificationPart * specPart = mainProgram->getSpecificationPart();
   sgDeclList = dynamic_cast<SgUntypedDeclarationList*>(specPart->givePayload());  assert(sgDeclList);
   program->get_scope()->set_declaration_list(sgDeclList);

   printf("         spec_list_size: ........... %lu\n", sgDeclList->get_decl_list().size());

   // ExecutionPart
   //
   ExecutionPart * execPart = mainProgram->getExecutionPart();
   sgStmtList = dynamic_cast<SgUntypedStatementList*>(execPart->givePayload());  assert(sgStmtList);
   program->get_scope()->set_statement_list(sgStmtList);

   printf("         exec_list_size: ........... %lu\n", sgStmtList->get_stmt_list().size());

   // InternalSubprogramPart
   //
   InternalSubprogramPart * isubPart = mainProgram->getInternalSubprogramPart();
   if (isubPart) {
      SgUntypedFunctionDeclarationList* sgFuncList;
      sgFuncList = dynamic_cast<SgUntypedFunctionDeclarationList*>(isubPart->givePayload());  assert(sgFuncList);
      program->get_scope()->set_function_list(sgFuncList);
   }

   // EndProgramStmt
   //
   stmt = dynamic_cast<SgUntypedNamedStatement*>(mainProgram->getEndProgramStmt()->getPayload());  assert(stmt);
   program->set_end_statement(stmt);

   printf("              end label: ........... %s\n", stmt->get_label_string().c_str());
   printf("              end  name: ........... %s\n", stmt->get_statement_name().c_str());

   mainProgram->setPayload(program);
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
   program->set_scope(new SgUntypedFunctionScope(start));

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
// Binary operators
//----------------------------------------------------------------------------------------
void UntypedASTBuilder::build_BinaryOp(Expr * expr, SgToken::ROSE_Fortran_Operators op, std::string name)
{
   Sg_File_Info * start = NULL;
   SgUntypedBinaryOperator * binop = NULL;

   SgUntypedExpression * lhs = dynamic_cast<SgUntypedExpression*>(expr->getExpr1()->getPayload());
   SgUntypedExpression * rhs = dynamic_cast<SgUntypedExpression*>(expr->getExpr2()->getPayload());
   assert(rhs);  assert(lhs);

   //TODO-DQ-2014.3.7 I don't think a Fortran enum should be in constructor
   binop = new SgUntypedBinaryOperator(start, SgToken::FORTRAN_UNKNOWN, op, name, lhs, rhs); 
   expr->setPayload(binop);
}


}; // namespace OFP
