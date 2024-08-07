/* Unparse_Java.C
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#include "sage3basic.h"
#include "unparser.h" //charles4:  I replaced this include:   #include "unparseJava.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

using namespace std;

//-----------------------------------------------------------------------------------
//  void Unparse_Java::unparseType
//
//  General function that gets called when unparsing a Java type. Then it routes
//  to the appropriate function to unparse each Java type.
//-----------------------------------------------------------------------------------
void
Unparse_Java::unparseType(SgType* type, SgUnparse_Info& info)
   {

     ASSERT_not_null(type);

     switch (type->variantT())
        {
          case V_SgTypeVoid:       unparseTypeVoid( isSgTypeVoid(type), info); break;

          case V_SgTypeSignedChar: unparseTypeSignedChar( isSgTypeSignedChar(type), info); break;
          case V_SgTypeWchar:      unparseTypeWchar( isSgTypeWchar(type), info); break;
          case V_SgTypeShort:      unparseTypeShort( isSgTypeShort(type), info); break;
          case V_SgTypeInt:        unparseTypeInt( isSgTypeInt(type), info); break;
          case V_SgTypeLong:       unparseTypeLong( isSgTypeLong(type), info); break;
          case V_SgTypeFloat:      unparseTypeFloat( isSgTypeFloat(type), info); break;
          case V_SgTypeDouble:     unparseTypeDouble( isSgTypeDouble(type), info); break;
          case V_SgTypeBool:       unparseTypeBool( isSgTypeBool(type), info); break;

          case V_SgArrayType:      unparseArrayType( isSgArrayType(type), info); break;
          case V_SgTypedefType:    unparseTypedefType( isSgTypedefType(type), info); break;
          case V_SgClassType:      unparseClassType( isSgClassType(type), info); break;
          case V_SgEnumType:       unparseEnumType( isSgEnumType(type), info); break;
          case V_SgModifierType:   unparseModifierType( isSgModifierType(type), info); break;

       // DQ (9/5/2011): Added support for Java generics.
          case V_SgJavaQualifiedType:  unparseJavaQualifiedType(isSgJavaQualifiedType(type), info); break;
          case V_SgJavaParameterType:  unparseClassType(isSgJavaParameterType(type), info); break;
          case V_SgJavaParameterizedType:  unparseJavaParameterizedType(isSgJavaParameterizedType(type), info); break;
          case V_SgJavaWildcardType:  unparseJavaWildcardType(isSgJavaWildcardType(type), info); break;
          case V_SgJavaUnionType:  unparseJavaUnionType(isSgJavaUnionType(type), info); break;

          default:
               cout << "Unparse_Java::unparseType(" << type->class_name() << "*,info) is unimplemented." << endl;
               ROSE_ABORT();
        }
   }

void
Unparse_Java::unparseModifierType(SgModifierType* type, SgUnparse_Info& info) {
    unparseTypeModifier(type->get_typeModifier(), info);
    ROSE_ASSERT(type->get_base_type());
    unparseType(type->get_base_type(), info);
}

void
Unparse_Java::unparseTypedefType(SgTypedefType* type, SgUnparse_Info &)
   {
     curprint(type->get_name().getString());
   }


void
Unparse_Java::unparseClassType(SgClassType *type, SgUnparse_Info &)
   {
     //SgClassDeclaration *decl = isSgClassDeclaration(type->get_declaration());
     //ASSERT_not_null(decl);
     //unparseName(decl->get_name(), info);
     //todo templates and qualified names

     if (isSgJavaParameterType(type)) { // -> attributeExists("is_parameter_type")) {
         curprint(type -> get_name().getString());
     }
     else {
         curprint(type -> get_qualified_name().getString());
     }
   }


void Unparse_Java::unparseTypeArguments(SgTemplateParameterList *type_list, SgUnparse_Info &info) {
    ROSE_ASSERT(type_list);
    curprint("<");
    for (size_t i = 0; i < type_list -> get_args().size(); i++) {
        if (i != 0) {
            curprint(", ");
        }

        SgType* argumentType = NULL;
        SgTemplateParameter* templateParameter = type_list -> get_args()[i];
        ASSERT_not_null(templateParameter);
        if (templateParameter->get_parameterType() == SgTemplateParameter::type_parameter) {
            if (templateParameter -> get_type() != NULL) {
                argumentType = templateParameter -> get_type();
            }
            else {
                 // Do we need to support the default type when the type is not explicit.
            }
        }
        else {
            // This was not a type parameter (but it might be a template declaration or something work paying attention to).
        }

        // There are a number of way in which the argumentType can be set (but maybe a restricted set of ways for Java).
        if (argumentType != NULL) {
            unparseType(argumentType, info);
        }
        else {
            // It might be that this branch should be an error for Java. But likely caught elsewhere in ROSE.
        }
    }
    curprint(">");
}

void Unparse_Java::unparseJavaParameterizedType(SgJavaParameterizedType *type, SgUnparse_Info& info) {
    unparseType(type -> get_raw_type(), info);
    if (type -> get_type_list() != NULL) {
      unparseTypeArguments(type -> get_type_list(), info);
    }
}


void 
Unparse_Java::unparseJavaWildcardType(SgJavaWildcardType* wildcard_type, SgUnparse_Info& info) {
     curprint("?");

     SgType *bound_type = wildcard_type -> get_bound_type();
     if (wildcard_type -> get_has_extends()) {
         curprint(" extends "); 
     }
     else if (wildcard_type -> get_has_super()) {
         curprint(" super "); 
     }
     unparseType(bound_type, info);
}


void 
Unparse_Java::unparseJavaUnionType(SgJavaUnionType *union_type, SgUnparse_Info& info) {
    SgTypePtrList type_list = union_type -> get_type_list();
    for (size_t i = 0; i < type_list.size(); i++) {
        if (i > 0) {
            curprint(" | ");
        }
        unparseType(type_list[i], info);
    }
}


void
Unparse_Java::unparseEnumType(SgEnumType* type, SgUnparse_Info& info)
   {
     SgEnumType* enum_type = isSgEnumType(type);
     ROSE_ASSERT(enum_type);

     if (info.isTypeSecondPart() == false)
        {
          SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
          SgClassDefinition *cdefn = NULL;
          SgNamespaceDefinitionStatement* namespaceDefn = NULL;

          ASSERT_not_null(edecl);

       // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
       // to be used check if name qualification is required.
          unp->u_exprStmt->initializeDeclarationsFromParent ( edecl, cdefn, namespaceDefn );

          if (info.isTypeFirstPart() == true && info.SkipEnumDefinition() == false)
             {
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(edecl, info, PreprocessingInfo::before);
             }

          curprint ( "enum ");

       // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
       // SgNamedType *ptype = NULL;
       // if (cdefn != NULL)
       //    {
       //      ptype = isSgNamedType(cdefn->get_declaration()->get_type());
       //    }

          if (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true)
             {
               curprint ( enum_type->get_name().getString() + " ");
             }
            else
             {
            // DQ (7/20/2011): Test compilation without the generateNameQualifier() functions.
            // The C++ support is more complex and can require qualified names!
            // SgName nameQualifier = unp->u_name->generateNameQualifier( edecl , info );
               SgName nameQualifier;

            // printf ("nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
            // curprint ( "\n/* nameQualifier (from unp->u_name->generateNameQualifier function) = " + nameQualifier + " */ \n ";
               curprint ( nameQualifier.str());
               SgName nm = enum_type->get_name();

               if (nm.getString() != "")
                  {
                 // printf ("Output qualifier of current types to the name = %s \n",nm.str());
                    curprint ( nm.getString() + " ");
                  }
             }
        }

     if (info.isTypeFirstPart() == true)
        {
      // info.display("info before constructing ninfo");
         SgUnparse_Info ninfo(info);

      // don't skip the semicolon in the output of the statement in the class definition
         ninfo.unset_SkipSemiColon();

         ninfo.set_isUnsetAccess();

      // printf ("info.SkipEnumDefinition() = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");

         if ( info.SkipEnumDefinition() == false)
            {
              SgUnparse_Info ninfo(info);
              ninfo.set_inEnumDecl();
              SgInitializer *tmp_init = NULL;
              SgName tmp_name;

              SgEnumDeclaration *enum_stmt = isSgEnumDeclaration(enum_type->get_declaration());
              ASSERT_not_null(enum_stmt);

           // This permits support of the empty enum case! "enum x{};"
              curprint ( "{");

              SgInitializedNamePtrList::iterator p = enum_stmt->get_enumerators().begin();
              if (p != enum_stmt->get_enumerators().end())
                 {
                // curprint ( "{";
                   while (1)
                      {
                        unp->u_exprStmt->unparseAttachedPreprocessingInfo(*p, info, PreprocessingInfo::before);
                        tmp_name=(*p)->get_name();
                        tmp_init=(*p)->get_initializer();
                        curprint ( tmp_name.str());
                        if(tmp_init)
                           {
                             curprint ( "=");
                             unp->u_exprStmt->unparseExpression(tmp_init, ninfo);
                           }
                        p++;
                        if (p != enum_stmt->get_enumerators().end())
                           {
                             curprint ( ",");
                           }
                          else
                             break;
                       }
                 // curprint ( "}";
                  }

            // Putting the "inside" info right here is just a wild guess as to where it might really belong.
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(enum_stmt, info, PreprocessingInfo::inside);

               curprint ( "}");

               unp->u_exprStmt->unparseAttachedPreprocessingInfo(enum_stmt, info, PreprocessingInfo::after);
             }
        }
   }

