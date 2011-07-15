
#ifndef _CLANG_FRONTEND_PRIVATE_H_
# define _CLANG_FRONTEND_PRIVATE_H_

#include "ClangFrontend.h"

#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclGroup.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/ParentMap.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtCXX.h"
#include "clang/AST/StmtObjC.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/TemplateName.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeLoc.h"
#include "clang/AST/TypeLocVisitor.h"

#include "clang/Basic/Builtins.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"

#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Parse/ParseAST.h"

#include "clang/Sema/Sema.h"

#include "llvm/Config/config.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"

class ClangToSageTranslator : public clang::ASTConsumer {
    public:
        enum Language {
            C,
            CPLUSPLUS,
            OBJC,
            CUDA,
            OPENCL,
            unknown
        };

    protected:
        std::map<clang::Decl *, SgNode *> p_decl_translation_map;
        std::map<clang::Stmt *, SgNode *> p_stmt_translation_map;
        std::map<const clang::Type *, SgNode *> p_type_translation_map;
        SgGlobal * p_global_scope;

        clang::FileManager   * p_file_manager;
        clang::Diagnostic    * p_diagnostic;
        clang::SourceManager * p_source_manager;
        clang::HeaderSearch  * p_header_search;
        clang::Preprocessor  * p_preprocessor;

        void applySourceRange(SgNode * node, clang::SourceRange source_range);

    public:
        ClangToSageTranslator(std::vector<std::string> & arg);

        virtual ~ClangToSageTranslator();

        SgGlobal * getGlobalScope();

  /* ASTConsumer's methods overload */

        virtual void HandleTranslationUnit(clang::ASTContext & ast_context);

  /* Traverse methods */

        virtual SgNode * Traverse(clang::Decl * decl);
        virtual SgNode * Traverse(clang::Stmt * stmt);
        virtual SgNode * Traverse(const clang::Type * type);
        virtual SgNode * TraverseForDeclContext(clang::DeclContext * decl_context);

  /* Visit methods */

