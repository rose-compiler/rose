#ifndef ATERM_TO_SAGE_FORTRAN_TRAVERSAL_H_
#define ATERM_TO_SAGE_FORTRAN_TRAVERSAL_H_

#include "ATerm/ATermTraversal.h"
#include "general_language_translation.h"
#include "FASTNodes.h"

#include "sage-tree-builder.h"

namespace rb = Rose::builder;

// From SourceryInstitute
typedef std::tuple<std::string, SgType*, SgExpression*> EntityDeclTuple;

namespace ATermSupport {

class ATermToSageFortranTraversal : public ATermTraversal
{
 private:
  // C++11
  // rb::SageTreeBuilder sage_tree_builder{rb::SageTreeBuilder::e_language_fortran}
  rb::SageTreeBuilder sage_tree_builder;

 public:
   ATermToSageFortranTraversal(SgSourceFile* source)
     : ATermTraversal(source), sage_tree_builder(rb::SageTreeBuilder(rb::SageTreeBuilder::e_language_fortran))
     {
     }

//R201
ATbool traverse_Program(ATerm term);

ATbool traverse_ProgramUnitList(ATerm term);

ATbool traverse_InitialSpecPart(ATerm term);
ATbool traverse_SpecAndExecPart(ATerm term);

// R204
ATbool traverse_SpecificationPart(ATerm term);

// R205
ATbool traverse_OptImplicitPart(ATerm term);

// R206
ATbool traverse_ImplicitPartStmtList(ATerm term);

// R207
ATbool traverse_DeclarationConstruct(ATerm term);
ATbool traverse_DeclarationConstructList(ATerm term);

// R209
ATbool traverse_ExecutionPartConstruct(ATerm term);
ATbool traverse_BlockExecutionPartConstruct(ATerm term);

ATbool traverse_OptInternalSubprogramPart(ATerm term);

// R211
ATbool traverse_InternalSubprogramList(ATerm term);

ATbool traverse_SpecStmt( ATerm term );
ATbool traverse_ExecStmt( ATerm term );

ATbool traverse_Name       ( ATerm term, std::string & name  );
ATbool traverse_NameString ( ATerm term, std::string & name  );
ATbool traverse_OptName    ( ATerm term, std::string & name  );
ATbool traverse_OptLabel   ( ATerm term, std::string & label );
ATbool traverse_eos        ( ATerm term, std::string & eos   );

ATbool traverse_NameList   ( ATerm term, SgUntypedNameList* name_list );

ATbool traverse_StartCommentBlock(ATerm term, std::string &start_comments);
ATbool traverse_OptModuleNature(ATerm term);

// R305
ATbool traverse_LiteralConstant(ATerm term, SgExpression* &expr);

// R309
ATbool traverse_Operator(ATerm term, SgExpression* &expr);

// R310
ATbool traverse_DefinedOperator(ATerm term, std::string &name);

// R405
ATbool traverse_KindSelector(ATerm term, SgExpression* &expr);

// R422
ATbool traverse_OptCharLength(ATerm term, SgExpression* &expr);

// R425
ATbool traverse_DerivedTypeDef(ATerm term);

// R436
ATbool traverse_DataComponentDefStmt     (ATerm term);
ATbool traverse_DataComponentDefStmtList (ATerm term);

// R501
ATbool traverse_TypeDeclarationStmt(ATerm term);

// R502
ATbool traverse_OptAttrSpecList(ATerm term, SgExprListExp* attr_list);
ATbool traverse_CudaAttributesSpec(ATerm term, SgUntypedOtherExpression* &attr_spec);

// R503
ATbool traverse_EntityDecl     (ATerm term, SgType* declared_type, std::list<EntityDeclTuple> &entity_decls);
ATbool traverse_EntityDeclList (ATerm term, SgType* declared_type, std::list<EntityDeclTuple> &entity_decls);

// R505
ATbool traverse_OptInitialization(ATerm term, SgExpression* &expr);

// R509
ATbool traverse_CoarraySpec    (ATerm term, SgType* base_type, SgArrayType* &array_type);
ATbool traverse_OptCoarraySpec (ATerm term, SgType* base_type, SgArrayType* &array_type);

// R515
ATbool traverse_ArraySpec      (ATerm term, SgType* base_type, SgArrayType* &array_type);
ATbool traverse_OptArraySpec   (ATerm term, SgType* base_type, SgArrayType* &array_type);

// R516
ATbool traverse_ExplicitShape    (ATerm term, SgExpression* &lower_bound, SgExpression* &upper_bound);
ATbool traverse_ExplicitShapeList(ATerm term, SgExprListExp* dim_info);

// R519
ATbool traverse_AssumedShape   (ATerm term, SgExpression* &lower_bound);

// R521
ATbool traverse_AssumedSize    (ATerm term, SgType* declared_type, SgArrayType* &array_type);

// R522
ATbool traverse_AssumedOrImpliedSpec(ATerm term, SgExpression* &lower_bound);

// R545
ATbool traverse_DimensionStmt(ATerm term);
ATbool traverse_ArrayNameSpec(ATerm term, SgType* base_type, SgUntypedInitializedNameList* name_list);

// R560
ATbool traverse_ImplicitStmt(ATerm term);
ATbool traverse_ImplicitSpecList(ATerm term);

// R603
ATbool traverse_VarRef(ATerm term, SgExpression* &var_expr);

// R611
ATbool traverse_DataRef(ATerm term, SgExpression* &var_expr);
// R612
ATbool traverse_PartRef(ATerm term, SgExpression* &expr);

// R620
ATbool traverse_SectionSubscript(ATerm term, SgExpression* &subscript);
ATbool traverse_OptSectionSubscripts(ATerm term, SgExprListExp* &subscripts);

// R621
ATbool traverse_Triplet(ATerm term, SgExpression* &range);

// R624
ATbool traverse_OptImageSelector(ATerm term, SgExprListExp* &image_selector);

ATbool traverse_ContinueStmt(ATerm term, SgUntypedStatement* &continue_stmt);

ATbool traverse_DeclarationTypeSpec ( ATerm term, SgType* &type );
ATbool traverse_IntrinsicTypeSpec   ( ATerm term, SgType* &type );
ATbool traverse_DerivedTypeSpec     ( ATerm term, SgType* &type );

// R722
ATbool traverse_Expression ( ATerm term, SgExpression* &expr );
ATbool traverse_OptExpr    ( ATerm term, SgExpression* &expr );

// R732
ATbool traverse_AssignmentStmt(ATerm term);

// R801
ATbool traverse_Block(ATerm term);

// R817
ATbool traverse_NonlabelDoStmt(ATerm term);

// R818
ATbool traverse_OptLoopControl(ATerm term, SgExpression* &initialization,
                                           SgExpression* &upper_bound, SgExpression* &increment);
// R822
ATbool traverse_EndDoStmt(ATerm term);

// R832
ATbool traverse_IfConstruct(ATerm term);

// R833
ATbool traverse_IfThenStmt(ATerm term);

// R834
ATbool traverse_ElseIfStmt(ATerm term);
ATbool traverse_ElseIfStmtList(ATerm term);
ATbool traverse_ElseIfStmtBlock(ATerm term);

// R835
ATbool traverse_ElseStmt(ATerm term);
ATbool traverse_OptElseStmtAndBlock(ATerm term);

// R837
ATbool traverse_IfStmt(ATerm term);

// R838
ATbool traverse_CaseConstruct(ATerm term);
ATbool traverse_CaseStmtAndBlock(ATerm term);

// R839
ATbool traverse_SelectCaseStmt(ATerm term);

// R840
ATbool traverse_CaseStmt(ATerm term);

// R841
ATbool traverse_EndSelectStmt(ATerm term);

// R843
ATbool traverse_CaseSelector(ATerm term, SgExprListExp* &selector);

// R844
ATbool traverse_CaseValueRange(ATerm term, SgExpression* &case_range);

// R851
ATbool traverse_GotoStmt(ATerm term);

// R854
ATbool traverse_ContinueStmt(ATerm term);

// R855
ATbool traverse_StopStmt      ( ATerm term  );
ATbool traverse_ErrorStopStmt ( ATerm term  );
ATbool traverse_OptStopCode   ( ATerm term, SgExpression* &var_StopCode );

// R1050-2018-N2146
ATbool traverse_ForallConstruct(ATerm term);

// R1051-2018-N2146
ATbool traverse_ForallConstructStmt(ATerm term, SgUntypedForAllStatement* &forall_stmt);

// R1052-2018-N2146
ATbool traverse_ForallBodyConstruct(ATerm term);

// R1054-2018-N2146
ATbool traverse_EndForallStmt(ATerm term, SgUntypedNamedStatement* &end_forall_stmt);

// R1055-2018-N2146
ATbool traverse_ForallStmt(ATerm term);

// R1101
ATbool traverse_MainProgram (ATerm term);
ATbool traverse_OptProgramStmt(ATerm term, SgProgramHeaderStatement* &program_decl);
ATbool traverse_OptProgramStmt(ATerm term, boost::optional<std::string> &name, std::vector<std::string> labels);
ATbool traverse_EndProgramStmt(ATerm term, SgProgramHeaderStatement*  program_decl);

// R1104
ATbool traverse_Module (ATerm term);

// R1105
ATbool traverse_ModuleStmt(ATerm term);

// R1106
ATbool traverse_EndModuleStmt(ATerm term);

// R1107-2018-N2146
ATbool traverse_BlockConstruct(ATerm term);

// R1108-2018-N2146
ATbool traverse_BlockStmt(ATerm term);

// R1110-2018-N2146
ATbool traverse_EndBlockStmt(ATerm term);

// R1107
ATbool traverse_OptModuleSubprogramPart(ATerm term);

// R1108
ATbool traverse_ModuleSubprogramList(ATerm term);

// R1109
ATbool traverse_UseStmt(ATerm term);
ATbool traverse_UseStmtList(ATerm term);

// R1110
ATbool traverse_OptModuleNature(ATerm term, SgToken::ROSE_Fortran_Keywords &module_nature);

// R1111, R1112
ATbool traverse_RenameOrOnlyList(ATerm term, bool isOnlyList, SgUntypedTokenPairList* rename_or_only_list);

// R1116
ATbool traverse_Submodule(ATerm term);

// R1117
ATbool traverse_SubmoduleStmt(ATerm term, std::string &ancestor, std::string &parent);
// R1118
ATbool traverse_ParentIdentifier(ATerm term, std::string &ancestor, std::string &parent);

// R1119
ATbool traverse_EndSubmoduleStmt(ATerm term);

// R1120
ATbool traverse_BlockData(ATerm term);

// R1121
ATbool traverse_BlockDataStmt(ATerm term);

// R1122
ATbool traverse_EndBlockDataStmt(ATerm term);

// R1123-2018-N2146
ATbool traverse_LoopConcurrentControl(ATerm term, SgType* &type, SgExprListExp* &header,
                                                  SgExprListExp* &locality, SgExpression* &mask);

// R1125-2018-N2146
ATbool traverse_ConcurrentHeader(ATerm term, SgType* &type, SgExprListExp* &header, SgExpression* &mask);

// R1126-2018-N2146
ATbool traverse_ConcurrentControl(ATerm term, SgUntypedNamedExpression* &control);

// R1130-2018-N2146
ATbool traverse_ConcurrentLocality(ATerm term, SgExprListExp* &locality);

// R1164-2018-N2146
ATbool traverse_SyncAllStmt   (ATerm term);
ATbool traverse_ImageControlStatList (ATerm term, SgExprListExp* sync_stat_list);

// R1166-2018-N2146
ATbool traverse_SyncImagesStmt(ATerm term);

// R1168-2018-N2146
ATbool traverse_SyncMemoryStmt(ATerm term);

// R1169-2018-N2146
ATbool traverse_SyncTeamStmt  (ATerm term);

// R1179-2018-N2146
ATbool traverse_LockStmt  (ATerm term);

// R1181-2018-N2146
ATbool traverse_UnlockStmt  (ATerm term);

// R1201
ATbool traverse_InterfaceBlock(ATerm term);

// R1202
ATbool traverse_InterfaceSpecificationList(ATerm term);

// R1203
ATbool traverse_InterfaceStmt(ATerm term);

// R1204
ATbool traverse_EndInterfaceStmt(ATerm term);

// R1205
ATbool traverse_InterfaceBody(ATerm term);

// R1206
ATbool traverse_ProcedureStmt(ATerm term);

// R1207
ATbool traverse_OptGenericSpec(ATerm term, std::string &name, SgUntypedToken* &generic_spec);

// R1209
ATbool traverse_ImportStmt(ATerm term);
ATbool traverse_ImportStmtList(ATerm term);

// R1210
ATbool traverse_ExternalStmt(ATerm term);

// R1220
ATbool traverse_CallStmt(ATerm term);
ATbool traverse_ProcedureDesignator(ATerm term, SgExpression* &procedure);

// R1222
ATbool traverse_ActualArgSpec      (ATerm term, SgExpression* &arg);
ATbool traverse_ActualArgSpecList  (ATerm term, SgExprListExp* &args);

// R1225
ATbool traverse_OptPrefix(ATerm term, SgExprListExp* prefix_list, SgType* &type);
ATbool traverse_CudaAttributesPrefix(ATerm term, SgUntypedOtherExpression* &prefix);

// R1227
ATbool traverse_FunctionSubprogram (ATerm term);

// R1229
ATbool traverse_OptProcLanguageBindingSpec(ATerm term);

// R1231
ATbool traverse_OptSuffix(ATerm term);

// R1232
ATbool traverse_EndFunctionStmt(ATerm term);

// R1233
ATbool traverse_SubroutineSubprogram (ATerm term);

// R1235
ATbool traverse_OptDummyArgList(ATerm term);

// R1236
ATbool traverse_EndSubroutineStmt(ATerm term);

// R1237
ATbool traverse_SeparateModuleSubprogram(ATerm term);

// R1238
ATbool traverse_MpSubprogramStmt( ATerm term);
ATbool traverse_EndMpSubprogramStmt( ATerm term);

// R1240 entry-stmt

// R1141
ATbool traverse_ReturnStmt( ATerm term);

// R1142
ATbool traverse_ContainsStmt(ATerm term);

ATbool traverse_CudaAttributesStmt(ATerm term);

}; // class Traversal
}  // namespace ATermSupport

#endif
