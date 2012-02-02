
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/SourceManager.h"

#include "clang/Sema/Sema.h"

#include <iostream>
#include <map>
#include <utility>
#include <sstream>
#include <string>
#include <set>

using namespace clang;

class DotVisitor;

class DotNode {
    public:
        enum NodeClass {
            Decl,
            Type,
            Stmt
        };

        enum Variant {
            none,
            decl_context
        };

    protected:
        std::map<std::string, std::pair<std::string, Variant> > p_children;
        std::map<std::string, std::string> p_attributes;

        std::string p_title;

        NodeClass p_class_node;

        std::string p_identifier;

        bool p_compiler_generated;

        std::string getLabel();

    public:
        DotNode(NodeClass class_node, std::string identifier);
        ~DotNode();

        void setCompilerGenerated(bool val);
        bool isCompilerGenerated();

        void addAttribute(std::string label, std::string val);
        void setChild(std::string label, std::string node, Variant v = none);
        void setTitle(std::string title);

        void applySourceRange(const clang::SourceRange & src_range, const clang::SourceManager & source_manager);

        void useAsDeclContext(clang::DeclContext * decl_context, DotVisitor & dot_visitor);

        void toDot(std::ostream & out, std::string indent = std::string());
};

class DotVisitor : public clang::RecursiveASTVisitor<DotVisitor> {
     protected:
         std::map<std::string, DotNode> p_ast_nodes_map;

         std::set<std::string> p_nodes_in_global_scope;

         std::map<const clang::Stmt *, std::string> p_stmts_name;
         std::map<const clang::Decl *, std::string> p_decls_name;
         std::map<const clang::Type *, std::string> p_types_name;

         std::map<std::string, clang::Decl *> p_all_decls;
         std::map<std::string, clang::Stmt *> p_all_stmts;
         std::map<std::string, const clang::Type *> p_all_types;
         std::set<std::string> p_seen_nodes;

         const clang::SourceManager & p_source_manager;
         const clang::LangOptions   & p_lang_opts;

         unsigned p_empty_cnt;

         bool p_final_stage;

     public:
         DotVisitor(const clang::SourceManager & src_mgr, const clang::LangOptions & lang_opts);
         virtual ~DotVisitor();

         void finalyse();

         void toDot(std::ostream & out);

         std::string genNameForEmptyNode();

         std::string findNameForDeclContext(clang::DeclContext * decl_context);

         std::string genNameForNode(clang::Stmt * stmt);
         std::string genNameForNode(clang::Decl * decl);
         std::string genNameForNode(const clang::Type * type);

         virtual bool shouldVisitTemplateInstantiations();

         void addGlobalScopeChild(clang::Decl * decl);

         bool TraverseDecl(clang::Decl * decl);

