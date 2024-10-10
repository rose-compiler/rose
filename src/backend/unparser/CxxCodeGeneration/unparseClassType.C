
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparseClassType 0

void Unparse_Type::unparseClassType(SgType * type, SgUnparse_Info & info) {
#if DEBUG__unparseClassType
     printf("Enter Unparse_Type::unparseClassType()\n");
     printf("  type = %p = %s\n", type, type->class_name().c_str());
     printf("  info.SkipClassDefinition() = %s\n", info.SkipClassDefinition() ? "true" : "false");
     printf("  info.SkipEnumDefinition()  = %s\n", info.SkipEnumDefinition()  ? "true" : "false");
     printf("  info.isWithType()          = %s\n", info.isWithType()          ? "true" : "false");
     printf("  info.SkipBaseType()        = %s\n", info.SkipBaseType()        ? "true" : "false");
     printf("  info.isTypeFirstPart()     = %s\n", info.isTypeFirstPart()     ? "true" : "false");
     printf("  info.isTypeSecondPart()    = %s\n", info.isTypeSecondPart()    ? "true" : "false");
     printf("  info.get_use_generated_name_for_template_arguments() = %s\n", info.get_use_generated_name_for_template_arguments() ? "true" : "false");
     printf("  info.get_reference_node_for_qualification() = %p = %s\n", info.get_reference_node_for_qualification(), info.get_reference_node_for_qualification() ? info.get_reference_node_for_qualification()->class_name().c_str() : "");
#endif

     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     SgClassType* class_type = isSgClassType(type);
     ASSERT_not_null(class_type);
     SgClassDeclaration *decl = isSgClassDeclaration(class_type->get_declaration());
     ASSERT_not_null(decl);
#if DEBUG__unparseClassType
     printf("  decl = %p = %s\n", decl, decl->class_name().c_str());
     printf("    ->get_name() = %s\n", decl->get_name().str());
     printf("    ->get_firstNondefiningDeclaration() = %p = %s\n", decl->get_firstNondefiningDeclaration(), decl->get_firstNondefiningDeclaration() ? decl->get_firstNondefiningDeclaration()->class_name().c_str() : "");
     printf("    ->get_definingDeclaration()         = %p = %s\n", decl->get_definingDeclaration(), decl->get_definingDeclaration() ? decl->get_definingDeclaration()->class_name().c_str() : "");
     printf ("  decl->isForward()                    = %s \n", decl->isForward()                   ? "true" : "false");
     printf ("  decl->get_isUnNamed()                = %s \n", decl->get_isUnNamed()               ? "true" : "false");
     printf ("  decl->get_isAutonomousDeclaration()  = %s \n", decl->get_isAutonomousDeclaration() ? "true" : "false");    
#endif

     SgTemplateClassDeclaration *tpldecl = isSgTemplateClassDeclaration(decl);

     ROSE_ASSERT(decl == decl->get_firstNondefiningDeclaration());

     if (decl->get_definition() == NULL)
        {
          ASSERT_not_null(class_type->get_declaration());
          if (decl->get_definingDeclaration() != NULL)
             {
               decl = isSgClassDeclaration(decl->get_definingDeclaration());
               ASSERT_not_null(decl);
               ASSERT_not_null(decl->get_definition());
             }
        }

     ROSE_ASSERT(decl == decl->get_definingDeclaration() || decl->get_definingDeclaration() == NULL);

     SgClassDeclaration *cDefiningDecl = isSgClassDeclaration(decl->get_definingDeclaration());

     if ( info.isTypeFirstPart() || !info.isTypeSecondPart() )
        {
          if(!info.SkipClassSpecifier())
             {
            // GB (09/18/2007): If the class definition is unparsed, also unparse its
            // attached preprocessing info.
               if (cDefiningDecl != NULL && !info.SkipClassDefinition())
                  {
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::before);
                  }

               if (tpldecl != NULL) {
                 curprint ( "template ");
               } else {
                 bool useElaboratedType = generateElaboratedType(decl,info);
                 if (useElaboratedType) {
                   switch (decl->get_class_type()) {
                     case SgClassDeclaration::e_class:  curprint("class ");  break;
                     case SgClassDeclaration::e_struct: curprint("struct "); break;
                     case SgClassDeclaration::e_union:  curprint("union ");  break;
                     default: ROSE_ABORT();
                   }
                 }
               }
             }
        }

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
     if (info.isTypeSecondPart() == false)
        {
          SgName nm;
          if (!decl->get_isUnNamed() || info.PrintName()) {
            nm = decl->get_name();
          }
#if DEBUG__unparseClassType
          printf ("  nm = %s \n",nm.str());
#endif
          if (!nm.is_null()) {
            bool is_c_not_cxx = SageInterface::is_C_language() || SageInterface::is_C99_language();
#if DEBUG__unparseClassType
            printf("  is_c_not_cxx = %s\n", is_c_not_cxx ? "true" : "false");
#endif
            if (is_c_not_cxx) {
              curprint(std::string(nm.str()) + " ");
            } else if (info.get_reference_node_for_qualification() == NULL) {
              SgName nameQualifierAndType = class_type->get_qualified_name();
              curprint(nameQualifierAndType.str());
            } else {
              SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
              curprint(nameQualifier.str());

              SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(decl);
              if (isSgTemplateInstantiationDecl(decl) != NULL) {
                SgUnparse_Info ninfo(info);
                if (ninfo.isTypeFirstPart()) ninfo.unset_isTypeFirstPart();
                if (ninfo.isTypeSecondPart()) ninfo.unset_isTypeSecondPart();

                ROSE_ASSERT(ninfo.isTypeFirstPart()  == false);
                ROSE_ASSERT(ninfo.isTypeSecondPart() == false);

                unp->u_exprStmt->unparseTemplateName(templateInstantiationDeclaration, ninfo);
              } else {
                curprint ( std::string(nm.str()) + " ");
              }
            }
          } else if (info.get_use_generated_name_for_template_arguments()) {
            SgName nm = class_type->get_name();
            curprint(std::string(nm.str()) + " ");
          } else {
            SgClassDeclaration* parentClassDeclaration = isSgClassDeclaration(class_type->get_declaration());
            ASSERT_not_null(parentClassDeclaration);
            SgLambdaExp* lambdaExpresssion = isSgLambdaExp(parentClassDeclaration->get_parent());
            if (lambdaExpresssion == NULL) {
              info.unset_SkipClassDefinition();
              info.unset_SkipEnumDefinition();
            }
            ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());
          }
        }