void
Unparse_Java::unparseArrayType(SgArrayType *array_type, SgUnparse_Info& info) {
    unparseType(array_type->get_base_type(), info);
    curprint("[]");
}

void
Unparse_Java::unparseJavaQualifiedType(SgJavaQualifiedType *qualified_type, SgUnparse_Info& info) {
    unparseType(qualified_type -> get_parent_type(), info);
    curprint(".");

    SgType *type = qualified_type -> get_type();
    SgJavaParameterizedType *param_type = isSgJavaParameterizedType(type);
    SgClassType *class_type = isSgClassType(type);

    if (param_type) {
      curprint(isSgClassType(param_type -> get_raw_type()) -> get_name().getString());
        unparseTypeArguments(param_type -> get_type_list(), info);
    }
    else {
        ROSE_ASSERT(class_type);
        class_type -> get_name().getString();
    }
}

void Unparse_Java::unparseTypeSignedChar(SgTypeSignedChar*, SgUnparse_Info &) { curprint("byte"); }
void Unparse_Java::unparseTypeWchar(SgTypeWchar*, SgUnparse_Info &)   { curprint("char"); }
void Unparse_Java::unparseTypeVoid(SgTypeVoid*, SgUnparse_Info &)     { curprint("void"); }
void Unparse_Java::unparseTypeShort(SgTypeShort*, SgUnparse_Info &)   { curprint("short"); }
void Unparse_Java::unparseTypeInt(SgTypeInt*, SgUnparse_Info &)       { curprint("int"); }
void Unparse_Java::unparseTypeLong(SgTypeLong*, SgUnparse_Info &)     { curprint("long"); }
void Unparse_Java::unparseTypeFloat(SgTypeFloat*, SgUnparse_Info &)   { curprint("float"); }
void Unparse_Java::unparseTypeDouble(SgTypeDouble*, SgUnparse_Info &) { curprint("double"); }
void Unparse_Java::unparseTypeBool(SgTypeBool*, SgUnparse_Info &)     { curprint("boolean"); }
