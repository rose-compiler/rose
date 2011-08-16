/* Unparse_Java.C
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#include "sage3basic.h"
#include "unparser.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

using namespace std;

// If this is turned on then we get the message to the
// generted code showing up in the mangled names!
#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS 0
#define OUTPUT_DEBUGGING_UNPARSE_INFO 0

string get_java_type_name(SgType* t)
   {
  // printf ("t->class_name() = %s \n",t->class_name().c_str());

     switch (t->variant())
        {
          case T_UNKNOWN:            return "UNKNOWN";
          case T_CHAR:               return "char";
          case T_SIGNED_CHAR:        return "signed char";
          case T_UNSIGNED_CHAR:      return "unsigned char";
          case T_SHORT:              return "short";
          case T_SIGNED_SHORT:       return "signed short";
          case T_UNSIGNED_SHORT:     return "unsigned short";
          case T_INT:                return "int";
          case T_SIGNED_INT:         return "signed int";
          case T_UNSIGNED_INT:       return "unsigned int";
          case T_LONG:               return "long";
          case T_SIGNED_LONG:        return "signed long";
          case T_UNSIGNED_LONG:      return "unsigned long";
          case T_VOID:               return "void";
          case T_GLOBAL_VOID:        return "global void";
          case T_WCHAR:              return "wchar_t";
          case T_FLOAT:              return "float";
          case T_DOUBLE:             return "double";
          case T_LONG_LONG:          return "long long";
          case T_SIGNED_LONG_LONG:   return "signed long long";
          case T_UNSIGNED_LONG_LONG: return "unsigned long long";
          case T_LONG_DOUBLE:        return "long double";
          case T_STRING:             return "char*";

          case T_BOOL:
             {
               if (SageInterface::is_C_language() == true)
                  {
                    printf ("Warning: SgTypeBool used for C application (reserved for use in C99 and C++) \n");
                  }

               return (SageInterface::is_C99_language() == true) ? "_Bool" : "bool";
             }

          case T_COMPLEX:
             {
                SgTypeComplex* complexType = isSgTypeComplex(t);
                ROSE_ASSERT(complexType != NULL);
                string returnString = get_java_type_name(complexType->get_base_type()) + " _Complex";
                return returnString;
             }

          case T_IMAGINARY:
             {
                string backEndCompiler = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
                SgTypeImaginary* imaginaryType = isSgTypeImaginary(t);
                ROSE_ASSERT(imaginaryType != NULL);
                string returnString = get_java_type_name(imaginaryType->get_base_type());
                if (backEndCompiler == "g++" || backEndCompiler == "gcc" || backEndCompiler == "mpicc" || backEndCompiler == "mpicxx") {
                  // Handle special case of GNU compilers
                } else {
                  returnString + " _Imaginary";
                }
               return returnString;
             }

          case T_DEFAULT: return "int";

          case T_POINTER:
                {
                  SgPointerType* pointer_type = isSgPointerType(t);
                  ROSE_ASSERT(pointer_type != NULL);
                  return get_java_type_name(pointer_type->get_base_type()) + "*";
                }
          case T_REFERENCE:
                {
                  SgReferenceType* ref_type = isSgReferenceType(t);
                  ROSE_ASSERT(ref_type != NULL);
                  return get_java_type_name(ref_type->get_base_type()) + "&";
                }
          case T_MEMBER_POINTER:
               {
                  SgPointerMemberType* mpointer_type = isSgPointerMemberType(t);
                  ROSE_ASSERT(mpointer_type != NULL);

                  SgType *btype = mpointer_type->get_base_type();
                  SgMemberFunctionType *ftype = NULL;

                  if ( (ftype = isSgMemberFunctionType(btype)) != NULL) {
                      string res = get_java_type_name(ftype->get_return_type()) +  "("
                        + get_java_type_name(mpointer_type->get_class_type())
                        + "::*" + ")" + "(" ;
                      SgTypePtrList::iterator p = ftype->get_arguments().begin();
                      while ( p != ftype->get_arguments().end() )
                       {
                         res = res + get_java_type_name(*p);
                         p++;
                         if (p != ftype->get_arguments().end()) { res = res + ","; }
                       }
                       return res + ")";
                  }
                  else {
                     return get_java_type_name(btype) + "(" +
                       get_java_type_name(mpointer_type->get_class_type()) + "::*" + ")";
                  }
               }
          case T_CLASS:
              {
                SgClassType* class_type = isSgClassType(t);
                ROSE_ASSERT(class_type != NULL);
                SgClassDeclaration* decl;
                decl = isSgClassDeclaration(class_type->get_declaration());
                SgName nm = decl->get_qualified_name();
             // printf ("In unparseType(%p): nm = %s \n",t,nm.str());
                if (nm.getString() != "")
                    return nm.getString();
                else
                   return string(class_type->get_name().str());
               }

          case T_ENUM:
               {
                SgEnumType* enum_type = isSgEnumType(t);
                ROSE_ASSERT(enum_type);
                SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
                SgName nm = edecl->get_qualified_name();
                if (nm.getString() != "")
                    return nm.getString();
                else
                    return string(enum_type->get_name().str());
               }

          case T_TYPEDEF:
               {
                SgTypedefType* typedef_type = isSgTypedefType(t);
                ROSE_ASSERT(typedef_type != NULL);
                return string(typedef_type->get_qualified_name().str());
               }

          case T_MODIFIER:
               {
                SgModifierType* mod_type = isSgModifierType(t);
                ROSE_ASSERT(mod_type != NULL);
                string res;
                bool unparse_base = true;
                if ( isSgReferenceType(mod_type->get_base_type()) ||
                     isSgPointerType(mod_type->get_base_type()) ) {
                    res = get_java_type_name(mod_type->get_base_type());
                    unparse_base = false;
                }
                if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())
                    res = res + "const ";
                if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile())
                    res = res + "volatile ";
                if (mod_type->get_typeModifier().isRestrict())
                   {
                     string compilerName = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
                     if ( (compilerName == "g++") || (compilerName == "gcc")  || compilerName == "mpicc" || compilerName == "mpicxx")
                        res = res + "__restrict__ ";
                     else
                        res = res + "restrict ";
                   }
                 if (unparse_base)
                    res = res + get_java_type_name(mod_type->get_base_type());
                  return res;
                }

          case T_PARTIAL_FUNCTION:
          case T_FUNCTION:
               {
                SgFunctionType* func_type = isSgFunctionType(t);
                ROSE_ASSERT (func_type != NULL);
                string res = get_java_type_name(func_type->get_return_type()) + "(";
                SgTypePtrList::iterator p = func_type->get_arguments().begin();
                while(p != func_type->get_arguments().end())
                  {
                    res = res + get_java_type_name(*p);
                    p++;
                    if (p != func_type->get_arguments().end())
                        res = res + ",";
                  }
               res = res + ")";
               return res;
              }

          case T_MEMBERFUNCTION:
              {
               SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(t);
               ROSE_ASSERT(mfunc_type != NULL);

               string res = get_java_type_name(mfunc_type->get_return_type()) + "(";
               SgTypePtrList::iterator p = mfunc_type->get_arguments().begin();
               while (p != mfunc_type->get_arguments().end()) {
                    res = res + get_java_type_name(*p);
                    p++;
                    if (p != mfunc_type->get_arguments().end())
                         res = res + ",";
                  }
               res = res + ")";
               return res;
             }

          case T_ARRAY:
             {
              SgArrayType* array_type = isSgArrayType(t);
              ROSE_ASSERT(array_type != NULL);

              string res = get_java_type_name(array_type->get_base_type()) + "[";
              if (array_type->get_index())
                  res = res + array_type->get_index()->unparseToString();
               res = res + "]";
               return res;
             }

          case T_ELLIPSE: return "...";

          default:
             {
               printf("Error: unparse_type.C get_java_type_name(): Default case reached in switch: Unknown type \n");
               ROSE_ASSERT(false);
               break;
             }
        }
   }

//-----------------------------------------------------------------------------------
//  void Unparse_Java::unparseType
//
//  General function that gets called when unparsing a C++ type. Then it routes
//  to the appropriate function to unparse each C++ type.
//-----------------------------------------------------------------------------------
void
Unparse_Java::unparseType(SgType* type, SgUnparse_Info& info)
   {
     ROSE_ASSERT(type != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
     string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
     printf ("In Unparse_Java::unparseType(): type->sage_class_name() = %s firstPart = %s secondPart = %s \n",
          type->sage_class_name(),firstPartString.c_str(),secondPartString.c_str());
     curprint ( string("\n/* Top of unparseType name ") + type->sage_class_name()
         + " firstPart " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif

#if 0
     printf ("In Unparse_Java::unparseType(): type->sage_class_name() = %s \n",type->sage_class_name());
     curprint ( string("\n/* Top of unparseType: sage_class_name() = ") + type->sage_class_name() + " */ \n");
