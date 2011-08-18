/* Unparse_Java.C
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#include "sage3basic.h"
#include "unparseJava.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

using namespace std;

//-----------------------------------------------------------------------------------
//  void Unparse_Java::unparseType
//
//  General function that gets called when unparsing a C++ type. Then it routes
//  to the appropriate function to unparse each C++ type.
//-----------------------------------------------------------------------------------
void
Unparse_Java::unparseType(SgType* type, SgUnparse_Info& info) {

     ROSE_ASSERT(type != NULL);
     switch (type->variantT()) {
         case V_SgTypeVoid:   unparseTypeVoid( isSgTypeVoid(type), info); break;

         case V_SgTypeSignedChar: unparseTypeSignedChar( isSgTypeSignedChar(type), info); break;
         case V_SgTypeWchar:  unparseTypeWchar( isSgTypeWchar(type), info); break;
         case V_SgTypeShort:  unparseTypeShort( isSgTypeShort(type), info); break;
         case V_SgTypeInt:    unparseTypeInt( isSgTypeInt(type), info); break;
         case V_SgTypeLong:   unparseTypeLong( isSgTypeLong(type), info); break;
         case V_SgTypeFloat:  unparseTypeFloat( isSgTypeFloat(type), info); break;
         case V_SgTypeDouble: unparseTypeDouble( isSgTypeDouble(type), info); break;
         case V_SgTypeBool:   unparseTypeBool( isSgTypeBool(type), info); break;

         case V_SgArrayType:  unparseArrayType( isSgArrayType(type), info); break;
         case V_SgClassType:  unparseClassType( isSgClassType(type), info); break;
         case V_SgEnumType:   unparseEnumType( isSgEnumType(type), info); break;
         case V_SgModifierType: unparseModifierType( isSgModifierType(type), info); break;

         default:
            cout << "Unparse_Java::unparseType(" << type->class_name() << "*,info) is unimplemented." << endl;
            ROSE_ASSERT(false);
            break;
     }
}

void
Unparse_Java::unparseModifierType(SgModifierType* type, SgUnparse_Info& info) {
    unparseTypeModifier(type->get_typeModifier(), info);
    unparseType(type->get_base_type(), info);
}

void
Unparse_Java::unparseClassType(SgClassType* type, SgUnparse_Info& info)
   {
     SgClassDeclaration *decl = isSgClassDeclaration(type->get_declaration());
     ROSE_ASSERT(decl != NULL);

     unparseName(decl->get_name(), info);
     //todo templates and qualified names
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

          ROSE_ASSERT(edecl != NULL);

       // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
       // to be used check if name qualification is required.
          unp->u_exprStmt->initializeDeclarationsFromParent ( edecl, cdefn, namespaceDefn );

          if (info.isTypeFirstPart() == true && info.SkipEnumDefinition() == false)
             {
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(edecl, info, PreprocessingInfo::before);
             }

          curprint ( "enum ");

          SgNamedType *ptype = NULL;
          if (cdefn != NULL)
             {
               ptype = isSgNamedType(cdefn->get_declaration()->get_type());
             }

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
              ROSE_ASSERT(enum_stmt != NULL);

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
Unparse_Java::unparseArrayType(SgArrayType* type, SgUnparse_Info& info)
   {
     SgArrayType* array_type = isSgArrayType(type);
     ROSE_ASSERT(array_type != NULL);

     unparseType(array_type->get_base_type(), info);
     curprint("[]");
   }

void Unparse_Java::unparseTypeSignedChar(SgTypeSignedChar* type, SgUnparse_Info& info) { curprint("byte"); }
void Unparse_Java::unparseTypeWchar(SgTypeWchar* type, SgUnparse_Info& info)   { curprint("char"); }
void Unparse_Java::unparseTypeVoid(SgTypeVoid* type, SgUnparse_Info& info)     { curprint("void"); }
void Unparse_Java::unparseTypeShort(SgTypeShort* type, SgUnparse_Info& info)   { curprint("short"); }
void Unparse_Java::unparseTypeInt(SgTypeInt* type, SgUnparse_Info& info)       { curprint("int"); }
void Unparse_Java::unparseTypeLong(SgTypeLong* type, SgUnparse_Info& info)     { curprint("long"); }
void Unparse_Java::unparseTypeFloat(SgTypeFloat* type, SgUnparse_Info& info)   { curprint("float"); }
void Unparse_Java::unparseTypeDouble(SgTypeDouble* type, SgUnparse_Info& info) { curprint("double"); }
void Unparse_Java::unparseTypeBool(SgTypeBool* type, SgUnparse_Info& info)     { curprint("boolean");