         bool VisitDecl(clang::Decl * decl);
             bool VisitAccessSpecDecl(clang::AccessSpecDecl * access_spec_decl);
             bool VisitBlockDecl(clang::BlockDecl * block_decl); // TODO
             bool VisitFileScopeAsmDecl(clang::FileScopeAsmDecl * file_scope_asm_decl); // TODO
             bool VisitFriendDecl(clang::FriendDecl * friend_decl); // TODO
             bool VisitFriendTemplateDecl(clang::FriendTemplateDecl * friend_template_decl); // TODO
             bool VisitNamedDecl(clang::NamedDecl * named_decl);
                 bool VisitLabelDecl(clang::LabelDecl * label_decl); // TODO
                 bool VisitNamespaceDecl(clang::NamespaceDecl * namespace_decl); // TODO
//               bool VisitObjCCompatibleAliasDecl(clang::ObjCCompatibleAliasDecl * objc_compatible_alias_decl); // TODO
//               bool VisitObjCContainerDecl(clang::ObjCContainerDecl * objc_container_decl); // TODO
//                   bool VisitObjCCategoryDecl(clang::ObjCCategoryDecl * objc_categoty_decl); // TODO
//                   bool VisitObjCImplDecl(clang::ObjCImplDecl * objc_impl_decl); // TODO
//                       bool VisitObjCCategoryImplDecl(clang::ObjCCategoryImplDecl * objc_category_impl_decl); // TODO
//                       bool VisitObjCImplementationDecl(clang::ObjCImplementationDecl * objc_implementation_decl); // TODO
//                   bool VisitObjCInterfaceDecl(clang::ObjCInterfaceDecl * objc_interface_decl); // TODO
//                   bool VisitObjCProtocolDecl(clang::ObjCProtocolDecl * objc_protocol_decl); // TODO
//               bool VisitObjCMethodDecl(clang::ObjCMethodDecl * objc_method_decl); // TODO
//               bool VisitObjCPropertyDecl(clang::ObjCPropertyDecl * objc_property_decl); // TODO
                 bool VisitTemplateDecl(clang::TemplateDecl * template_decl); // TODO
                     bool VisitRedeclarableTemplateDecl(clang::RedeclarableTemplateDecl * redeclarable_template_decl); // TODO
                         bool VisitClassTemplateDecl(clang::ClassTemplateDecl * class_template_decl); // TODO
                         bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl * function_template_decl); // TODO
                         bool VisitTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl * type_alias_template_decl); // TODO
                     bool VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl * template_template_parm_decl); // TODO
                 bool VisitTypeDecl(clang::TypeDecl * type_decl); // TODO
                     bool VisitTagDecl(clang::TagDecl * tag_decl); // TODO
                         bool VisitRecordDecl(clang::RecordDecl * record_decl);
                             bool VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl);
                                 bool VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl * class_tpl_spec_decl); // TODO
                                     bool VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl * class_tpl_part_spec_decl); // TODO
                         bool VisitEnumDecl(clang::EnumDecl * enum_decl); // TODO
                     bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl * template_type_parm_decl); // TODO
                     bool VisitTypedefNameDecl(clang::TypedefNameDecl * typedef_name_decl); // TODO
                         bool VisitTypedefDecl(clang::TypedefDecl * typedef_decl); // TODO
                         bool VisitTypeAliasDecl(clang::TypeAliasDecl * type_alias_decl); // TODO
                 bool VisitValueDecl(clang::ValueDecl * value_decl);
                     bool VisitDeclaratorDecl(clang::DeclaratorDecl * declarator_decl);
                         bool VisitFieldDecl(clang::FieldDecl * field_decl);
