
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparse_alignas 0
#define DEBUG__setup_decl_item_type_unparse_infos 0
#define DEBUG__build_decl_item_name 0
#define DEBUG__build_decl_item_asm_register 0
#define DEBUG__unparseVarDeclStmt 0

void unparse_alignas(
  SgInitializedName * decl_item,
  Unparse_ExprStmt & unparser,
  SgUnparse_Info & info
) {
#if DEBUG__unparse_alignas
  printf("Enter unparse_alignas()\n");
  printf("  decl_item = %p = %s\n", decl_item, decl_item->class_name().c_str());
#endif
  if (decl_item->get_using_C11_Alignas_keyword()) {
    unparser.curprint("_Alignas(");
    SgNode*       constant_or_type = decl_item->get_constant_or_type_argument_for_Alignas_keyword();
    SgType*       type_operand     = isSgType(constant_or_type);
    SgExpression* constant_operand = isSgExpression(constant_or_type);
    if (type_operand != nullptr) {
      unparser.unp->u_type->unparseType(type_operand, info);
    } else if (constant_operand != nullptr) {
      unparser.unparseExpression(constant_operand, info);
    } else {
      printf ("Error: C11 _Alignas operand is not a type or constant \n");
      ROSE_ABORT();
    }
    unparser.curprint(")");
  }
#if DEBUG__unparse_alignas
  printf("Leave DEBUG__build_decl_item_name()\n");
#endif
}

static bool setup_decl_item_type_unparse_infos(
  SgUnparse_Info & ninfo_for_type,
  SgVariableDeclaration * vdecl,
  SgInitializedName * decl_item,
  SgType * decl_type,
  std::string & unparse_str
) {
#if DEBUG__setup_decl_item_type_unparse_infos
  printf("Enter setup_decl_item_type_unparse_infos()\n");
  printf("  vdecl     = %p = %s\n", vdecl,     vdecl->class_name().c_str());
  printf("  decl_item = %p = %s\n", decl_item, decl_item->class_name().c_str());
  printf("  decl_type = %p = %s\n", decl_type, decl_type->class_name().c_str());
#endif
  bool need_unparse = false;
  SgDeclarationStatement * declStmt = nullptr;
  SgNamedType * namedType = isSgNamedType(decl_type->findBaseType());
  if (namedType != nullptr) {
    declStmt = namedType->get_declaration();
    ASSERT_not_null(declStmt);
    if (declStmt->get_definingDeclaration()) {
      declStmt = declStmt->get_definingDeclaration();
    }
    if (vdecl->get_variableDeclarationContainsBaseTypeDefiningDeclaration()) {
      ninfo_for_type.set_SkipQualifiedNames();
    } else {
      ninfo_for_type.set_SkipDefinition();
    }
  }

  if ( vdecl->skipElaborateType() && declStmt && !isSgTypedefDeclaration(declStmt) ) {
    ninfo_for_type.set_SkipClassSpecifier();
  }

  if (decl_item->get_hasArrayTypeWithEmptyBracketSyntax()) {
    ninfo_for_type.set_supressArrayBound();
  }

  SgDeclarationStatement * base_type_defn_decl = vdecl->get_baseTypeDefiningDeclaration();
  if (isSgClassDeclaration(base_type_defn_decl) != NULL)  {
    ninfo_for_type.set_useAlternativeDefiningDeclaration();
    ninfo_for_type.set_declstatement_associated_with_type(base_type_defn_decl);
  }

  if (vdecl->get_requiresGlobalNameQualificationOnType()) {
    ninfo_for_type.set_requiresGlobalNameQualification();
  }
  ninfo_for_type.set_reference_node_for_qualification(decl_item);
  ninfo_for_type.set_isTypeFirstPart();
  ninfo_for_type.set_name_qualification_length(decl_item->get_name_qualification_length_for_type());
  ninfo_for_type.set_global_qualification_required(decl_item->get_global_qualification_required_for_type());
  ninfo_for_type.set_type_elaboration_required(decl_item->get_type_elaboration_required_for_type());

  if (vdecl->get_isAssociatedWithDeclarationList()) {
    if (ninfo_for_type.SkipBaseType()) {
      SgPointerType* pointerType = isSgPointerType(decl_type);
      SgPointerType* nested_pointerType = pointerType ? isSgPointerType(pointerType->get_base_type()) : nullptr;
      if (pointerType)        unparse_str += " *";
      if (nested_pointerType) unparse_str += " *";
    } else {
      ninfo_for_type.set_PrintName();
      need_unparse = true;
    }
  } else {
    if (!ninfo_for_type.SkipBaseType()) {
      if (decl_item->get_name_qualification_length() > 0) {
        ninfo_for_type.set_reference_node_for_qualification(decl_item);
      }
      need_unparse = true;
    }
  }
#if DEBUG__setup_decl_item_type_unparse_infos
  printf("  need_unparse = %s\n", need_unparse ? "true" : "false");
  printf("  unparse_str  = %s\n", unparse_str.c_str());
  printf("Leave DEBUG__build_decl_item_name()\n");
#endif
  return need_unparse;
}

