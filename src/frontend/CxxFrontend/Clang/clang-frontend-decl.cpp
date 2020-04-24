#include "sage3basic.h"
#include "clang-frontend-private.hpp"

SgSymbol * ClangToSageTranslator::GetSymbolFromSymbolTable(clang::NamedDecl * decl) {
    if (decl == NULL) return NULL;

    SgScopeStatement * scope = SageBuilder::topScopeStack();

    SgName name(decl->getNameAsString());

#if DEBUG_SYMBOL_TABLE_LOOKUP
    std::cerr << "Lookup symbol for: " << name << std::endl;
#endif

    if (name == "") {
        return NULL;
    }

    std::list<SgScopeStatement *>::reverse_iterator it;
    SgSymbol * sym = NULL;
    switch (decl->getKind()) {
        case clang::Decl::Typedef:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                 sym = (*it)->lookup_typedef_symbol(name);
                 it++;
            }
            break;
        }
        case clang::Decl::Var:
        case clang::Decl::ParmVar:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_variable_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::Function:
        {
            SgType * tmp_type = buildTypeFromQualifiedType(((clang::FunctionDecl *)decl)->getType());
            SgFunctionType * type = isSgFunctionType(tmp_type);
            ROSE_ASSERT(type);
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_function_symbol(name, type);
                it++;
            }
            break;
        }
        case clang::Decl::Field:
        {
            SgClassDeclaration * sg_class_decl = isSgClassDeclaration(Traverse(((clang::FieldDecl *)decl)->getParent()));
            ROSE_ASSERT(sg_class_decl != NULL);
            if (sg_class_decl->get_definingDeclaration() == NULL)
                std::cerr << "Runtime Error: cannot find the definition of the class/struct associate to the field: " << name << std::endl;
            else {
                scope = isSgClassDeclaration(sg_class_decl->get_definingDeclaration())->get_definition();
                // TODO: for C++, if 'scope' is in 'SageBuilder::ScopeStack': problem!!!
                //       It means that we are currently building the class
                while (scope != NULL && sym == NULL) {
                    sym = scope->lookup_variable_symbol(name);
                    scope = scope->get_scope();
                }
            }
            break;
        }
        case clang::Decl::CXXRecord:
        case clang::Decl::Record:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_class_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::Label:
        {
            // Should not be reach as we use Traverse to retrieve Label (they are "terminal" statements) (it avoids the problem of forward use of label: goto before declaration)
            name = SgName(((clang::LabelDecl *)decl)->getStmt()->getName());
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_label_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::EnumConstant:
        {
            name = SgName(((clang::EnumConstantDecl *)decl)->getName());
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_enum_field_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::Enum:
        {
            name = SgName(((clang::EnumDecl *)decl)->getName());
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_enum_symbol(name);
                it++;
            }
            break;
        }
        default:
            std::cerr << "Runtime Error: Unknown type of Decl. (" << decl->getDeclKindName() << ")" << std::endl;
    }

    return sym;
}