//                           bool VisitObjCAtDefsFieldDecl(clang::ObjCAtDefsFieldDecl * objc_at_defs_field_decl); // TODO
//                           bool VisitObjCIvarDecl(clang::ObjCIvarDecl * objc_ivar_decl); // TODO
                         bool VisitFunctionDecl(clang::FunctionDecl * function_decl);
                             bool VisitCXXMethodDecl(clang::CXXMethodDecl * cxx_method_decl);
                                 bool VisitCXXConstructorDecl(clang::CXXConstructorDecl * cxx_constructor_decl); //TODO
                                 bool VisitCXXConversionDecl(clang::CXXConversionDecl * cxx_conversion_decl); //TODO
                                 bool VisitCXXDestructorDecl(clang::CXXDestructorDecl * cxx_destructor_decl); //TODO
                         bool VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl * non_type_template_param_decl); // TODO
                         bool VisitVarDecl(clang::VarDecl * var_decl);
                             bool VisitImplicitParamDecl(clang::ImplicitParamDecl * implicit_param_decl);
                             bool VisitParmVarDecl(clang::ParmVarDecl * param_var_decl);
                     bool VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl); // TODO
                     bool VisitIndirectFieldDecl(clang::IndirectFieldDecl * indirect_field_decl); // TODO
             bool VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl); // TODO


         bool VisitStmt(clang::Stmt * stmt);
            bool VisitAsmStmt(clang::AsmStmt * asm_stmt); // TODO
            bool VisitBreakStmt(clang::BreakStmt * break_stmt); // TODO
            bool VisitCompoundStmt(clang::CompoundStmt * compound_stmt);
            bool VisitContinueStmt(clang::ContinueStmt * continue_stmt); // TODO
            bool VisitCXXCatchStmt(clang::CXXCatchStmt * cxx_catch_stmt); // TODO
            bool VisitCXXForRangeStmt(clang::CXXForRangeStmt * cxx_for_range_stmt); // TODO
            bool VisitCXXTryStmt(clang::CXXTryStmt * cxx_try_stmt); // TODO
            bool VisitDeclStmt(clang::DeclStmt * decl_stmt);
            bool VisitDoStmt(clang::DoStmt * do_stmt); // TODO
            bool VisitExpr(clang::Expr * expr); // TODO
                bool VisitAbstractConditionalOperator(clang::AbstractConditionalOperator * abstract_conditional_operator); // TODO
                    bool VisitBinaryConditionalOperator(clang::BinaryConditionalOperator * binary_conditionnal_operator); // TODO
                    bool VisitConditionalOperator(clang::ConditionalOperator * conditional_operator); // TODO
                bool VisitAddrLabelExpr(clang::AddrLabelExpr * addr_label_expr); // TODO
                bool VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr); // TODO
                bool VisitArrayTypeTraitExpr(clang::ArrayTypeTraitExpr * array_type_trait_expr); // TODO
                bool VisitAsTypeExpr(clang::AsTypeExpr * as_type_expr); // TODO
                bool VisitBinaryOperator(clang::BinaryOperator * binary_operator); // TODO
                    bool VisitCompoundAssignOperator(clang::CompoundAssignOperator * compound_assign_operator); // TODO
                bool VisitBinaryTypeTraitExpr(clang::BinaryTypeTraitExpr * binary_type_trait_expr); // TODO
                bool VisitBlockDeclRefExpr(clang::BlockDeclRefExpr * block_decl_ref_expr); // TODO
                bool VisitBlockExpr(clang::BlockExpr * block_expr); // TODO
                bool VisitCallExpr(clang::CallExpr * call_expr); // TODO
                    bool VisitCUDAKernelCallExpr(clang::CUDAKernelCallExpr * cuda_kernel_call_expr); // TODO
                    bool VisitCXXMemberCallExpr(clang::CXXMemberCallExpr * cxx_member_call_expr); // TODO
                    bool VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr * cxx_operator_call_expr); // TODO
                bool VisitCastExpr(clang::CastExpr * cast_expr);
                    bool VisitExplicitCastExpr(clang::ExplicitCastExpr * explicit_cast_expr); // TODO
                        bool VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast); // TODO
                        bool VisitCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr * cxx_functional_cast_expr); // TODO
                        bool VisitCXXNamedCastExpr(clang::CXXNamedCastExpr * cxx_named_cast_expr); // TODO
                            bool VisitCXXConstCastExpr(clang::CXXConstCastExpr * cxx_const_cast_expr); // TODO
                            bool VisitCXXDynamicCastExpr(clang::CXXDynamicCastExpr * cxx_dynamic_cast_expr); // TODO
                            bool VisitCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr * cxx_reinterpret_cast_expr); // TODO
                            bool VisitCXXStaticCastExpr(clang::CXXStaticCastExpr * cxx_static_cast_expr); // TODO
                    bool VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr);
                bool VisitCharacterLiteral(clang::CharacterLiteral * character_literal); // TODO
                bool VisitChooseExpr(clang::ChooseExpr * choose_expr); // TODO
                bool VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal); // TODO
                bool VisitCXXBindTemporaryExpr(clang::CXXBindTemporaryExpr * cxx_bind_temporary_expr); // TODO
                bool VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr * cxx_bool_literal_expr); // TODO
                bool VisitCXXConstructExpr(clang::CXXConstructExpr * cxx_constructor_expr); // TODO
                bool VisitCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr * cxx_temporary_object_expr); // TODO
                bool VisitCXXDefaultArgExpr(clang::CXXDefaultArgExpr * cxx_default_arg_expr); // TODO
                bool VisitCXXDeleteExpr(clang::CXXDeleteExpr * cxx_delete_expr); // TODO
                bool VisitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr * cxx_dependent_scope_member_expr); // TODO
                bool VisitCXXNewExpr(clang::CXXNewExpr * cxx_new_expr); // TODO
                bool VisitCXXNoexceptExpr(clang::CXXNoexceptExpr * cxx_noexecept_expr); // TODO
                bool VisitCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr * cxx_null_ptr_literal_expr); // TODO
                bool VisitCXXPseudoDestructorExpr(clang::CXXPseudoDestructorExpr * cxx_pseudo_destructor_expr); // TODO
                bool VisitCXXScalarValueInitExpr(clang::CXXScalarValueInitExpr * cxx_scalar_value_init_expr); // TODO
                bool VisitCXXThisExpr(clang::CXXThisExpr * cxx_this_expr); // TODO
                bool VisitCXXThrowExpr(clang::CXXThrowExpr * cxx_throw_expr); // TODO
                bool VisitCXXTypeidExpr(clang::CXXTypeidExpr * cxx_typeid_expr); // TODO
                bool VisitCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr * cxx_unresolved_construct_expr); // TODO
                bool VisitCXXUuidofExpr(clang::CXXUuidofExpr * cxx_uuidof_expr); // TODO
                bool VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr);
                bool VisitDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr * dependent_scope_decl_ref_expr); // TODO
                bool VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr); // TODO
                bool VisitExpressionTraitExpr(clang::ExpressionTraitExpr * expression_trait_expr); // TODO
                bool VisitExprWithCleanups(clang::ExprWithCleanups * expr_with_cleanups); // TODO
                bool VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr); // TODO
                bool VisitFloatingLiteral(clang::FloatingLiteral * floating_literal); // TODO
                bool VisitGenericSelectionExpr(clang::GenericSelectionExpr * generic_selection_expr); // TODO
                bool VisitGNUNullExpr(clang::GNUNullExpr * gnu_null_expr); // TODO
                bool VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal); // TODO
                bool VisitImplicitValueInitExpr(clang::ImplicitValueInitExpr * implicit_value_init_expr); // TODO
                bool VisitInitListExpr(clang::InitListExpr * init_list_expr); // TODO
                bool VisitIntegerLiteral(clang::IntegerLiteral * integer_literal);
                bool VisitMemberExpr(clang::MemberExpr * member_expr); // TODO