std::string build_decl_item_name(SgInitializedName * decl_item) {
  std::string decl_name = decl_item->get_name().getString();
#if DEBUG__build_decl_item_name
  printf("Enter build_decl_item_name()\n");
  printf("  decl_item = %p = %s\n", decl_item, decl_item->class_name().c_str());
  printf("  decl_name = %s\n", decl_name.c_str());
#endif
  bool is_anonymous = decl_name.size() == 0 || decl_name.find("__anonymous_0x") == 0;
#if DEBUG__build_decl_item_name
  printf ("  is_anonymous = %s \n", is_anonymous ? "true" : "false");
#endif
  std::string unparse_name{""};
  if (!is_anonymous) {
    if (SageInterface::is_Cxx_language()) {
      unparse_name += decl_item->get_qualified_name_prefix().getString();
    }
    unparse_name += decl_name;
  }
#if DEBUG__build_decl_item_name
  printf("  unparse_name = %s\n", unparse_name.c_str());
  printf("Leave DEBUG__build_decl_item_name()\n");
#endif
  return unparse_name;
}

std::string build_decl_item_asm_register(SgInitializedName * decl_item) {
#if DEBUG__build_decl_item_asm_register
  printf("Enter build_decl_item_asm_register()\n");
  printf("  decl_item = %p = %s\n", decl_item, decl_item->class_name().c_str());
#endif

  bool has_register_code = decl_item->get_register_name_code() != SgInitializedName::e_invalid_register;
  bool has_register_name = decl_item->get_register_name_string().size() > 0;
  ROSE_ASSERT(!has_register_code || !has_register_name);

  bool need_asm_register = has_register_code || has_register_name;
  std::string decl_name = decl_item->get_name().getString();
  ROSE_ASSERT(!need_asm_register || decl_name.size() > 0);

  std::string asm_register{""};
  if (need_asm_register) {
#ifdef BACKEND_CXX_IS_INTEL_COMPILER
    asm_register += " __asm__ (\"";
#else
    asm_register += " asm (\"";
#endif
    if (has_register_code)
      asm_register += unparse_register_name(decl_item->get_register_name_code());
    if (has_register_name)
      asm_register += decl_item->get_register_name_string();
    asm_register += "\")";
  }
#if DEBUG__build_decl_item_asm_register
  printf("  asm_register = %s\n", asm_register.c_str());
  printf("Leave DEBUG__build_decl_item_name()\n");
#endif
  return asm_register;
}

#define DEBUG__need_assign_and_initializer_unparsed 0