SgNode * ClangToSageTranslator::Traverse(clang::Decl * decl) {
    if (decl == NULL)
        return NULL;

    std::map<clang::Decl *, SgNode *>::iterator it = p_decl_translation_map.find(decl);
    if (it != p_decl_translation_map.end()) {
#if DEBUG_TRAVERSE_DECL
        std::cerr << "Traverse Decl : " << decl << " ";
        if (clang::NamedDecl::classof(decl)) {
            std::cerr << ": " << ((clang::NamedDecl *)decl)->getNameAsString() << ") ";
        }
        std::cerr << " already visited : node = " << it->second << std::endl;
#endif
        return it->second;
    }

    SgNode * result = NULL;
    bool ret_status = false;

    switch (decl->getKind()) {
        case clang::Decl::AccessSpec:
            ret_status = VisitAccessSpecDecl((clang::AccessSpecDecl *)decl, &result);
            break;
        case clang::Decl::Block:
            ret_status = VisitBlockDecl((clang::BlockDecl *)decl, &result);
            break;
        case clang::Decl::Captured:
            ret_status = VisitCapturedDecl((clang::CapturedDecl *)decl, &result);
            break;
        case clang::Decl::Empty:
            ret_status = VisitEmptyDecl((clang::EmptyDecl *)decl, &result);
            break;
        case clang::Decl::Export:
            ret_status = VisitExportDecl((clang::ExportDecl *)decl, &result);
            break;
        case clang::Decl::ExternCContext:
            ret_status = VisitExternCContextDecl((clang::ExternCContextDecl *)decl, &result);
            break;
        case clang::Decl::FileScopeAsm:
            ret_status = VisitFileScopeAsmDecl((clang::FileScopeAsmDecl *)decl, &result);
            break;
        case clang::Decl::Friend:
            ret_status = VisitFriendDecl((clang::FriendDecl *)decl, &result);
            break;
        case clang::Decl::FriendTemplate:
            ret_status = VisitFriendTemplateDecl((clang::FriendTemplateDecl *)decl, &result);
            break;
        case clang::Decl::Import:
            ret_status = VisitImportDecl((clang::ImportDecl *)decl, &result);
            break;
        case clang::Decl::Label:
            ret_status = VisitLabelDecl((clang::LabelDecl *)decl, &result);
            break;
        case clang::Decl::NamespaceAlias:
            ret_status = VisitNamespaceAliasDecl((clang::NamespaceAliasDecl *)decl, &result);
            break;
        case clang::Decl::Namespace:
            ret_status = VisitNamespaceDecl((clang::NamespaceDecl *)decl, &result);
            break;
        case clang::Decl::BuiltinTemplate:
            ret_status = VisitBuiltinTemplateDecl((clang::BuiltinTemplateDecl *)decl, &result);
            break;
        case clang::Decl::Concept:
            ret_status = VisitConceptDecl((clang::ConceptDecl *)decl, &result);
            break;
        case clang::Decl::ClassTemplate:
            ret_status = VisitClassTemplateDecl((clang::ClassTemplateDecl *)decl, &result);
            break;
        case clang::Decl::FunctionTemplate:
            ret_status = VisitFunctionTemplateDecl((clang::FunctionTemplateDecl *)decl, &result);
            break;
        case clang::Decl::TypeAliasTemplate:
            ret_status = VisitTypeAliasTemplateDecl((clang::TypeAliasTemplateDecl *)decl, &result);
            break;
        case clang::Decl::VarTemplate:
            ret_status = VisitVarTemplateDecl((clang::VarTemplateDecl *)decl, &result);
            break;
        case clang::Decl::TemplateTemplateParm:
            ret_status = VisitTemplateTemplateParmDecl((clang::TemplateTemplateParmDecl *)decl, &result);
            break;
        case clang::Decl::Record:
            ret_status = VisitRecordDecl((clang::RecordDecl *)decl, &result);
            break;
        case clang::Decl::CXXRecord:
            ret_status = VisitCXXRecordDecl((clang::CXXRecordDecl *)decl, &result);
            break;
        case clang::Decl::ClassTemplateSpecialization:
            ret_status = VisitClassTemplateSpecializationDecl((clang::ClassTemplateSpecializationDecl *)decl, &result);
            break;
        case clang::Decl::ClassTemplatePartialSpecialization:
            ret_status = VisitClassTemplatePartialSpecializationDecl((clang::ClassTemplatePartialSpecializationDecl *)decl, &result);
            break;
        case clang::Decl::Enum:
            ret_status = VisitEnumDecl((clang::EnumDecl *)decl, &result);
            break;
        case clang::Decl::TemplateTypeParm:
            ret_status = VisitTemplateTypeParmDecl((clang::TemplateTypeParmDecl *)decl, &result);
            break;
        case clang::Decl::Typedef:
            ret_status = VisitTypedefDecl((clang::TypedefDecl *)decl, &result);
            break;
        case clang::Decl::TypeAlias:
            ret_status = VisitTypeAliasDecl((clang::TypeAliasDecl *)decl, &result);
            break;
        case clang::Decl::UnresolvedUsingTypename:
            ret_status = VisitUnresolvedUsingTypenameDecl((clang::UnresolvedUsingTypenameDecl *)decl, &result);
            break;
        case clang::Decl::Using:
            ret_status = VisitUsingDecl((clang::UsingDecl *)decl, &result);
            break;
        case clang::Decl::UsingDirective:
            ret_status = VisitUsingDirectiveDecl((clang::UsingDirectiveDecl *)decl, &result);
            break;
        case clang::Decl::UsingPack:
            ret_status = VisitUsingPackDecl((clang::UsingPackDecl *)decl, &result);
            break;
        case clang::Decl::ConstructorUsingShadow:
            ret_status = VisitConstructorUsingShadowDecl((clang::ConstructorUsingShadowDecl *)decl, &result);
            break;
        case clang::Decl::Binding:
            ret_status = VisitBindingDecl((clang::BindingDecl *)decl, &result);
            break;
        case clang::Decl::Field:
            ret_status = VisitFieldDecl((clang::FieldDecl *)decl, &result);
            break;
        case clang::Decl::Function:
            ret_status = VisitFunctionDecl((clang::FunctionDecl *)decl, &result);
            break;
        case clang::Decl::CXXDeductionGuide:
            ret_status = VisitCXXDeductionGuideDecl((clang::CXXDeductionGuideDecl *)decl, &result);
            break;
        case clang::Decl::CXXConstructor:
            ret_status = VisitCXXConstructorDecl((clang::CXXConstructorDecl *)decl, &result);
            break;
        case clang::Decl::CXXConversion:
            ret_status = VisitCXXConversionDecl((clang::CXXConversionDecl *)decl, &result);
            break;
        case clang::Decl::CXXDestructor:
            ret_status = VisitCXXDestructorDecl((clang::CXXDestructorDecl *)decl, &result);
            break;
        case clang::Decl::MSProperty:
            ret_status = VisitMSPropertyDecl((clang::MSPropertyDecl *)decl, &result);
            break;
        case clang::Decl::NonTypeTemplateParm:
            ret_status = VisitNonTypeTemplateParmDecl((clang::NonTypeTemplateParmDecl *)decl, &result);
            break;
        case clang::Decl::Decomposition:
            ret_status = VisitDecompositionDecl((clang::DecompositionDecl *)decl, &result);
            break;
        case clang::Decl::ImplicitParam:
            ret_status = VisitImplicitParamDecl((clang::ImplicitParamDecl *)decl, &result);
            break;
        case clang::Decl::OMPCapturedExpr:
            ret_status = VisitOMPCaptureExprDecl((clang::OMPCapturedExprDecl *)decl, &result);
            break;
        case clang::Decl::ParmVar:
            ret_status = VisitParmVarDecl((clang::ParmVarDecl *)decl, &result);
            break;
        case clang::Decl::VarTemplatePartialSpecialization:
            ret_status = VisitVarTemplatePartialSpecializationDecl((clang::VarTemplatePartialSpecializationDecl *)decl, &result);
            break;
        case clang::Decl::EnumConstant:
            ret_status = VisitEnumConstantDecl((clang::EnumConstantDecl *)decl, &result);
            break;
        case clang::Decl::IndirectField:
            ret_status = VisitIndirectFieldDecl((clang::IndirectFieldDecl *)decl, &result);
            break;
        case clang::Decl::OMPDeclareMapper:
            ret_status = VisitOMPDeclareMapperDecl((clang::OMPDeclareMapperDecl *)decl, &result);
            break;
        case clang::Decl::OMPDeclareReduction:
            ret_status = VisitOMPDeclareReductionDecl((clang::OMPDeclareReductionDecl *)decl, &result);
            break;
        case clang::Decl::UnresolvedUsingValue:
            ret_status = VisitUnresolvedUsingValueDecl((clang::UnresolvedUsingValueDecl *)decl, &result);
            break;
        case clang::Decl::OMPAllocate:
            ret_status = VisitOMPAllocateDecl((clang::OMPAllocateDecl *)decl, &result);
            break;
        case clang::Decl::OMPRequires:
            ret_status = VisitOMPRequiresDecl((clang::OMPRequiresDecl *)decl, &result);
            break;
        case clang::Decl::OMPThreadPrivate:
            ret_status = VisitOMPThreadPrivateDecl((clang::OMPThreadPrivateDecl *)decl, &result);
            break;
        case clang::Decl::PragmaComment:
            ret_status = VisitPragmaCommentDecl((clang::PragmaCommentDecl *)decl, &result);
            break;
        case clang::Decl::PragmaDetectMismatch:
            ret_status = VisitPragmaDetectMismatchDecl((clang::PragmaDetectMismatchDecl *)decl, &result);
            break;
        case clang::Decl::StaticAssert:
            ret_status = VisitStaticAssertDecl((clang::StaticAssertDecl *)decl, &result);
            break;
        case clang::Decl::TranslationUnit:
            ret_status = VisitTranslationUnitDecl((clang::TranslationUnitDecl *)decl, &result);
            break;
        default:
            std::cerr << "Unknown declacaration kind: " << decl->getDeclKindName() << " !" << std::endl;
            ROSE_ASSERT(false);
    }

    ROSE_ASSERT(ret_status == false || result != NULL);

    p_decl_translation_map.insert(std::pair<clang::Decl *, SgNode *>(decl, result));

#if DEBUG_TRAVERSE_DECL
    std::cerr << "Traverse(clang::Decl : " << decl << " ";
    if (clang::NamedDecl::classof(decl)) {
        std::cerr << ": " << ((clang::NamedDecl *)decl)->getNameAsString() << ") ";
    }
    std::cerr << " visit done : node = " << result << std::endl;
#endif

    return ret_status ? result : NULL;
}

