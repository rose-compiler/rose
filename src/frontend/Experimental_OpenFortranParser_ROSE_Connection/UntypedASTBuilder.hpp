#ifndef ROSE_BUILDER_HPP
#define ROSE_BUILDER_HPP

#include "ASTBuilder.hpp"

namespace OFP {

class UntypedASTBuilder : public ASTBuilder
{
 public:
   UntypedASTBuilder();
  ~UntypedASTBuilder();

   // Programs
   //
   virtual void build_Program(Program * program);
   virtual void build_MainProgram(MainProgram * mainProgram);
   virtual void build_ProgramStmt(ProgramStmt * programStmt);
   virtual void build_EndProgramStmt(EndProgramStmt * endProgramStmt);

   // Modules
   //
   virtual void build_Module(Module* module);
   virtual void build_ModuleStmt(ModuleStmt* moduleStmt);
   virtual void build_EndModuleStmt(EndModuleStmt* endModuleStmt);
   virtual void build_ModuleSubprogramPart(ModuleSubprogramPart* moduleSubprogramPart);
   virtual void build_SeparateModuleSubprogram(SeparateModuleSubprogram* separateModuleSubprogram);
   virtual void build_MpSubprogramStmt(MpSubprogramStmt* mpSubprogramStmt);
   virtual void build_EndMpSubprogramStmt(EndMpSubprogramStmt* endMpSubprogramStmt);

   // Functions
   //
   virtual void build_FunctionSubprogram(FunctionSubprogram* functionSubprogram);
   virtual void build_FunctionStmt(FunctionStmt* functionStmt);
   virtual void build_EndFunctionStmt(EndFunctionStmt* endFunctionStmt);

   virtual void build_InitialSpecPart(InitialSpecPart * initialSpecPart);
   virtual void build_SpecificationPart(SpecificationPart * specificationPart);
   virtual void build_ImplicitPart(ImplicitPart * implicitPart);
   virtual void build_SpecAndExecPart(SpecAndExecPart * specAndExecPart);
   virtual void build_ExecutionPart(ExecutionPart * executionPart);
   virtual void build_InternalSubprogramPart(InternalSubprogramPart* internalSubprogramPart);
   virtual void build_DeclarationTypeSpec(DeclarationTypeSpec * declarationTypeSpec);
   virtual void build_TypeDeclarationStmt(TypeDeclarationStmt * typeDeclarationStmt);
   virtual void build_EntityDecl(EntityDecl* entityDecl);
   virtual void build_ImplicitStmt(ImplicitStmt * implicitStmt);
   virtual void build_DataRef(DataRef * dataRef);
   virtual void build_PartRef(PartRef * partRef);
   virtual void build_AssignmentStmt(AssignmentStmt * assignmentStmt);
   virtual void build_SubroutineSubprogram(SubroutineSubprogram * subroutineSubprogram);
   virtual void build_SubroutineStmt(SubroutineStmt * subroutineStmt);
   virtual void build_EndSubroutineStmt(EndSubroutineStmt * endSubroutineStmt);
   virtual void build_ContainsStmt(ContainsStmt* containsStmt);

   // Expressions
   //
   virtual void build_IntLiteralConstant(IntLiteralConstant * intLiteralConstant);

   // Operators
   //
   virtual void build_BinaryOp(Expr * expr, SgToken::ROSE_Fortran_Operators op, std::string name);

   // Terminals
   //
   virtual void build_Dop(Dop* dop);
   virtual void build_HexConstant(HexConstant* hexConstant);
   virtual void build_OctalConstant(OctalConstant* octalConstant);
   virtual void build_BinaryConstant(BinaryConstant* binaryConstant);
   virtual void build_Rcon(Rcon* rcon);
   virtual void build_Scon(Scon* scon);
   virtual void build_Icon(Icon* icon);
   virtual void build_Ident(Ident* ident);
   virtual void build_Letter(Letter* letter);
   virtual void build_LblRef(LblRef* lblRef);
   virtual void build_StartCommentBlock(StartCommentBlock* startCommentBlock);
   virtual void build_EOS(EOS* eos);
};

} // namespace OFP

#endif