static void need_assign_and_initializer_unparsed(
  SgInitializedName * decl_item,
  bool & need_assign_op,
  bool & need_initializer,
  SgUnparse_Info & unparse_info,
  bool inside_for_init_stmt
) {
  SgInitializer * decl_init = decl_item->get_initializer();
#if DEBUG__need_assign_and_initializer_unparsed
  printf("Enter need_assign_and_initializer_unparsed()\n");
  printf("  decl_item = %p = %s\n", decl_item, decl_item->class_name().c_str());
  printf("  decl_init = %p = %s\n", decl_init, decl_init ? decl_init->class_name().c_str() : "");
  printf("  unparse_info.SkipInitializer() = %s \n", unparse_info.SkipInitializer() ? "true" : "false");
#endif
  if (!decl_init || unparse_info.SkipInitializer()) return;

  auto finfo = decl_init->get_file_info();
  need_initializer = !finfo->isCompilerGenerated() || finfo->isOutputInCodeGeneration();

  if (need_initializer) {
    SgAssignInitializer * assign_init = isSgAssignInitializer(decl_init);
    SgConstructorInitializer * ctor_init = isSgConstructorInitializer(decl_init);
    SgAggregateInitializer * aggr_init = isSgAggregateInitializer(decl_init);

    if ( assign_init || aggr_init ) need_assign_op = true;

    if (ctor_init) {
      bool ctor_args_empty = (ctor_init->get_args()->get_expressions().size() == 0);
      bool use_copy_ctor_syntax = decl_item->get_using_assignment_copy_constructor_syntax();
      bool might_need_assign_op = ctor_init->get_need_name() ||
                                  ctor_init->get_associated_class_unknown() ||
                                  use_copy_ctor_syntax ||
                                  unparse_info.inConditional();
                   
      if (might_need_assign_op && inside_for_init_stmt && ctor_init->get_need_name() && ctor_init->get_is_explicit_cast()) {
        need_assign_op = true;
      } else if (might_need_assign_op && ( ctor_init->get_need_name() && ctor_init->get_is_explicit_cast() ) || use_copy_ctor_syntax) {
        bool suppressAssignmentSyntax = ( ctor_args_empty && !ctor_init->get_is_explicit_cast() ) ||
                                        ctor_init->get_is_braced_initialized();
        if (!suppressAssignmentSyntax) need_assign_op = true;
      }
    }
  }
#if DEBUG__need_assign_and_initializer_unparsed
  printf("  need_assign_op   = %s\n", need_assign_op   ? "true" : "false");
  printf("  need_initializer = %s\n", need_initializer ? "true" : "false");
  printf("Leave need_assign_and_initializer_unparsed()\n");
#endif
}

/**
 * 3 types of output
 *    var1=2, var2=3 (enum list)
 *    int var1=2, int var2=2 (arg list)
 *    int var1=2, var2=2 ; (vardecl list)
 * must also allow for this
 *    void (*set_foo)()=doo
 */