SgNode * ClangToSageTranslator::TraverseForDeclContext(clang::DeclContext * decl_context) {
    return Traverse((clang::Decl*)decl_context);
}

/**********************/
/* Visit Declarations */
/**********************/

bool ClangToSageTranslator::VisitDecl(clang::Decl * decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitDecl" << std::endl;
#endif    
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the declaration..." << std::endl;
        return false;
    }

    if (!isSgGlobal(*node))
        applySourceRange(*node, decl->getSourceRange());

    // TODO attributes
/*
    std::cerr << "Attribute list for " << decl->getDeclKindName() << " (" << decl << "): ";
    clang::Decl::attr_iterator it;
    for (it = decl->attr_begin(); it != decl->attr_end(); it++) {
        std::cerr << (*it)->getKind() << ", ";
    }
    std::cerr << std::endl;

    if (clang::VarDecl::classof(decl)) {
        clang::VarDecl * var_decl = (clang::VarDecl *)decl;
        std::cerr << "Stoprage class for " << decl->getDeclKindName() << " (" << decl << "): " << var_decl->getStorageClass() << std::endl;
    }
*/
    return true;
}

bool ClangToSageTranslator::VisitAccessSpecDecl(clang::AccessSpecDecl * access_spec_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitAccessSpecDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(access_spec_decl, node) && res;
}

bool ClangToSageTranslator::VisitBlockDecl(clang::BlockDecl * block_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitBlockDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(block_decl, node) && res;
}

bool ClangToSageTranslator::VisitCapturedDecl(clang::CapturedDecl * captured_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitCapturedDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(captured_decl, node) && res;
}

bool ClangToSageTranslator::VisitEmptyDecl(clang::EmptyDecl * empty_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitEmptyDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(empty_decl, node) && res;
}

bool ClangToSageTranslator::VisitExportDecl(clang::ExportDecl * export_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitExportDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(export_decl, node) && res;
}

bool ClangToSageTranslator::VisitExternCContextDecl(clang::ExternCContextDecl * ccontent_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitCContextDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(ccontent_decl, node) && res;
}

bool ClangToSageTranslator::VisitFileScopeAsmDecl(clang::FileScopeAsmDecl * file_scope_asm_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitFileScopeAsmDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(file_scope_asm_decl, node) && res;
}

bool ClangToSageTranslator::VisitFriendDecl(clang::FriendDecl * friend_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitFriendDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(friend_decl, node) && res;
}

bool ClangToSageTranslator::VisitFriendTemplateDecl(clang::FriendTemplateDecl * friend_template_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitFriendTemplateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(friend_template_decl, node) && res;
}

bool ClangToSageTranslator::VisitImportDecl(clang::ImportDecl * import_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitImportDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitDecl(import_decl, node) && res;
}

bool ClangToSageTranslator::VisitNamedDecl(clang::NamedDecl * named_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitNamedDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitDecl(named_decl, node) && res;
}

bool ClangToSageTranslator::VisitLabelDecl(clang::LabelDecl * label_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitLabelDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitNamedDecl(label_decl, node) && res;
}

bool ClangToSageTranslator::VisitNamespaceAliasDecl(clang::NamespaceAliasDecl * namespace_alias_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitNamespaceAliasDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitNamedDecl(namespace_alias_decl, node) && res;
}

bool ClangToSageTranslator::VisitNamespaceDecl(clang::NamespaceDecl * namespace_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitNamespaceDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitNamedDecl(namespace_decl, node) && res;
}

bool ClangToSageTranslator::VisitTemplateDecl(clang::TemplateDecl * template_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTemplateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitNamedDecl(template_decl, node) && res;
}

bool ClangToSageTranslator::VisitBuiltinTemplateDecl(clang::BuiltinTemplateDecl * builtin_template_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitBuiltinTemplateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitTemplateDecl(builtin_template_decl, node) && res;
}

bool ClangToSageTranslator::VisitConceptDecl(clang::ConceptDecl * concept_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitConceptDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitTemplateDecl(concept_decl, node) && res;
}

bool ClangToSageTranslator::VisitRedeclarableTemplateDecl(clang::RedeclarableTemplateDecl * redeclarable_template_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitRedeclarableTemplateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitTemplateDecl(redeclarable_template_decl, node) && res;
}

bool ClangToSageTranslator::VisitClassTemplateDecl(clang::ClassTemplateDecl * class_template_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitClassTemplateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitRedeclarableTemplateDecl(class_template_decl, node) && res;
}

bool ClangToSageTranslator::VisitFunctionTemplateDecl(clang::FunctionTemplateDecl * function_template_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitFunctionTemplateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitRedeclarableTemplateDecl(function_template_decl, node) && res;
}

bool ClangToSageTranslator::VisitTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl * type_alias_template_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTypeAliasTemplateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitRedeclarableTemplateDecl(type_alias_template_decl, node) && res;
}

bool ClangToSageTranslator::VisitVarTemplateDecl(clang::VarTemplateDecl * var_template_decl, SgNode ** node) {

#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitVarTemplateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitRedeclarableTemplateDecl(var_template_decl, node) && res;
}


bool ClangToSageTranslator::VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl * template_template_parm_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTemplateTemplateParmDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitTemplateDecl(template_template_parm_decl, node) && res;
}

bool ClangToSageTranslator::VisitTypeDecl(clang::TypeDecl * type_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTypeDecl" << std::endl;
#endif

    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitNamedDecl(type_decl, node) && res;
}

bool ClangToSageTranslator::VisitTagDecl(clang::TagDecl * tag_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTagDecl" << std::endl;
#endif

    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitTypeDecl(tag_decl, node) && res;
}