//              bool VisitObjCEncodeExpr(clang::ObjCEncodeExpr * objc_encoder_expr);
//              bool VisitObjCIsaExpr(clang::ObjCIsaExpr * objc_isa_expr);
//              bool VisitObjCIvarRefExpr(clang::ObjCIvarRefExpr * objc_ivar_ref_expr);
//              bool VisitObjCMessageExpr(clang::ObjCMessageExpr * objc_message_expr);
//              bool VisitObjCPropertyRefExpr(clang::ObjCPropertyRefExpr * objc_propery_ref_expr);
//              bool VisitObjCProtocolExpr(clang::ObjCProtocolExpr * objc_protocol_expr);
//              bool VisitObjCSelectorExpr(clang::ObjCSelectorExpr * objc_selector_expr);
//              bool VisitObjCStringLiteral(clang::ObjCStringLiteral * objc_string_literal);
                bool VisitOffsetOfExpr(clang::OffsetOfExpr * offset_of_expr); // TODO
                bool VisitOpaqueValueExpr(clang::OpaqueValueExpr * opaque_value_expr); // TODO
                bool VisitOverloadExpr(clang::OverloadExpr * overload_expr); // TODO
                    bool VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr * unresolved_lookup_expr); // TODO
                    bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr * unresolved_member_expr); // TODO
                bool VisitPackExpansionExpr(clang::PackExpansionExpr * pack_expansion_expr); // TODO
                bool VisitParenExpr(clang::ParenExpr * paren_expr); // TODO
                bool VisitParenListExpr(clang::ParenListExpr * paren_list_expr); // TODO
                bool VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr); // TODO
                bool VisitShuffleVectorExpr(clang::ShuffleVectorExpr * shuffle_vector_expr); // TODO
                bool VisitSizeOfPackExpr(clang::SizeOfPackExpr * size_of_pack_expr); // TODO
                bool VisitStmtExpr(clang::StmtExpr * stmt_expr); // TODO
                bool VisitStringLiteral(clang::StringLiteral * string_literal); // TODO
                bool VisitSubstNonTypeTemplateParmPackExpr(clang::SubstNonTypeTemplateParmPackExpr * subst_non_type_template_parm_pack_expr); // TODO
                bool VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr); // TODO
                bool VisitUnaryOperator(clang::UnaryOperator * unary_operator); // TODO
                bool VisitUnaryTypeTraitExpr(clang::UnaryTypeTraitExpr * unary_type_trait_expr); // TODO
                bool VisitVAArgExpr(clang::VAArgExpr * va_arg_expr); // TODO
            bool VisitForStmt(clang::ForStmt * for_stmt); // TODO
            bool VisitGotoStmt(clang::GotoStmt * goto_stmt); // TODO
            bool VisitIfStmt(clang::IfStmt * if_stmt); // TODO
            bool VisitIndirectGotoStmt(clang::IndirectGotoStmt * indirect_goto_stmt); // TODO
            bool VisitLabelStmt(clang::LabelStmt * label_stmt); // TODO
            bool VisitNullStmt(clang::NullStmt * null_stmt); // TODO
