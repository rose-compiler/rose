#include "sage3basic.h"

// #include "clang-frontend-private.hpp"
#include "clang-to-dot-private.hpp"

#if 0
SgType * ClangToDotTranslator::buildTypeFromQualifiedType(const clang::QualType & qual_type) {
    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);

    ROSE_ASSERT(type != NULL); 

    if (qual_type.hasLocalQualifiers()) {
        SgModifierType * modified_type = new SgModifierType(type);
        SgTypeModifier & sg_modifer = modified_type->get_typeModifier();
        clang::Qualifiers qualifier = qual_type.getLocalQualifiers();

        if (qualifier.hasConst()) sg_modifer.get_constVolatileModifier().setConst();
        if (qualifier.hasVolatile()) sg_modifer.get_constVolatileModifier().setVolatile();
        if (qualifier.hasRestrict()) sg_modifer.setRestrict();
        
        if (qualifier.hasAddressSpace()) {
            clang::LangAS addrspace = qualifier.getAddressSpace();
            switch (addrspace) {
                case clang::LangAS::opencl_global:
                    sg_modifer.setOpenclGlobal();
                    break;
                case clang::LangAS::opencl_local:
                    sg_modifer.setOpenclLocal();
                    break;
                case clang::LangAS::opencl_constant:
                    sg_modifer.setOpenclConstant();
                    break;
                default:
                    sg_modifer.setAddressSpace();
                    sg_modifer.set_address_space_value(static_cast<unsigned int>(addrspace));
            }
        }
        modified_type = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);

        return modified_type;
    }
    else {
        return type;
    }
}
#endif