bool ClangToSageTranslator::VisitRecordDecl(clang::RecordDecl * record_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitRecordDecl" << std::endl;
#endif

    // FIXME May have to check the symbol table first, because of out-of-order traversal of C++ classes (Could be done in CxxRecord class...)

    bool res = true;

    SgClassDeclaration * sg_class_decl = NULL;

  // Find previous declaration

    clang::RecordDecl * prev_record_decl = record_decl->getPreviousDecl();
    SgClassSymbol * sg_prev_class_sym = isSgClassSymbol(GetSymbolFromSymbolTable(prev_record_decl));
    SgClassDeclaration * sg_prev_class_decl = sg_prev_class_sym == NULL ? NULL : isSgClassDeclaration(sg_prev_class_sym->get_declaration());

    SgClassDeclaration * sg_first_class_decl = sg_prev_class_decl == NULL ? NULL : isSgClassDeclaration(sg_prev_class_decl->get_firstNondefiningDeclaration());
    SgClassDeclaration * sg_def_class_decl = sg_prev_class_decl == NULL ? NULL : isSgClassDeclaration(sg_prev_class_decl->get_definingDeclaration());

    ROSE_ASSERT(sg_first_class_decl != NULL || sg_def_class_decl == NULL);

    bool had_prev_decl = sg_first_class_decl != NULL;

  // Name

    SgName name(record_decl->getNameAsString());

  // Type of class

    SgClassDeclaration::class_types type_of_class;
    switch (record_decl->getTagKind()) {
        case clang::TTK_Struct:
            type_of_class = SgClassDeclaration::e_struct;
            break;
        case clang::TTK_Class:
            type_of_class = SgClassDeclaration::e_class;
            break;
        case clang::TTK_Union:
            type_of_class = SgClassDeclaration::e_union;
            break;
        default:
            std::cerr << "Runtime error: RecordDecl can only be a struct/class/union." << std::endl;
            res = false;
    }

  // Build declaration(s)

    sg_class_decl = new SgClassDeclaration(name, type_of_class, NULL, NULL);

    sg_class_decl->set_scope(SageBuilder::topScopeStack());
    sg_class_decl->set_parent(SageBuilder::topScopeStack());

    SgClassType * type = NULL;
    if (sg_first_class_decl != NULL) {
        type = sg_first_class_decl->get_type();
    }
    else {
        type = SgClassType::createType(sg_class_decl);
    }
    ROSE_ASSERT(type != NULL);
    sg_class_decl->set_type(type);

    if (record_decl->isAnonymousStructOrUnion()) sg_class_decl->set_isUnNamed(true);

    if (!had_prev_decl) {
        sg_first_class_decl = sg_class_decl;
        sg_first_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
        sg_first_class_decl->set_definingDeclaration(NULL);
        sg_first_class_decl->set_definition(NULL);
        sg_first_class_decl->setForward();
        if (!record_decl->field_empty()) {
            sg_def_class_decl = new SgClassDeclaration(name, type_of_class, type, NULL);
            sg_def_class_decl->set_scope(SageBuilder::topScopeStack());
            if (record_decl->isAnonymousStructOrUnion()) sg_def_class_decl->set_isUnNamed(true);
            sg_def_class_decl->set_parent(SageBuilder::topScopeStack());

            sg_class_decl = sg_def_class_decl; // we return thew defining decl

            sg_def_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
            sg_def_class_decl->set_definingDeclaration(sg_def_class_decl);

            sg_first_class_decl->set_definingDeclaration(sg_def_class_decl);
            setCompilerGeneratedFileInfo(sg_first_class_decl);
        }
    }
    else if (!record_decl->field_empty()) {
        if (sg_def_class_decl != NULL) {
            delete sg_class_decl;
            *node = sg_def_class_decl;
            return true;
        }
        sg_def_class_decl = sg_class_decl;
        sg_def_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
        sg_def_class_decl->set_definingDeclaration(sg_def_class_decl);
        sg_first_class_decl->set_definingDeclaration(sg_def_class_decl);
    }
    else // second (or more) non-defining declaration
        return false; // FIXME ROSE need only one non-defining declaration (SageBuilder don't let me build another one....)

  // Update symbol table

    if (!had_prev_decl) {
        SgScopeStatement * scope = SageBuilder::topScopeStack();
        SgClassSymbol * class_symbol = new SgClassSymbol(sg_first_class_decl);
        scope->insert_symbol(name, class_symbol);
    }

  // Build ClassDefinition

    if (!record_decl->field_empty()) {
        SgClassDefinition * sg_class_def = isSgClassDefinition(sg_def_class_decl->get_definition());
        if (sg_class_def == NULL) {
            sg_class_def = SageBuilder::buildClassDefinition_nfi(sg_def_class_decl);
        }
        sg_def_class_decl->set_definition(sg_class_def);

        ROSE_ASSERT(sg_class_def->get_symbol_table() != NULL);

        applySourceRange(sg_class_def, record_decl->getSourceRange());

        SageBuilder::pushScopeStack(sg_class_def);

        clang::RecordDecl::field_iterator it;
        for (it = record_decl->field_begin(); it != record_decl->field_end(); it++) {
            SgNode * tmp_field = Traverse(*it);
            SgDeclarationStatement * field_decl = isSgDeclarationStatement(tmp_field);
            ROSE_ASSERT(field_decl != NULL);
            sg_class_def->append_member(field_decl);
            field_decl->set_parent(sg_class_def);
        }

        SageBuilder::popScopeStack();
    }

    ROSE_ASSERT(sg_class_decl->get_definingDeclaration() == NULL || isSgClassDeclaration(sg_class_decl->get_definingDeclaration())->get_definition() != NULL);
    ROSE_ASSERT(sg_first_class_decl->get_definition() == NULL);
    ROSE_ASSERT(sg_def_class_decl == NULL || sg_def_class_decl->get_definition() != NULL);

    *node = sg_class_decl;

    return VisitTagDecl(record_decl, node) && res;
}

bool ClangToSageTranslator::VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitCXXRecordDecl" << std::endl;
#endif
    bool res = VisitRecordDecl(cxx_record_decl, node);

    clang::CXXRecordDecl::base_class_iterator it_base;
    for (it_base = cxx_record_decl->bases_begin(); it_base !=  cxx_record_decl->bases_end(); it_base++) {
        // TODO add base classes
    }

    clang::CXXRecordDecl::method_iterator it_method;
    for (it_method = cxx_record_decl->method_begin(); it_method !=  cxx_record_decl->method_end(); it_method++) {
        // TODO
    }

    clang::CXXRecordDecl::ctor_iterator it_ctor;
    for (it_ctor = cxx_record_decl->ctor_begin(); it_ctor != cxx_record_decl->ctor_end(); it_ctor++) {
        // TODO if not tranversed as methods
    }

    clang::CXXRecordDecl::friend_iterator it_friend;
    for (it_friend = cxx_record_decl->friend_begin(); it_friend != cxx_record_decl->friend_end(); it_friend++) {
        // TODO
    }

    clang::CXXDestructorDecl * destructor = cxx_record_decl->getDestructor();
    // TODO

    return res;
}

bool ClangToSageTranslator::VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl * class_tpl_spec_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitClassTemplateSpecializationDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitCXXRecordDecl(class_tpl_spec_decl, node) && res;
}

bool ClangToSageTranslator::VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl * class_tpl_part_spec_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitClassTemplatePartialSpecializationDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitClassTemplateSpecializationDecl(class_tpl_part_spec_decl, node) && res;
}