#endif

     char buffer[512];
     sprintf (buffer,"entering case for %s",type->sage_class_name());
     unp->u_debug->printDebugInfo(buffer,true);

     switch (type->variant())
        {
          case T_UNKNOWN:            curprint ( get_java_type_name(type) + " ");          break;
          case T_CHAR:
          case T_SIGNED_CHAR:
          case T_UNSIGNED_CHAR:
          case T_SHORT:
          case T_SIGNED_SHORT:
          case T_UNSIGNED_SHORT:
          case T_INT:
          case T_SIGNED_INT:
          case T_UNSIGNED_INT:
          case T_LONG:
          case T_SIGNED_LONG:
          case T_UNSIGNED_LONG:
          case T_VOID:
          case T_GLOBAL_VOID:
          case T_WCHAR:
          case T_FLOAT:
          case T_DOUBLE:
          case T_LONG_LONG:
          case T_UNSIGNED_LONG_LONG:
          case T_LONG_DOUBLE:
          case T_STRING:
          case T_BOOL:
          case T_COMPLEX:
          case T_IMAGINARY:
          case T_DEFAULT:
          case T_ELLIPSE:
             {
               if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )
                  { /* do nothing */; }
                 else
                  { curprint ( get_java_type_name(type) + " "); }
               break;
             }

          case T_POINTER:            unparsePointerType(type, info);          break;
          case T_MEMBER_POINTER:     unparseMemberPointerType(type, info);    break;
          case T_REFERENCE:          unparseReferenceType(type, info);        break;
       // case T_NAME:               unparseNameType(type, info);             break;
          case T_CLASS:              unparseClassType(type, info);            break;
          case T_ENUM:               unparseEnumType(type, info);             break;
          case T_TYPEDEF:            unparseTypedefType(type, info);          break;
          case T_MODIFIER:           unparseModifierType(type, info);         break;
          case T_QUALIFIED_NAME:     unparseQualifiedNameType(type, info);    break;
          case T_PARTIAL_FUNCTION:
          case T_FUNCTION:           unparseFunctionType(type, info);         break;
          case T_MEMBERFUNCTION:     unparseMemberFunctionType(type, info);   break;
          case T_ARRAY:              unparseArrayType(type, info);            break;

          default:
             {
               printf("Error: Unparse_Java::unparseType(): Default case reached in switch: Unknown type %p = %s \n",type,type->class_name().c_str());
               ROSE_ASSERT(false);
               break;
             }
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving Unparse_Java::unparseType(): type->sage_class_name() = %s firstPart = %s secondPart = %s \n",
          type->sage_class_name(),firstPartString.c_str(),secondPartString.c_str());
     curprint ( string("\n/* Bottom of unparseType name ") + type->sage_class_name()
         + " firstPart  " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif
   }