// SgNode * ClangToDotTranslator::Traverse(const clang::Type * type) 
std::string ClangToDotTranslator::Traverse(const clang::Type * type) 
   {
#if 0
     if (type == NULL)
          return NULL;

  // std::map<const clang::Type *, SgNode *>::iterator it = p_type_translation_map.find(type);
     std::map<const clang::Type *, std::string *>::iterator it = p_type_translation_map.find(type);
     if (it != p_type_translation_map.end()) 
          return it->second;
#endif
     if (type == NULL)
          return "";

    // Look for previous translation
    std::map<const clang::Type *, std::string>::iterator it = p_type_translation_map.find(type);
    if (it != p_type_translation_map.end()) 
         return it->second;

  // SgNode * result = NULL;
  // bool ret_status = false;

    // If first time, create a new entry
    std::string node_ident = genNextIdent();
    p_type_translation_map.insert(std::pair<const clang::Type *, std::string>(type, node_ident));
    NodeDescriptor & node_desc = p_node_desc.insert(std::pair<std::string, NodeDescriptor>(node_ident, NodeDescriptor(node_ident))).first->second;

    bool ret_status = false;

     CLANG_ROSE_Graph::graph (type);

     switch (type->getTypeClass())
        {
        case clang::Type::Decayed:
            ret_status = VisitDecayedType((clang::DecayedType *)type, node_desc);
            break;
        case clang::Type::ConstantArray:
            ret_status = VisitConstantArrayType((clang::ConstantArrayType *)type, node_desc);
            break;
        case clang::Type::DependentSizedArray:
            ret_status = VisitDependentSizedArrayType((clang::DependentSizedArrayType *)type, node_desc);
            break;
       case clang::Type::IncompleteArray:
            ret_status = VisitIncompleteArrayType((clang::IncompleteArrayType *)type, node_desc);
            break;
        case clang::Type::VariableArray:
            ret_status = VisitVariableArrayType((clang::VariableArrayType *)type, node_desc);
            break;
        case clang::Type::Atomic:
            ret_status = VisitAtomicType((clang::AtomicType *)type, node_desc);
            break;
        case clang::Type::Attributed:
            ret_status = VisitAttributedType((clang::AttributedType *)type, node_desc);
            break;
        case clang::Type::BlockPointer:
            ret_status = VisitBlockPointerType((clang::BlockPointerType *)type, node_desc);
            break;
        case clang::Type::Builtin:
            ret_status = VisitBuiltinType((clang::BuiltinType *)type, node_desc);
            break;
        case clang::Type::Complex:
            ret_status = VisitComplexType((clang::ComplexType *)type, node_desc);
            break;
        case clang::Type::Decltype:
            ret_status = VisitDecltypeType((clang::DecltypeType *)type, node_desc);
            break;
     // case clang::Type::DependentDecltype:
     //     ret_status = VisitDependentDecltypeType((clang::DependentDecltypeType *)type, node_desc);
     //     break;
        case clang::Type::Auto:
            ret_status = VisitAutoType((clang::AutoType *)type, node_desc);
            break;
        case clang::Type::DeducedTemplateSpecialization:
            ret_status = VisitDeducedTemplateSpecializationType((clang::DeducedTemplateSpecializationType *)type, node_desc);
            break;
        case clang::Type::DependentSizedExtVector:
            ret_status = VisitDependentSizedExtVectorType((clang::DependentSizedExtVectorType *)type, node_desc);
            break;
        case clang::Type::DependentVector:
            ret_status = VisitDependentVectorType((clang::DependentVectorType *)type, node_desc);
            break;
        case clang::Type::FunctionNoProto:
            ret_status = VisitFunctionNoProtoType((clang::FunctionNoProtoType *)type, node_desc);
            break;
        case clang::Type::FunctionProto:
            ret_status = VisitFunctionProtoType((clang::FunctionProtoType *)type, node_desc);
            break;
        case clang::Type::InjectedClassName:
            ret_status = VisitInjectedClassNameType((clang::InjectedClassNameType *)type, node_desc);
            break;
     // case clang::Type::LocInfo:
     //     ret_status = VisitLocInfoType((clang::LocInfoType *)type, node_desc);
     //     break;
        case clang::Type::MacroQualified:
            ret_status = VisitMacroQualifiedType((clang::MacroQualifiedType *)type, node_desc);
            break;
        case clang::Type::MemberPointer:
            ret_status = VisitMemberPointerType((clang::MemberPointerType *)type, node_desc);
            break;
        case clang::Type::PackExpansion:
            ret_status = VisitPackExpansionType((clang::PackExpansionType *)type, node_desc);
            break;
        case clang::Type::Paren:
            ret_status = VisitParenType((clang::ParenType *)type, node_desc);
            break;
        case clang::Type::Pipe:
            ret_status = VisitPipeType((clang::PipeType *)type, node_desc);
            break;
        case clang::Type::Pointer:
            ret_status = VisitPointerType((clang::PointerType *)type, node_desc);
            break;
        case clang::Type::LValueReference:
            ret_status = VisitLValueReferenceType((clang::LValueReferenceType *)type, node_desc);
            break;
        case clang::Type::RValueReference:
            ret_status = VisitRValueReferenceType((clang::RValueReferenceType *)type, node_desc);
            break;
        case clang::Type::SubstTemplateTypeParmPack:
            ret_status = VisitSubstTemplateTypeParmPackType((clang::SubstTemplateTypeParmPackType *)type, node_desc);
            break;
        case clang::Type::SubstTemplateTypeParm:
            ret_status = VisitSubstTemplateTypeParmType((clang::SubstTemplateTypeParmType *)type, node_desc);
            break;
        case clang::Type::Enum:
            ret_status = VisitEnumType((clang::EnumType *)type, node_desc);
            break;
        case clang::Type::Record:
            ret_status = VisitRecordType((clang::RecordType *)type, node_desc);
            break;
        case clang::Type::TemplateSpecialization:
            ret_status = VisitTemplateSpecializationType((clang::TemplateSpecializationType *)type, node_desc);
            break;
        case clang::Type::TemplateTypeParm:
            ret_status = VisitTemplateTypeParmType((clang::TemplateTypeParmType *)type, node_desc);
            break;
        case clang::Type::Typedef:
            ret_status = VisitTypedefType((clang::TypedefType *)type, node_desc);
            break;
      case clang::Type::TypeOfExpr:
          ret_status = VisitTypeOfExprType((clang::TypeOfExprType *)type, node_desc);
          break;
    //  case clang::Type::DependentTypeOfExpr:
    //      ret_status = VisitDependentTypeOfExprType((clang::DependentTypeOfExprType *)type, node_desc);
    //      break;
        case clang::Type::TypeOf:
            ret_status = VisitTypeOfType((clang::TypeOfType *)type, node_desc);
            break;
        case clang::Type::DependentName:
            ret_status = VisitDependentNameType((clang::DependentNameType *)type, node_desc);
            break;
        case clang::Type::DependentTemplateSpecialization:
            ret_status = VisitDependentTemplateSpecializationType((clang::DependentTemplateSpecializationType *)type, node_desc);
            break;
        case clang::Type::Elaborated:
            ret_status = VisitElaboratedType((clang::ElaboratedType *)type, node_desc);
            break;
        case clang::Type::UnaryTransform:
            ret_status = VisitUnaryTransformType((clang::UnaryTransformType *)type, node_desc);
            break;
        case clang::Type::UnresolvedUsing:
            ret_status = VisitUnresolvedUsingType((clang::UnresolvedUsingType *)type, node_desc);
            break;
        case clang::Type::Vector:
            ret_status = VisitVectorType((clang::VectorType *)type, node_desc);
            break;
        case clang::Type::ExtVector:
            ret_status = VisitExtVectorType((clang::ExtVectorType *)type, node_desc);
            break;
        default:
            std::cerr << "Unhandled type" << std::endl;
            ROSE_ASSERT(false);
    }

 // ROSE_ASSERT(result != NULL);
 // p_type_translation_map.insert(std::pair<const clang::Type *, SgNode *>(type, result));
 // return result;

    assert(ret_status != false);

    return node_ident;
}

/***************/
/* Visit Types */
/***************/

#if 0
bool ClangToDotTranslator::VisitType(clang::Type * type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitType" << std::endl;
#endif

    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the type..." << std::endl;
        return false;
    }
/*
    std::cerr << "Dump type " << type->getTypeClassName() << "(" << type << "): ";
    type->dump();
    std::cerr << std::endl;
*/
    // TODO

    return true;
}
#else
bool ClangToDotTranslator::VisitType(clang::Type * type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitType" << std::endl;
#endif

#if 0
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the type..." << std::endl;
        return false;
    }
/*
    std::cerr << "Dump type " << type->getTypeClassName() << "(" << type << "): ";
    type->dump();
    std::cerr << std::endl;
*/
#endif

    // TODO

    return true;
}
#endif