bool ClangToSageTranslator::VisitEnumDecl(clang::EnumDecl * enum_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitEnumDecl" << std::endl;
#endif
    bool res = true;

    SgName name(enum_decl->getNameAsString());


    clang::EnumDecl * prev_enum_decl = enum_decl->getPreviousDecl();
    SgEnumSymbol * sg_prev_enum_sym = isSgEnumSymbol(GetSymbolFromSymbolTable(prev_enum_decl));
    SgEnumDeclaration * sg_prev_enum_decl = sg_prev_enum_sym == NULL ? NULL : isSgEnumDeclaration(sg_prev_enum_sym->get_declaration());
    sg_prev_enum_decl = sg_prev_enum_decl == NULL ? NULL : isSgEnumDeclaration(sg_prev_enum_decl->get_definingDeclaration());

    SgEnumDeclaration * sg_enum_decl = SageBuilder::buildEnumDeclaration(name, SageBuilder::topScopeStack());
    *node = sg_enum_decl;

    if (sg_prev_enum_decl == NULL || sg_prev_enum_decl->get_enumerators().size() == 0) {
      clang::EnumDecl::enumerator_iterator it;
      for (it = enum_decl->enumerator_begin(); it != enum_decl->enumerator_end(); it++) {
          SgNode * tmp_enumerator = Traverse(*it);
          SgInitializedName * enumerator = isSgInitializedName(tmp_enumerator);

          ROSE_ASSERT(enumerator);

          enumerator->set_scope(SageBuilder::topScopeStack());
          sg_enum_decl->append_enumerator(enumerator);
      }
    }
    else {
      sg_enum_decl->set_definingDeclaration(sg_prev_enum_decl);
      sg_enum_decl->set_firstNondefiningDeclaration(sg_prev_enum_decl->get_firstNondefiningDeclaration());
    }

    return VisitDecl(enum_decl, node) && res;
}

bool ClangToSageTranslator::VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl * template_type_parm_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTemplateTypeParmDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitTypeDecl(template_type_parm_decl, node) && res;
}

bool ClangToSageTranslator::VisitTypedefNameDecl(clang::TypedefNameDecl * typedef_name_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTypedefNameDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitTypeDecl(typedef_name_decl, node) && res;
}