#if 1
void
Unparse_Java::unparseQualifiedNameType ( SgType* type, SgUnparse_Info& info )
   {
#if 1
     printf ("Error: This function should not be called, it represents the wrong approach to the design of the IR \n");
     ROSE_ASSERT(false);
#endif

     SgQualifiedNameType* qualifiedNameType = isSgQualifiedNameType(type);
     ROSE_ASSERT (qualifiedNameType != NULL);

     SgUnparse_Info ninfo(info);

     SgType* internalType = qualifiedNameType->get_base_type();

  // Note that this modifies the list and that SgUnparse_Info& info is passed by reference.  This could be a problem!
     ninfo.set_qualifiedNameList(qualifiedNameType->get_qualifiedNameList());

     ROSE_ASSERT(internalType != NULL);

  // Call unparseType on the base type with a modified SgUnparse_Info
     unparseType(internalType,ninfo);
   }
#endif

void
Unparse_Java::unparsePointerType(SgType* type, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparse_Java::unparsePointerType \n");
  // curprint ( "\n/* Inside of Unparse_Java::unparsePointerType */ \n";

#if 0
     info.display("Inside of Unparse_Java::unparsePointerType");
#endif

     SgPointerType* pointer_type = isSgPointerType(type);
     ROSE_ASSERT(pointer_type != NULL);

  /* special cases: ptr to array, int (*p) [10] */
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */

     if (isSgReferenceType(pointer_type->get_base_type()) ||
         isSgPointerType(pointer_type->get_base_type()) ||
         isSgArrayType(pointer_type->get_base_type()) ||
         isSgFunctionType(pointer_type->get_base_type()) ||
         isSgMemberFunctionType(pointer_type->get_base_type()) ||
         isSgModifierType(pointer_type->get_base_type()) )
        {
          info.set_isPointerToSomething();
        }

  // If not isTypeFirstPart nor isTypeSecondPart this unparse call
  // is not controlled from the statement level but from the type level

#if OUTPUT_DEBUGGING_UNPARSE_INFO
  // curprint ( "\n/* " + info.displayString("unparsePointerType") + " */ \n";
#endif

     if (info.isTypeFirstPart() == true)
        {
       // curprint ( "\n /* Calling unparseType from unparsePointerType */ \n";

          unparseType(pointer_type->get_base_type(), info);
       // curprint ( "\n /* DONE: Calling unparseType from unparsePointerType */ \n";

          ROSE_ASSERT(info.isTypeSecondPart() == false);
       // if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )
#if 0
          printf ("info.isWithType()   = %s \n",info.isWithType()   ? "true" : "false");
          printf ("info.SkipBaseType() = %s \n",info.SkipBaseType() ? "true" : "false");
          curprint ( string("\n/* info.isWithType()           = ") + (info.isWithType()   ? "true" : "false") + " */ \n");
          curprint ( string("\n/* info.SkipBaseType()         = ") + (info.SkipBaseType() ? "true" : "false") + " */ \n");
          curprint ( string("\n/* info.isPointerToSomething() = ") + (info.isPointerToSomething() ? "true" : "false") + " */ \n");
#endif

          curprint ( "*");
        }
       else
        {
          if (info.isTypeSecondPart() == true)
             {
            // printf ("In Unparse_Java::unparsePointerType(): unparse 2nd part of type \n");
            // curprint ( "\n/* In Unparse_Java::unparsePointerType(): unparse 2nd part of type */ \n";
               unparseType(pointer_type->get_base_type(), info);
            // printf ("DONE: Unparse_Java::unparsePointerType(): unparse 2nd part of type \n");
            // curprint ( "\n/* DONE: Unparse_Java::unparsePointerType(): unparse 2nd part of type */ \n";
             }
            else
             {
               SgUnparse_Info ninfo(info);
               ninfo.set_isTypeFirstPart();
               unparseType(pointer_type, ninfo);
               ninfo.set_isTypeSecondPart();
               unparseType(pointer_type, ninfo);
             }
        }

  // printf ("Leaving of Unparse_Java::unparsePointerType \n");
  // curprint ( "\n/* Leaving of Unparse_Java::unparsePointerType */ \n";
   }