void Unparse_ExprStmt::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info) {
#if DEBUG__unparseVarDeclStmt
     printf ("Enter unparseVarDeclStmt()\n");
     printf ("  stmt = %p = %s\n", stmt, stmt->class_name().c_str());
#endif

     SgVariableDeclaration * vardecl_stmt = isSgVariableDeclaration(stmt);
     ASSERT_not_null(vardecl_stmt);
     ROSE_ASSERT(vardecl_stmt->get_variables().size() > 0);

#if DEBUG__unparseVarDeclStmt
     auto & decl_mod = vardecl_stmt->get_declarationModifier();
     printf ("  - isStatic()  = %s \n", decl_mod.get_storageModifier().isStatic() ? "true" : "false");
     printf ("  - isExtern()  = %s \n", decl_mod.get_storageModifier().isExtern() ? "true" : "false");
     printf ("  - isMutable() = %s \n", decl_mod.get_storageModifier().isMutable() ? "true" : "false");
     printf ("    ->get_is_thread_local  = %s \n", vardecl_stmt->get_is_thread_local() ? "true" : "false");
#endif

     ROSE_ASSERT(!info.unparsedPartiallyUsingTokenStream());
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     SgUnparse_Info ninfo(info);
#if DEBUG__unparseVarDeclStmt
     printf ("  ninfo.SkipBaseType() = %s \n", ninfo.SkipBaseType() ? "true" : "false");
#endif
     ninfo.set_declstatement_ptr(vardecl_stmt);

     SgClassDefinition * classDefinition = isSgClassDefinition(vardecl_stmt->get_parent());
     if (classDefinition != nullptr && classDefinition->get_declaration()->get_class_type() == SgClassDeclaration::e_class && !info.skipCheckAccess()) {
       ninfo.set_CheckAccess();
     }

     unp->u_sage->printSpecifier1(vardecl_stmt, ninfo);

     SgUnparse_Info saved_ninfo(ninfo); 

     ninfo.unset_CheckAccess();
     info.set_access_attribute(ninfo.get_access_attribute());

     if (ninfo.inEnumDecl()) {
       ninfo.unset_isWithType();
     } else {
       ninfo.set_isWithType();
     }

     SgInitializedNamePtrList::iterator vdecl_iname_it = vardecl_stmt->get_variables().begin();
     unparse_alignas(*vdecl_iname_it, *this, info);

     while (vdecl_iname_it != vardecl_stmt->get_variables().end()) {
       SgInitializedName * decl_item = *vdecl_iname_it;
       ASSERT_not_null(decl_item);
       SgType * decl_type = decl_item->get_type();
       ASSERT_not_null(decl_type);
       SgName decl_name = decl_item->get_name();
       SgInitializer * decl_init = decl_item->get_initializer();

#if DEBUG__unparseVarDeclStmt
       printf ("  - decl_item = %p = %s\n", decl_item, decl_item->class_name().c_str());
       printf ("    decl_type = %p = %s\n", decl_type, decl_type->class_name().c_str());
       printf ("    decl_name = %s\n", decl_name.str());
       printf ("    decl_init = %p = %s\n", decl_init,  decl_init ? decl_init->class_name().c_str() : "");
#endif
       if (decl_item->get_auto_decltype() != nullptr)
         decl_type = decl_item->get_auto_decltype();
       ASSERT_not_null(decl_type);
#if DEBUG__unparseVarDeclStmt
       printf ("    decl_type = %p = %s\n", decl_type, decl_type->class_name().c_str());
#endif

       if (vdecl_iname_it != vardecl_stmt->get_variables().begin()) {
         // FIXME could need piece of type (const, *, &) and initializer
         curprint(decl_name.str());
       } else {
         bool apply_vdecl_attr = !ninfo.inEnumDecl() && !ninfo.inArgList() && !ninfo.SkipSemiColon();

         unp->u_exprStmt->unparseAttachedPreprocessingInfo(decl_item, info, PreprocessingInfo::before);

         // FIXME block below before while loop: does it break preprocessor unparsing?
         unp->u_sage->printSpecifier2(vardecl_stmt, saved_ninfo);
         if (vardecl_stmt->get_is_thread_local()) {
           SgFile * file = TransformationSupport::getFile(vardecl_stmt);
           if (file && file->get_C_only()) {
             curprint("_Thread_local ");
           } else if (file && file->get_Cxx_only()) {
             curprint("thread_local ");
           }
         }

         SgUnparse_Info ninfo_for_type(ninfo);
         std::string unparse_str{""};
         if (setup_decl_item_type_unparse_infos(ninfo_for_type, vardecl_stmt, decl_item, decl_type, unparse_str)) {
           unp->u_type->unparseType(decl_type, ninfo_for_type);
         } else {
           curprint(unparse_str);
         }

         if (apply_vdecl_attr) unp->u_sage->printAttributesForType(vardecl_stmt, info);

         curprint(build_decl_item_name(decl_item));

         ninfo_for_type.set_isTypeSecondPart();
         unp->u_type->unparseType(decl_type, ninfo_for_type);

         curprint(build_decl_item_asm_register(decl_item));

         unp->u_sage->printAttributes(decl_item, info);
         if (apply_vdecl_attr) unp->u_sage->printAttributes(vardecl_stmt, info);

         bool need_assign = false;
         bool need_initializer = false;
         need_assign_and_initializer_unparsed(decl_item, need_assign, need_initializer, ninfo, isSgForInitStatement(vardecl_stmt->get_parent()));
         if (need_assign) curprint(" = ");
         if (need_initializer) {
           SgUnparse_Info statementInfo(ninfo);
           statementInfo.set_SkipClassDefinition();
           statementInfo.set_SkipEnumDefinition();
           statementInfo.set_declstatement_ptr(NULL);
           statementInfo.set_reference_node_for_qualification(decl_init);
           ASSERT_not_null(statementInfo.get_reference_node_for_qualification());
           ROSE_ASSERT(statementInfo.SkipClassDefinition() == statementInfo.SkipEnumDefinition());
           unparseExpression(decl_init, statementInfo);
         }
       }

       vdecl_iname_it++;

       if (vdecl_iname_it != vardecl_stmt->get_variables().end()) {
         if (!ninfo.inArgList())
           ninfo.set_SkipBaseType();
         curprint(",");
       }
       unparseAttachedPreprocessingInfo(decl_item, ninfo, PreprocessingInfo::after);
     }

     SgVariableDefinition * defn = vardecl_stmt->get_definition();
     if (defn != NULL) {
       unparseVarDefnStmt(defn,ninfo);
     }

     if (!ninfo.SkipSemiColon()) {
       curprint(";");
     }

#if DEBUG__unparseVarDeclStmt
     printf ("Leaving unparseVarDeclStmt()\n");
#endif
}