bool ClangToSageTranslator::VisitTypedefDecl(clang::TypedefDecl * typedef_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTypedefDecl" << std::endl;
#endif
    bool res = true;

    SgName name(typedef_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(typedef_decl->getUnderlyingType());

    SgTypedefDeclaration * sg_typedef_decl = SageBuilder::buildTypedefDeclaration_nfi(name, type, SageBuilder::topScopeStack());

    if (isSgClassType(type)) {
        std::map<SgClassType *, bool>::iterator bool_it = p_class_type_decl_first_see_in_type.find(isSgClassType(type));
        ROSE_ASSERT(bool_it != p_class_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            sg_typedef_decl->set_declaration(isSgNamedType(type)->get_declaration()->get_definingDeclaration());
            sg_typedef_decl->set_typedefBaseTypeContainsDefiningDeclaration(true);
        }
    }
    else if (isSgEnumType(type)) {
        std::map<SgEnumType *, bool>::iterator bool_it = p_enum_type_decl_first_see_in_type.find(isSgEnumType(type));
        ROSE_ASSERT(bool_it != p_enum_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            sg_typedef_decl->set_declaration(isSgEnumType(type)->get_declaration()->get_definingDeclaration());
            sg_typedef_decl->set_typedefBaseTypeContainsDefiningDeclaration(true);
        }
    }

    *node = sg_typedef_decl;

    return VisitTypedefNameDecl(typedef_decl, node) && res;
}

bool ClangToSageTranslator::VisitTypeAliasDecl(clang::TypeAliasDecl * type_alias_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTypeAliasDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitTypedefNameDecl(type_alias_decl, node) && res;
}

bool ClangToSageTranslator::VisitUnresolvedUsingTypenameDecl(clang::UnresolvedUsingTypenameDecl * unresolved_using_type_name_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitUnresolvedUsingTypenameDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitTypeDecl(unresolved_using_type_name_decl, node) && res;
}

bool ClangToSageTranslator::VisitUsingDecl(clang::UsingDecl * using_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitUsingDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitNamedDecl(using_decl, node) && res;
}

bool ClangToSageTranslator::VisitUsingDirectiveDecl(clang::UsingDirectiveDecl * using_directive_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitUsingDirectiveDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitNamedDecl(using_directive_decl, node) && res;
}

bool ClangToSageTranslator::VisitUsingPackDecl(clang::UsingPackDecl * using_pack_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitUsingPackDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitNamedDecl(using_pack_decl, node) && res;
}

bool ClangToSageTranslator::VisitUsingShadowDecl(clang::UsingShadowDecl * using_shadow_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitUsingShadowDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitNamedDecl(using_shadow_decl, node) && res;
}

bool ClangToSageTranslator::VisitConstructorUsingShadowDecl(clang::ConstructorUsingShadowDecl * constructor_using_shadow_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitConstructorUsingShadowDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitNamedDecl(constructor_using_shadow_decl, node) && res;
}

bool ClangToSageTranslator::VisitValueDecl(clang::ValueDecl * value_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitValueDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitNamedDecl(value_decl, node) && res;
}

bool ClangToSageTranslator::VisitBindingDecl(clang::BindingDecl * binding_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitBindingDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitValueDecl(binding_decl, node) && res;
}
    
bool ClangToSageTranslator::VisitDeclaratorDecl(clang::DeclaratorDecl * declarator_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitDeclaratorDecl" << std::endl;
#endif  
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitValueDecl(declarator_decl, node) && res;
}
    

bool ClangToSageTranslator::VisitFieldDecl(clang::FieldDecl * field_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitFieldDecl" << std::endl;
#endif  
    bool res = true;
    
    SgName name(field_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(field_decl->getType());

    clang::Expr * init_expr = field_decl->getInClassInitializer();
    SgNode * tmp_init = Traverse(init_expr);
    SgExpression * expr = isSgExpression(tmp_init);
    // TODO expression list if aggregated initializer !
    if (tmp_init != NULL && expr == NULL) {
        std::cerr << "Runtime error: not a SgInitializer..." << std::endl;
        res = false;
    }
    SgInitializer * init = expr != NULL ? SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type()) : NULL;
    if (init != NULL)
        applySourceRange(init, init_expr->getSourceRange());

  // Cannot use 'SageBuilder::buildVariableDeclaration' because of anonymous field
    // *node = SageBuilder::buildVariableDeclaration(name, type, init, SageBuilder::topScopeStack());
  // Build it by hand...
    SgVariableDeclaration * var_decl = new SgVariableDeclaration(name, type, init);

    if (isSgClassType(type)) {
        std::map<SgClassType *, bool>::iterator bool_it = p_class_type_decl_first_see_in_type.find(isSgClassType(type));
        ROSE_ASSERT(bool_it != p_class_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            var_decl->set_baseTypeDefiningDeclaration(isSgNamedType(type)->get_declaration()->get_definingDeclaration());
            var_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
        }
    }
    else if (isSgEnumType(type)) {
        std::map<SgEnumType *, bool>::iterator bool_it = p_enum_type_decl_first_see_in_type.find(isSgEnumType(type));
        ROSE_ASSERT(bool_it != p_enum_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            var_decl->set_baseTypeDefiningDeclaration(isSgEnumType(type)->get_declaration()->get_definingDeclaration());
            var_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
        }
    }

    var_decl->set_firstNondefiningDeclaration(var_decl);
    var_decl->set_parent(SageBuilder::topScopeStack());

    ROSE_ASSERT(var_decl->get_variables().size() == 1);

    SgInitializedName * init_name = var_decl->get_variables()[0];
    ROSE_ASSERT(init_name != NULL);
    init_name->set_scope(SageBuilder::topScopeStack());

    applySourceRange(init_name, field_decl->getSourceRange());

    SgVariableDefinition * var_def = isSgVariableDefinition(init_name->get_declptr());
    ROSE_ASSERT(var_def != NULL);
    applySourceRange(var_def, field_decl->getSourceRange());

    SgVariableSymbol * var_symbol = new SgVariableSymbol(init_name);
    SageBuilder::topScopeStack()->insert_symbol(name, var_symbol);

    *node = var_decl;

    return VisitDeclaratorDecl(field_decl, node) && res; 
}

bool ClangToSageTranslator::VisitFunctionDecl(clang::FunctionDecl * function_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitFunctionDecl" << std::endl;
#endif
    bool res = true;

    // FIXME: There is something weird here when try to Traverse a function reference in a recursive function (when first Traverse is not complete)
    //        It seems that it tries to instantiate the decl inside the function...
    //        It may be faster to recode from scratch...
    //   If I am not wrong this have been fixed....

    SgName name(function_decl->getNameAsString());

    SgType * ret_type = buildTypeFromQualifiedType(function_decl->getReturnType());

    SgFunctionParameterList * param_list = SageBuilder::buildFunctionParameterList_nfi();
      applySourceRange(param_list, function_decl->getSourceRange()); // FIXME find the good SourceRange (should be stored by Clang...)

    for (unsigned i = 0; i < function_decl->getNumParams(); i++) {
        SgNode * tmp_init_name = Traverse(function_decl->getParamDecl(i));
        SgInitializedName * init_name = isSgInitializedName(tmp_init_name);
        if (tmp_init_name != NULL && init_name == NULL) {
            std::cerr << "Runtime error: tmp_init_name != NULL && init_name == NULL" << std::endl;
            res = false;
            continue;
        }

        param_list->append_arg(init_name);
    }

    if (function_decl->isVariadic()) {
        SgName empty = "";
        SgType * ellipses_type = SgTypeEllipse::createType();
        param_list->append_arg(SageBuilder::buildInitializedName_nfi(empty, ellipses_type, NULL));
    }

    SgFunctionDeclaration * sg_function_decl;

    if (function_decl->isThisDeclarationADefinition()) {
        sg_function_decl = SageBuilder::buildDefiningFunctionDeclaration(name, ret_type, param_list, NULL);
        sg_function_decl->set_definingDeclaration(sg_function_decl);

        if (function_decl->isVariadic()) {
            sg_function_decl->hasEllipses();
        }

        if (!function_decl->hasBody()) {
            std::cerr << "Defining function declaration without body..." << std::endl;
            res = false;
        }
/*
        if (sg_function_decl->get_definition() != NULL) SageInterface::deleteAST(sg_function_decl->get_definition());

        SgFunctionDefinition * function_definition = new SgFunctionDefinition(sg_function_decl, NULL);

        SgInitializedNamePtrList & init_names = param_list->get_args();
        SgInitializedNamePtrList::iterator it;
        for (it = init_names.begin(); it != init_names.end(); it++) {
            (*it)->set_scope(function_definition);
            SgSymbolTable * st = function_definition->get_symbol_table();
            ROSE_ASSERT(st != NULL);
            SgVariableSymbol * tmp_sym  = new SgVariableSymbol(*it);
            st->insert((*it)->get_name(), tmp_sym);
        }
*/
        SgFunctionDefinition * function_definition = sg_function_decl->get_definition();

        if (sg_function_decl->get_definition()->get_body() != NULL)
            SageInterface::deleteAST(sg_function_decl->get_definition()->get_body());

        SageBuilder::pushScopeStack(function_definition);

        SgNode * tmp_body = Traverse(function_decl->getBody());
        SgBasicBlock * body = isSgBasicBlock(tmp_body);

        SageBuilder::popScopeStack();

        if (tmp_body != NULL && body == NULL) {
            std::cerr << "Runtime error: tmp_body != NULL && body == NULL" << std::endl;
            res = false;
        }
        else {
            function_definition->set_body(body);
            body->set_parent(function_definition);
            applySourceRange(function_definition, function_decl->getSourceRange());
        }

        sg_function_decl->set_definition(function_definition);
        function_definition->set_parent(sg_function_decl);

        SgFunctionDeclaration * first_decl;
        if (function_decl->getFirstDecl() == function_decl) {
            SgFunctionParameterList * param_list_ = SageBuilder::buildFunctionParameterList_nfi();
              setCompilerGeneratedFileInfo(param_list_);
            SgInitializedNamePtrList & init_names = param_list->get_args();
            SgInitializedNamePtrList::iterator it;
            for (it = init_names.begin(); it != init_names.end(); it++) {
                SgInitializedName * init_param = new SgInitializedName(**it);
                setCompilerGeneratedFileInfo(init_param);
                param_list_->append_arg(init_param);
            }

            first_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, ret_type, param_list_, NULL);
            setCompilerGeneratedFileInfo(first_decl);
            first_decl->set_parent(SageBuilder::topScopeStack());
            first_decl->set_firstNondefiningDeclaration(first_decl);
            if (function_decl->isVariadic()) first_decl->hasEllipses();
        }
        else {
            SgSymbol * tmp_symbol = GetSymbolFromSymbolTable(function_decl->getFirstDecl());
            SgFunctionSymbol * symbol = isSgFunctionSymbol(tmp_symbol);
            if (tmp_symbol != NULL && symbol == NULL) {
                std::cerr << "Runtime error: tmp_symbol != NULL && symbol == NULL" << std::endl;
                res = false;
            }
            if (symbol != NULL)
                first_decl = isSgFunctionDeclaration(symbol->get_declaration());
        }

        sg_function_decl->set_firstNondefiningDeclaration(first_decl);
        first_decl->set_definingDeclaration(sg_function_decl);
    }
    else {
        sg_function_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, ret_type, param_list, NULL);

        if (function_decl->isVariadic()) sg_function_decl->hasEllipses();

        SgInitializedNamePtrList & init_names = param_list->get_args();
        SgInitializedNamePtrList::iterator it;
        for (it = init_names.begin(); it != init_names.end(); it++) {
             (*it)->set_scope(SageBuilder::topScopeStack());
        }

        if (function_decl->getFirstDecl() != function_decl) {
            SgSymbol * tmp_symbol = GetSymbolFromSymbolTable(function_decl->getFirstDecl());
            SgFunctionSymbol * symbol = isSgFunctionSymbol(tmp_symbol);
            if (tmp_symbol != NULL && symbol == NULL) {
                std::cerr << "Runtime error: tmp_symbol != NULL && symbol == NULL" << std::endl;
                res = false;
            }
            SgFunctionDeclaration * first_decl = NULL;
            if (symbol != NULL) {
                first_decl = isSgFunctionDeclaration(symbol->get_declaration());
            }
            else {
                // FIXME Is it correct?
                SgNode * tmp_first_decl = Traverse(function_decl->getFirstDecl());
                first_decl = isSgFunctionDeclaration(tmp_first_decl);
                ROSE_ASSERT(first_decl != NULL);
                // ROSE_ASSERT(!"We should have see the first declaration already");
            }

            if (first_decl != NULL) {
                if (first_decl->get_firstNondefiningDeclaration() != NULL)
                    sg_function_decl->set_firstNondefiningDeclaration(first_decl->get_firstNondefiningDeclaration());
                else {
                    ROSE_ASSERT(first_decl->get_firstNondefiningDeclaration() != NULL);
                }
            }
            else {
                ROSE_ASSERT(!"First declaration not found!");
            }
        }
        else {
            sg_function_decl->set_firstNondefiningDeclaration(sg_function_decl);
        }
    }

    ROSE_ASSERT(sg_function_decl->get_firstNondefiningDeclaration() != NULL);