void 
Unparse_Java::unparseMemberPointerType(SgType* type, SgUnparse_Info& info)
   {
     SgPointerMemberType* mpointer_type = isSgPointerMemberType(type);
     ROSE_ASSERT(mpointer_type != NULL);

  // plain type :  int (P::*)
  // type with name:  int P::* pmi = &X::a;
  // use: obj.*pmi=7;
     SgType *btype = mpointer_type->get_base_type();
     SgMemberFunctionType *ftype = NULL;

  // printf ("In unparseMemberPointerType(): btype = %p = %s \n",btype,(btype != NULL) ? btype->sage_class_name() : "NULL" );
  // curprint ( "\n/* In unparseMemberPointerType() */ \n";

     if ( (ftype = isSgMemberFunctionType(btype)) != NULL)
        {
       /* pointer to member function data */
       // printf ("In unparseMemberPointerType(): pointer to member function data \n");

          if (info.isTypeFirstPart())
             {
               unparseType(ftype->get_return_type(), info); // first part
               curprint ( "(");
            // curprint ( "\n/* mpointer_type->get_class_of() = " + mpointer_type->get_class_of()->sage_class_name() + " */ \n";
               curprint ( get_java_type_name(mpointer_type->get_class_type()) );
               curprint ( "::*");
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
                    curprint ( ")");

                 // argument list
                    SgUnparse_Info ninfo(info);
                    ninfo.unset_SkipBaseType();
                    ninfo.unset_isTypeSecondPart();
                    ninfo.unset_isTypeFirstPart();

                 // curprint ( "\n/* In unparseMemberPointerType(): start of argument list */ \n";
                    curprint ( "(");

                    SgTypePtrList::iterator p = ftype->get_arguments().begin();
                    while ( p != ftype->get_arguments().end() )
                       {
                      // printf ("In unparseMemberPointerType: output the arguments \n");
                         unparseType(*p, ninfo);
                         p++;
                         if (p != ftype->get_arguments().end()) { curprint ( ", "); }
                       }
                    curprint ( ")");
                 // curprint ( "\n/* In unparseMemberPointerType(): end of argument list */ \n";

                    unparseType(ftype->get_return_type(), info); // second part

                    if (ftype->isConstFunc())
                      curprint(" const ");

                  }
                 else
                  {
                 // not called from statement level (not sure where this is used, but it does showout in Kull)
                 // printf ("What is this 3rd case of neither 1st part nor 2nd part \n");
                    SgUnparse_Info ninfo(info);
                    ninfo.set_isTypeFirstPart();
                    unparseType(mpointer_type, ninfo);
                    ninfo.set_isTypeSecondPart();
                    unparseType(mpointer_type, ninfo);
                  }
             }
        }
       else
        {
       /* pointer to member data */
       // printf ("In unparseMemberPointerType(): pointer to member data \n");
          if (info.isTypeFirstPart())
             {
            // curprint ( "\n/* start of btype */ \n";
               unparseType(btype, info);
            // curprint ( "\n/* end of btype */ \n";
               curprint ( "(");
               curprint ( get_java_type_name(mpointer_type->get_class_type()) );
               curprint ( "::*");
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
                 // printf ("Handling the second part \n");
                    curprint ( ")");
                  }
                 else
                  {
                 // printf ("What is this 3rd case of neither 1st part nor 2nd part \n");
                    SgUnparse_Info ninfo(info);
                    ninfo.set_isTypeFirstPart();
                    unparseType(mpointer_type, ninfo);
                    ninfo.set_isTypeSecondPart();
                    unparseType(mpointer_type, ninfo);
                  }
             }
        }

  // curprint ( "\n/* Leaving unparseMemberPointerType() */ \n";
   }