        virtual bool VisitDecl(clang::Decl * decl, SgNode ** node);
    //      virtual bool VisitAccessSpecDecl(clang::AccessSpecDecl * access_spec_decl);
    //      virtual bool VisitBlockDecl(clang::BlockDecl * block_decl);
    //      virtual bool VisitFileScopeAsmDecl(clang::FileScopeAsmDecl * file_scope_asm_decl);
    //      virtual bool VisitFriendDecl(clang::FriendDecl * friend_decl);
    //      virtual bool VisitFriendTemplateDecl(clang::FriendTemplateDecl * friend_template_decl);
    //      virtual bool VisitNamedDecl(clang::NamedDecl * named_decl);
    //          virtual bool VisitLabelDecl(clang::LabelDecl * label_decl);
    //          virtual bool VisitNamespaceDecl(clang::NamespaceDecl * namespace_decl);
//              virtual bool VisitObjCCompatibleAliasDecl(clang::ObjCCompatibleAliasDecl * objc_compatible_alias_decl)
//              virtual bool VisitObjCContainerDecl(clang::ObjCContainerDecl * objc_container_decl);
//                  virtual bool VisitObjCCategoryDecl(clang::ObjCCategoryDecl * objc_categoty_decl);
//                  virtual bool VisitObjCImplDecl(clang::ObjCImplDecl * objc_impl_decl);
//                      virtual bool VisitObjCCategoryImplDecl(clang::ObjCCategoryImplDecl * objc_category_impl_decl);
//                      virtual bool VisitObjCImplementationDecl(clang::ObjCImplementationDecl * objc_implementation_decl);
//                  virtual bool VisitObjCInterfaceDecl(clang::ObjCInterfaceDecl * objc_interface_decl);
//                  virtual bool VisitObjCProtocolDecl(clang::ObjCProtocolDecl * objc_protocol_decl);
//              virtual bool VisitObjCMethodDecl(clang::ObjCMethodDecl * objc_method_decl);
//              virtual bool VisitObjCPropertyDecl(clang::ObjCPropertyDecl * objc_property_decl);
    //          virtual bool VisitTemplateDecl(clang::TemplateDecl * template_decl);
    //              virtual bool VisitRedeclarableTemplateDecl(clang::RedeclarableTemplateDecl * redeclarable_template_decl);
    //                  virtual bool VisitClassTemplateDecl(clang::ClassTemplateDecl * class_template_decl);
    //                  virtual bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl * function_template_decl);
    //                  virtual bool VisitTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl * type_alias_template_decl);
    //              virtual bool VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl * template_template_parm_decl);
    //          virtual bool VisitTypeDecl(clang::TypeDecl * type_decl);
    //              virtual bool VisitTagDecl(clang::TagDecl * tag_decl);
    //                  virtual bool VisitRecordDecl(clang::RecordDecl * record_decl);
    //                      virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl);
    //                          virtual bool VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl * class_tpl_spec_decl);
    //                              virtual bool VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl * class_tpl_part_spec_decl);
    //                  virtual bool VisitEnumDecl(clang::EnumDecl * enum_decl); 
    //              virtual bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl * template_type_parm_decl);
    //              virtual bool VisitTypedefNameDecl(clang::TypedefNameDecl * typedef_name_decl);
    //                  virtual bool VisitTypedefDecl(clang::TypedefDecl * typedef_decl);
    //                  virtual bool VisitTypeAliasDecl(clang::TypeAliasDecl * type_alias_decl);
    //          virtual bool VisitValueDecl(clang::ValueDecl * value_decl);
    //              virtual bool VisitDeclaratorDecl(clang::DeclaratorDecl * declarator_decl);
    //                  virtual bool VisitFieldDecl(clang::FieldDecl * field_decl);
//                          virtual bool VisitObjCAtDefsFieldDecl(clang::ObjCAtDefsFieldDecl * objc_at_defs_field_decl);
//                          virtual bool VisitObjCIvarDecl(clang::ObjCIvarDecl * objc_ivar_decl);
    //                  virtual bool VisitFunctionDecl(clang::FunctionDecl * function_decl);
    //                      virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl * cxx_method_decl);
    //                          virtual bool VisitCXXConstructorDecl(clang::CXXConstructorDecl * cxx_constructor_decl);
    //                          virtual bool VisitCXXConversionDecl(clang::CXXConversionDecl * cxx_conversion_decl);
    //                          virtual bool VisitCXXDestructorDecl(clang::CXXDestructorDecl * cxx_destructor_decl);
    //                  virtual bool VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl * non_type_template_param_decl);
    //                  virtual bool VisitVarDecl(clang::VarDecl * var_decl);
    //                      virtual bool VisitImplicitParamDecl(clang::ImplicitParamDecl * implicit_param_decl);
    //                      virtual bool VisitParmVarDecl(clang::ParmVarDecl * param_var_decl);
    //              virtual bool VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl);
    //              virtual bool VisitIndirectFieldDecl(clang::IndirectFieldDecl * indirect_field_decl);
            virtual bool VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, SgNode ** node);

        virtual bool VisitStmt(clang::Stmt * stmt, SgNode ** node);
    //      virtual bool VisitAsmStmt(clang::AsmStmt * asm_stmt);
    //      virtual bool VisitBreakStmt(clang::BreakStmt * break_stmt);
    //      virtual bool VisitCompoundStmt(clang::CompoundStmt * compound_stmt);
    //      virtual bool VisitContinueStmt(clang::ContinueStmt * continue_stmt);
    //      virtual bool VisitCXXCatchStmt(clang::CXXCatchStmt * cxx_catch_stmt);
    //      virtual bool VisitCXXForRangeStmt(clang::CXXForRangeStmt * cxx_for_range_stmt);
    //      virtual bool VisitCXXTryStmt(clang::CXXTryStmt * cxx_try_stmt);
    //      virtual bool VisitDeclStmt(clang::DeclStmt * decl_stmt);
    //      virtual bool VisitDoStmt(clang::DoStmt * do_stmt);
    //      virtual bool VisitExpr(clang::Expr * expr);
    //          virtual bool VisitAbstractConditionalOperator(clang::AbstractConditionalOperator * abstract_conditional_operator);
    //              virtual bool VisitBinaryConditionalOperator(clang::BinaryConditionalOperator * binary_conditionnal_operator);
    //              virtual bool VisitConditionalOperator(clang::ConditionalOperator * conditional_operator);
    //          virtual bool VisitAddrLabelExpr(clang::AddrLabelExpr * addr_label_expr);
    //          virtual bool VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr);
    //          virtual bool VisitArrayTypeTraitExpr(clang::ArrayTypeTraitExpr * array_type_trait_expr);
    //          virtual bool VisitAsTypeExpr(clang::AsTypeExpr * as_type_expr);
    //          virtual bool VisitBinaryOperator(clang::BinaryOperator * binary_operator);
    //              virtual bool VisitCompoundAssignOperator(clang::CompoundAssignOperator * compound_assign_operator);
    //          virtual bool VisitBinaryTypeTraitExpr(clang::BinaryTypeTraitExpr * binary_type_trait_expr);
    //          virtual bool VisitBlockDeclRefExpr(clang::BlockDeclRefExpr * block_decl_ref_expr);
    //          virtual bool VisitBlockExpr(clang::BlockExpr * block_expr);
    //          virtual bool VisitCallExpr(clang::CallExpr * call_expr);
    //              virtual bool VisitCUDAKernelCallExpr(clang::CUDAKernelCallExpr * cuda_kernel_call_expr);
    //              virtual bool VisitCXXMemberCallExpr(clang::CXXMemberCallExpr * cxx_member_call_expr);
    //              virtual bool VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr * cxx_operator_call_expr);
    //          virtual bool VisitCastExpr(clang::CastExpr * cast_expr);
    //              virtual bool VisitExplicitCastExpr(clang::ExplicitCastExpr * explicit_cast_expr);
    //                  virtual bool VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast);
    //                  virtual bool VisitCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr * cxx_functional_cast_expr);
    //                  virtual bool VisitCXXNamedCastExpr(clang::CXXNamedCastExpr * cxx_named_cast_expr);
    //                      virtual bool VisitCXXConstCastExpr(clang::CXXConstCastExpr * cxx_const_cast_expr);
    //                      virtual bool VisitCXXDynamicCastExpr(clang::CXXDynamicCastExpr * cxx_dynamic_cast_expr);
    //                      virtual bool VisitCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr * cxx_reinterpret_cast_expr);
    //                      virtual bool VisitCXXStaticCastExpr(clang::CXXStaticCastExpr * cxx_static_cast_expr);
    //              virtual bool VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr);
    //          virtual bool VisitCharacterLiteral(clang::CharacterLiteral * character_literal);
    //          virtual bool VisitChooseExpr(clang::ChooseExpr * choose_expr);
    //          virtual bool VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal);
    //          virtual bool VisitCXXBindTemporaryExpr(clang::CXXBindTemporaryExpr * cxx_bind_temporary_expr);
    //          virtual bool VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr * cxx_bool_literal_expr);
    //          virtual bool VisitCXXConstructExpr(clang::CXXConstructExpr * cxx_constructor_expr);
    //          virtual bool VisitCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr * cxx_temporary_object_expr);
    //          virtual bool VisitCXXDefaultArgExpr(clang::CXXDefaultArgExpr * cxx_default_arg_expr);
    //          virtual bool VisitCXXDeleteExpr(clang::CXXDeleteExpr * cxx_delete_expr);
    //          virtual bool VisitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr * cxx_dependent_scope_member_expr);
    //          virtual bool VisitCXXNewExpr(clang::CXXNewExpr * cxx_new_expr);
    //          virtual bool VisitCXXNoexceptExpr(clang::CXXNoexceptExpr * cxx_noexecept_expr);
    //          virtual bool VisitCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr * cxx_null_ptr_literal_expr);
    //          virtual bool VisitCXXPseudoDestructorExpr(clang::CXXPseudoDestructorExpr * cxx_pseudo_destructor_expr);
    //          virtual bool VisitCXXScalarValueInitExpr(clang::CXXScalarValueInitExpr * cxx_scalar_value_init_expr);
    //          virtual bool VisitCXXThisExpr(clang::CXXThisExpr * cxx_this_expr);
    //          virtual bool VisitCXXThrowExpr(clang::CXXThrowExpr * cxx_throw_expr);
    //          virtual bool VisitCXXTypeidExpr(clang::CXXTypeidExpr * cxx_typeid_expr);
    //          virtual bool VisitCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr * cxx_unresolved_construct_expr);
    //          virtual bool VisitCXXUuidofExpr(clang::CXXUuidofExpr * cxx_uuidof_expr);
    //          virtual bool VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr);
    //          virtual bool VisitDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr * dependent_scope_decl_ref_expr);
    //          virtual bool VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr);
    //          virtual bool VisitExpressionTraitExpr(clang::ExpressionTraitExpr * expression_trait_expr);
    //          virtual bool VisitExprWithCleanups(clang::ExprWithCleanups * expr_with_cleanups);
    //          virtual bool VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr);
    //          virtual bool VisitFloatingLiteral(clang::FloatingLiteral * floating_literal);
    //          virtual bool VisitGenericSelectionExpr(clang::GenericSelectionExpr * generic_selection_expr);
    //          virtual bool VisitGNUNullExpr(clang::GNUNullExpr * gnu_null_expr);
    //          virtual bool VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal);
    //          virtual bool VisitImplicitValueInitExpr(clang::ImplicitValueInitExpr * implicit_value_init_expr);
    //          virtual bool VisitInitListExpr(clang::InitListExpr * init_list_expr);
    //          virtual bool VisitIntegerLiteral(clang::IntegerLiteral * integer_literal);
    //          virtual bool VisitMemberExpr(clang::MemberExpr * member_expr);