/* // TODO Fix problem with function symbols...
    SgSymbol * symbol = GetSymbolFromSymbolTable(function_decl);
    if (symbol == NULL) {
        SgFunctionSymbol * func_sym = new SgFunctionSymbol(isSgFunctionDeclaration(sg_function_decl->get_firstNondefiningDeclaration()));
        SageBuilder::topScopeStack()->insert_symbol(name, func_sym);        
    }
*/
//  ROSE_ASSERT(GetSymbolFromSymbolTable(function_decl) != NULL);

    *node = sg_function_decl;

    return VisitDeclaratorDecl(function_decl, node) && res;
}

bool ClangToSageTranslator::VisitCXXDeductionGuideDecl(clang::CXXDeductionGuideDecl * cxx_deduction_guide_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitCXXDeductionGuideDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitFunctionDecl(cxx_deduction_guide_decl, node) && res;
}

bool ClangToSageTranslator::VisitCXXMethodDecl(clang::CXXMethodDecl * cxx_method_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitCXXMethodDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitFunctionDecl(cxx_method_decl, node) && res;
}

bool ClangToSageTranslator::VisitCXXConstructorDecl(clang::CXXConstructorDecl * cxx_constructor_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitCXXConstructorDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitCXXMethodDecl(cxx_constructor_decl, node) && res;
}

bool ClangToSageTranslator::VisitCXXConversionDecl(clang::CXXConversionDecl * cxx_conversion_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitCXXConversionDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitCXXMethodDecl(cxx_conversion_decl, node) && res;
}

bool ClangToSageTranslator::VisitCXXDestructorDecl(clang::CXXDestructorDecl * cxx_destructor_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitCXXDestructorDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitCXXMethodDecl(cxx_destructor_decl, node) && res;
}

bool ClangToSageTranslator::VisitMSPropertyDecl(clang::MSPropertyDecl * ms_property_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitMSPropertyDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDeclaratorDecl(ms_property_decl, node) && res;
}

bool ClangToSageTranslator::VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl * non_type_template_param_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitNonTypeTemplateParmDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDeclaratorDecl(non_type_template_param_decl, node) && res;
}

bool ClangToSageTranslator::VisitVarDecl(clang::VarDecl * var_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitVarDecl" << std::endl;
#endif
    bool res = true;

  // Create the SAGE node: SgVariableDeclaration

    SgName name(var_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(var_decl->getType());

    clang::Expr * init_expr = var_decl->getInit();
    SgNode * tmp_init = Traverse(init_expr);
    SgExpression * expr = isSgExpression(tmp_init);
    if (tmp_init != NULL && expr == NULL) {
        std::cerr << "Runtime error: not a SgInitializer..." << std::endl; // TODO
        res = false;
    }
    SgExprListExp * expr_list_expr = isSgExprListExp(expr);

    SgInitializer * init = NULL;
    if (expr_list_expr != NULL)
        init = SageBuilder::buildAggregateInitializer(expr_list_expr, type);
    else if (expr != NULL)
        init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
    if (init != NULL)
        applySourceRange(init, init_expr->getSourceRange());

    SgVariableDeclaration * sg_var_decl = new SgVariableDeclaration(name, type, init); // scope: obtain from the scope stack.

    if (isSgClassType(type)) {
        std::map<SgClassType *, bool>::iterator bool_it = p_class_type_decl_first_see_in_type.find(isSgClassType(type));
        ROSE_ASSERT(bool_it != p_class_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            sg_var_decl->set_baseTypeDefiningDeclaration(isSgNamedType(type)->get_declaration()->get_definingDeclaration());
            sg_var_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
        }
    }
    else if (isSgEnumType(type)) {
        std::map<SgEnumType *, bool>::iterator bool_it = p_enum_type_decl_first_see_in_type.find(isSgEnumType(type));
        ROSE_ASSERT(bool_it != p_enum_type_decl_first_see_in_type.end());
        if (bool_it->second) {
            sg_var_decl->set_baseTypeDefiningDeclaration(isSgEnumType(type)->get_declaration()->get_definingDeclaration());
            sg_var_decl->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
        }
    }

    sg_var_decl->set_firstNondefiningDeclaration(sg_var_decl);
    sg_var_decl->set_parent(SageBuilder::topScopeStack());

    ROSE_ASSERT(sg_var_decl->get_variables().size() == 1);

    SgInitializedName * init_name = sg_var_decl->get_variables()[0];
    ROSE_ASSERT(init_name != NULL);
    init_name->set_scope(SageBuilder::topScopeStack());

    applySourceRange(init_name, var_decl->getSourceRange());

    SgVariableDefinition * var_def = isSgVariableDefinition(init_name->get_declptr());
    ROSE_ASSERT(var_def != NULL);
    applySourceRange(var_def, var_decl->getSourceRange());

    SgVariableSymbol * var_symbol = new SgVariableSymbol(init_name);
    SageBuilder::topScopeStack()->insert_symbol(name, var_symbol);

    *node = sg_var_decl;

    return VisitDeclaratorDecl(var_decl, node) && res;
}

bool ClangToSageTranslator::VisitDecompositionDecl(clang::DecompositionDecl * decomposition_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitDecompositionDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitVarDecl(decomposition_decl, node) && res;
}

bool ClangToSageTranslator::VisitImplicitParamDecl(clang::ImplicitParamDecl * implicit_param_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitImplicitParamDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitVarDecl(implicit_param_decl, node) && res;
}

bool ClangToSageTranslator::VisitOMPCaptureExprDecl(clang::OMPCapturedExprDecl * omp_capture_expr_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitOMPCaptureExprDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitVarDecl(omp_capture_expr_decl, node) && res;
}

bool ClangToSageTranslator::VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitParmVarDecl" << std::endl;
#endif
    bool res = true;

    SgName name(param_var_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(param_var_decl->getOriginalType());

    SgInitializer * init = NULL;

    if (param_var_decl->hasDefaultArg()) {
        SgNode * tmp_expr = Traverse(param_var_decl->getDefaultArg());
        SgExpression * expr = isSgExpression(tmp_expr);
        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
        }
        else {
            applySourceRange(expr, param_var_decl->getDefaultArgRange());
            init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
            applySourceRange(init, param_var_decl->getDefaultArgRange());
        }
    }

    *node = SageBuilder::buildInitializedName(name, type, init);

    return VisitDeclaratorDecl(param_var_decl, node) && res;
}

