#ifndef AT_TO_UNTYPED_TRAVERSAL_H
#define AT_TO_UNTYPED_TRAVERSAL_H

#include <aterm2.h>
#include "FASTNodes.hpp"

// Needed SgUntypedNodes (or something like them)
class SgUntypedExpressionStatement;

namespace OFP {

class ATtoUntypedTraversal
{
 public:
   ATtoUntypedTraversal(SgSourceFile* source);
   virtual ~ATtoUntypedTraversal();

   SgUntypedFile*        get_file()  { return pUntypedFile; }
   SgUntypedGlobalScope* get_scope() { return pUntypedFile->get_scope(); }

   std::string getCurrentFilename()
      {
         return pSourceFile->get_sourceFileNameWithPath();
      }

 protected:
   SgUntypedFile* pUntypedFile;
   SgSourceFile*  pSourceFile;

 public:

static void setSourcePositionUnknown(SgLocatedNode* locatedNode);

static FAST::PosInfo getLocation(ATerm term);

void setSourcePosition              ( SgLocatedNode* locatedNode, ATerm term );
void setSourcePosition              ( SgLocatedNode* locatedNode, FAST::PosInfo & pos );
void setSourcePositionFrom          ( SgLocatedNode* locatedNode, SgLocatedNode* fromNode );
void setSourcePositionUptoTerm      ( SgLocatedNode* locatedNode, ATerm startTerm, ATerm endTerm );
void setSourcePositionIncludingTerm ( SgLocatedNode* locatedNode, ATerm startTerm, ATerm endTerm );
void setSourcePositionIncludingNode ( SgLocatedNode* locatedNode, ATerm startTerm, SgLocatedNode* endNode );

// These should probably go in UntypedBuilder class
static SgUntypedType* buildType(SgUntypedType::type_enum type_enum = SgUntypedType::e_unknown);

//R201
ATbool traverse_Program(ATerm term);

ATbool traverse_ProgramUnitList(ATerm term, SgUntypedGlobalScope* scope);

ATbool traverse_InitialSpecPart(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_SpecAndExecPart(ATerm term, SgUntypedDeclarationStatementList* decl_list,
                                                       SgUntypedStatementList* stmt_list);
// R204
ATbool traverse_SpecificationPart(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// R205
ATbool traverse_OptImplicitPart(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// R206
ATbool traverse_ImplicitPartStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// R207
ATbool traverse_DeclarationConstruct     (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_DeclarationConstructList (ATerm term, SgUntypedDeclarationStatementList* decl_list);

// R209
ATbool traverse_ExecutionPartConstruct(ATerm term, SgUntypedStatementList* stmt_list);

ATbool traverse_OptInternalSubprogramPart (ATerm term, SgUntypedOtherStatement** contains_stmt,
                                                       SgUntypedScope* scope);
// R211
ATbool traverse_InternalSubprogramList    (ATerm term, SgUntypedScope* scope);

ATbool traverse_SpecStmt( ATerm term, SgUntypedDeclarationStatementList* decl_list );
ATbool traverse_ExecStmt( ATerm term,            SgUntypedStatementList* stmt_list );

ATbool traverse_Name       ( ATerm term, std::string & name  );
ATbool traverse_NameString ( ATerm term, std::string & name  );
ATbool traverse_OptName    ( ATerm term, std::string & name  );
ATbool traverse_OptLabel   ( ATerm term, std::string & label );
ATbool traverse_eos        ( ATerm term, std::string & eos   );

ATbool traverse_NameList ( ATerm term, std::vector<std::string> & name_list);

ATbool traverse_StartCommentBlock(ATerm term, std::string & start_comments);
ATbool traverse_OptModuleNature(ATerm term, FAST::UseStmt::ModuleNature* var_OptModuleNature);

// R305
ATbool traverse_LiteralConstant(ATerm term, SgUntypedExpression** var_expr);

// R309
ATbool traverse_Operator(ATerm term, SgUntypedExpression** var_expr);

// R422
ATbool traverse_OptCharLength(ATerm term, SgUntypedExpression** expr);

// R501
ATbool traverse_TypeDeclarationStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// R503
ATbool traverse_EntityDecl     (ATerm term, SgUntypedType* declared_type, SgUntypedInitializedNameList* name_list);
ATbool traverse_EntityDeclList (ATerm term, SgUntypedType* declared_type, SgUntypedInitializedNameList* name_list);

// R505
ATbool traverse_OptInitialization(ATerm term, SgUntypedExpression** expr);

// R509
ATbool traverse_OptCoarraySpec (ATerm term, SgUntypedType* declared_type, SgUntypedType** initialized_type);

// R515
ATbool traverse_OptArraySpec   (ATerm term, SgUntypedType* declared_type, SgUntypedType** initialized_type);

// R560
ATbool traverse_ImplicitStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_ImplicitSpecList(ATerm term, std::vector<FAST::ImplicitSpec> & ref_ImplicitSpecList);

// R611
ATbool traverse_DataRef(ATerm term, SgUntypedExpression** var_expr);
// R612
ATbool traverse_PartRef(ATerm term, SgUntypedExpression** var_expr);

// R620
ATbool traverse_OptSectionSubscripts(ATerm term);
// R624
ATbool traverse_OptImageSelector(ATerm term);

ATbool traverse_ContinueStmt(ATerm term, SgUntypedStatement** continue_stmt);

ATbool traverse_DeclarationTypeSpec ( ATerm term, SgUntypedType** type );
ATbool traverse_IntrinsicTypeSpec   ( ATerm term, SgUntypedType** type );
//TODO traverse_DerivedTypeSpec

// R722
ATbool traverse_Expression ( ATerm term, SgUntypedExpression** expr );
ATbool traverse_OptExpr    ( ATerm term, SgUntypedExpression** expr );

// R732
ATbool traverse_AssignmentStmt(ATerm term, SgUntypedStatementList* stmt_list);

// R854
ATbool traverse_ContinueStmt(ATerm term, SgUntypedStatementList* stmt_list);

// R1101
ATbool traverse_MainProgram(ATerm term, SgUntypedScope* scope);
ATbool traverse_OptProgramStmt(ATerm term, SgUntypedNamedStatement** program_stmt);
ATbool traverse_EndProgramStmt(ATerm term, SgUntypedNamedStatement** var_EndProgramStmt);

// R1104
ATbool traverse_Module    (ATerm term, SgUntypedScope* scope);

// R1105
ATbool traverse_ModuleStmt(ATerm term, SgUntypedNamedStatement** module_stmt);

// R1106
ATbool traverse_EndModuleStmt(ATerm term, SgUntypedNamedStatement** end_module_stmt);

// R1107
ATbool traverse_OptModuleSubprogramPart(ATerm term, SgUntypedOtherStatement** contains_stmt, SgUntypedScope* scope);

// R1108
ATbool traverse_ModuleSubprogramList(ATerm term, SgUntypedScope* scope);

// R1109
ATbool traverse_UseStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_UseStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// R1116
ATbool traverse_Submodule (ATerm term, SgUntypedScope* scope);

// R1120
ATbool traverse_BlockData (ATerm term, SgUntypedScope* scope);

// R1209
ATbool traverse_ImportStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_ImportStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// R1210
ATbool traverse_ExternalStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// R1225
ATbool traverse_OptPrefix(ATerm term, std::vector<FAST::PrefixSpec*> & prefix_list, SgUntypedType** type);

// R1226
ATbool traverse_PrefixSpecList(ATerm term, std::vector<FAST::PrefixSpec*> & prefix_list, SgUntypedType** type);

// R1227
ATbool traverse_FunctionSubprogram(ATerm term, SgUntypedScope* scope);

// R1229
ATbool traverse_OptProcLanguageBindingSpec(ATerm term);

// R1231
ATbool traverse_OptSuffix(ATerm term);

// R1232
ATbool traverse_EndFunctionStmt(ATerm term, SgUntypedNamedStatement** end_function_stmt);

// R1233
ATbool traverse_SubroutineSubprogram(ATerm term, SgUntypedScope* scope);

// R1235
ATbool traverse_OptDummyArgList(ATerm term, SgUntypedInitializedNameList* param_list);

// R1236
ATbool traverse_EndSubroutineStmt(ATerm term, SgUntypedNamedStatement** end_subroutine_stmt);

// R1237
ATbool traverse_SeparateModuleSubprogram(ATerm term, SgUntypedScope* scope);

// R1238
ATbool traverse_MpSubprogramStmt    ( ATerm term, SgUntypedNamedStatement**     mp_subprogram_stmt );
ATbool traverse_EndMpSubprogramStmt ( ATerm term, SgUntypedNamedStatement** end_mp_subprogram_stmt );

// R1240 entry-stmt

// R1141
ATbool traverse_ReturnStmt   ( ATerm term, SgUntypedExpressionStatement** return_stmt );

// R1142
ATbool traverse_ContainsStmt ( ATerm term, SgUntypedOtherStatement**    contains_stmt );


}; // class Traversal
}  // namespace OFP

#endif