//              virtual bool VisitObjCEncodeExpr(clang::ObjCEncodeExpr * objc_encoder_expr);
//              virtual bool VisitObjCIsaExpr(clang::ObjCIsaExpr * objc_isa_expr);
//              virtual bool VisitObjCIvarRefExpr(clang::ObjCIvarRefExpr * objc_ivar_ref_expr);
//              virtual bool VisitObjCMessageExpr(clang::ObjCMessageExpr * objc_message_expr);
//              virtual bool VisitObjCPropertyRefExpr(clang::ObjCPropertyRefExpr * objc_propery_ref_expr);
//              virtual bool VisitObjCProtocolExpr(clang::ObjCProtocolExpr * objc_protocol_expr);
//              virtual bool VisitObjCSelectorExpr(clang::ObjCSelectorExpr * objc_selector_expr);
//              virtual bool VisitObjCStringLiteral(clang::ObjCStringLiteral * objc_string_literal);
    //          virtual bool VisitOffsetOfExpr(clang::OffsetOfExpr * offset_of_expr);
    //          virtual bool VisitOpaqueValueExpr(clang::OpaqueValueExpr * opaque_value_expr);
    //          virtual bool VisitOverloadExpr(clang::OverloadExpr * overload_expr);
    //              virtual bool VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr * unresolved_lookup_expr);
    //              virtual bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr * unresolved_member_expr);
    //          virtual bool VisitPackExpansionExpr(clang::PackExpansionExpr * pack_expansion_expr);
    //          virtual bool VisitParenExpr(clang::ParenExpr * paren_expr);
    //          virtual bool VisitParenListExpr(clang::ParenListExpr * paren_list_expr);
    //          virtual bool VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr);
    //          virtual bool VisitShuffleVectorExpr(clang::ShuffleVectorExpr * shuffle_vector_expr);
    //          virtual bool VisitSizeOfPackExpr(clang::SizeOfPackExpr * size_of_pack_expr);
    //          virtual bool VisitStmtExpr(clang::StmtExpr * stmt_expr);
    //          virtual bool VisitStringLiteral(clang::StringLiteral * string_literal);
    //          virtual bool VisitSubstNonTypeTemplateParmPackExpr(clang::SubstNonTypeTemplateParmPackExpr * subst_non_type_template_parm_pack_expr);
    //          virtual bool VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr);
    //          virtual bool VisitUnaryOperator(clang::UnaryOperator * unary_operator);
    //          virtual bool VisitUnaryTypeTraitExpr(clang::UnaryTypeTraitExpr * unary_type_trait_expr);
    //          virtual bool VisitVAArgExpr(clang::VAArgExpr * va_arg_expr);
    //      virtual bool VisitForStmt(clang::ForStmt * for_stmt);
    //      virtual bool VisitGotoStmt(clang::GotoStmt * goto_stmt);
    //      virtual bool VisitIfStmt(clang::IfStmt * if_stmt);
    //      virtual bool VisitIndirectGotoStmt(clang::IndirectGotoStmt * indirect_goto_stmt);
    //      virtual bool VisitLabelStmt(clang::LabelStmt * label_stmt);
    //      virtual bool VisitNullStmt(clang::NullStmt * null_stmt);