#if 0
bool ClangToDotTranslator::VisitAdjustedType(clang::AdjustedType * adjusted_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitAdjustedType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(adjusted_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitAdjustedType(clang::AdjustedType * adjusted_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitAdjustedType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(adjusted_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDecayedType(clang::DecayedType * decayed_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDecayedType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitAdjustedType(decayed_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDecayedType(clang::DecayedType * decayed_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDecayedType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitAdjustedType(decayed_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitArrayType(clang::ArrayType * array_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitArrayType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitType(array_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitArrayType(clang::ArrayType * array_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitArrayType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitType(array_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitConstantArrayType(clang::ConstantArrayType * constant_array_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitConstantArrayType" << std::endl;
#endif

    SgType * type = buildTypeFromQualifiedType(constant_array_type->getElementType());

    // TODO clang::ArrayType::ArraySizeModifier

    SgExpression * expr = SageBuilder::buildIntVal(constant_array_type->getSize().getSExtValue());

    *node = SageBuilder::buildArrayType(type, expr);

    return VisitArrayType(constant_array_type, node);
}
#else
bool ClangToDotTranslator::VisitConstantArrayType(clang::ConstantArrayType * constant_array_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitConstantArrayType" << std::endl;
#endif

#if 0
    SgType * type = buildTypeFromQualifiedType(constant_array_type->getElementType());

    // TODO clang::ArrayType::ArraySizeModifier

    SgExpression * expr = SageBuilder::buildIntVal(constant_array_type->getSize().getSExtValue());

    *node = SageBuilder::buildArrayType(type, expr);
#endif

    return VisitArrayType(constant_array_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentSizedArrayType(clang::DependentSizedArrayType * dependent_sized_array_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDependentSizedArrayType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitArrayType(dependent_sized_array_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentSizedArrayType(clang::DependentSizedArrayType * dependent_sized_array_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDependentSizedArrayType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitArrayType(dependent_sized_array_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitIncompleteArrayType" << std::endl;
#endif

    SgType * type = buildTypeFromQualifiedType(incomplete_array_type->getElementType());

    // TODO clang::ArrayType::ArraySizeModifier

    *node = SageBuilder::buildArrayType(type);

    return VisitArrayType(incomplete_array_type, node);
}
#else
bool ClangToDotTranslator::VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitIncompleteArrayType" << std::endl;
#endif

#if 0
    SgType * type = buildTypeFromQualifiedType(incomplete_array_type->getElementType());

    // TODO clang::ArrayType::ArraySizeModifier

    *node = SageBuilder::buildArrayType(type);
#endif

    return VisitArrayType(incomplete_array_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitVariableArrayType(clang::VariableArrayType * variable_array_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitVariableArrayType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitArrayType(variable_array_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitVariableArrayType(clang::VariableArrayType * variable_array_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitVariableArrayType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitArrayType(variable_array_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitAtomicType(clang::AtomicType * atomic_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitAtomicType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(atomic_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitAtomicType(clang::AtomicType * atomic_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitAtomicType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(atomic_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitAttributedType(clang::AttributedType * attributed_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitAttributedType" << std::endl;
#endif

    SgType * type = buildTypeFromQualifiedType(attributed_type->getModifiedType());

    SgModifierType * modified_type = SgModifierType::createType(type);
    SgTypeModifier & sg_modifer = modified_type->get_typeModifier();

//(01/29/2020) Pei-Hung needs to revisit this part.
/*
    switch (attributed_type->getAttrKind()) {
        case clang::AttributedType::attr_noreturn:             sg_modifer.setGnuAttributeNoReturn();      break;
        case clang::AttributedType::attr_cdecl:                sg_modifer.setGnuAttributeCdecl();         break;
        case clang::AttributedType::attr_stdcall:              sg_modifer.setGnuAttributeStdcall();       break;

        case clang::AttributedType::attr_address_space:
            std::cerr << "Unsupported attribute attr_address_space" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_regparm:
            std::cerr << "Unsupported attribute attr_regparm" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_vector_size:
            std::cerr << "Unsupported attribute attr_vector_size" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_neon_vector_type:
            std::cerr << "Unsupported attribute attr_neon_vector_type" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_neon_polyvector_type:
            std::cerr << "Unsupported attribute attr_neon_polyvector_type" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_objc_gc:
            std::cerr << "Unsupported attribute attr_objc_gc" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_objc_ownership:
            std::cerr << "Unsupported attribute attr_objc_ownership" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_pcs:
            std::cerr << "Unsupported attribute attr_pcs" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_fastcall:
            std::cerr << "Unsupported attribute attr_fastcall" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_thiscall:
            std::cerr << "Unsupported attribute attr_thiscall" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_pascal:
            std::cerr << "Unsupported attribute attr_pascal" << std::endl; ROSE_ASSERT(false);
        default:
            std::cerr << "Unknown attribute" << std::endl; ROSE_ASSERT(false);
    } 
*/
    *node = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);;

    return VisitType(attributed_type, node);
}
#else
bool ClangToDotTranslator::VisitAttributedType(clang::AttributedType * attributed_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitAttributedType" << std::endl;
#endif

#if 0
    SgType * type = buildTypeFromQualifiedType(attributed_type->getModifiedType());

    SgModifierType * modified_type = SgModifierType::createType(type);
    SgTypeModifier & sg_modifer = modified_type->get_typeModifier();

//(01/29/2020) Pei-Hung needs to revisit this part.
/*
    switch (attributed_type->getAttrKind()) {
        case clang::AttributedType::attr_noreturn:             sg_modifer.setGnuAttributeNoReturn();      break;
        case clang::AttributedType::attr_cdecl:                sg_modifer.setGnuAttributeCdecl();         break;
        case clang::AttributedType::attr_stdcall:              sg_modifer.setGnuAttributeStdcall();       break;

        case clang::AttributedType::attr_address_space:
            std::cerr << "Unsupported attribute attr_address_space" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_regparm:
            std::cerr << "Unsupported attribute attr_regparm" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_vector_size:
            std::cerr << "Unsupported attribute attr_vector_size" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_neon_vector_type:
            std::cerr << "Unsupported attribute attr_neon_vector_type" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_neon_polyvector_type:
            std::cerr << "Unsupported attribute attr_neon_polyvector_type" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_objc_gc:
            std::cerr << "Unsupported attribute attr_objc_gc" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_objc_ownership:
            std::cerr << "Unsupported attribute attr_objc_ownership" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_pcs:
            std::cerr << "Unsupported attribute attr_pcs" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_fastcall:
            std::cerr << "Unsupported attribute attr_fastcall" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_thiscall:
            std::cerr << "Unsupported attribute attr_thiscall" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_pascal:
            std::cerr << "Unsupported attribute attr_pascal" << std::endl; ROSE_ASSERT(false);
        default:
            std::cerr << "Unknown attribute" << std::endl; ROSE_ASSERT(false);
    } 
*/
    *node = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);;
#endif

    return VisitType(attributed_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitBlockPointerType(clang::BlockPointerType * block_pointer_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitBlockPointerType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(block_pointer_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitBlockPointerType(clang::BlockPointerType * block_pointer_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitBlockPointerType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(block_pointer_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitBuiltinType(clang::BuiltinType * builtin_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitBuiltinType" << std::endl;
#endif

    switch (builtin_type->getKind()) {
        case clang::BuiltinType::Void:       *node = SageBuilder::buildVoidType();             break;
        case clang::BuiltinType::Bool:       *node = SageBuilder::buildBoolType();             break;
        case clang::BuiltinType::Short:      *node = SageBuilder::buildShortType();            break;
        case clang::BuiltinType::Int:        *node = SageBuilder::buildIntType();              break;
        case clang::BuiltinType::Long:       *node = SageBuilder::buildLongType();             break;
        case clang::BuiltinType::LongLong:   *node = SageBuilder::buildLongLongType();         break;
        case clang::BuiltinType::Float:      *node = SageBuilder::buildFloatType();            break;
        case clang::BuiltinType::Double:     *node = SageBuilder::buildDoubleType();           break;
        case clang::BuiltinType::LongDouble: *node = SageBuilder::buildLongDoubleType();       break;

        case clang::BuiltinType::Char_S:     *node = SageBuilder::buildCharType();             break;

        case clang::BuiltinType::UInt:       *node = SageBuilder::buildUnsignedIntType();      break;
        case clang::BuiltinType::UChar:      *node = SageBuilder::buildUnsignedCharType();     break;
        case clang::BuiltinType::SChar:      *node = SageBuilder::buildSignedCharType();       break;
        case clang::BuiltinType::UShort:     *node = SageBuilder::buildUnsignedShortType();    break;
        case clang::BuiltinType::ULong:      *node = SageBuilder::buildUnsignedLongType();     break;
        case clang::BuiltinType::ULongLong:  *node = SageBuilder::buildUnsignedLongLongType(); break;
/*
        case clang::BuiltinType::NullPtr:    *node = SageBuilder::build(); break;
*/
        // TODO ROSE type ?
        case clang::BuiltinType::UInt128:    *node = SageBuilder::buildUnsignedLongLongType(); break;
        case clang::BuiltinType::Int128:     *node = SageBuilder::buildLongLongType();         break;
 
        case clang::BuiltinType::Char_U:    std::cerr << "Char_U    -> "; break;
        case clang::BuiltinType::WChar_U:   std::cerr << "WChar_U   -> "; break;
        case clang::BuiltinType::Char16:    std::cerr << "Char16    -> "; break;
        case clang::BuiltinType::Char32:    std::cerr << "Char32    -> "; break;
        case clang::BuiltinType::WChar_S:   std::cerr << "WChar_S   -> "; break;


        case clang::BuiltinType::ObjCId:
        case clang::BuiltinType::ObjCClass:
        case clang::BuiltinType::ObjCSel:
        case clang::BuiltinType::Dependent:
        case clang::BuiltinType::Overload:
        case clang::BuiltinType::BoundMember:
        case clang::BuiltinType::UnknownAny:
        default:
            std::cerr << "Unknown builtin type: " << builtin_type->getName(p_compiler_instance->getLangOpts()).str() << " !" << std::endl;
            exit(-1);
    }

    return VisitType(builtin_type, node);
}
#else
bool ClangToDotTranslator::VisitBuiltinType(clang::BuiltinType * builtin_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitBuiltinType" << std::endl;
#endif

#if 0
    switch (builtin_type->getKind()) {
        case clang::BuiltinType::Void:       *node = SageBuilder::buildVoidType();             break;
        case clang::BuiltinType::Bool:       *node = SageBuilder::buildBoolType();             break;
        case clang::BuiltinType::Short:      *node = SageBuilder::buildShortType();            break;
        case clang::BuiltinType::Int:        *node = SageBuilder::buildIntType();              break;
        case clang::BuiltinType::Long:       *node = SageBuilder::buildLongType();             break;
        case clang::BuiltinType::LongLong:   *node = SageBuilder::buildLongLongType();         break;
        case clang::BuiltinType::Float:      *node = SageBuilder::buildFloatType();            break;
        case clang::BuiltinType::Double:     *node = SageBuilder::buildDoubleType();           break;
        case clang::BuiltinType::LongDouble: *node = SageBuilder::buildLongDoubleType();       break;

        case clang::BuiltinType::Char_S:     *node = SageBuilder::buildCharType();             break;

        case clang::BuiltinType::UInt:       *node = SageBuilder::buildUnsignedIntType();      break;
        case clang::BuiltinType::UChar:      *node = SageBuilder::buildUnsignedCharType();     break;
        case clang::BuiltinType::SChar:      *node = SageBuilder::buildSignedCharType();       break;
        case clang::BuiltinType::UShort:     *node = SageBuilder::buildUnsignedShortType();    break;
        case clang::BuiltinType::ULong:      *node = SageBuilder::buildUnsignedLongType();     break;
        case clang::BuiltinType::ULongLong:  *node = SageBuilder::buildUnsignedLongLongType(); break;
/*
        case clang::BuiltinType::NullPtr:    *node = SageBuilder::build(); break;
*/
        // TODO ROSE type ?
        case clang::BuiltinType::UInt128:    *node = SageBuilder::buildUnsignedLongLongType(); break;
        case clang::BuiltinType::Int128:     *node = SageBuilder::buildLongLongType();         break;
 
        case clang::BuiltinType::Char_U:    std::cerr << "Char_U    -> "; break;
        case clang::BuiltinType::WChar_U:   std::cerr << "WChar_U   -> "; break;
        case clang::BuiltinType::Char16:    std::cerr << "Char16    -> "; break;
        case clang::BuiltinType::Char32:    std::cerr << "Char32    -> "; break;
        case clang::BuiltinType::WChar_S:   std::cerr << "WChar_S   -> "; break;


        case clang::BuiltinType::ObjCId:
        case clang::BuiltinType::ObjCClass:
        case clang::BuiltinType::ObjCSel:
        case clang::BuiltinType::Dependent:
        case clang::BuiltinType::Overload:
        case clang::BuiltinType::BoundMember:
        case clang::BuiltinType::UnknownAny:
        default:
            std::cerr << "Unknown builtin type: " << builtin_type->getName(p_compiler_instance->getLangOpts()).str() << " !" << std::endl;
            exit(-1);
    }
#endif

    return VisitType(builtin_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitComplexType(clang::ComplexType * complex_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitComplexType" << std::endl;
#endif

    bool res = true;

    SgType * type = buildTypeFromQualifiedType(complex_type->getElementType());

    *node = SageBuilder::buildComplexType(type);

    return VisitType(complex_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitComplexType(clang::ComplexType * complex_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitComplexType" << std::endl;
#endif

    bool res = true;

#if 0
    SgType * type = buildTypeFromQualifiedType(complex_type->getElementType());

    *node = SageBuilder::buildComplexType(type);
#endif

    return VisitType(complex_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDecltypeType(clang::DecltypeType * decltype_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDecltypeType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(decltype_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDecltypeType(clang::DecltypeType * decltype_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDecltypeType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(decltype_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentDecltypeType(clang::DependentDecltypeType * dependent_decltype_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDependentDecltypeType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitDecltypeType(dependent_decltype_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentDecltypeType(clang::DependentDecltypeType * dependent_decltype_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDependentDecltypeType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitDecltypeType(dependent_decltype_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDeducedType(clang::DeducedType * deduced_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDeducedType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(deduced_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDeducedType(clang::DeducedType * deduced_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDeducedType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(deduced_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitAutoType(clang::AutoType * auto_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitAutoType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitDeducedType(auto_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitAutoType(clang::AutoType * auto_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitAutoType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitDeducedType(auto_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDeducedTemplateSpecializationType(clang::DeducedTemplateSpecializationType * deduced_template_specialization_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDeducedTemplateSpecializationType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitDeducedType(deduced_template_specialization_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDeducedTemplateSpecializationType(clang::DeducedTemplateSpecializationType * deduced_template_specialization_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDeducedTemplateSpecializationType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitDeducedType(deduced_template_specialization_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentAddressSpaceType(clang::DependentAddressSpaceType * dependent_address_space_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDependentAddressSpaceType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(dependent_address_space_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentAddressSpaceType(clang::DependentAddressSpaceType * dependent_address_space_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitDependentAddressSpaceType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(dependent_address_space_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentSizedExtVectorType(clang::DependentSizedExtVectorType * dependent_sized_ext_vector_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentSizedExtVectorType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(dependent_sized_ext_vector_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentSizedExtVectorType(clang::DependentSizedExtVectorType * dependent_sized_ext_vector_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentSizedExtVectorType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(dependent_sized_ext_vector_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentVectorType(clang::DependentVectorType * dependent_vector_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentVectorType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(dependent_vector_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentVectorType(clang::DependentVectorType * dependent_vector_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentVectorType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(dependent_vector_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitFunctionType(clang::FunctionType * function_type, SgNode ** node)  {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitFunctionType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitType(function_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitFunctionType(clang::FunctionType * function_type, NodeDescriptor & node_desc)  {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitFunctionType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitType(function_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitFunctionNoProtoType" << std::endl;
#endif

    bool res = true;

    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();

    SgType * ret_type = buildTypeFromQualifiedType(function_no_proto_type->getReturnType()); 

    *node = SageBuilder::buildFunctionType(ret_type, param_type_list);

    return VisitType(function_no_proto_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitFunctionNoProtoType" << std::endl;
#endif

    bool res = true;

#if 0
    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();

    SgType * ret_type = buildTypeFromQualifiedType(function_no_proto_type->getReturnType()); 

    *node = SageBuilder::buildFunctionType(ret_type, param_type_list);
#endif

    return VisitType(function_no_proto_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitFunctionProtoType" << std::endl;
#endif

    bool res = true;
    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();
    for (unsigned i = 0; i < function_proto_type->getNumParams(); i++) {
        SgType * param_type = buildTypeFromQualifiedType(function_proto_type->getParamType(i));

        param_type_list->append_argument(param_type);
    }

    if (function_proto_type->isVariadic()) {
        param_type_list->append_argument(SgTypeEllipse::createType());
    }

    SgType * ret_type = buildTypeFromQualifiedType(function_proto_type->getReturnType());

    SgFunctionType * func_type = SageBuilder::buildFunctionType(ret_type, param_type_list);
    if (function_proto_type->isVariadic()) func_type->set_has_ellipses(1);

    *node = func_type;

    return VisitType(function_proto_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitFunctionProtoType" << std::endl;
#endif

    bool res = true;

#if 0
    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();
    for (unsigned i = 0; i < function_proto_type->getNumParams(); i++) {
        SgType * param_type = buildTypeFromQualifiedType(function_proto_type->getParamType(i));

        param_type_list->append_argument(param_type);
    }

    if (function_proto_type->isVariadic()) {
        param_type_list->append_argument(SgTypeEllipse::createType());
    }

    SgType * ret_type = buildTypeFromQualifiedType(function_proto_type->getReturnType());

    SgFunctionType * func_type = SageBuilder::buildFunctionType(ret_type, param_type_list);
    if (function_proto_type->isVariadic()) func_type->set_has_ellipses(1);

    *node = func_type;
#endif

    return VisitType(function_proto_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitInjectedClassNameType(clang::InjectedClassNameType * injected_class_name_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::InjectedClassNameType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(injected_class_name_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitInjectedClassNameType(clang::InjectedClassNameType * injected_class_name_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::InjectedClassNameType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(injected_class_name_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitLocInfoType(clang::LocInfoType * loc_info_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::LocInfoType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(loc_info_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitLocInfoType(clang::LocInfoType * loc_info_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::LocInfoType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(loc_info_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitMacroQualifiedType(clang::MacroQualifiedType * macro_qualified_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::MacroQualifiedType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(macro_qualified_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitMacroQualifiedType(clang::MacroQualifiedType * macro_qualified_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::MacroQualifiedType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(macro_qualified_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitMemberPointerType(clang::MemberPointerType * member_pointer_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::MemberPointerType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(member_pointer_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitMemberPointerType(clang::MemberPointerType * member_pointer_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::MemberPointerType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(member_pointer_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitPackExpansionType(clang::PackExpansionType * pack_expansion_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::PackExpansionType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(pack_expansion_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitPackExpansionType(clang::PackExpansionType * pack_expansion_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::PackExpansionType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(pack_expansion_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitParenType(clang::ParenType * paren_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitParenType" << std::endl;
#endif

    *node = buildTypeFromQualifiedType(paren_type->getInnerType());

    return VisitType(paren_type, node);
}
#else
bool ClangToDotTranslator::VisitParenType(clang::ParenType * paren_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitParenType" << std::endl;
#endif

#if 0
    *node = buildTypeFromQualifiedType(paren_type->getInnerType());
#endif

    return VisitType(paren_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitPipeType(clang::PipeType * pipe_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::PipeType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(pipe_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitPipeType(clang::PipeType * pipe_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::PipeType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(pipe_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitPointerType(clang::PointerType * pointer_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitPointerType" << std::endl;
#endif

    SgType * type = buildTypeFromQualifiedType(pointer_type->getPointeeType());

    *node = SageBuilder::buildPointerType(type);

    return VisitType(pointer_type, node);
}
#else
bool ClangToDotTranslator::VisitPointerType(clang::PointerType * pointer_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitPointerType" << std::endl;
#endif

#if 0
    SgType * type = buildTypeFromQualifiedType(pointer_type->getPointeeType());

    *node = SageBuilder::buildPointerType(type);
#endif

    return VisitType(pointer_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitReferenceType(clang::ReferenceType * reference_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::ReferenceType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(reference_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitReferenceType(clang::ReferenceType * reference_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::ReferenceType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(reference_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitLValueReferenceType(clang::LValueReferenceType * lvalue_reference_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::LValueReferenceType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitReferenceType(lvalue_reference_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitLValueReferenceType(clang::LValueReferenceType * lvalue_reference_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::LValueReferenceType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitReferenceType(lvalue_reference_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitRValueReferenceType(clang::RValueReferenceType * rvalue_reference_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::RValueReferenceType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitReferenceType(rvalue_reference_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitRValueReferenceType(clang::RValueReferenceType * rvalue_reference_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::RValueReferenceType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitReferenceType(rvalue_reference_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitSubstTemplateTypeParmPackType(clang::SubstTemplateTypeParmPackType * subst_template_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::SubstTemplateTypeParmPackType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(subst_template_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitSubstTemplateTypeParmPackType(clang::SubstTemplateTypeParmPackType * subst_template_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::SubstTemplateTypeParmPackType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(subst_template_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType * subst_template_type_parm_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::SubstTemplateTypeParmType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(subst_template_type_parm_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType * subst_template_type_parm_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::SubstTemplateTypeParmType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(subst_template_type_parm_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitTagType(clang::TagType * tag_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitTagType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitType(tag_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitTagType(clang::TagType * tag_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitTagType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitType(tag_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitEnumType(clang::EnumType * enum_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitEnumType" << std::endl;
#endif

    SgSymbol * sym = GetSymbolFromSymbolTable(enum_type->getDecl());

    SgEnumSymbol * enum_sym = isSgEnumSymbol(sym);

    if (enum_sym == NULL) {
        SgNode * tmp_decl = Traverse(enum_type->getDecl());
        SgEnumDeclaration * sg_decl = isSgEnumDeclaration(tmp_decl);

        ROSE_ASSERT(sg_decl != NULL);

        *node = sg_decl->get_type();
    }
    else {
        *node = enum_sym->get_type();
    }

    if (isSgEnumType(*node) != NULL) {
        if (enum_sym == NULL) {
            p_enum_type_decl_first_see_in_type.insert(std::pair<SgEnumType *, bool>(isSgEnumType(*node), true));
        }
        else
            p_enum_type_decl_first_see_in_type.insert(std::pair<SgEnumType *, bool>(isSgEnumType(*node), false));
    }

    return VisitType(enum_type, node);
}
#else
bool ClangToDotTranslator::VisitEnumType(clang::EnumType * enum_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitEnumType" << std::endl;
#endif

#if 0
    SgSymbol * sym = GetSymbolFromSymbolTable(enum_type->getDecl());

    SgEnumSymbol * enum_sym = isSgEnumSymbol(sym);

    if (enum_sym == NULL) {
        SgNode * tmp_decl = Traverse(enum_type->getDecl());
        SgEnumDeclaration * sg_decl = isSgEnumDeclaration(tmp_decl);

        ROSE_ASSERT(sg_decl != NULL);

        *node = sg_decl->get_type();
    }
    else {
        *node = enum_sym->get_type();
    }

    if (isSgEnumType(*node) != NULL) {
        if (enum_sym == NULL) {
            p_enum_type_decl_first_see_in_type.insert(std::pair<SgEnumType *, bool>(isSgEnumType(*node), true));
        }
        else
            p_enum_type_decl_first_see_in_type.insert(std::pair<SgEnumType *, bool>(isSgEnumType(*node), false));
    }
#endif

    return VisitType(enum_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitRecordType(clang::RecordType * record_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitRecordType" << std::endl;
#endif

    SgSymbol * sym = GetSymbolFromSymbolTable(record_type->getDecl());

    SgClassSymbol * class_sym = isSgClassSymbol(sym);

    if (class_sym == NULL) {
        SgNode * tmp_decl = Traverse(record_type->getDecl());
        SgClassDeclaration * sg_decl = isSgClassDeclaration(tmp_decl);

        ROSE_ASSERT(sg_decl != NULL);

        *node = sg_decl->get_type();
    }
    else {
        *node = class_sym->get_type();
    }

    if (isSgClassType(*node) != NULL) {
        if (class_sym == NULL) {
            p_class_type_decl_first_see_in_type.insert(std::pair<SgClassType *, bool>(isSgClassType(*node), true));
            isSgNamedType(*node)->set_autonomous_declaration(true);
        }
        else
            p_class_type_decl_first_see_in_type.insert(std::pair<SgClassType *, bool>(isSgClassType(*node), false));
    }

    return VisitType(record_type, node);
}
#else
bool ClangToDotTranslator::VisitRecordType(clang::RecordType * record_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitRecordType" << std::endl;
#endif

#if 0
    SgSymbol * sym = GetSymbolFromSymbolTable(record_type->getDecl());

    SgClassSymbol * class_sym = isSgClassSymbol(sym);

    if (class_sym == NULL) {
        SgNode * tmp_decl = Traverse(record_type->getDecl());
        SgClassDeclaration * sg_decl = isSgClassDeclaration(tmp_decl);

        ROSE_ASSERT(sg_decl != NULL);

        *node = sg_decl->get_type();
    }
    else {
        *node = class_sym->get_type();
    }

    if (isSgClassType(*node) != NULL) {
        if (class_sym == NULL) {
            p_class_type_decl_first_see_in_type.insert(std::pair<SgClassType *, bool>(isSgClassType(*node), true));
            isSgNamedType(*node)->set_autonomous_declaration(true);
        }
        else
            p_class_type_decl_first_see_in_type.insert(std::pair<SgClassType *, bool>(isSgClassType(*node), false));
    }
#endif

    return VisitType(record_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitTemplateSpecializationType(clang::TemplateSpecializationType * template_specialization_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::TemplateSpecializationType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(template_specialization_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitTemplateSpecializationType(clang::TemplateSpecializationType * template_specialization_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::TemplateSpecializationType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(template_specialization_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitTemplateTypeParmType(clang::TemplateTypeParmType * template_type_parm_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::TemplateTypeParmType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(template_type_parm_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitTemplateTypeParmType(clang::TemplateTypeParmType * template_type_parm_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::TemplateTypeParmType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(template_type_parm_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitTypedefType(clang::TypedefType * typedef_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitTypedefType" << std::endl;
#endif

    bool res = true;

    SgSymbol * sym = GetSymbolFromSymbolTable(typedef_type->getDecl());
    SgTypedefSymbol * tdef_sym = isSgTypedefSymbol(sym);

    if (tdef_sym == NULL) {
        std::cerr << "Runtime Error: Cannot find a typedef symbol for the TypedefType." << std::endl;
        res = false;
    }

    *node = tdef_sym->get_type();

   return VisitType(typedef_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitTypedefType(clang::TypedefType * typedef_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitTypedefType" << std::endl;
#endif

    bool res = true;

#if 0
    SgSymbol * sym = GetSymbolFromSymbolTable(typedef_type->getDecl());
    SgTypedefSymbol * tdef_sym = isSgTypedefSymbol(sym);

    if (tdef_sym == NULL) {
        std::cerr << "Runtime Error: Cannot find a typedef symbol for the TypedefType." << std::endl;
        res = false;
    }

    *node = tdef_sym->get_type();
#endif

   return VisitType(typedef_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitTypeOfExprType(clang::TypeOfExprType * type_of_expr_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::TypeOfExprType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(type_of_expr_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitTypeOfExprType(clang::TypeOfExprType * type_of_expr_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::TypeOfExprType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(type_of_expr_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentTypeOfExprType(clang::DependentTypeOfExprType * dependent_type_of_expr_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentTypeOfExprType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitTypeOfExprType(dependent_type_of_expr_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentTypeOfExprType(clang::DependentTypeOfExprType * dependent_type_of_expr_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentTypeOfExprType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitTypeOfExprType(dependent_type_of_expr_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitTypeOfType(clang::TypeOfType * type_of_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::TypeOfType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(type_of_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitTypeOfType(clang::TypeOfType * type_of_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::TypeOfType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(type_of_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitTypeWithKeyword(clang::TypeWithKeyword * type_with_keyword, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitTypeWithKeyword" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitType(type_with_keyword, node) && res;
}
#else
bool ClangToDotTranslator::VisitTypeWithKeyword(clang::TypeWithKeyword * type_with_keyword, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitTypeWithKeyword" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME

    return VisitType(type_with_keyword, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentNameType(clang::DependentNameType * dependent_name_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentNameType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitTypeWithKeyword(dependent_name_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentNameType(clang::DependentNameType * dependent_name_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentNameType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitTypeWithKeyword(dependent_name_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentTemplateSpecializationType(clang::DependentTemplateSpecializationType * ependent_template_specialization_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentTemplateSpecializationType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitTypeWithKeyword(ependent_template_specialization_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentTemplateSpecializationType(clang::DependentTemplateSpecializationType * ependent_template_specialization_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentTemplateSpecializationType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitTypeWithKeyword(ependent_template_specialization_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitElaboratedType(clang::ElaboratedType * elaborated_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitElaboratedType" << std::endl;
#endif

    SgType * type = buildTypeFromQualifiedType(elaborated_type->getNamedType());

    // FIXME clang::ElaboratedType contains the "sugar" of a type reference (eg, "struct A" or "M::N::A"), it should be pass down to ROSE

    *node = type;

    return VisitTypeWithKeyword(elaborated_type, node);
}
#else
bool ClangToDotTranslator::VisitElaboratedType(clang::ElaboratedType * elaborated_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitElaboratedType" << std::endl;
#endif

#if 0
    SgType * type = buildTypeFromQualifiedType(elaborated_type->getNamedType());

    // FIXME clang::ElaboratedType contains the "sugar" of a type reference (eg, "struct A" or "M::N::A"), it should be pass down to ROSE

    *node = type;
#endif

    return VisitTypeWithKeyword(elaborated_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitUnaryTransformType(clang::UnaryTransformType * unary_transform_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::UnaryTransformType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(unary_transform_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitUnaryTransformType(clang::UnaryTransformType * unary_transform_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::UnaryTransformType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(unary_transform_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitDependentUnaryTransformType(clang::DependentUnaryTransformType * dependent_unary_transform_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentUnaryTransformType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitUnaryTransformType(dependent_unary_transform_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitDependentUnaryTransformType(clang::DependentUnaryTransformType * dependent_unary_transform_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::DependentUnaryTransformType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitUnaryTransformType(dependent_unary_transform_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitUnresolvedUsingType(clang::UnresolvedUsingType * unresolved_using_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::UnresolvedUsingType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(unresolved_using_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitUnresolvedUsingType(clang::UnresolvedUsingType * unresolved_using_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::UnresolvedUsingType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME 

    return VisitType(unresolved_using_type, node_desc) && res;
}
#endif

#if 0
bool ClangToDotTranslator::VisitVectorType(clang::VectorType * vector_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitVectorType" << std::endl;
#endif

    SgType * type = buildTypeFromQualifiedType(vector_type->getElementType());

    SgModifierType * modified_type = new SgModifierType(type);
    SgTypeModifier & sg_modifer = modified_type->get_typeModifier();

    sg_modifer.setVectorType();
    sg_modifer.set_vector_size(vector_type->getNumElements());

    *node = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);

    return VisitType(vector_type, node);
}
#else
bool ClangToDotTranslator::VisitVectorType(clang::VectorType * vector_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitVectorType" << std::endl;
#endif

#if 0
    SgType * type = buildTypeFromQualifiedType(vector_type->getElementType());

    SgModifierType * modified_type = new SgModifierType(type);
    SgTypeModifier & sg_modifer = modified_type->get_typeModifier();

    sg_modifer.setVectorType();
    sg_modifer.set_vector_size(vector_type->getNumElements());

    *node = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);
#endif

    return VisitType(vector_type, node_desc);
}
#endif

#if 0
bool ClangToDotTranslator::VisitExtVectorType(clang::ExtVectorType * ext_vector_type, SgNode ** node) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitExtVectorType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME Is it anything to be done here?

    return VisitVectorType(ext_vector_type, node) && res;
}
#else
bool ClangToDotTranslator::VisitExtVectorType(clang::ExtVectorType * ext_vector_type, NodeDescriptor & node_desc) {
#if DEBUG_VISIT_TYPE
    std::cerr << "ClangToDotTranslator::VisitExtVectorType" << std::endl;
#endif
    bool res = true;

    ROSE_ASSERT(FAIL_FIXME == 0); // FIXME Is it anything to be done here?

    return VisitVectorType(ext_vector_type, node_desc) && res;
}
#endif