void
Unparse_Java::unparseReferenceType(SgType* type, SgUnparse_Info& info)
   {
     SgReferenceType* ref_type = isSgReferenceType(type);
     ROSE_ASSERT(ref_type != NULL);

  /* special cases: ptr to array, int (*p) [10] */
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */
     SgUnparse_Info ninfo(info);

     if (isSgReferenceType(ref_type->get_base_type()) ||
         isSgPointerType(ref_type->get_base_type()) ||
         isSgArrayType(ref_type->get_base_type()) ||
         isSgFunctionType(ref_type->get_base_type()) ||
         isSgMemberFunctionType(ref_type->get_base_type()) ||
         isSgModifierType(ref_type->get_base_type()) )
          ninfo.set_isReferenceToSomething();

     if (ninfo.isTypeFirstPart())
        {
          unparseType(ref_type->get_base_type(), ninfo);
       // curprint ( "& /* reference */ ";
          curprint ( "&");
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
               unparseType(ref_type->get_base_type(), ninfo);
             }
            else
             {
               SgUnparse_Info ninfo2(ninfo);
               ninfo2.set_isTypeFirstPart();
               unparseType(ref_type, ninfo2);
               ninfo2.set_isTypeSecondPart();
               unparseType(ref_type, ninfo2);
             }
        }
   }

