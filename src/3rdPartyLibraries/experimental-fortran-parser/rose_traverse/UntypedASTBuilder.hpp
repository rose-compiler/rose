#ifndef	ROSE_BUILDER_HPP
#define	ROSE_BUILDER_HPP

#include "ASTBuilder.hpp"

namespace OFP {

class UntypedASTBuilder : public ASTBuilder
{
 public:
   UntypedASTBuilder();
  ~UntypedASTBuilder();

   virtual void build_SpecificationPart(SpecificationPart * specificationPart);
   virtual void build_ImplicitPart(ImplicitPart * implicitPart);
   virtual void build_ExecutionPart(ExecutionPart * executionPart);
   virtual void build_DeclarationTypeSpec(DeclarationTypeSpec * declarationTypeSpec);
   virtual void build_TypeDeclarationStmt(TypeDeclarationStmt * typeDeclarationStmt);
   virtual void build_EntityDecl(EntityDecl* entityDecl);
   virtual void build_ImplicitStmt(ImplicitStmt * implicitStmt);

   virtual void build_MainProgram(MainProgram * main);
   virtual void build_ProgramStmt(ProgramStmt * programStmt);
   virtual void build_EndProgramStmt(EndProgramStmt * endProgramStmt);
   
   // Operators
   //
   virtual void build_BinaryOp(Expr * expr, SgToken::ROSE_Fortran_Operators op, std::string name);

 private:
   
};

#ifdef OBSOLETE
// Helper class to contain a vector of SgUntypedStatements.  The class is needed because
// the node payload must be an SgNode; otherwise not needed as a regular SgUntypedNode.
//
class SgUntypedStatementList : public SgUntypedNode
   {
     public:
       SgUntypedStatementList(std::vector<SgUntypedStatement*>* list) : stmt_list(list) {}

       virtual ~SgUntypedStatementList()
          {
             // TODO - what about content of list                                         
             if (stmt_list) delete stmt_list;
          }

       std::vector<SgUntypedStatement*>* get_statement_list() {return stmt_list;}
       std::vector<SgUntypedStatement*>* give_statement_list()
          {
             std::vector<SgUntypedStatement*>* tmp_stmt_list = stmt_list;
             stmt_list = NULL;
             return tmp_stmt_list;
          }
       //TODO delete void set_statement_list(std::vector<SgUntypedStatement*>* list) {stmt_list = list;}                                                                         
     private:
      //TODO - change from pointer type
      std::vector<SgUntypedStatement*>* stmt_list;
   };
#endif


} // namespace OFP

#endif
