#ifndef	AST_BUILDER_HPP
#define	AST_BUILDER_HPP

#include "traversal.hpp"

namespace OFP {

class ASTBuilder;

void setASTBuilder(ASTBuilder * ast);

class ASTBuilder
{
 public:
   ASTBuilder()            {}
   virtual ~ASTBuilder()   {}

   // Programs
   //
   virtual void build_Program(Program * program) = 0;
   virtual void build_MainProgram(MainProgram * program) = 0;
   virtual void build_ProgramStmt(ProgramStmt * programStmt) = 0;
   virtual void build_EndProgramStmt(EndProgramStmt * endProgramStmt) = 0;

   // Modules
   //
   virtual void build_Module(Module* module) = 0;
   virtual void build_ModuleStmt(ModuleStmt* moduleStmt) = 0;
   virtual void build_EndModuleStmt(EndModuleStmt* endModuleStmt) = 0;
   virtual void build_ModuleSubprogramPart(ModuleSubprogramPart* moduleSubprogramPart) = 0;
   virtual void build_SeparateModuleSubprogram(SeparateModuleSubprogram* separateModuleSubprogram) = 0;
   virtual void build_MpSubprogramStmt(MpSubprogramStmt* mpSubprogramStmt) = 0;
   virtual void build_EndMpSubprogramStmt(EndMpSubprogramStmt* endMpSubprogramStmt) = 0;

   // Functions
   //
   virtual void build_FunctionSubprogram(FunctionSubprogram* functionSubprogram) = 0;
   virtual void build_FunctionStmt(FunctionStmt* functionStmt) = 0;
   virtual void build_EndFunctionStmt(EndFunctionStmt* endFunctionStmt) = 0;

   virtual void build_InitialSpecPart(InitialSpecPart * initialSpecPart) = 0;
   virtual void build_SpecificationPart(SpecificationPart * specificationPart) = 0;
   virtual void build_ImplicitPart(ImplicitPart * implicitPart) = 0;
   virtual void build_SpecAndExecPart(SpecAndExecPart * specAndExecPart) = 0;
   virtual void build_ExecutionPart(ExecutionPart * executionPart) = 0;
   virtual void build_InternalSubprogramPart(InternalSubprogramPart* internalSubprogramPart) = 0;
   virtual void build_DeclarationTypeSpec(DeclarationTypeSpec * declarationTypeSpec) = 0;
   virtual void build_TypeDeclarationStmt(TypeDeclarationStmt * typeDeclarationStmt) = 0;
   virtual void build_EntityDecl(EntityDecl* entityDecl) = 0;
   virtual void build_ImplicitStmt(ImplicitStmt * implicitStmt) = 0;
   virtual void build_DataRef(DataRef * dataRef) = 0;
   virtual void build_PartRef(PartRef * partRef) = 0;
   virtual void build_AssignmentStmt(AssignmentStmt * assignmentStmt) = 0;
   virtual void build_SubroutineSubprogram(SubroutineSubprogram * subroutineSubprogram) = 0;
   virtual void build_SubroutineStmt(SubroutineStmt * subroutineStmt) = 0;
   virtual void build_EndSubroutineStmt(EndSubroutineStmt * endSubroutineStmt) = 0;
   virtual void build_ContainsStmt(ContainsStmt* containsStmt) = 0;

   // Expressions
   //
   virtual void build_IntLiteralConstant(IntLiteralConstant * intLiteralConstant) = 0;

   // Operators
   //
   virtual void build_BinaryOp(Expr * expr, SgToken::ROSE_Fortran_Operators op, std::string name) = 0;

   // Terminals
   //
   virtual void build_Dop(Dop* dop) = 0;
   virtual void build_HexConstant(HexConstant* hexConstant) = 0;
   virtual void build_OctalConstant(OctalConstant* octalConstant) = 0;
   virtual void build_BinaryConstant(BinaryConstant* binaryConstant) = 0;
   virtual void build_Rcon(Rcon* rcon) = 0;
   virtual void build_Scon(Scon* scon) = 0;
   virtual void build_Icon(Icon* icon) = 0;
   virtual void build_Ident(Ident* ident) = 0;
   virtual void build_Letter(Letter* letter) = 0;
   virtual void build_LblRef(LblRef* lblRef) = 0;
   virtual void build_StartCommentBlock(StartCommentBlock* startCommentBlock) = 0;
   virtual void build_EOS(EOS* eos) = 0;
};

} // namespace OFP

#endif