bool ClangToSageTranslator::VisitVarTemplateSpecializationDecl(clang::VarTemplateSpecializationDecl * var_template_specialization_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitVarTemplateSpecializationDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDeclaratorDecl(var_template_specialization_decl, node) && res;
}

bool ClangToSageTranslator::VisitVarTemplatePartialSpecializationDecl(clang::VarTemplatePartialSpecializationDecl * var_template_partial_specialization_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitVarTemplatePartialSpecializationDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitVarTemplateSpecializationDecl(var_template_partial_specialization_decl, node) && res;
}

bool  ClangToSageTranslator::VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitEnumConstantDecl" << std::endl;
#endif
    bool res = true;

    SgName name(enum_constant_decl->getNameAsString());

    SgType * type = buildTypeFromQualifiedType(enum_constant_decl->getType());

    SgInitializer * init = NULL;

    if (enum_constant_decl->getInitExpr() != NULL) {
        SgNode * tmp_expr = Traverse(enum_constant_decl->getInitExpr());
        SgExpression * expr = isSgExpression(tmp_expr);
        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
        }
        else {
            init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
        }
    }

    SgInitializedName * init_name = SageBuilder::buildInitializedName(name, type, init);

    SgEnumFieldSymbol * symbol = new SgEnumFieldSymbol(init_name);

    SageBuilder::topScopeStack()->insert_symbol(name, symbol);

    *node = init_name;

    return VisitValueDecl(enum_constant_decl, node) && res;
}

bool ClangToSageTranslator::VisitIndirectFieldDecl(clang::IndirectFieldDecl * indirect_field_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitIndirectFieldDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitValueDecl(indirect_field_decl, node) && res;
}

bool ClangToSageTranslator::VisitOMPDeclareMapperDecl(clang::OMPDeclareMapperDecl * omp_declare_mapper_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitOMPDeclareMapperDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitValueDecl(omp_declare_mapper_decl, node) && res;
}

bool ClangToSageTranslator::VisitOMPDeclareReductionDecl(clang::OMPDeclareReductionDecl * omp_declare_reduction_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitOMPDeclareReductionDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitValueDecl(omp_declare_reduction_decl, node) && res;
}

bool ClangToSageTranslator::VisitUnresolvedUsingValueDecl(clang::UnresolvedUsingValueDecl * unresolved_using_value_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitUnresolvedUsingValueDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitValueDecl(unresolved_using_value_decl, node) && res;
}

bool ClangToSageTranslator::VisitOMPAllocateDecl(clang::OMPAllocateDecl * omp_allocate_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitOMPAllocateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(omp_allocate_decl, node) && res;
}

bool ClangToSageTranslator::VisitOMPRequiresDecl(clang::OMPRequiresDecl * omp_requires_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitOMPRequiresDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(omp_requires_decl, node) && res;
}

bool ClangToSageTranslator::VisitOMPThreadPrivateDecl(clang::OMPThreadPrivateDecl * omp_thread_private_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitOMPThreadPrivateDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(omp_thread_private_decl, node) && res;
}

bool ClangToSageTranslator::VisitPragmaCommentDecl(clang::PragmaCommentDecl * pragma_comment_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitPragmaCommentDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(pragma_comment_decl, node) && res;
}

bool ClangToSageTranslator::VisitPragmaDetectMismatchDecl(clang::PragmaDetectMismatchDecl * pragma_detect_mismatch_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitPragmaDetectMismatchDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(pragma_detect_mismatch_decl, node) && res;
}

bool ClangToSageTranslator::VisitStaticAssertDecl(clang::StaticAssertDecl * pragma_static_assert_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitStaticAssertDecl" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_TODO == 0); // TODO

    return VisitDecl(pragma_static_assert_decl, node) && res;
}

bool ClangToSageTranslator::VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, SgNode ** node) {
#if DEBUG_VISIT_DECL
    std::cerr << "ClangToSageTranslator::VisitTranslationUnitDecl" << std::endl;
#endif
    if (*node != NULL) {
        std::cerr << "Runtime error: The TranslationUnitDecl is already associated to a SAGE node." << std::endl;
        return false;
    }

  // Create the SAGE node: SgGlobal

    if (p_global_scope != NULL) {
        std::cerr << "Runtime error: Global Scope have already been set !" << std::endl;
        return false;
    }

    *node = p_global_scope = new SgGlobal();

    p_decl_translation_map.insert(std::pair<clang::Decl *, SgNode *>(translation_unit_decl, p_global_scope));

  // Traverse the children

 // DQ (4/5/2017): Fixed code to use updated SageBuilder API.
 // SageBuilder::pushScopeStack(*node);
    SgScopeStatement* global_scope = isSgGlobal(*node);
    ROSE_ASSERT(global_scope != NULL);
    SageBuilder::pushScopeStack(global_scope);

    clang::DeclContext * decl_context = (clang::DeclContext *)translation_unit_decl; // useless but more clear

    bool res = true;
    clang::DeclContext::decl_iterator it;
    for (it = decl_context->decls_begin(); it != decl_context->decls_end(); it++) {
        if (*it == NULL) continue;
        SgNode * child = Traverse(*it);

        SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(child);
        if (decl_stmt == NULL && child != NULL) {
            std::cerr << "Runtime error: the node produce for a clang::Decl is not a SgDeclarationStatement !" << std::endl;
            std::cerr << "    class = " << child->class_name() << std::endl;
            res = false;
        }
        else if (child != NULL) {
            // FIXME This is a hack to avoid autonomous decl of unnamed type to being added to the global scope....
            SgClassDeclaration * class_decl = isSgClassDeclaration(child);
            if (class_decl != NULL && (class_decl->get_name() == "" || class_decl->get_isUnNamed())) continue;
            SgEnumDeclaration * enum_decl = isSgEnumDeclaration(child);
            if (enum_decl != NULL && (enum_decl->get_name() == "" || enum_decl->get_isUnNamed())) continue;

            p_global_scope->append_declaration(decl_stmt);
        }
    }

    SageBuilder::popScopeStack();

  // Traverse the class hierarchy

    return VisitDecl(translation_unit_decl, node) && res;
}