//          virtual bool VisitObjCAtCatchStmt(clang::ObjCAtCatchStmt * objc_catch_stmt);
//          virtual bool VisitObjCAtFinallyStmt(clang::ObjCAtFinallyStmt * objc_finally_stmt);
//          virtual bool VisitObjCAtSynchronizedStmt(clang::ObjCAtSynchronizedStmt * objc_synchronized_stmt);
//          virtual bool VisitObjCAtThrowStmt(clang::ObjCAtThrowStmt * objc_throw_stmt);
//          virtual bool VisitObjCAtTryStmt(clang::ObjCAtTryStmt * objc_try_stmt);
//          virtual bool VisitObjCAtForCollectionStmt(clang::ObjCAtForCollectionStmt * objc_for_collection_stmt);
    //      virtual bool VisitReturnStmt(clang::ReturnStmt * return_stmt);
//          virtual bool VisitSEHExceptStmt(clang::SEHExceptStmt * seh_except_stmt);
//          virtual bool VisitSEHFinallyStmt(clang::SEHFinallyStmt * seh_finally_stmt);
//          virtual bool VisitSEHTryStmt(clang::SEHTryStmt * seh_try_stmt);
    //      virtual bool VisitSwitchCase(clang::SwitchCase * switch_case);
    //          virtual bool VisitCaseStmt(clang::CaseStmt * case_stmt);
    //          virtual bool VisitDefaultStmt(clang::DefaultStmt * default_stmt);
    //      virtual bool VisitSwitchStmt(clang::SwitchStmt * switch_stmt);
    //      virtual bool VisitWhileStmt(clang::WhileStmt * while_stmt);

        virtual bool VisitType(clang::Type * type, SgNode ** node);
    //      virtual bool VisitArrayType(clang::ArrayType * array_type);
    //          virtual bool VisitConstantArrayType(clang::ConstantArrayType * constant_array_type);
    //          virtual bool VisitDependentSizedArrayType(clang::DependentSizedArrayType * dependent_sized_array_type);
    //          virtual bool VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type);
    //          virtual bool VisitVariableArrayType(clang::VariableArrayType * variable_array_type);
    //      virtual bool VisitAttributedType(clang::AttributedType * attributed_type);
    //      virtual bool VisitAutoType(clang::AutoType * auto_type);
    //      virtual bool VisitBlockPointerType(clang::BlockPointerType * block_pointer_type);
    //      virtual bool VisitBuiltinType(clang::BuiltinType * builtin_type);
    //      virtual bool VisitComplexType(clang::ComplexType * complex_type);
    //      virtual bool VisitDecltypeType(clang::DecltypeType * decltype_type);
    //          virtual bool VisitDependentDecltypeType(clang::DependentDecltypeType * dependent_decltype_type);
    //      virtual bool VisitDependentSizedExtVectorType(clang::DependentSizedExtVectorType * dependent_sized_ext_vector_type);
    //      virtual bool VisitFunctionType(clang::FunctionType * function_type);
    //          virtual bool VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type);
    //          virtual bool VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type);
    //      virtual bool VisitInjectedClassNameType(clang::InjectedClassNameType * injected_class_name_type);
    //      virtual bool VisitLocInfoType(clang::LocInfoType * loc_info_type);
    //      virtual bool VisitMemberPointerType(clang::MemberPointerType * member_pointer_type);