void
Unparse_Java::unparseClassType(SgType* type, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of Unparse_Java::unparseClassType \n");
#endif

#if 0
     info.display("Inside of Unparse_Java::unparseClassType");
#endif

     SgClassType* class_type = isSgClassType(type);
     ROSE_ASSERT(class_type != NULL);

     SgClassDeclaration *decl = isSgClassDeclaration(class_type->get_declaration());
     ROSE_ASSERT(decl != NULL);
     if (decl->get_definition() == NULL)
        {
       // We likely have a forward declaration so get the defining declaration if it is available
       // (likely the first non-defining declaration and the forward declaration are the same).
          ROSE_ASSERT(class_type->get_declaration() != NULL);
          if (decl->get_definingDeclaration() != NULL)
             {
               ROSE_ASSERT(decl->get_definingDeclaration() != NULL);
               decl = isSgClassDeclaration(decl->get_definingDeclaration());
               ROSE_ASSERT(decl != NULL);
               ROSE_ASSERT(decl->get_definition() != NULL);
             }
            else
             {
            // printf ("Can't find a class declaration with an attached definition! \n");
             }
        }

     SgClassDeclaration *cDefiningDecl = isSgClassDeclaration(decl->get_definingDeclaration());

#if 0
     printf ("info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     if ( (info.isTypeFirstPart() == true) || (info.isTypeSecondPart() == false) )
        {
       /* print the class specifiers */
       // printf ("I think that for C++ we can skip the class specifier, where for C it is required: print the class specifiers \n");
       // curprint ( "/* I think that for C++ we can skip the class specifier, where for C it is required: info.SkipClassSpecifier() = " + (info.SkipClassSpecifier() ? "true" : "false") + " */ ";
          if(!info.SkipClassSpecifier())
             {
               if (cDefiningDecl != NULL && !info.SkipClassDefinition())
                  {
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::before);
                  }
             }
        }

     if (info.isTypeSecondPart() == false)
        {
          SgName nm = decl->get_name();

          if (nm.is_null() == false)
             {
            // if (SageInterface::is_C_language() == true)
               if (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true)
                  {
                    curprint ( string(nm.str()) + " ");
                  }
                 else
                  {
#if 0
                    curprint ( string("\n/* In unparseClassType: info.forceQualifiedNames() = ") + ((info.forceQualifiedNames() == true) ? "true" : "false") + " */ \n");

                 // curprint ( "\n/* cdecl->get_need_name_qualifier() = " + (cdecl->get_need_name_qualifier() == true ? "true" : "false") + " */ \n";
                    curprint ( string("\n/* cdecl->get_scope() = ") + cdecl->get_scope()->class_name() + " */\n ");
                    curprint ( string("\n/* info.get_current_namespace() = ") + ((info.get_current_namespace() != NULL) ? info.get_current_namespace()->class_name() : "no namespace in use") + " */\n ";
                    curprint ( string("\n/* info.get_declstatement_ptr() = ") + ((info.get_declstatement_ptr() != NULL) ? info.get_declstatement_ptr()->class_name() : "no declaration statement being generated") + " */\n ";
                 // curprint ( "\n/* SageInterface::get_name(info.get_declstatement_ptr()) = " + ((info.get_declstatement_ptr() != NULL) ? SageInterface::get_name(info.get_declstatement_ptr()) : "no declaration statement available") + " */\n ";
#endif

                 // info.display("In unparseClassType: The C++ support is more complex and can require qualified names");

                 // DQ (7/20/2011): Test compilation without the generateNameQualifier() functions.
                 // The C++ support is more complex and can require qualified names!
                 // SgName nameQualifier = unp->u_name->generateNameQualifier( decl , info );
                    SgName nameQualifier;
                 // SgName nameQualifier = unp->u_name->generateNameQualifierForType( type , info );
#if 0
                    printf ("In unparseClassType: nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
                    curprint ( string("\n/* In unparseClassType: nameQualifier (from unp->u_name->generateNameQualifier function) = ") + nameQualifier + " */ \n ");
#endif
                    curprint ( nameQualifier.str());

                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(decl);
                    if (isSgTemplateInstantiationDecl(decl) != NULL)
                       {
                      // Handle case of class template instantiation (code located in unparse_stmt.C)
                         unp->u_exprStmt->unparseTemplateName(templateInstantiationDeclaration,info);
                       }
                      else
                       {
                         curprint ( string(nm.str()) + " ");
                      // printf ("class type name: nm = %s \n",nm.str());
                       }
                  }
             }
        }

#if 0
     printf ("info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("cdecl->isForward()         = %s \n",(cdecl->isForward()         == true) ? "true" : "false");
#endif

     if (info.isTypeFirstPart() == true)
        {
          if ( !info.SkipClassDefinition() )
             {
               SgClassDefinition* classdefn_stmt = decl->get_definition();
               if (classdefn_stmt != NULL)
                  {
                    SgUnparse_Info ninfo(info);
                    ninfo.unset_SkipSemiColon();

                    SgNamedType *saved_context = ninfo.get_current_context();

                    ninfo.set_current_context(NULL);
                    ninfo.set_current_context(class_type);

                    ninfo.set_current_scope(NULL);
                    ninfo.set_current_scope(classdefn_stmt);

                 // curprint ( "\n/* Unparsing class definition within unparseClassType */ \n";

                    ninfo.set_isUnsetAccess();
                    curprint ( "{");
                 // printf ("In unparseClassType: classdefn_stmt = %p \n",classdefn_stmt);
                    if (classdefn_stmt == NULL)
                       {
                         printf ("Error: In unparseClassType(): classdefn_stmt = NULL cdecl = %p = %s \n",decl,decl->get_name().str());
                       }
                    ROSE_ASSERT(classdefn_stmt != NULL);
                    SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();
                    while (pp != classdefn_stmt->get_members().end())
                       {
                         unp->u_exprStmt->unparseStatement((*pp), ninfo);
                         pp++;
                       }
                    if (cDefiningDecl->get_definition() != NULL)
                       {
                         unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl->get_definition(), info, PreprocessingInfo::inside);
                       }
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::inside);

                    curprint ( "}");

                    ninfo.set_current_context(NULL);
                    ninfo.set_current_context(saved_context);
                  }
               if (cDefiningDecl != NULL)
                  {
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::after);
                  }
             }
        }
   }


void
Unparse_Java::unparseEnumType(SgType* type, SgUnparse_Info& info)
   {
     SgEnumType* enum_type = isSgEnumType(type);
     ROSE_ASSERT(enum_type);

  // printf ("Inside of unparseEnumType() \n");

     if (info.isTypeSecondPart() == false)
        {
          SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
          SgClassDefinition *cdefn = NULL;
          SgNamespaceDefinitionStatement* namespaceDefn = NULL;

          ROSE_ASSERT(edecl != NULL);

       // printf ("edecl->isForward()         = %s \n",(edecl->isForward() == true) ? "true" : "false");

       // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
       // to be used check if name qualification is required.
          unp->u_exprStmt->initializeDeclarationsFromParent ( edecl, cdefn, namespaceDefn );

       // printf ("After initializeDeclarationsFromParent: cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);
       // printf ("In unparseEnumType: cdefn = %p \n",cdefn);

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

#if 0
     printf ("info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
#endif

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
Unparse_Java::unparseTypedefType(SgType* type, SgUnparse_Info& info)
   {
     SgTypedefType* typedef_type = isSgTypedefType(type);
     ROSE_ASSERT(typedef_type != NULL);

  // printf ("Inside of Unparse_Java::unparseTypedefType name = %p = %s \n",typedef_type,typedef_type->get_name().str());
  // curprint ( "\n/* Inside of Unparse_Java::unparseTypedefType */ \n";

#if 0
     curprint ( string("\n /* info.isWithType()       = ") + ((info.isWithType()       == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* info.SkipBaseType()     = ") + ((info.SkipBaseType()     == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* info.isTypeSecondPart() = ") + ((info.isTypeSecondPart() == true) ? "true" : "false") + " */ \n");
#endif
#if 0
     printf ("info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     if ((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
        {
       /* do nothing */;
       // printf ("Inside of Unparse_Java::unparseTypedefType (do nothing) \n");
       // curprint ( "\n /* Inside of Unparse_Java::unparseTypedefType (do nothing) */ \n");
        }
       else
        {
       // could be a scoped typedef type
       // check if currrent type's parent type is the same as the context type */
       // SgNamedType *ptype = NULL;

       // curprint ( "\n /* Inside of Unparse_Java::unparseTypedefType (normal handling) */ \n";

          SgTypedefDeclaration *tdecl = isSgTypedefDeclaration(typedef_type->get_declaration());
          ROSE_ASSERT (tdecl != NULL);

          ROSE_ASSERT(typedef_type != NULL);
          ROSE_ASSERT(typedef_type->get_declaration() != NULL);
#if 0
          printf ("typedef_type->get_declaration() = %p = %s \n",
               typedef_type->get_declaration(),typedef_type->get_declaration()->sage_class_name());
#endif
          ROSE_ASSERT(typedef_type->get_declaration()->get_parent() != NULL);

          if (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true)
             {
               ROSE_ASSERT (typedef_type->get_name().getString() != "");
               curprint ( typedef_type->get_name().getString() + " ");
             }
            else
             {
            // DQ (7/20/2011): Test compilation without the generateNameQualifier() functions.
            // The C++ support is more complex and can require qualified names!
            // SgName nameQualifier = unp->u_name->generateNameQualifier( tdecl , info );
               SgName nameQualifier;

            // printf ("nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
            // curprint ( "\n/* nameQualifier (from unp->u_name->generateNameQualifier function) = " + nameQualifier + " */ \n ";
               curprint ( nameQualifier.str());
               SgName nm = typedef_type->get_name();

               if (nm.getString() != "")
                  {
                 // printf ("Output qualifier of current types to the name = %s \n",nm.str());
                    curprint ( nm.getString() + " ");
                  }
             }
        }

  // printf ("Leaving Unparse_Java::unparseTypedefType \n");
  // curprint ( "\n/* Leaving Unparse_Java::unparseTypedefType */ \n";
   }


void Unparse_Java::unparseModifierType(SgType* type, SgUnparse_Info& info)
   {
     SgModifierType* mod_type = isSgModifierType(type);
     ROSE_ASSERT(mod_type != NULL);

  // printf ("Top of Unparse_Java::unparseModifierType \n");
  // curprint ( "\n/* Top of Unparse_Java::unparseModifierType */ \n";

#if 0
  // mod_type->get_typeModifier().display("called from Unparse_Java::unparseModifierType()");
     printf ("modifier values (at %p): %s \n",mod_type,mod_type->get_typeModifier().displayString().c_str());
#endif

  // Determine if we have to print the base type first (before printing the modifier).
  // This is true in case of a pointer (e.g., int * a) or a reference (e.g., int & a)
     bool btype_first = false;
     if ( isSgReferenceType(mod_type->get_base_type()) || isSgPointerType(mod_type->get_base_type()) )
          btype_first = true;

  // printf ("info.isTypeFirstPart() = %s \n",info.isTypeFirstPart() ? "true" : "false");
     if (info.isTypeFirstPart())
        {
       // Print the base type if this has to come first
          if (btype_first)
               unparseType(mod_type->get_base_type(), info);

          if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())
             { curprint ( "const "); }
          if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile())
             { curprint ( "volatile "); }

       // Print the base type unless it has been printed up front
          if (!btype_first)
               unparseType(mod_type->get_base_type(), info);
        }
       else
        {
       // printf ("info.isTypeSecondPart() = %s \n",info.isTypeSecondPart() ? "true" : "false");
          if (info.isTypeSecondPart())
             {
               unparseType(mod_type->get_base_type(), info);
             }
            else
             {
               SgUnparse_Info ninfo(info);
               ninfo.set_isTypeFirstPart();
               unparseType(mod_type, ninfo);
               ninfo.set_isTypeSecondPart();
               unparseType(mod_type, ninfo);
             }
        }

  // printf ("Leaving Unparse_Java::unparseModifierType \n");
  // curprint ( "\n/* Leaving Unparse_Java::unparseModifierType */ \n";
   }

void
Unparse_Java::unparseFunctionType(SgType* type, SgUnparse_Info& info)
   {
     SgFunctionType* func_type = isSgFunctionType(type);
     ROSE_ASSERT (func_type != NULL);

     SgUnparse_Info ninfo(info);
     int needParen = 0;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen=1;
        }

     ninfo.set_SkipClassDefinition();

     if (ninfo.isTypeFirstPart())
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( "\n/* In unparseFunctionType: handling first part */ \n");
          curprint ( "\n/* Skipping the first part of the return type! */ \n");
#endif
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               printf ("Skipping the first part of the return type (in needParen == true case)! \n");
               curprint ( "\n/* Skipping the first part of the return type (in needParen == true case)! */ \n");
#endif
#if OUTPUT_DEBUGGING_UNPARSE_INFO
               curprint ( string("\n/* ") + ninfo.displayString("Skipping the first part of the return type (in needParen == true case)") + " */ \n");
#endif
               unparseType(func_type->get_return_type(), ninfo);
               curprint ( "(");
             }
            else
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               printf ("Skipping the first part of the return type (in needParen == false case)! \n");
               curprint ( "\n/* Skipping the first part of the return type (in needParen == false case)! */ \n");
#endif
               unparseType(func_type->get_return_type(), ninfo);
             }
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               curprint ( "\n/* In unparseFunctionType: handling second part */ \n");
#endif
               if (needParen)
                  {
                    curprint ( ")");
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
            // print the arguments
               SgUnparse_Info ninfo2(info);
               ninfo2.unset_SkipBaseType();
               ninfo2.unset_isTypeSecondPart();
               ninfo2.unset_isTypeFirstPart();

               ninfo2.set_SkipDefinition();

               curprint ( "(");
               SgTypePtrList::iterator p = func_type->get_arguments().begin();
               while(p != func_type->get_arguments().end())
                  {
                 // printf ("Output function argument ... \n");
                    unparseType(*p, ninfo2);
                    p++;
                    if (p != func_type->get_arguments().end())
                       { curprint ( ", "); }
                  }
               curprint ( ")");
               unparseType(func_type->get_return_type(), info); // catch the 2nd part of the rtype
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               curprint ( "\n/* Done: In unparseFunctionType: handling second part */ \n");
#endif
             }
            else
             {
               ninfo.set_isTypeFirstPart();
               unparseType(func_type, ninfo);
               ninfo.set_isTypeSecondPart();
               unparseType(func_type, ninfo);
             }
        }
   }