#if DEBUG__unparseClassType
     printf("  info.SkipClassDefinition() = %s\n", info.SkipClassDefinition() ? "true" : "false");
     printf("  info.SkipEnumDefinition()  = %s\n", info.SkipEnumDefinition()  ? "true" : "false");
     printf("  info.isWithType()          = %s\n", info.isWithType()          ? "true" : "false");
     printf("  info.SkipBaseType()        = %s\n", info.SkipBaseType()        ? "true" : "false");
     printf("  info.isTypeFirstPart()     = %s\n", info.isTypeFirstPart()     ? "true" : "false");
     printf("  info.isTypeSecondPart()    = %s\n", info.isTypeSecondPart()    ? "true" : "false");
     printf("  info.isTypeSecondPart()    = %s\n", info.isTypeSecondPart()    ? "true" : "false");
#endif
     if ( info.isTypeFirstPart() || (!info.isTypeSecondPart() && !decl->get_isAutonomousDeclaration() && !info.SkipClassDefinition()) )
        {
          if ( !info.SkipClassDefinition() )
             {
               SgClassDefinition* classdefn_stmt = decl->get_definition();
               if (info.get_declaration_of_context() != NULL)
                  {
                    decl = isSgClassDeclaration(info.get_declaration_of_context());
                    ROSE_ASSERT(decl != NULL);
                    classdefn_stmt = decl->get_definition();
                  }

               if (classdefn_stmt != NULL)
                  {
                    SgUnparse_Info ninfo(info);
                    ninfo.unset_SkipSemiColon();

                    SgNamedType * saved_context = ninfo.get_current_context();
                    ninfo.set_current_context(NULL);
                    ninfo.set_current_context(class_type);
                    ninfo.set_current_scope(NULL);
                    ninfo.set_current_scope(classdefn_stmt);

                    if (info.useAlternativeDefiningDeclaration())
                       {
                         ASSERT_not_null(info.get_declstatement_associated_with_type());

                         SgClassDeclaration* class_declstatement_associated_with_type = isSgClassDeclaration(info.get_declstatement_associated_with_type());
                         ASSERT_not_null(class_declstatement_associated_with_type);
                         ASSERT_not_null(class_declstatement_associated_with_type->get_definition());
                         classdefn_stmt = class_declstatement_associated_with_type->get_definition();
                         ASSERT_not_null(classdefn_stmt);
                       }
                    ASSERT_not_null(classdefn_stmt);
                    unp->u_exprStmt->unparseClassInheritanceList (classdefn_stmt,info);
                    ninfo.set_isUnsetAccess();
                    curprint("{");
                    ASSERT_not_null(classdefn_stmt);
                    SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();
                    while (pp != classdefn_stmt->get_members().end())
                       {
                         ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

                         unp->u_exprStmt->unparseStatement((*pp), ninfo);
                         pp++;
                       }

                    ASSERT_not_null(cDefiningDecl);
                    if (cDefiningDecl->get_definition() != NULL)
                       {
                         unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl->get_definition(), info, PreprocessingInfo::inside);
                       }
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::inside);
                    curprint("}");

                    ninfo.set_current_context(NULL);
                    ninfo.set_current_context(saved_context);
                  }

               if (cDefiningDecl != NULL)
                  {
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::after);
                  }
             }
        }

#if DEBUG__unparseClassType
     printf ("Leaving Unparse_Type::unparseClassType()\n");
#endif
   }