//          virtual bool VisitObjCObjectPointerType(clang::ObjCObjectPointerType * objc_object_pointer_type);
//          virtual bool VisitObjCObjectType(clang::ObjCObjectType * objc_object_type);
//              virtual bool VisitObjCInterfaceType(clang::ObjCInterfaceType * objc_interface_type);
//              virtual bool VisitObjCObjectTypeImpl(clang::ObjCObjectTypeImpl * objc_object_type_impl);
    //      virtual bool VisitPackExpansionType(clang::PackExpansionType * pack_expansion_type);
    //      virtual bool VisitParenType(clang::ParenType * paren_type);
    //      virtual bool VisitPointerType(clang::PointerType * pointer_type);
    //      virtual bool VisitReferenceType(clang::ReferenceType * reference_type);
    //          virtual bool VisitLValueReferenceType(clang::LValueReferenceType * lvalue_reference_type);
    //          virtual bool VisitRValueReferenceType(clang::RValueReferenceType * rvalue_reference_type);
    //      virtual bool VisitSubstTemplateTypeParmPackType(clang::SubstTemplateTypeParmPackType * subst_template_type_parm_pack_type);
    //      virtual bool VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType * subst_template_type_parm_type);
    //      virtual bool VisitTagType(clang::TagType * tag_type);
    //          virtual bool VisitEnumType(clang::EnumType * enum_type);
    //          virtual bool VisitRecordType(clang::RecordType * record_type);
    //      virtual bool VisitTemplateSpecializationType(clang::TemplateSpecializationType * template_specialization_type);
    //      virtual bool VisitTemplateTypeParmType(clang::TemplateTypeParmType * template_type_parm_type);
    //      virtual bool VisitTypedefType(clang::TypedefType * typedef_type);
    //      virtual bool VisitTypeOfExprType(clang::TypeOfExprType * type_of_expr_type);
    //          virtual bool VisitDependentTypeOfExprType(clang::DependentTypeOfExprType * dependent_type_of_expr_type);
    //      virtual bool VisitTypeOfType(clang::TypeOfType * type_of_type);
    //      virtual bool VisitTypeWithKeyword(clang::TypeWithKeyword * type_with_keyword);
    //          virtual bool VisitDependentNameType(clang::DependentNameType * dependent_name_type);
    //          virtual bool VisitDependentTemplateSpecializationType(clang::DependentTemplateSpecializationType * dependent_template_specialization_type);
    //          virtual bool VisitElaboratedType(clang::ElaboratedType * elaborated_type);
    //      virtual bool VisitUnaryTransformType(clang::UnaryTransformType * unary_transform_type);
    //      virtual bool VisitUnresolvedUsingType(clang::UnresolvedUsingType * unresolved_using_type);
    //      virtual bool VisitVectorType(clang::VectorType * vector_type);
    //          virtual bool VisitExtVectorType(clang::ExtVectorType * ext_vector_type);
};

#endif /* _CLANG_FRONTEND_PRIVATE_H_ */