void
Unparse_Java::unparseMemberFunctionType(SgType* type, SgUnparse_Info& info)
   {
     SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(type);
     ROSE_ASSERT(mfunc_type != NULL);

     SgUnparse_Info ninfo(info);
     int needParen=0;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen=1;
        }

     ninfo.set_SkipClassDefinition();

     if (ninfo.isTypeFirstPart())
        {
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();
               unparseType(mfunc_type->get_return_type(), ninfo);
               curprint ( "(");
             }
            else
               unparseType(mfunc_type->get_return_type(), ninfo);
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
               if (needParen)
                  {
                    curprint ( ")");
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
            // print the arguments
               SgUnparse_Info ninfo2(info);
               ninfo2.unset_SkipBaseType();
               ninfo2.unset_isTypeFirstPart();
               ninfo2.unset_isTypeSecondPart();

               curprint ( "(");
               SgTypePtrList::iterator p = mfunc_type->get_arguments().begin();
               while (p != mfunc_type->get_arguments().end())
                  {
                    unparseType(*p, ninfo2);
                    p++;
                    if (p != mfunc_type->get_arguments().end())
                       {
                         curprint ( ", ");
                       }
                  }
               curprint ( ")");
               unparseType(mfunc_type->get_return_type(), info); // catch the 2nd part of the rtype
             }
            else
             {
               ninfo.set_isTypeFirstPart();
               unparseType(mfunc_type, ninfo);
               ninfo.set_isTypeSecondPart();
               unparseType(mfunc_type, ninfo);
             }
        }
   }

void
Unparse_Java::unparseArrayType(SgType* type, SgUnparse_Info& info)
   {
     SgArrayType* array_type = isSgArrayType(type);
     ROSE_ASSERT(array_type != NULL);

     SgUnparse_Info ninfo(info);
     bool needParen = false;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen = true;
        }

     if (ninfo.isTypeFirstPart())
        {
          if(needParen == true)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();
               unparseType(array_type->get_base_type(), ninfo);
               curprint ( "(");
             }
            else
             {
               unparseType(array_type->get_base_type(), ninfo);
             }
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
               if (needParen == true)
                  {
                    curprint ( ")");
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
               curprint ( "[");
               if (array_type->get_index())
                  {
                    SgUnparse_Info ninfo2(ninfo);
                    ninfo2.unset_isTypeSecondPart();
                    unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                  }
               curprint ( "]");
               unparseType(array_type->get_base_type(), info); // second part
             }
            else
             {
               ninfo.set_isTypeFirstPart();
               unparseType(array_type, ninfo);
               ninfo.set_isTypeSecondPart();
               unparseType(array_type, ninfo);
             }
        }
   }