//          bool VisitObjCAtCatchStmt(clang::ObjCAtCatchStmt * objc_catch_stmt);
//          bool VisitObjCAtFinallyStmt(clang::ObjCAtFinallyStmt * objc_finally_stmt);
//          bool VisitObjCAtSynchronizedStmt(clang::ObjCAtSynchronizedStmt * objc_synchronized_stmt);
//          bool VisitObjCAtThrowStmt(clang::ObjCAtThrowStmt * objc_throw_stmt);
//          bool VisitObjCAtTryStmt(clang::ObjCAtTryStmt * objc_try_stmt);
//          bool VisitObjCAtForCollectionStmt(clang::ObjCAtForCollectionStmt * objc_for_collection_stmt);
            bool VisitReturnStmt(clang::ReturnStmt * return_stmt);
//          bool VisitSEHExceptStmt(clang::SEHExceptStmt * seh_except_stmt);
//          bool VisitSEHFinallyStmt(clang::SEHFinallyStmt * seh_finally_stmt);
//          bool VisitSEHTryStmt(clang::SEHTryStmt * seh_try_stmt);
            bool VisitSwitchCase(clang::SwitchCase * switch_case); // TODO
                bool VisitCaseStmt(clang::CaseStmt * case_stmt); // TODO
                bool VisitDefaultStmt(clang::DefaultStmt * default_stmt); // TODO
            bool VisitSwitchStmt(clang::SwitchStmt * switch_stmt); // TODO
            bool VisitWhileStmt(clang::WhileStmt * while_stmt); // TODO

         bool VisitType(clang::Type * type);
             bool VisitArrayType(clang::ArrayType * array_type); // TODO
                 bool VisitConstantArrayType(clang::ConstantArrayType * constant_array_type); // TODO
                 bool VisitDependentSizedArrayType(clang::DependentSizedArrayType * dependent_sized_array_type); // TODO
                 bool VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type); // TODO
                 bool VisitVariableArrayType(clang::VariableArrayType * variable_array_type); // TODO
             bool VisitAttributedType(clang::AttributedType * attributed_type); // TODO
             bool VisitAutoType(clang::AutoType * auto_type); // TODO
             bool VisitBlockPointerType(clang::BlockPointerType * block_pointer_type); // TODO
             bool VisitBuiltinType(clang::BuiltinType * builtin_type);
             bool VisitComplexType(clang::ComplexType * complex_type); // TODO
             bool VisitDecltypeType(clang::DecltypeType * decltype_type); // TODO
                 bool VisitDependentDecltypeType(clang::DependentDecltypeType * dependent_decltype_type); // TODO
             bool VisitDependentSizedExtVectorType(clang::DependentSizedExtVectorType * dependent_sized_ext_vector_type); // TODO
             bool VisitFunctionType(clang::FunctionType * function_type);
                 bool VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type);
                 bool VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type);
             bool VisitInjectedClassNameType(clang::InjectedClassNameType * injected_class_name_type); // TODO
             bool VisitLocInfoType(clang::LocInfoType * loc_info_type); // TODO
             bool VisitMemberPointerType(clang::MemberPointerType * member_pointer_type); // TODO
