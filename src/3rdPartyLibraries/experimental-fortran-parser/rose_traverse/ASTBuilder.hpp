#ifndef	AST_BUILDER_HPP
#define	AST_BUILDER_HPP

#include "traversal.h"

namespace OFP {

class ASTBuilder;

void setASTBuilder(ASTBuilder * ast);

class ASTBuilder
{
 public:
   ASTBuilder()            {}
   virtual ~ASTBuilder()   {}

   virtual void build_Program(Program * program) = 0;
   virtual void build_MainProgram(MainProgram * program) = 0;
   virtual void build_ProgramStmt(ProgramStmt * programStmt) = 0;
   virtual void build_EndProgramStmt(EndProgramStmt * endProgramStmt) = 0;

   virtual void build_SpecificationPart(SpecificationPart * specificationPart) = 0;
   virtual void build_ImplicitPart(ImplicitPart * implicitPart) = 0;
   virtual void build_ExecutionPart(ExecutionPart * executionPart) = 0;
   virtual void build_DeclarationTypeSpec(DeclarationTypeSpec * declarationTypeSpec) = 0;
   virtual void build_TypeDeclarationStmt(TypeDeclarationStmt * typeDeclarationStmt) = 0;
   virtual void build_EntityDecl(EntityDecl* entityDecl) = 0;
   virtual void build_ImplicitStmt(ImplicitStmt * implicitStmt) = 0;

   // Expressions
   //
   virtual void build_IntLiteralConstant(IntLiteralConstant * intLiteralConstant) = 0;

   // Operators
   //
   virtual void build_BinaryOp(Expr * expr, SgToken::ROSE_Fortran_Operators op, std::string name) = 0;

};

} // namespace OFP

#endif