//             bool VisitObjCObjectPointerType(clang::ObjCObjectPointerType * objc_object_pointer_type);
//             bool VisitObjCObjectType(clang::ObjCObjectType * objc_object_type);
//                 bool VisitObjCInterfaceType(clang::ObjCInterfaceType * objc_interface_type);
//                 bool VisitObjCObjectTypeImpl(clang::ObjCObjectTypeImpl * objc_object_type_impl);
             bool VisitPackExpansionType(clang::PackExpansionType * pack_expansion_type); // TODO
             bool VisitParenType(clang::ParenType * paren_type); // TODO
             bool VisitPointerType(clang::PointerType * pointer_type);
             bool VisitReferenceType(clang::ReferenceType * reference_type); // TODO
                 bool VisitLValueReferenceType(clang::LValueReferenceType * lvalue_reference_type); // TODO
                 bool VisitRValueReferenceType(clang::RValueReferenceType * rvalue_reference_type); // TODO
             bool VisitSubstTemplateTypeParmPackType(clang::SubstTemplateTypeParmPackType * subst_template_type_parm_pack_type); // TODO
             bool VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType * subst_template_type_parm_type); // TODO
             bool VisitTagType(clang::TagType * tag_type); // TODO
                 bool VisitEnumType(clang::EnumType * enum_type); // TODO
                 bool VisitRecordType(clang::RecordType * record_type); // TODO
             bool VisitTemplateSpecializationType(clang::TemplateSpecializationType * template_specialization_type); // TODO
             bool VisitTemplateTypeParmType(clang::TemplateTypeParmType * template_type_parm_type); // TODO
             bool VisitTypedefType(clang::TypedefType * typedef_type); // TODO
             bool VisitTypeOfExprType(clang::TypeOfExprType * type_of_expr_type); // TODO
                 bool VisitDependentTypeOfExprType(clang::DependentTypeOfExprType * dependent_type_of_expr_type); // TODO
             bool VisitTypeOfType(clang::TypeOfType * type_of_type); // TODO
             bool VisitTypeWithKeyword(clang::TypeWithKeyword * type_with_keyword); // TODO
                 bool VisitDependentNameType(clang::DependentNameType * dependent_name_type); // TODO
                 bool VisitDependentTemplateSpecializationType(clang::DependentTemplateSpecializationType * dependent_template_specialization_type); // TODO
                 bool VisitElaboratedType(clang::ElaboratedType * elaborated_type); // TODO
             bool VisitUnaryTransformType(clang::UnaryTransformType * unary_transform_type); // TODO
             bool VisitUnresolvedUsingType(clang::UnresolvedUsingType * unresolved_using_type); // TODO
             bool VisitVectorType(clang::VectorType * vector_type); // TODO
                 bool VisitExtVectorType(clang::ExtVectorType * ext_vector_type); // TODO

};

