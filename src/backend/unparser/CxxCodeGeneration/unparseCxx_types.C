/* unparse_type.C
1;95;0c * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// If this is turned on then we get the message to the
// generted code showing up in the mangled names!
#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS 0
#define OUTPUT_DEBUGGING_UNPARSE_INFO 0

Unparse_Type::Unparse_Type(Unparser* unp)
   : unp(unp)
   {
  // Nothing to do here!
   }

Unparse_Type::~Unparse_Type()
   {
  // Nothing to do here!
   }

void Unparse_Type::curprint (std::string str) {
  unp->u_sage->curprint(str);
}

bool
Unparse_Type::generateElaboratedType(SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info )
   {
  // For now we just return true, later we will check the scopeStatement->get_type_elaboration_list();
     bool useElaboratedType = true;

  // unp-> cur << "\n /* In generateElaboratedType = " << (useElaboratedType ? "true" : "false") << " */ \n ";

     return useElaboratedType;
   }


string get_type_name(SgType* t)
   {
  // DQ (2/12/2017): Added assertion.
     ASSERT_not_null(t);

#if 0
     printf ("In get_type_name(t = %p): t->class_name() = %s \n",t,t->class_name().c_str());
#endif

  // CH (4/7/2010): This issue is because of using a MSVC keyword 'cdecl' as a variable name

//#ifndef _MSCx_VER
//#pragma message ("WARNING: Commented out body of get_type_name()")
//         printf ("Error: Commented out body of get_type_name() \n");
//         ROSE_ASSERT(false);
//         return "ERROR IN get_type_name()";
//#else
     switch (t->variant())
        {
          case T_UNKNOWN:                 return "UNKNOWN";
          case T_CHAR:                    return "char";
          case T_SIGNED_CHAR:             return "signed char";
          case T_UNSIGNED_CHAR:           return "unsigned char";
          case T_SHORT:                   return "short";
          case T_SIGNED_SHORT:            return "signed short";
          case T_UNSIGNED_SHORT:          return "unsigned short";
          case T_INT:                     return "int";
          case T_SIGNED_INT:              return "signed int";
          case T_UNSIGNED_INT:            return "unsigned int";
          case T_LONG:                    return "long";
          case T_SIGNED_LONG:             return "signed long";
          case T_UNSIGNED_LONG:           return "unsigned long";
          case T_VOID:                    return "void";
          case T_GLOBAL_VOID:             return "global void";
          case T_WCHAR:                   return "wchar_t";

       // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
          case T_CHAR16:                  return "char16_t";
          case T_CHAR32:                  return "char32_t";

          case T_FLOAT:                   return "float";
          case T_DOUBLE:                  return "double";
          case T_LONG_LONG:               return "long long";
          case T_SIGNED_LONG_LONG:        return "signed long long";
          case T_UNSIGNED_LONG_LONG:      return "unsigned long long";

          case T_FLOAT80:                 return "__float80";
          case T_FLOAT128:                return "__float128";

        case T_MATRIX:                    return "Matrix<" + get_type_name(isSgTypeMatrix(t)->get_base_type()) + ">";

        case T_TUPLE:
          {
            SgTypeTuple *typeTuple = isSgTypeTuple(t);
            SgTypePtrList typeList = typeTuple->get_types();

            SgTypePtrList::iterator typeIterator = typeList.begin();
              
            std::string typeString = "std::tuple<";
            if(typeList.size() != 0)
              {
                typeString += get_type_name(*typeIterator);
                ++typeIterator;
              }
            
            for(; typeIterator != typeList.end(); ++typeIterator)
              {
                typeString += "," + get_type_name(*typeIterator);
              }

            typeString += ">";

            return typeString;
            
          }
       // DQ (3/24/2014): Added support for 128-bit integers.
          case T_SIGNED_128BIT_INTEGER:   return "__int128";
          case T_UNSIGNED_128BIT_INTEGER: return "unsigned __int128";

          case T_LONG_DOUBLE:             return "long double";
          case T_STRING:                  return "char*";

          case T_BOOL:
             {
            // DQ (8/27/2006): Modified to support C99 "_Bool" type (accepted by some C compilers, e.g. gcc).
            // return "bool";

            // DQ (8/27/2006): Later we want to make this an error!
            // if (SgProject::get_C_only() == true)
               if (SageInterface::is_C_language() == true)
                  {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                    printf ("Warning: SgTypeBool used for C application (reserved for use in C99 and C++) \n");
#endif
                  }

            // DQ (10/27/2012): Modified to generate consistant with C applications used with GNU.
            // I'm not sure if this is a great idea, but it appears to be more consistant with the 
            // larger scale C applications that we are seeing.
            // ROSE_ASSERT(SgProject::get_C_only() == false);
            // return (SgProject::get_C99_only() == true) ? "_Bool" : "bool";
            // return (SageInterface::is_C99_language() == true) ? "_Bool" : "bool";
               return (SageInterface::is_C99_language() == true || SageInterface::is_C_language() == true) ? "_Bool" : "bool";
             }

       // DQ (8/27/2006): Now this is finally done better!
       // DQ (10/30/2005): Need to support correct C99 name for complex
       // case T_COMPLEX:            return "complex";
          case T_COMPLEX:
             {
                SgTypeComplex* complexType = isSgTypeComplex(t);
                ASSERT_not_null(complexType);
                string returnString = get_type_name(complexType->get_base_type()) + " _Complex";
                return returnString;
             }

       // DQ (8/27/2006): Added require imaginary support to complete the complex support.
          case T_IMAGINARY:
             {
                SgTypeImaginary* imaginaryType = isSgTypeImaginary(t);
                ASSERT_not_null(imaginaryType);
                string returnString = get_type_name(imaginaryType->get_base_type());

                bool usingGcc = false;
                #ifdef USE_CMAKE
                  #ifdef CMAKE_COMPILER_IS_GNUCC
                    usingGcc = true;
                  #endif
                #else
               // DQ (2/1/2016): Make the behavior of ROSE independent of the exact name of the backend compiler (problem when packages name compilers such as "g++-4.8").
               // string backEndCompiler = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
               // usingGcc = (backEndCompiler == "g++" || backEndCompiler == "gcc" || backEndCompiler == "mpicc" || backEndCompiler == "mpicxx");
                  #if BACKEND_CXX_IS_GNU_COMPILER
                     usingGcc = true;
                  #endif
                #endif

                if (usingGcc) {
                  // Handle special case of GNU compilers
                } else {
                  returnString + " _Imaginary";
                }
               return returnString;
             }

          case T_DEFAULT:
             {
#if 0
               printf ("Output case T_DEFAULT: (int) \n");
#endif
               return "int";
             }

          case T_POINTER:
                {
#if 0
                  printf ("Output case T_POINTER: \n");
#endif
                  SgPointerType* pointer_type = isSgPointerType(t);
                  ASSERT_not_null(pointer_type);
                  return get_type_name(pointer_type->get_base_type()) + "*";
                }

          case T_REFERENCE:
                {
                  SgReferenceType* ref_type = isSgReferenceType(t);
                  ASSERT_not_null(ref_type);
                  return get_type_name(ref_type->get_base_type()) + "&";
                }

          case T_MEMBER_POINTER:
             {
               SgPointerMemberType* mpointer_type = isSgPointerMemberType(t);
               ASSERT_not_null(mpointer_type);

               SgType *btype = mpointer_type->get_base_type();
               SgMemberFunctionType *ftype = NULL;

#if 0
               printf ("In get_type_name(): case T_MEMBER_POINTER: output name: btype = %p = %s \n",btype,btype->class_name().c_str());
            // unp->u_sage->curprint ("/* In get_type_name(): output name */ \n ");
#endif
#if 0
               printf ("In get_type_name(): case T_MEMBER_POINTER: I think this is not called here! \n");
               printf ("Exting as a test! \n");
// Liao 10/16/2019. We do see code reaches this point now.            
//               ROSE_ASSERT(false);
#endif

               if ( (ftype = isSgMemberFunctionType(btype)) != NULL)
                  {
#if 0
                     printf ("In get_type_name(): ftype != NULL: output name: btype = %p = %s \n",btype,btype->class_name().c_str());
                  // unp->u_sage->curprint ("/* In get_type_name(): ftype != NULL: output name */ \n ");
#endif
                    string res = get_type_name(ftype->get_return_type()) +  "("
                                + get_type_name(mpointer_type->get_class_type())
                                + "::*" + ")" + "(" ;
                    SgTypePtrList::iterator p = ftype->get_arguments().begin();
                    while ( p != ftype->get_arguments().end() )
                       {
                         res = res + get_type_name(*p);
                         p++;
                         if (p != ftype->get_arguments().end()) { res = res + ","; }
                       }

                    res = res + ")";

#if 0
                    printf ("In get_type_name(): ftype != NULL: after unparsing function arguments: unparse modifiers \n");
#endif

                    if (ftype->isConstFunc()) {
                         res = res + " const";
                       }

                    if (ftype->get_ref_qualifiers() == 1) {
                         res = res + " &";
                       } else if (ftype->get_ref_qualifiers() == 2) {
                         res = res + " &&";
                       }

                    return res;
                  }
                 else
                  {
#if 0
                    printf ("In get_type_name(): ftype == NULL: output name: btype = %p = %s \n",btype,btype->class_name().c_str());
                 // unp->u_sage->curprint ("/* In get_type_name(): ftype == NULL: output name */ \n ");
#endif

#if 0
                 // DQ (3/31/2019): We need access to the Sg_Unparse_Info and we don't have it, so implement this in the unparsePointerMember() function more directly.
                 // NODE: This may be called as part of unparseToString() which often lacks the context needed to always get the name qualification correct.
#error "DEAD CODE!"

#if 0
                 // DQ (3/31/2019): I think this is where the added name qualification for the associated class must be output.
                    printf ("Additional name qualification for the associated class_type must be output here (inbetween the base type and the class type) \n");
#endif

#define DEBUG_UNPARSE_POINTER_MEMBER_TYPE 0

#error "DEAD CODE!"

                    if (info.get_reference_node_for_qualification() != NULL)
                       {
#if DEBUG_UNPARSE_POINTER_MEMBER_TYPE
                         printf ("info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif
                         SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());

#error "DEAD CODE!"

#if DEBUG_UNPARSE_POINTER_MEMBER_TYPE
                         printf ("nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
#endif
#if 1
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                         return get_type_name(btype) + "(" + nameQualifier.str() + get_type_name(mpointer_type->get_class_type()) + "::*" + ")";
                       }
                      else
                       {
#error "DEAD CODE!"

                         return get_type_name(btype) + "(" + get_type_name(mpointer_type->get_class_type()) + "::*" + ")";
                       }
#else
                    return get_type_name(btype) + "(" + get_type_name(mpointer_type->get_class_type()) + "::*" + ")";
#endif
                  }

#if 0
                  printf ("In get_type_name(): DONE: output name: btype = %p = %s \n",btype,btype->class_name().c_str());
               // unp->u_sage->curprint ("/* In get_type_name(): DONE: output name */ \n ");
#endif
               }

          case T_CLASS:
              {
                SgClassType* class_type = isSgClassType(t);
                ASSERT_not_null(class_type);
             // CH (4/7/2010): 'cdecl' is a keywork of MSVC
             // SgClassDeclaration* cdecl;
                SgClassDeclaration* decl = isSgClassDeclaration(class_type->get_declaration());

             // DQ (3/29/2019): We don't what the qualified name, this is an error for C++11 test2019_316.C.
             // SgName nm = decl->get_qualified_name();
                SgName nm = decl->get_name();
#if 0
                printf ("In get_type_name(%p): case T_CLASS: nm = %s \n",t,nm.str());
#endif
#if 0
                printf ("decl = %p = %s \n",decl,decl->class_name().c_str());
                SgClassDeclaration* nondefining_decl = isSgClassDeclaration(decl->get_firstNondefiningDeclaration());
                printf ("decl = %p = %s \n",nondefining_decl,nondefining_decl->class_name().c_str());
                SgClassDeclaration* defining_decl    = isSgClassDeclaration(decl->get_definingDeclaration());
                printf ("decl = %p = %s \n",defining_decl,defining_decl->class_name().c_str());
#endif
#if 0
                printf ("In get_type_name(): case T_CLASS: Exiting as a test! \n");
                ROSE_ASSERT(false);
#endif
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

             // DQ (3/29/2019): We don't what the qualified name, this is an error for C++11 test2019_316.C.
             // So this may have to be corrected.
             // SgName nm = edecl->get_qualified_name();
                SgName nm = edecl->get_name();

                if (nm.getString() != "")
                   {
#if 0
                     printf ("In get_type_name(SgType* t): Using enum type name = %s \n",nm.str());
#endif
                     return nm.getString();
                   }
                  else
                   {
#if 0
                  // DQ (4/22/2013): Debugging test2013_125.C.
                     printf ("WARNING: In get_type_name(SgType* t): edecl->get_qualified_name() == \"\": so we have to use the enum_type->get_name() = %s \n",enum_type->get_name().str());
#endif
                     return string(enum_type->get_name().str());
                   }
               }

          case T_TYPEDEF:
               {
                SgTypedefType* typedef_type = isSgTypedefType(t);
                ASSERT_not_null(typedef_type);

             // DQ (3/29/2019): We don't what the qualified name, this is an error for C++11 test2019_316.C.
             // So this may have to be corrected.
             // return string(typedef_type->get_qualified_name().str());
                SgName nm = typedef_type->get_name();
                return string(nm.str());
               }

          case T_MODIFIER:
             {
               SgModifierType* mod_type = isSgModifierType(t);
               ASSERT_not_null(mod_type);
               string res;
               bool unparse_base = true;
               if (mod_type->get_typeModifier().isOpenclGlobal())
                    res = "__global " + res;
               if (mod_type->get_typeModifier().isOpenclLocal())
                    res = "__local " + res;
               if (mod_type->get_typeModifier().isOpenclConstant())
                    res = "__constant " + res;
               if ( isSgReferenceType(mod_type->get_base_type()) ||
                     isSgPointerType(mod_type->get_base_type()) ) 
                  {
                    res = get_type_name(mod_type->get_base_type());
                    unparse_base = false;
                  }
               if (mod_type->get_typeModifier().haveAddressSpace()) 
                  {
                    std::ostringstream outstr;
                    outstr << mod_type->get_typeModifier().get_address_space_value(); 
                    res = res + "__attribute__((address_space(" + outstr.str() + ")))";
                  }

            // DQ (1/20/2019): By design this is handling the casse of the combination of const and volatile modifiers.
               if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())
                    res = res + "const ";
               if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile())
                    res = res + "volatile ";

               if (mod_type->get_typeModifier().isRestrict())
                  {
                 // DQ (9/2/2014): Added support for mpiicpc used at LLNL.
                 // DQ (8/29/2005): Added support for classification of back-end compilers (independent of the name invoked to execute them)
#if 1
                 // DQ (4/16/2016): Use the new refactored form of this support code.
                    res = res + Unparse_Type::unparseRestrictKeyword();
#else

#error "DEAD CODE!"
                    bool usingGcc = false;
                    #ifdef USE_CMAKE
                       #ifdef CMAKE_COMPILER_IS_GNUCC
                          usingGcc = true;
                       #endif
                    #else
                    // DQ (4/16/2016): The clang compiler also uses the GNU form of the restrict keyword.
                    // DQ (2/1/2016): Make the behavior of ROSE independent of the exact name of the backend compiler (problem when packages name compilers such as "g++-4.8").
                    // string compilerName = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
                    // usingGcc = (compilerName == "g++" || compilerName == "gcc" || compilerName == "mpicc" || compilerName == "mpicxx" || compilerName == "mpiicpc");
                    // #if BACKEND_CXX_IS_GNU_COMPILER
                       #if BACKEND_CXX_IS_GNU_COMPILER || BACKEND_CXX_IS_CLANG_COMPILER
                          usingGcc = true;
                       #endif
#if 0
                    string compilerName = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
                    printf ("Processing restrict keyword: compilerName = %s \n",compilerName.c_str());
#endif
                    #endif
#error "DEAD CODE!"
                    if ( usingGcc )
                       {
#if 0
                         printf ("Using GNU form of restrict keyword! \n");
#endif
                         res = res + "__restrict__ ";
                       }
                      else
                       {
#if 0
                         printf ("Using non-GNU form of restrict keyword! \n");
#endif
                         res = res + "restrict ";
                       }
#error "DEAD CODE!"
#endif
                  }
               if (unparse_base)
                    res = res + get_type_name(mod_type->get_base_type());

               return res;
             }
#if 0
          case T_QUALIFIED_NAME:
             {
               SgQualifiedNameType* qualifiedNameType = isSgQualifiedNameType(t);
               ASSERT_not_null(qualifiedNameType);
                string res;
                printf ("WARNING: In unparser get_type_name() generated name should use a prefix generated from mangled names \n");
                res = "__qualified_name_";
                return res;
             }
#endif
          case T_PARTIAL_FUNCTION:
          case T_FUNCTION:
               {
                SgFunctionType* func_type = isSgFunctionType(t);
                ASSERT_not_null(func_type);
                string res = get_type_name(func_type->get_return_type()) + "(";
                SgTypePtrList::iterator p = func_type->get_arguments().begin();
                while(p != func_type->get_arguments().end())
                  {
                    res = res + get_type_name(*p);
                    p++;
                    if (p != func_type->get_arguments().end())
                        res = res + ",";
#if 0
                    printf ("In get_type_name(): case T_FUNCTION:res = %s \n",res.c_str());
#endif
                  }
               res = res + ")";
               return res;
              }

          case T_MEMBERFUNCTION:
              {
               SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(t);
               ASSERT_not_null(mfunc_type);

               string res = get_type_name(mfunc_type->get_return_type()) + "(";
               SgTypePtrList::iterator p = mfunc_type->get_arguments().begin();
               while (p != mfunc_type->get_arguments().end()) 
                  {
                    res = res + get_type_name(*p);
                    p++;
                    if (p != mfunc_type->get_arguments().end())
                         res = res + ",";
#if 0
                    printf ("In get_type_name(): case T_MEMBERFUNCTION:res = %s \n",res.c_str());
#endif
                  }
               res = res + ")";

               if (mfunc_type->isConstFunc()) {
                 res = res + " const";
               }

               if (mfunc_type->get_ref_qualifiers() == 1) {
                 res = res + " &";
               } else if (mfunc_type->get_ref_qualifiers() == 2) {
                 res = res + " &&";
               }

               return res;
             }

          case T_ARRAY:
             {
              SgArrayType* array_type = isSgArrayType(t);
              ASSERT_not_null(array_type);

              string res = get_type_name(array_type->get_base_type()) + "[";
#if 0
              printf ("In get_type_name(): case T_ARRAY: res = %s \n",res.c_str());
#endif
              if (array_type->get_index())
                  res = res + array_type->get_index()->unparseToString();
               res = res + "]";
               return res;
             }

          case T_ELLIPSE: return "...";

          case T_TEMPLATE:
             {
               SgTemplateType * tpl_type = isSgTemplateType(t);
               ASSERT_not_null(tpl_type);
               return tpl_type->get_name();
             }

          case T_NONREAL:
             {
               SgNonrealType * nrtype = isSgNonrealType(t);
               ASSERT_not_null(nrtype);
               return nrtype->get_name();
             }

         case T_RVALUE_REFERENCE:
             {
               SgRvalueReferenceType* rref_type = isSgRvalueReferenceType(t);
               ASSERT_not_null(rref_type);
               return get_type_name(rref_type->get_base_type()) + "&&";
             }

          default:
             {
               printf("Error: unparse_type.C get_type_name(): Default case reached in switch: %s\n", t->class_name().c_str());
               ROSE_ASSERT(false);
               break;
             }
        }
//#endif

#if 0
     printf ("Leaving get_type_name(t = %p): t->class_name() = %s \n",t,t->class_name().c_str());
#endif
   }

//-----------------------------------------------------------------------------------
//  void Unparse_Type::unparseType
//
//  General function that gets called when unparsing a C++ type. Then it routes
//  to the appropriate function to unparse each C++ type.
//-----------------------------------------------------------------------------------
void
Unparse_Type::unparseType(SgType* type, SgUnparse_Info& info)
   {
     ASSERT_not_null(type);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
     string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
     printf ("In Unparse_Type::unparseType(): type = %p type->class_name() = %s firstPart = %s secondPart = %s \n",
             type,type->class_name().c_str(),firstPartString.c_str(),secondPartString.c_str());
#endif
#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     curprint ( string("\n/* Top of unparseType name ") + type->class_name().c_str()
         + " firstPart " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif

#if 0
     printf ("In Unparse_Type::unparseType(): type = %p = %s (%s/%s)\n",type,type->class_name().c_str(), info.isTypeFirstPart() ? "true" : "false", info.isTypeSecondPart() ? "true" : "false");
#endif

  // DQ (10/31/2018): Adding assertion.
  // ASSERT_not_null(info.get_current_source_file());

#if 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint(string("\n/* Top of unparseType: class_name() = ") + type->class_name() + " */ \n");
#endif

#if 0
     printf ("In unparseType: info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
#endif

#if 0
     if (info.SkipClassDefinition() != info.SkipEnumDefinition())
        {
          printf ("In unparseType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
          printf ("In unparseType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition()  == true) ? "true" : "false");
        }
#endif
  // DQ (1/13/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if 0
     char buffer[512];
     sprintf (buffer,"entering case for %s",type->class_name().c_str());
     unp->u_debug->printDebugInfo(buffer,true);
#endif

#if 0
     printf ("In unparseType(): info.SkipBaseType() = %s \n",info.SkipBaseType() ? "true" : "false");
  // curprint ("\n /* In unparseType(): info.SkipBaseType() = " + string(info.SkipBaseType() ? "true" : "false") + " */ \n");
#endif

  // DQ (6/4/2011): This controls if we output the generated name of the type (required to 
  // support name qualification of subtypes) or if we unparse the type from the AST (where 
  // name qualification of subtypes is not required).
     bool usingGeneratedNameQualifiedTypeNameString = false;
     string typeNameString;

  // DQ (6/4/2011): Support for output of generated string for type (used where name 
  // qualification is required for subtypes (e.g. template arguments)).
     SgNode* nodeReferenceToType = info.get_reference_node_for_qualification();

     SgInitializedName * init_name_reference_node = isSgInitializedName(nodeReferenceToType);
     if (init_name_reference_node != NULL) {
       if (init_name_reference_node->get_auto_decltype() != NULL) {
         nodeReferenceToType = NULL;
       }
     }

#define DEBUG_GENERATED_STRING_USE 0

#if DEBUG_GENERATED_STRING_USE
     printf ("In unparseType(): nodeReferenceToType = %p = %s \n",nodeReferenceToType,(nodeReferenceToType != NULL) ? nodeReferenceToType->class_name().c_str() : "null");
#endif

     if (nodeReferenceToType != NULL)
        {
#if DEBUG_GENERATED_STRING_USE
          printf ("rrrrrrrrrrrr In unparseType() output type generated name: nodeReferenceToType = %p = %s SgNode::get_globalTypeNameMap().size() = %" PRIuPTR " \n",
               nodeReferenceToType,nodeReferenceToType->class_name().c_str(),SgNode::get_globalTypeNameMap().size());
#endif
          std::map<SgNode*,std::string>::iterator i = SgNode::get_globalTypeNameMap().find(nodeReferenceToType);
          if (i != SgNode::get_globalTypeNameMap().end())
             {
            // usingGeneratedNameQualifiedTypeNameString = true;
               if (info.isTypeSecondPart() == false)
                  {
                    usingGeneratedNameQualifiedTypeNameString = true;
                  }

               typeNameString = i->second.c_str();
#if DEBUG_GENERATED_STRING_USE
               printf ("ssssssssssssssss Found type name in SgNode::get_globalTypeNameMap() typeNameString = %s for nodeReferenceToType = %p = %s \n",
                    typeNameString.c_str(),nodeReferenceToType,nodeReferenceToType->class_name().c_str());
#endif
             }
            else
             {
#if DEBUG_GENERATED_STRING_USE
               printf ("In unparseType(): string not found in globalTypeNameMap \n");
#endif
             }
        }

#if DEBUG_GENERATED_STRING_USE
     printf ("In unparseType(): usingGeneratedNameQualifiedTypeNameString = %s \n",usingGeneratedNameQualifiedTypeNameString ? "true" : "false");
#endif

#if 0
  // DQ (4/21/2019): This debugging information is helpful in identifying that typese were taken from generated strings at specific locations.
  // However, could it be that this can be confusing because even types that might not be unparsed may have some output (is this true?).
     if (usingGeneratedNameQualifiedTypeNameString == true)
        {
          curprint ("\n /* In unparseType(): usingGeneratedNameQualifiedTypeNameString = " + string(usingGeneratedNameQualifiedTypeNameString ? "true" : "false") + " */ \n");
        }
#endif

     if (usingGeneratedNameQualifiedTypeNameString == true)
        {
       // Output the previously generated type name contianing the correct name qualification of subtypes (e.g. template arguments).
#if 0
       // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
          string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
          string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
          printf ("In Unparse_Type::unparseType() using generated type name string: type->class_name() = %s firstPart = %s secondPart = %s \n",type->class_name().c_str(),firstPartString.c_str(),secondPartString.c_str());
          curprint(string("\n/* Top of unparseType() using generated type name string: ") + type->sage_class_name() + " firstPart " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif
          if (info.isTypeFirstPart() == true)
             {
#if 0
               printf ("Ouput typeNameString = %s \n",typeNameString.c_str());
#endif
               if (info.SkipBaseType() == false)
                  {
                    curprint(typeNameString);
                  }
             }
            else
             {
            // Sometimes neither is set and this is the trivial case where we want to output the generated type name (see test2011_74.C).
            // if (isSgPointerType(type) != NULL)
               if (info.isTypeFirstPart() == false && info.isTypeSecondPart() == false)
                  {
#if 0
                    printf ("Note: Handling unparsing of name qualified type as special case (typeNameString = %s) \n",typeNameString.c_str());
#endif
                 // DQ (6/18/2013): Added support to skip output of typenames when handling multiple variable declarations in 
                 // SgForInitStmt IR nodes and multiple SgInitializedName IR nodes in a single SgVariableDeclaration IR node.
                    if (info.SkipBaseType() == false)
                       {
                         curprint(typeNameString);
                       }
                  }
             }
        }
       else
        {
#if 0
          printf("Top of unparseType() processing main switch statement: type = %p = %s \n",type,type->class_name().c_str());
#endif
#if 0
          curprint("\n/* Top of unparseType() processing main switch statement */ \n");
#endif

       // This is the code that was always used before the addition of type names generated from where name qualification of subtypes are required.
          switch (type->variant())
             {
               case T_UNKNOWN:            curprint ( get_type_name(type) + " ");          break;
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

            // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
               case T_CHAR16:
               case T_CHAR32:

               case T_FLOAT:
               case T_DOUBLE:
               case T_FLOAT80:
               case T_FLOAT128:
               case T_LONG_LONG:
               case T_UNSIGNED_LONG_LONG:
               case T_SIGNED_LONG_LONG:

            // DQ (3/24/2014): Added support for 128-bit integers.
               case T_SIGNED_128BIT_INTEGER:
               case T_UNSIGNED_128BIT_INTEGER:

            // SK: Matrix type for Matlab
               case T_MATRIX:
               case T_TUPLE:
               case T_LONG_DOUBLE:
               case T_STRING:
               case T_BOOL:
               case T_COMPLEX:
               case T_IMAGINARY:
               case T_DEFAULT:
               case T_ELLIPSE:
                  {
                    if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )
                       {
                      /* do nothing */
                       }
                      else
                       {
                         curprint ( get_type_name(type) + " ");
                       }
                    break;
                  }

            // case T_POINTER:            unparsePointerType(type, info);          break;
               case T_POINTER:
                  {
#if 0
                    printf ("Calling unparsePointerType(%p) \n",type);
#endif
                    unparsePointerType(type, info);
#if 0
                    printf ("DONE: Calling unparsePointerType(%p) \n",type);
#endif
                    break;
                  }

               case T_MEMBER_POINTER:     unparseMemberPointerType(type, info);    break;
               case T_REFERENCE:          unparseReferenceType(type, info);        break;

               case T_RVALUE_REFERENCE:   unparseRvalueReferenceType(type, info);  break;

            // case T_NAME:               unparseNameType(type, info);             break;

            // DQ (6/18/2013): Test to see if this is the correct handling of test2013_214.C.
#if 1
            // DQ (6/18/2013): Original version of code.
               case T_CLASS:              unparseClassType(type, info);            break;
#else
               case T_CLASS:
                  {
                    if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )
                       {
                      /* do nothing */
#if 0
                         printf ("In unparseType(): Skipping output of SgClassType \n");
#endif
#if 0
                         curprint ("/* In unparseType(): Skipping output of SgClassType */ \n");
#endif
                       }

#error "DEAD CODE!"

                      else
                       {
                         unparseClassType(type, info);
                       }
                    break;
                  }
#endif
               case T_ENUM:               unparseEnumType(type, info);             break;

            // DQ (6/18/2013): Test to see if this is the correct handling of test2013_214.C.
#if 1
            // DQ (6/18/2013): Original version of code.
               case T_TYPEDEF:            unparseTypedefType(type, info);          break;
#else
               case T_TYPEDEF:
                  {
                 // if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )
                    if ( info.SkipBaseType() == true )
#error "DEAD CODE!"

                       {
                      /* do nothing */
#if 0
                         printf ("In unparseType(): Skipping output of SgTypedefType \n");
#endif
#if 0
                         curprint ("/* In unparseType(): Skipping output of SgTypedefType */ \n");
#endif
                       }
                      else
                       {
#if 0
                         printf ("In unparseType(): Calling unparseTypedefType \n");
                         curprint ("/* In unparseType(): Calling unparseTypedefType */ \n");
#endif
                         unparseTypedefType(type, info);
                       }
#error "DEAD CODE!"

                    break;
                  }
#endif
               case T_MODIFIER:           unparseModifierType(type, info);         break;

            // DQ (5/3/2013): This approach is no longer supported, as I recall.
            // case T_QUALIFIED_NAME:     unparseQualifiedNameType(type, info);    break;

               case T_PARTIAL_FUNCTION:
               case T_FUNCTION:           unparseFunctionType(type, info);         break;

               case T_MEMBERFUNCTION:     unparseMemberFunctionType(type, info);   break;
               case T_ARRAY:              unparseArrayType(type, info);            break;

            // DQ (11/20/2011): Adding support for template declarations within the AST.
               case T_TEMPLATE:
                  {
#if 0
                    printf ("Calling unparseTemplateType(%p) \n",type);
#endif
                    unparseTemplateType(type, info);
#if 0
                    printf ("DONE: Calling unparseTemplateType(%p) \n",type);
#endif
                    break;
                  }

            // TV (09/06/2018): Adding support for auto typed variable declaration.
               case T_AUTO:
                  {
                    unparseAutoType(type, info);
                    break;
                  }

             // DQ (3/10/2014): Added so that we could get past this call in the dot file generator (fix later).
             // SgJavaWildcardType
               case T_JAVA_WILD:
                  {
                    printf ("ERROR: SgJavaWildcardType is appearing in call to unparseType from graph generation (allow this for now) \n");
                    break;
                  }

            // DQ (7/30/2014): Fixed spelling of T_LABEL tag.
            // DQ (4/27/2014): After some fixes to ROSE to permit the new shared memory DSL, we now get this 
            // IR node appearing in test2007_168.f90 (I don't yet understand why).
            // case T_LABLE:
               case T_LABEL:
                  {
                    printf ("ERROR: Unparse_Type::unparseType(): SgTypeLabel is appearing in test2007_168.f90 (where it had not appeared before) (allow this for now) \n");
                    break;
                  }

            // DQ (7/31/2014): Adding support for nullptr constant expression and its associated type.
               case T_NULLPTR:
                  {
                    unparseNullptrType(type, info);
                 // printf ("ERROR: Unparse_Type::unparseType(): SgTypeNullptr: we should not have to be unparsing this type (C++11 specific) \n");
                    break;
                  }

            // DQ (8/2/2014): Adding support for C++11 decltype.
               case T_DECLTYPE:
                  {
                    unparseDeclType(type, info);
                    break;
                  }

            // DQ (3/28/2015): Adding support for GNU C typeof language extension.
               case T_TYPEOF_TYPE:
                  {
                    unparseTypeOfType(type, info);
                    break;
                  }

               case T_NONREAL:
                  {
                    unparseNonrealType(type, info);
                    break;
                  }

               default:
                  {
                    printf("Error: Unparse_Type::unparseType(): Default case reached in switch: Unknown type %p = %s \n",type,type->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
                  }
             }
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     printf ("Leaving Unparse_Type::unparseType(): type->class_name() = %s firstPart = %s secondPart = %s \n",
             type->class_name().c_str(),firstPartString.c_str(),secondPartString.c_str());
     curprint ( string("\n/* Bottom of unparseType name ") + type->class_name().c_str()
         + " firstPart  " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif
   }


void
Unparse_Type::unparseNullptrType(SgType* type, SgUnparse_Info& info)
   {
  // DQ (7/31/2014): Adding support for nullptr constant expression and its associated type.

     curprint("std::nullptr_t");
   }


void
Unparse_Type::unparseDeclType(SgType* type, SgUnparse_Info& info)
   {
  // DQ (8/2/2014): Adding support for C++11 decltype.

     SgDeclType* decltype_node = isSgDeclType(type);
     ASSERT_not_null(decltype_node);

     ASSERT_not_null(decltype_node->get_base_expression());

#if 0
     printf ("In Unparse_Type::unparseDeclType(): decltype_node = %p \n",decltype_node);
#endif

     if (info.isTypeFirstPart() == true)
        {
#if 0
       // DQ (12/13/2015): We need to know if this is using the C++11 mode, else we need to output "__decltype(" instead.
          info.display("debugging decltype for non-c++11 mode");
          printf ("decltype_node->get_base_expression() = %p = %s \n",decltype_node->get_base_expression(),decltype_node->get_base_expression()->class_name().c_str());
#endif
          SgFunctionParameterRefExp* functionParameterRefExp = isSgFunctionParameterRefExp(decltype_node->get_base_expression());
          if (functionParameterRefExp != NULL)
             {
            // In this case just use the type directly.
               ASSERT_not_null(decltype_node->get_base_type());
#if 0
               printf ("In unparseDeclType(): detected SgFunctionParameterRefExp: using decltype_node->get_base_type() = %p = %s \n",decltype_node->get_base_type(),decltype_node->get_base_type()->class_name().c_str());
#endif
               unparseType(decltype_node->get_base_type(),info);
             }
            else
             {
               curprint("decltype(");
               unp->u_exprStmt->unparseExpression(decltype_node->get_base_expression(),info);
               curprint(") ");
             }
        }
   }


void
Unparse_Type::unparseTypeOfType(SgType* type, SgUnparse_Info& info)
   {
  // DQ (3/28/2015): Adding support for GNU C typeof language extension.

     SgTypeOfType* typeof_node = isSgTypeOfType(type);
     ASSERT_not_null(typeof_node);

#define DEBUG_TYPEOF_TYPE 0

#if DEBUG_TYPEOF_TYPE || 0
     printf ("In unparseTypeOfType(): typeof_node       = %p \n",typeof_node);
     printf ("   --- typeof_node->get_base_expression() = %p \n",typeof_node->get_base_expression());
     if (typeof_node->get_base_expression() != NULL)
        {
          printf ("   --- typeof_node->get_base_expression() = %p = %s \n",typeof_node->get_base_expression(),typeof_node->get_base_expression()->class_name().c_str());
        }
     printf ("   --- typeof_node->get_base_type()            = %p \n",typeof_node->get_base_type());
     if (typeof_node->get_base_type() != NULL)
        {
          printf ("   --- typeof_node->get_base_type() = %p = %s \n",typeof_node->get_base_type(),typeof_node->get_base_type()->class_name().c_str());
        }
     printf ("   --- info.isTypeFirstPart()             = %s \n",info.isTypeFirstPart() ? "true" : "false");
     printf ("   --- info.isTypeSecondPart()            = %s \n",info.isTypeSecondPart() ? "true" : "false");
#endif

  // ASSERT_not_null(typeof_node->get_base_expression());

  // DQ (3/31/2015): I think we can assert this.
  // ROSE_ASSERT (info.isTypeFirstPart() == true  || info.isTypeSecondPart() == true);
     ROSE_ASSERT (info.isTypeFirstPart() == false || info.isTypeSecondPart() == false);


  // DQ (3/31/2015): We can't use the perenthesis in this case (see test2015_49.c).
#if 0
     printf("################ In unparseTypeOfType(): handle special case of SgTypeOfType \n");
#endif

  // DQ (3/31/2015): Note that (info.isTypeFirstPart() == false && info.isTypeSecondPart() == false) is required because
  // we have implemented some special case handling for SgTypeOfType in SgArrayType, SgPointerType, etc.  With this
  // implementation below, we might not need this special case handling.
  // if (info.isTypeFirstPart() == true)
     if (info.isTypeFirstPart() == true || (info.isTypeFirstPart() == false && info.isTypeSecondPart() == false) )
        {
          curprint("__typeof(");
          if (typeof_node->get_base_expression() != NULL)
             {
               unp->u_exprStmt->unparseExpression(typeof_node->get_base_expression(),info);
             }
            else
             {
               SgUnparse_Info ninfo1(info);

               ninfo1.set_SkipClassDefinition();
            // ninfo1.set_SkipClassSpecifier();
               ninfo1.set_SkipEnumDefinition();

               ninfo1.unset_isTypeSecondPart();
               ninfo1.unset_isTypeFirstPart();

// DQ (5/10/2015): Changing this back to calling "unparseType(typeof_node->get_base_type(), ninfo1);" once.
#if 1

// #error "DEAD CODE!"

               unparseType(typeof_node->get_base_type(), ninfo1);
#else
               ninfo1.set_isTypeFirstPart();
#if DEBUG_TYPEOF_TYPE
               printf("In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) */ \n");
#endif
               unparseType(typeof_node->get_base_type(), ninfo1);
               ninfo1.set_isTypeSecondPart();
#if DEBUG_TYPEOF_TYPE
               printf("In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) */ \n");
#endif
               unparseType(typeof_node->get_base_type(), ninfo1);
#endif
             }
       // curprint("/* end of typeof */ )");
          curprint(") ");
        }

#if 0
  // DQ (3/31/2015): This is an inferior implementation.

     if (info.isTypeFirstPart() == true)
        {
          SgFunctionParameterRefExp* functionParameterRefExp = isSgFunctionParameterRefExp(typeof_node->get_base_expression());
          if (functionParameterRefExp != NULL)
             {
            // DQ (3/31/2015): I am not sure I understand this case well enough and I want to debug it seperately.

            // In this case just use the type directly.
               ASSERT_not_null(typeof_node->get_base_type());
#if 0
               printf ("In unparseTypeOfType(): detected SgFunctionParameterRefExp: using typeof_node->get_base_type() = %p = %s \n",typeof_node->get_base_type(),typeof_node->get_base_type()->class_name().c_str());
#endif

#error "DEAD CODE!"

               printf ("Exiting as a test: debug this case seperately for unparseTypeOfType() \n");
               ROSE_ASSERT(false);

               unparseType(typeof_node->get_base_type(),info);
             }
            else
             {
               curprint("typeof(");

            // unp->u_exprStmt->unparseExpression(typeof_node->get_base_expression(),info);
               if (typeof_node->get_base_expression() != NULL)
                  {
                    unp->u_exprStmt->unparseExpression(typeof_node->get_base_expression(),info);
#error "DEAD CODE!"

                  }
                 else
                  {
                    SgUnparse_Info newinfo(info);
#if 0
                    printf ("typeof_node->get_base_type() = %p = %s \n",typeof_node->get_base_type(),typeof_node->get_base_type()->class_name().c_str());
#endif
                    newinfo.set_SkipClassDefinition();
                 // newinfo.set_SkipClassSpecifier();
                    newinfo.set_SkipEnumDefinition();
#if 0
                 // DQ (3/30/2015): We need to unset these to support when the base type is a SgArrayType.
                 // if (isSgArrayType(typeof_node->get_base_type()) != NULL)
                    if (isSgModifierType(typeof_node->get_base_type()) != NULL)
                       {
                         newinfo.unset_isReferenceToSomething();
                         newinfo.unset_isPointerToSomething();
                       }
#endif
#if 0
                    newinfo.unset_isTypeFirstPart();
                    newinfo.unset_isTypeSecondPart();
#endif

#error "DEAD CODE!"

#if 1
                 // unparseType(typeof_node->get_base_type(),info);
                    unparseType(typeof_node->get_base_type(),newinfo);
#else
                    SgType* type = typeof_node->get_base_type();

#error "DEAD CODE!"

                 // SgUnparse_Info newinfo(info);
                 // newinfo.set_reference_node_for_qualification(operatorExp);
                    if (newinfo.get_reference_node_for_qualification() == NULL)
                       {
                         printf ("Note: In unparseTypeOfType(): newinfo.get_reference_node_for_qualification() == NULL \n");
                       }
                 // ASSERT_not_null(newinfo.get_reference_node_for_qualification());

                    newinfo.set_isTypeFirstPart();
#if 0
                    printf ("In unparseTypeOfType(): isTypeFirstPart: sizeof_op->get_operand_type() = %p = %s \n",type,type->class_name().c_str());
                    curprint ("/* In unparseTypeOfType(): isTypeFirstPart \n */ ");
#endif
                    unp->u_type->unparseType(type, newinfo);

                    newinfo.unset_isTypeFirstPart();
#if 0
                    newinfo.set_isTypeSecondPart();
#if 0
                    printf ("In unparseTypeOfType(): isTypeSecondPart: type = %p = %s \n",type,type->class_name().c_str());
                    curprint ("/* In unparseTypeOfType(): isTypeSecondPart \n */ ");
#endif
                    unp->u_type->unparseType(type, newinfo);
#endif

                    newinfo.unset_isTypeFirstPart();
                    newinfo.unset_isTypeSecondPart();
#endif
                  }

#error "DEAD CODE!"

            // curprint(") ");
               curprint(" /* closing typeof paren: first part */ ) ");
             }
        }
       else
        {
       // DQ (3/31/2015): Implemented this as an else-case of when first-part is false.

       // DQ (3/29/2015): We need to handle the 2nd part of the type when the types are more complex (see case of array type used in test2015_49.c).
          if (info.isTypeSecondPart() == true)
             {
            // curprint(" /* closing typeof paren: second part */ ) ");

               if (typeof_node->get_base_expression() != NULL)
                  {
#if 0
                    printf ("Nothing to do for info.isTypeSecondPart() == true and typeof_node->get_base_expression() != NULL \n");
#endif
                 // unp->u_exprStmt->unparseExpression(typeof_node->get_base_expression(),info);
                  }
                 else
                  {
                    SgUnparse_Info newinfo(info);
#if 0
                 // DQ (3/30/2015): We need to unset these to support when the base type is a SgArrayType.
                 // if (isSgArrayType(typeof_node->get_base_type()) != NULL)
                    if (isSgModifierType(typeof_node->get_base_type()) != NULL)
                       {
                         newinfo.unset_isReferenceToSomething();
                         newinfo.unset_isPointerToSomething();
                       }
#endif
                    newinfo.set_SkipClassDefinition();
                 // newinfo.set_SkipClassSpecifier();
                    newinfo.set_SkipEnumDefinition();

                 // unparseType(typeof_node->get_base_type(),info);
                    unparseType(typeof_node->get_base_type(),newinfo);
                  }

#error "DEAD CODE!"

            // curprint(") ");
            // curprint(" /* closing typeof paren: second part */ ) ");
             }
            else
             {
            // DQ (3/31/2015): Added general case for for when neither first-part nor second-part flags are set.
            // This is done to support more general simplified use with a sing call to the unpars function for 
            // cases the type is a SgTypeOfType.

               SgUnparse_Info ninfo1(info);
               ninfo1.unset_isTypeSecondPart();
               ninfo1.unset_isTypeFirstPart();

               ninfo1.set_isTypeFirstPart();
#if 0
               printf("In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) */ \n");
#endif
               unparseType(typeof_node, ninfo1);
               ninfo1.set_isTypeSecondPart();
#if 0
               printf("In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) */ \n");
#endif

#error "DEAD CODE!"

               unparseType(typeof_node, ninfo1);
             }
        }
#endif
   }



#if 0
void
Unparse_Type::unparseQualifiedNameType ( SgType* type, SgUnparse_Info& info )
   {
  // DQ (10/11/2006): Reactivated this design of support for name qualification in ROSE
  // (I hope it works out better this time). This time we handle some internal details
  // differently and attach the qualified name list to the SgUnparse_Info so that the lower
  // level unparse functions can have the control required to generate the names more precisely.

  // DQ (12/21/2005): Added to support name qualification when explicitly stored in the AST
  // (rather than generated).  However, it appears not possible to get the qualified name
  // inbetween the "enum" and the "X" in "enum ::X", so we have to add the support for
  // qualified names more directly to the SgNamedType object to get this level of control
  // in the unparsing.

#if 1
     printf ("Error: This function should not be called, it represents the wrong approach to the design of the IR \n");
     ROSE_ASSERT(false);
#endif

     SgQualifiedNameType* qualifiedNameType = isSgQualifiedNameType(type);
     ASSERT_not_null(qualifiedNameType);

     SgUnparse_Info ninfo(info);

#if 0
     printf ("In unparseQualifiedNameType(): info.isTypeFirstPart()  = %s \n",info.isTypeFirstPart()  ? "true" : "false");
     printf ("In unparseQualifiedNameType(): info.isTypeSecondPart() = %s \n",info.isTypeSecondPart() ? "true" : "false");

     ninfo.unset_isTypeFirstPart();
     ninfo.set_isTypeSecondPart();
     unparseType(qualifiedNameType->get_base_type(),ninfo);

     if (info.isTypeFirstPart() == true)
        {
       // This is copy by value (we might do something better if ROSETTA could generate references).
          unparseQualifiedNameList(qualifiedNameType->get_qualifiedNameList());
          unparseType(qualifiedNameType->get_base_type(),info);
        }
#else
  // DQ (10/10/2006): New support for qualified names for types.
     SgType* internalType = qualifiedNameType->get_base_type();

  // Note that this modifies the list and that SgUnparse_Info& info is passed by reference.  This could be a problem!
     ninfo.set_qualifiedNameList(qualifiedNameType->get_qualifiedNameList());

     ASSERT_not_null(internalType);
  // ASSERT_not_null(ninfo.get_qualifiedNameList());

  // printf ("Processing a SgQualifiedNameType IR node base_type = %p = %s qualified name list size = %ld \n",
  //      type,type->class_name().c_str(),qualifiedNameType->get_qualifiedNameList().size());

  // Call unparseType on the base type with a modified SgUnparse_Info
     unparseType(internalType,ninfo);
#endif
   }
#endif


void Unparse_Type::unparsePointerType(SgType* type, SgUnparse_Info& info)
   {

#define DEBUG_UNPARSE_POINTER_TYPE 0

#if DEBUG_UNPARSE_POINTER_TYPE
     printf("Inside of Unparse_Type::unparsePointerType \n");
     curprint("\n/* Inside of Unparse_Type::unparsePointerType */ \n");
#endif

#if 0
     info.display("Inside of Unparse_Type::unparsePointerType");
#endif

#if DEBUG_UNPARSE_POINTER_TYPE
     printf ("In unparsePointerType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     SgPointerType* pointer_type = isSgPointerType(type);
     ASSERT_not_null(pointer_type);

#if DEBUG_UNPARSE_POINTER_TYPE
     printf ("In unparsePointerType(): isSgReferenceType(pointer_type->get_base_type())      = %s \n",(isSgReferenceType(pointer_type->get_base_type())      != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgPointerType(pointer_type->get_base_type())        = %s \n",(isSgPointerType(pointer_type->get_base_type())        != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgArrayType(pointer_type->get_base_type())          = %s \n",(isSgArrayType(pointer_type->get_base_type())          != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgFunctionType(pointer_type->get_base_type())       = %s \n",(isSgFunctionType(pointer_type->get_base_type())       != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgMemberFunctionType(pointer_type->get_base_type()) = %s \n",(isSgMemberFunctionType(pointer_type->get_base_type()) != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgModifierType(pointer_type->get_base_type())       = %s \n",(isSgModifierType(pointer_type->get_base_type())       != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgTypeOfType(pointer_type->get_base_type())         = %s \n",(isSgTypeOfType(pointer_type->get_base_type())         != NULL) ? "true" : "false");
#endif

  // DQ (3/31/2015): I think this TypeOf GNU extension needs to be supported as a special case.
  // if (isSgTypeOfType(pointer_type->get_base_type()) != NULL && (info.isTypeFirstPart() == true) )
     if (isSgTypeOfType(pointer_type->get_base_type()) != NULL)
        {
#if DEBUG_UNPARSE_POINTER_TYPE
          printf ("############### Warning: unparsePointerType(): pointer_type->get_base_type() is SgTypeOfType \n");
#endif
          if (info.isTypeFirstPart() == true)
             {
               SgUnparse_Info ninfo1(info);
               ninfo1.unset_isTypeSecondPart();
               ninfo1.unset_isTypeFirstPart();

               ninfo1.set_isTypeFirstPart();
#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) */ \n");
#endif
               unparseType(pointer_type->get_base_type(), ninfo1);
               ninfo1.set_isTypeSecondPart();
#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) */ \n");
#endif
               unparseType(pointer_type->get_base_type(), ninfo1);

               curprint("*");
#if DEBUG_UNPARSE_POINTER_TYPE
               curprint(" /* unparsePointerType(): typeof: first part */ ");
#endif
             }
            else
             {
#if DEBUG_UNPARSE_POINTER_TYPE
               printf ("TypeofType not output because info.isTypeFirstPart() == false \n");
#endif
            // DQ (4/19/2015): We need to output the typeof operator when it appears as a parameter in function type arguments.
            // See test2015_110.c for an example.
               if (info.isTypeFirstPart() == false && info.isTypeSecondPart() == false)
                  {
#if DEBUG_UNPARSE_POINTER_TYPE
                    printf ("info.isTypeFirstPart() == false && info.isTypeSecondPart() == false (need to output typeof type) \n");
#endif
                    SgUnparse_Info ninfo1(info);
                    ninfo1.unset_isTypeSecondPart();
                    ninfo1.unset_isTypeFirstPart();

                    unparseType(pointer_type->get_base_type(), ninfo1);

                    curprint("*");
#if DEBUG_UNPARSE_POINTER_TYPE
                    curprint(" /* unparsePointerType(): typeof: first and second part false */ ");
#endif
                  }
             }

          return;
        }

  /* special cases: ptr to array, int (*p) [10] */
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */

     if (isSgReferenceType(pointer_type->get_base_type()) ||
         isSgPointerType(pointer_type->get_base_type()) ||
         isSgArrayType(pointer_type->get_base_type()) ||
         isSgFunctionType(pointer_type->get_base_type()) ||
         isSgMemberFunctionType(pointer_type->get_base_type()) ||

      // DQ (1/8/2014): debugging test2014_25.c.
      // isSgModifierType(pointer_type->get_base_type()) ||

         false)
        {
#if DEBUG_UNPARSE_POINTER_TYPE
          printf ("In unparsePointerType(): calling info.set_isPointerToSomething() \n");
#endif
          info.set_isPointerToSomething();
        }

  // If not isTypeFirstPart nor isTypeSecondPart this unparse call
  // is not controlled from the statement level but from the type level

#if OUTPUT_DEBUGGING_UNPARSE_INFO
  // curprint ( "\n/* " + info.displayString("unparsePointerType") + " */ \n";
#endif

     if (info.isTypeFirstPart() == true)
        {
#if DEBUG_UNPARSE_POINTER_TYPE
          curprint("\n /* Calling unparseType from unparsePointerType (1st part) */ \n");
#endif
       // DQ (5/3/2013): The base type can not be unparsed if this is part of a list of types in a SgForInitStmt.
       // Here the SkipBaseType() flag is set and it must be respected in the unparsing.
       // unparseType(pointer_type->get_base_type(), info);
          if (info.SkipBaseType() == false)
             {
#if 0
               printf ("In unparsePointerType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparsePointerType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

               unparseType(pointer_type->get_base_type(), info);
             }
#if 0
          curprint("\n /* DONE: Calling unparseType from unparsePointerType (1st part) */ \n");
#endif
       // DQ (9/21/2004): Moved this conditional into this branch (to fix test2004_93.C)
       // DQ (9/21/2004): I think we can assert this, and if so we can simplify the logic below
          ROSE_ASSERT(info.isTypeSecondPart() == false);
       // if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )

#if DEBUG_UNPARSE_POINTER_TYPE
          printf ("info.isWithType()   = %s \n",info.isWithType()   ? "true" : "false");
          printf ("info.SkipBaseType() = %s \n",info.SkipBaseType() ? "true" : "false");
          curprint ( string("\n/* info.isWithType()           = ") + (info.isWithType()   ? "true" : "false") + " */ \n");
          curprint ( string("\n/* info.SkipBaseType()         = ") + (info.SkipBaseType() ? "true" : "false") + " */ \n");
          curprint ( string("\n/* info.isPointerToSomething() = ") + (info.isPointerToSomething() ? "true" : "false") + " */ \n");
#endif

#if 0
       // DQ (9/21/2004): This is currently an error!
          if ( info.isWithType() && info.SkipBaseType() )
             {
            // Do nothing (don't unparse the "*" using the same logic for which we don't unparse the type name)
            // curprint ( "\n/* $$$$$ In unparsePointerType: Do Nothing Case for output of type $$$$$  */ \n";
             }
            else
#error "DEAD CODE!"
             {
            // curprint ( "\n/* $$$$$ In unparsePointerType: Unparse Type Case for output of type $$$$$  */ \n";
            // curprint ( "* /* pointer */ ";
               curprint ( "*");
             }
#else
       // if (info.SkipDefinition() == true)
          curprint("*");
       // curprint(" /* unparsePointerType(): first part */ ");
#endif
        }
       else
        {
          if (info.isTypeSecondPart() == true)
             {
#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): unparse 2nd part of type \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): unparse 2nd part of type */ \n");
#endif
               unparseType(pointer_type->get_base_type(), info);

#if DEBUG_UNPARSE_POINTER_TYPE
               printf("DONE: Unparse_Type::unparsePointerType(): unparse 2nd part of type \n");
               curprint("\n/* DONE: Unparse_Type::unparsePointerType(): unparse 2nd part of type */ \n");
#endif
             }
            else
             {
            // DQ (11/27/2004): I think that this is important for unparing functions or function pointers
               SgUnparse_Info ninfo(info);
               ninfo.set_isTypeFirstPart();

#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): (call on base type: part 1) */ \n");
#endif
               unparseType(pointer_type, ninfo);
               ninfo.set_isTypeSecondPart();

#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): (call on base type: part 2) */ \n");
#endif
               unparseType(pointer_type, ninfo);
             }
        }

#if DEBUG_UNPARSE_POINTER_TYPE
     printf("Leaving of Unparse_Type::unparsePointerType \n");
     curprint("\n /* Leaving of Unparse_Type::unparsePointerType */ \n");
#endif
   }


void Unparse_Type::unparseMemberPointerType(SgType* type, SgUnparse_Info& info)
   {
     SgPointerMemberType* mpointer_type = isSgPointerMemberType(type);
     ASSERT_not_null(mpointer_type);

#define DEBUG_MEMBER_POINTER_TYPE 0
#define CURPRINT_MEMBER_POINTER_TYPE 0

#if DEBUG_MEMBER_POINTER_TYPE || 0
     printf ("In unparseMemberPointerType: mpointer_type = %p \n",mpointer_type);
#endif

#if 0
     printf ("In unparseMemberPointerType: info.inTypedefDecl() = %s \n",info.inTypedefDecl() ? "true" : "false");
     printf ("In unparseMemberPointerType: info.inArgList()     = %s \n",info.inArgList() ? "true" : "false");
#endif

  // plain type :  int (P::*)
  // type with name:  int P::* pmi = &X::a;
  // use: obj.*pmi=7;
     SgType *btype = mpointer_type->get_base_type();
     SgMemberFunctionType *ftype = NULL;

#if DEBUG_MEMBER_POINTER_TYPE
     printf ("In unparseMemberPointerType(): btype = %p = %s \n",btype,(btype != NULL) ? btype->class_name().c_str() : "NULL" );
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
     curprint("\n/* In unparseMemberPointerType() */ \n");
#endif

  // if ( (ftype = isSgMemberFunctionType(btype)) != NULL)
     ftype = isSgMemberFunctionType(btype);
#if 0
     printf ("In unparseMemberPointerType(): ftype = %p \n",ftype);
#endif
     if (ftype != NULL)
        {
       // pointer to member function data
#if DEBUG_MEMBER_POINTER_TYPE
          printf ("In unparseMemberPointerType(): pointer to member function \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
          curprint("\n/* In unparseMemberPointerType(): pointer to member function */ \n");
#endif
          if (info.isTypeFirstPart())
             {
#if DEBUG_MEMBER_POINTER_TYPE
               printf ("In unparseMemberPointerType(): pointer to member function:  first part of type \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE
               printf ("In unparseMemberPointerType(): pointer to member function: unparse return type \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
               curprint("\n/* In unparseMemberPointerType(): pointer to member function: first part of type */ \n");
#endif

            // DQ (4/28/2019): Adding name qualification to the base type unparsing for the SgPointerMemberType when it is a member function pointer.
               SgName nameQualifierForBaseType = mpointer_type->get_qualified_name_prefix_for_base_type();

#if DEBUG_MEMBER_POINTER_TYPE || 0
               printf ("In unparseMemberPointerType(): pointer to member data: nameQualifierForBaseType = %s \n",nameQualifierForBaseType.str());
#endif

            // DQ (4/21/2019): Output the associated name qualification.
               curprint(nameQualifierForBaseType);

            // DQ (1/20/2019): Suppress the definition (for enum, function, and class types).
            // unparseType(ftype->get_return_type(), info); // first part
               SgUnparse_Info ninfo(info);
               ninfo.set_SkipDefinition();

            // DQ (5/19/2019): If there was name qualification, then we didn't need the class specifier (and it would be put in the wrong place anyway).
            // SgUnparse_Info ninfo(info);
            // I don't like that we are checking the name qualificaiton string here.
               if (nameQualifierForBaseType.is_null() == false)
                  {
                 // DQ (5/18/2019): when in the SgAggregateInitializer, don't output the class specifier.
                    ninfo.set_SkipClassSpecifier();
#if 0
                    curprint ( "\n/* set SkipClassSpecifier */ \n");
#endif
                  }

               unparseType(ftype->get_return_type(), ninfo); // first part

#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
               curprint("\n/* In unparseMemberPointerType(): pointer to member function: DONE unparse return type */ \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE
               printf ("In unparseMemberPointerType(): pointer to member function: DONE unparse return type \n");
#endif

            // DQ (4/27/2019): Fixing up the function pointer handling to match the data member pointer handling.
            // curprint ("(");
#if 0
               printf ("Leading paren: info.inTypedefDecl() = %s \n",info.inArgList() ? "true" : "false");
               printf ("Leading paren: info.inArgList() = %s \n",info.inArgList() ? "true" : "false");
#endif
            // DQ (2/3/2019): Suppress parenthesis (see Cxx11_tests/test2019_76.C)
            // Not clear yet where this was required in the first place.
#if 1
            // DQ (4/27/2019): I think we always need this syntax for pointer to member functions.
               curprint ("(");
#else
            // if ( info.inTypedefDecl() == true)
               if ( info.inTypedefDecl() == true || info.inArgList() == true)
                  {
#error "DEAD CODE!"
                 // curprint(" /* leading paren */ ");
                    curprint ("(");
                  }
                 else
                  {
                    curprint(" /* leading paren not output */ ");
                  }
#endif

#if DEBUG_MEMBER_POINTER_TYPE || 0
               printf ("In unparseMemberPointerType(): pointer to member function: info.get_reference_node_for_qualification() = %p \n",info.get_reference_node_for_qualification());
#endif


#if 0
            // DQ (4/10/2019): Need to introduce name qualification from the SgInitalizedName.  However, this needs 
            // to use the qualified name for the SgInitalizedName instead of for the SgInitalizedName's type.
               if (info.get_reference_node_for_qualification() != NULL)
                  {
#if DEBUG_MEMBER_POINTER_TYPE || 0
                    printf ("In unparseMemberPointerType(): pointer to member function: info.get_reference_node_for_qualification() = %p = %s \n",
                         info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif

                 // DQ (4/16/2019): The reference node can be only either:
                 //    1) SgTypedefDeclaration
                 //    2) SgTemplateArgument
                 //    3) SgTypeIdOp
                 //    4) SgNewExp
                 //    5) SgSizeOfOp
                 //    6) SgCastExp
                 //    7) SgInitializedName

#error "DEAD CODE!"
                    SgName nameQualifier;
                    SgNode* referenceNode = info.get_reference_node_for_qualification();
                    ASSERT_not_null(referenceNode);
                    switch (referenceNode->variantT())
                       {
                         case V_SgInitializedName:
                            {
                              SgInitializedName* xxx = isSgInitializedName(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgTypedefDeclaration:
                            {
                              SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(referenceNode);
                              ASSERT_not_null(typedefDeclaration);
                              nameQualifier = typedefDeclaration->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgTemplateArgument:
                            {
                              SgTemplateArgument* xxx = isSgTemplateArgument(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

#error "DEAD CODE!"
                         case V_SgTypeIdOp:
                            {
                              SgTypeIdOp* xxx = isSgTypeIdOp(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgNewExp:
                            {
                              SgNewExp* xxx = isSgNewExp(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgSizeOfOp:
                            {
                              SgSizeOfOp* xxx = isSgSizeOfOp(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgCastExp:
                            {
                              SgCastExp* xxx = isSgCastExp(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

#error "DEAD CODE!"
                         default:
                            {
                           // DQ (4/11/2019): NOTE: In the testRoseHeaders_03.C this can be a SgCastExp.
                           // And in testRoseHeaders_05.C this can be a SgCastExp, SgTemplateFunctionDeclaration, or SgTemplateMemberFunctionDeclaration
#if 0
                              printf ("NOTE: In unparseMemberPointerType(): default case reached: info.get_reference_node_for_qualification() = %p = %s \n",
                                   info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
#error "DEAD CODE!"

#if 0
                 // DQ (4/10/2019): Handling pointer to member types.
                 // SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
                 // SgInitializedName* variable = isSgInitializedName(info.get_reference_node_for_qualification());
                 // ASSERT_not_null(variable);
                    SgInitializedName* variable = isSgInitializedName(info.get_reference_node_for_qualification());
                    SgName nameQualifier;
                 // ASSERT_not_null(variable);
                    if (variable != NULL)
                       {
                         nameQualifier = variable->get_qualified_name_prefix();
                       }
                      else
                       {
#if 0
                      // DQ (4/10/2019): In test2019_326.C this is a SgTypedefDeclaration.
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(info.get_reference_node_for_qualification());
                         ASSERT_not_null(typedefDeclaration);
#error "DEAD CODE!"
                         nameQualifier = typedefDeclaration->get_qualified_name_prefix();
#if 0
                         printf ("ERROR: not a SgInitializedName: info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
                         ASSERT_not_null(variable);
#endif
#else

                      // DQ (4/10/2019): In test2019_326.C this is a SgTypedefDeclaration.
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(info.get_reference_node_for_qualification());
                      // ASSERT_not_null(typedefDeclaration);
                         if (typedefDeclaration != NULL)
                            {
#error "DEAD CODE!"
                              ASSERT_not_null(typedefDeclaration);
                              nameQualifier = typedefDeclaration->get_qualified_name_prefix();
                            }
                           else
                            {
                           // DQ (4/10/2019): Cxx11_tests/test2016_01.C demonstrates this can be a SgTemplateArgument.
                              SgTemplateArgument* templateArgument = isSgTemplateArgument(info.get_reference_node_for_qualification());
                              if (templateArgument != NULL)
                                 {
                                   nameQualifier = templateArgument->get_qualified_name_prefix();
#error "DEAD CODE!"
                                 }
                                else
                                 {
                                   SgTypeIdOp* typeIdOp = isSgTypeIdOp(info.get_reference_node_for_qualification());
                                   if (typeIdOp != NULL)
                                      {
                                        nameQualifier = typeIdOp->get_qualified_name_prefix();
                                      }
                                     else
                                      {
#error "DEAD CODE!"
                                     // DQ (4/11/2019): NOTE: In the testRoseHeaders_03.C this can be a SgCastExp.
                                     // And in testRoseHeaders_05.C this can be a SgCastExp, SgTemplateFunctionDeclaration, or SgTemplateMemberFunctionDeclaration
#if 1
                                        printf ("NOTE: In unparseMemberPointerType(): not a SgInitializedName, SgTypedefDeclaration, SgTemplateArgument, or SgTypeIdOp: info.get_reference_node_for_qualification() = %p = %s \n",
                                             info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }
                            }
#endif
                       }
#endif
                 // SgName nameQualifier = info.get_reference_node_for_qualification()->get_qualified_name_prefix();
                 // SgName nameQualifier = variable->get_qualified_name_prefix();

#error "DEAD CODE!"

#if DEBUG_MEMBER_POINTER_TYPE
                    printf ("nameQualifier (from xxx->get_qualified_name_prefix() function) = %s \n",nameQualifier.str());
#endif
                    curprint(nameQualifier.str());
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif

#error "DEAD CODE!"

                  }
#else
            // DQ (4/27/2019): Actually where this is a valid pointer we will have used it to get the fully
            // generated string of the type in unparseType() and we will not have called this function.
            // DQ (4/20/2019): This should be a valid pointer, but we will not be using it.
            // ASSERT_not_null(info.get_reference_node_for_qualification());

            // DQ (4/20/2019): Get the name qualification directly using the SgPointerMemberType.
               SgName nameQualifier = mpointer_type->get_qualified_name_prefix_for_class_of();
               curprint(nameQualifier.str());

#if DEBUG_MEMBER_POINTER_TYPE
               printf ("nameQualifier (from xxx->get_qualified_name_prefix_for_class_of() function) = %s \n",nameQualifier.str());
#endif
#endif


#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
               curprint (" /* Calling get_type_name() */ ");
#endif
            // curprint ( "\n/* mpointer_type->get_class_of() = " + mpointer_type->get_class_of()->sage_class_name() + " */ \n";
               curprint ( get_type_name(mpointer_type->get_class_type()) );
#if DEBUG_MEMBER_POINTER_TYPE
               printf ("In unparseMemberPointerType(): pointer to member function: mpointer_type->get_class_type()                = %s \n",mpointer_type->get_class_type()->class_name().c_str());
               printf ("In unparseMemberPointerType(): pointer to member function: get_type_name(mpointer_type->get_class_type()) = %s \n",get_type_name(mpointer_type->get_class_type()).c_str());
#endif
               curprint ( "::*");
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
#if DEBUG_MEMBER_POINTER_TYPE
                    printf ("In unparseMemberPointerType(): pointer to member function data: second part of type \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
                    curprint("\n/* In unparseMemberPointerType(): pointer to member function data: second part of type */ \n");
#endif
                    curprint(")");

                 // argument list
                    SgUnparse_Info ninfo(info);
                    ninfo.unset_SkipBaseType();
                    ninfo.unset_isTypeSecondPart();
                    ninfo.unset_isTypeFirstPart();

                 // AAAAAAAAAAAAAAAAA
                 // DQ (4/27/2019): Need to unset the reference node used as a key to lookup the generated string, 
                 // else the pointer to member syntax will be output for each function argument type).
                 // ROSE_ASSERT(ninfo.get_reference_node_for_qualification() == NULL);
                    ninfo.set_reference_node_for_qualification(NULL);
                    ROSE_ASSERT(ninfo.get_reference_node_for_qualification() == NULL);
                 // AAAAAAAAAAAAAAAAA

#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
                    curprint("\n/* In unparseMemberPointerType(): start of argument list */ \n");
#endif
                    curprint("(");

                    SgTypePtrList::iterator p = ftype->get_arguments().begin();
                    while ( p != ftype->get_arguments().end() )
                       {
#if DEBUG_MEMBER_POINTER_TYPE
                         printf ("In unparseMemberPointerType: output the arguments *p = %p = %s \n",*p,(*p)->class_name().c_str());
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
                         curprint("\n/* In unparseMemberPointerType(): output function argument type */ \n");
#endif
                      // DQ (1/20/2019): Supress the definition (for enum, function, and class types.
                      // unparseType(*p, ninfo);
                         SgUnparse_Info ninfo2(ninfo);
                         ninfo2.set_SkipDefinition();

                         unparseType(*p, ninfo2);
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
                         curprint("\n/* In unparseMemberPointerType(): DONE: output function argument type */ \n");
#endif
                         p++;
                         if (p != ftype->get_arguments().end()) { curprint ( ", "); }
                       }
                    curprint(")");
                 // curprint("\n/* In unparseMemberPointerType(): end of argument list */ \n";

                    unparseType(ftype->get_return_type(), info); // second part
#if 0
                    printf ("In unparseMemberPointerType(): after unparseType() second part: unparse modifiers \n");
#endif
#if 0
                  // DQ (1/11/2020): This is the old code!
                     if (ftype->get_ref_qualifiers() == 1) {
                       curprint(" &");
                     } else if (ftype->get_ref_qualifiers() == 2) {
                       curprint(" &&");
                     }
#endif
                 // Liao, 2/27/2009, add "const" specifier to fix bug 327
                    if (ftype->isConstFunc())
                       {
                         curprint(" const ");
                       }

                 // DQ (1/11/2020): Adding support for volatile.
                    if (ftype->isVolatileFunc())
                       {
                         curprint(" volatile ");
                       }

                 // DQ (1/11/2020): Adding support for lvalue reference member function modifiers.
                    if (ftype->isLvalueReferenceFunc())
                       {
                         curprint(" &");
                       }

                 // DQ (1/11/2020): Adding support for rvalue reference member function modifiers.
                    if (ftype->isRvalueReferenceFunc())
                       {
                         curprint(" &&");
                       }
                  }
                 else
                  {
                 // not called from statement level (not sure where this is used, but it does show up in Kull)
                 // printf ("What is this 3rd case of neither 1st part nor 2nd part \n");
#if DEBUG_MEMBER_POINTER_TYPE
                    printf ("In unparseMemberPointerType(): pointer to member function data: neither first not second part of type??? \n");
#endif
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
#if DEBUG_MEMBER_POINTER_TYPE || 0
          printf ("In unparseMemberPointerType(): pointer to member data \n");
#endif
          if (info.isTypeFirstPart())
             {
#if DEBUG_MEMBER_POINTER_TYPE || 0
               printf ("In unparseMemberPointerType(): pointer to member data: first part of type \n");
#endif
            // DQ (9/16/2004): This appears to be an error, btype should not be unparsed here (of maybe btype is not set properly)!
            // printf ("Handling the first part \n");
#if 0
               curprint ( "\n/* start of btype (before name qualification) */ \n");
#endif
#if 0
               curprint ( "\n/* here is where the name qualification goes */ \n");
#endif
#if 0
               if (info.get_reference_node_for_qualification() != NULL)
                  {
                    printf ("info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
                  }
#endif
               SgName nameQualifierForBaseType = mpointer_type->get_qualified_name_prefix_for_base_type();
#if 0
               printf ("In unparseMemberPointerType(): pointer to member data: nameQualifierForBaseType = %s \n",nameQualifierForBaseType.str());
#endif

            // DQ (4/21/2019): Output the associated name qualification.
               curprint(nameQualifierForBaseType);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif

#if 0
               curprint ( "\n/* start of btype (after name qualification) */ \n");
#endif

            // DQ (5/19/2019): If there was name qualification, then we didn't need the class specifier (and it would be put in the wrong place anyway).
               SgUnparse_Info ninfo(info);
            // I don't like that we are checking the name qualificaiton string here.
               if (nameQualifierForBaseType.is_null() == false)
                  {
                 // DQ (5/18/2019): when in the SgAggregateInitializer, don't output the class specifier.
                    ninfo.set_SkipClassSpecifier();
#if 0
                    curprint ( "\n/* set SkipClassSpecifier */ \n");
#endif
                  }

            // unparseType(btype, info);
               unparseType(btype, ninfo);
#if 0
               curprint ( "\n/* end of btype */ \n");
#endif
#if 0
               printf ("Leading paren: info.inTypedefDecl() = %s \n",info.inTypedefDecl() ? "true" : "false");
               printf ("Leading paren: info.inArgList()     = %s \n",info.inArgList() ? "true" : "false");
#endif
            // DQ (2/3/2019): Suppress parenthesis (see Cxx11_tests/test2019_76.C)
            // Not clear yet where this was required in the first place.
            // curprint ( "(");
            // if ( info.inTypedefDecl() == true)
               if ( info.inTypedefDecl() == true || info.inArgList() == true)
                  {
#if 0
                    curprint(" /* leading paren */ ");
#endif
                    curprint ("(");
                  }

            // DQ (3/31/2019): Need to unparse the name qualification for the class used in the pointer member type.
#if 0
            // DQ (3/31/2019): I think this is where the added name qualification for the associated class must be output.
               printf ("Additional name qualification for the associated class_type must be output here (inbetween the base type and the class type) \n");
#endif

#define DEBUG_UNPARSE_POINTER_MEMBER_TYPE 0

#if 0
               if (info.get_reference_node_for_qualification() != NULL)
                  {
#if DEBUG_UNPARSE_POINTER_MEMBER_TYPE
                    printf ("info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif

#error "DEAD CODE!"

                 // DQ (4/16/2019): The reference node can be only either:
                 //    1) SgTypedefDeclaration
                 //    2) SgTemplateArgument
                 //    3) SgTypeIdOp
                 //    4) SgNewExp
                 //    5) SgSizeOfOp
                 //    6) SgCastExp
                 //    7) SgInitializedName

                    SgName nameQualifier;
                    SgNode* referenceNode = info.get_reference_node_for_qualification();
                    ASSERT_not_null(referenceNode);
                    switch (referenceNode->variantT())
                       {
                         case V_SgInitializedName:
                            {
                              SgInitializedName* xxx = isSgInitializedName(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgTypedefDeclaration:
                            {
                              SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(referenceNode);
                              ASSERT_not_null(typedefDeclaration);
                              nameQualifier = typedefDeclaration->get_qualified_name_prefix();
                              break;
                            }

#error "DEAD CODE!"

                         case V_SgTemplateArgument:
                            {
                              SgTemplateArgument* xxx = isSgTemplateArgument(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgTypeIdOp:
                            {
                              SgTypeIdOp* xxx = isSgTypeIdOp(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgNewExp:
                            {
                              SgNewExp* xxx = isSgNewExp(referenceNode);
                              ASSERT_not_null(xxx);
                           // nameQualifier = xxx->get_qualified_name_prefix();
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

#error "DEAD CODE!"

                         case V_SgSizeOfOp:
                            {
                              SgSizeOfOp* xxx = isSgSizeOfOp(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

                         case V_SgCastExp:
                            {
                              SgCastExp* xxx = isSgCastExp(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

#error "DEAD CODE!"

                      // DQ (4/18/2019): Now the we have to traverse chains of types where there can
                      // be SgPointerMemberType types, we have to handle this as a rererenceNode.
                         case V_SgPointerMemberType:
                            {
                              SgPointerMemberType* xxx = isSgPointerMemberType(referenceNode);
                              ASSERT_not_null(xxx);
                              nameQualifier = xxx->get_qualified_name_prefix();
                              break;
                            }

#error "DEAD CODE!"

                         default:
                            {
                           // DQ (4/11/2019): NOTE: In the testRoseHeaders_03.C this can be a SgCastExp.
                           // And in testRoseHeaders_05.C this can be a SgCastExp, SgTemplateFunctionDeclaration, or SgTemplateMemberFunctionDeclaration
#if 0
                              printf ("NOTE: In unparseMemberPointerType(): default case reached: info.get_reference_node_for_qualification() = %p = %s \n",
                                   info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }

#if 0
                 // DQ (4/10/2019): We need to get the name qualification for the varialbe name from the SgInitializedName.
                 // We don't want the name qualification for the SgInitializedName's type.
                 // SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
                    SgInitializedName* variable = isSgInitializedName(info.get_reference_node_for_qualification());
                    SgName nameQualifier;
                 // ASSERT_not_null(variable);
                    if (variable != NULL)
                       {
                         nameQualifier = variable->get_qualified_name_prefix();
                       }
                      else
                       {
                      // DQ (4/10/2019): In test2019_326.C this is a SgTypedefDeclaration.
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(info.get_reference_node_for_qualification());
                      // ASSERT_not_null(typedefDeclaration);
                         if (typedefDeclaration != NULL)
                            {
                              ASSERT_not_null(typedefDeclaration);
                              nameQualifier = typedefDeclaration->get_qualified_name_prefix();
#error "DEAD CODE!"
                            }
                           else
                            {
                           // DQ (4/10/2019): Cxx11_tests/test2016_01.C demonstrates this can be a SgTemplateArgument.
                              SgTemplateArgument* templateArgument = isSgTemplateArgument(info.get_reference_node_for_qualification());
                              if (templateArgument != NULL)
                                 {
                                   nameQualifier = templateArgument->get_qualified_name_prefix();
                                 }
                                else
                                 {
                                   SgTypeIdOp* typeIdOp = isSgTypeIdOp(info.get_reference_node_for_qualification());
                                   if (typeIdOp != NULL)
                                      {
                                        nameQualifier = typeIdOp->get_qualified_name_prefix();
                                      }
                                     else
                                      {
#if 0
                                        printf ("NOTE: In unparseMemberPointerType(): not a SgInitializedName, SgTypedefDeclaration, SgTemplateArgument, or SgTypeIdOp: info.get_reference_node_for_qualification() = %p = %s \n",
                                             info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }
                            }
#if 0
                         printf ("ERROR: unknown IR node with type reference: info.get_reference_node_for_qualification() = %p = %s \n",
                              info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
                         ASSERT_not_null(variable);
#endif
                       }
#endif

#error "DEAD CODE!"

#if DEBUG_UNPARSE_POINTER_MEMBER_TYPE
                    printf ("nameQualifier (from xxx->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
#endif
                    curprint(nameQualifier.str());
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif

#error "DEAD CODE!"

                  }
#else
            // DQ (4/20/2019): Actually where this is a valid pointer we will have used it to get the fully
            // generated string of the type in unparseType() and we will not have called this function.
            // DQ (4/20/2019): This should be a valid pointer, but we will not be using it.
            // ASSERT_not_null(info.get_reference_node_for_qualification());

            // DQ (4/20/2019): Get the name qualification directly using the SgPointerMemberType.
               SgName nameQualifier = mpointer_type->get_qualified_name_prefix_for_class_of();
               curprint(nameQualifier.str());
#endif

#if 0
               curprint ( "\n/* calling get_type_name */ \n");
#endif
               curprint ( get_type_name(mpointer_type->get_class_type()) );
               curprint ( "::*");
#if 0
               curprint ( "\n/* DONE: calling get_type_name */ \n");
#endif
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
#if DEBUG_MEMBER_POINTER_TYPE || 0
                //  printf ("In unparseMemberPointerType(): Handling the second part \n");
                    printf ("In unparseMemberPointerType(): pointer to member data: second part of type \n");
#endif
#if 0
                    curprint ( "\n/* start of second type part processing */ \n");
#endif
#if 0
                    printf ("Trailing paren: info.inTypedefDecl() = %s \n",info.inTypedefDecl() ? "true" : "false");
                    printf ("Trailing paren: info.inArgList() = %s \n",info.inArgList() ? "true" : "false");
#endif
                // DQ (2/3/2019): Suppress parenthesis (see Cxx11_tests/test2019_76.C)
                 // curprint(")");
                 // if ( info.inTypedefDecl() == true)
                 // if ( info.inTypedefDecl() == true || info.inArgList() == true)
                    if ( info.inTypedefDecl() == true || info.inArgList() == true)
                       {
#if 0
                         curprint(" /* trailing paren */ ");
#endif
                         curprint(")");
                       }
                      else
                       {
#if 0
                         curprint(" /* skip output of trailing paren */ ");
#endif
                       }

                 // DQ (8/19/2014): Handle array types (see test2014_129.C).
                    SgArrayType* arrayType = isSgArrayType(btype);
                    if (arrayType != NULL)
                       {
#if DEBUG_MEMBER_POINTER_TYPE
                         printf ("In unparseMemberPointerType(): Handling the array type \n");
#endif
                         SgUnparse_Info ninfo(info);
                         curprint("[");
                         unp->u_exprStmt->unparseExpression(arrayType->get_index(),ninfo);
                         curprint("]");
                       }
                  }
                 else
                  {
                 // printf ("What is this 3rd case of neither 1st part nor 2nd part \n");
#if 0
                    printf ("What is this 3rd case! \n");
#endif
#if 0
                    curprint ( "\n/* What is this 3rd case! */ \n");
#endif
                    SgUnparse_Info ninfo(info);
                    ninfo.set_isTypeFirstPart();
                    unparseType(mpointer_type, ninfo);
                    ninfo.set_isTypeSecondPart();
                    unparseType(mpointer_type, ninfo);
                  }
             }
        }

#if DEBUG_MEMBER_POINTER_TYPE || CURPRINT_MEMBER_POINTER_TYPE || 0
     printf ("Leaving unparseMemberPointerType() \n");
     curprint("\n/* Leaving unparseMemberPointerType() */ \n");
#endif
   }


void Unparse_Type::unparseReferenceType(SgType* type, SgUnparse_Info& info)
   {
     SgReferenceType* ref_type = isSgReferenceType(type);
     ASSERT_not_null(ref_type);

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

void Unparse_Type::unparseRvalueReferenceType(SgType* type, SgUnparse_Info& info)
   {
     SgRvalueReferenceType* rvalue_ref_type = isSgRvalueReferenceType(type);
     ASSERT_not_null(rvalue_ref_type);

  /* special cases: ptr to array, int (*p) [10] */
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */
     SgUnparse_Info ninfo(info);

     if (isSgReferenceType(rvalue_ref_type->get_base_type()) ||
         isSgPointerType(rvalue_ref_type->get_base_type()) ||
         isSgArrayType(rvalue_ref_type->get_base_type()) ||
         isSgFunctionType(rvalue_ref_type->get_base_type()) ||
         isSgMemberFunctionType(rvalue_ref_type->get_base_type()) ||
         isSgModifierType(rvalue_ref_type->get_base_type()) )
        {
          ninfo.set_isReferenceToSomething();
        }

     if (ninfo.isTypeFirstPart())
        {
          unparseType(rvalue_ref_type->get_base_type(), ninfo);
          curprint ( "&&");
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
               unparseType(rvalue_ref_type->get_base_type(), ninfo);
             }
            else
             {
               SgUnparse_Info ninfo2(ninfo);
               ninfo2.set_isTypeFirstPart();
               unparseType(rvalue_ref_type, ninfo2);
               ninfo2.set_isTypeSecondPart();
               unparseType(rvalue_ref_type, ninfo2);
             }
        }
   }

#if 0
void Unparse_Type::unparseNameType(SgType* type, SgUnparse_Info& info)
   {
  // DQ (10/7/2004): This should not exist! anything that is a SgNamedType is handled by the unparse
  // functions for the types that are derived from the SgNamedType (thus this function should not be here)

     printf ("Error: It should be impossible to call this unparseNameType() function (except directly which should not be done!) \n");
     ROSE_ASSERT(false);

     SgNamedType* named_type = isSgNamedType(type);
     ASSERT_not_null(named_type);

#error "DEAD CODE!"

     if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
    /* do nothing */;
       else
        {
       // DQ (10/15/2004): Output the qualified name instead of the name (I think) Opps! this function is never called!
       // curprint ( named_type->get_name().str() + " ";
          printf ("In unparseNameType(): named_type->get_qualified_name() = %s \n",named_type->get_qualified_name().str());
          curprint ( named_type->get_qualified_name().str() + " ");
        }
   }
#endif


void
Unparse_Type::unparseClassType(SgType* type, SgUnparse_Info& info)
   {

#define DEBUG_UNPARSE_CLASS_TYPE 0

#if DEBUG_UNPARSE_CLASS_TYPE
     printf ("\nInside of Unparse_Type::unparseClassType type = %p \n",type);
#endif
#if 0
     curprint("/* Inside of Unparse_Type::unparseClassType */ \n");
#endif

  // DQ (10/31/2018): Adding assertion.
  // ASSERT_not_null(info.get_current_source_file());

#if 0
     info.display("Inside of Unparse_Type::unparseClassType");
#endif

#if DEBUG_UNPARSE_CLASS_TYPE
     printf ("In unparseClassType(): TOP: ninfo.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseClassType(): TOP: ninfo.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

  // CH (4/7/2010): This issue is because of using a MSVC keyword 'cdecl' as a variable name

     SgClassType* class_type = isSgClassType(type);
     ASSERT_not_null(class_type);

  // DQ (6/22/2006): test2006_76.C demonstrates a problem with this code
  // SgClassDeclaration *cdecl = isSgClassDeclaration(class_type->get_declaration());
     SgClassDeclaration *decl = isSgClassDeclaration(class_type->get_declaration());
     ASSERT_not_null(decl);

     SgTemplateClassDeclaration *tpldecl = isSgTemplateClassDeclaration(decl);

  // DQ (7/28/2013): Added assertion.
     ROSE_ASSERT(decl == decl->get_firstNondefiningDeclaration());

#if DEBUG_UNPARSE_CLASS_TYPE
  // printf ("In Unparse_Type::unparseClassType(): decl = %p = %s \n",decl,decl->class_name().c_str());
     printf ("In Unparse_Type::unparseClassType(): class_type->get_autonomous_declaration() = %s \n",class_type->get_autonomous_declaration() ? "true" : "false");
     printf ("In Unparse_Type::unparseClassType(): decl->get_isAutonomousDeclaration()      = %s \n",decl->get_isAutonomousDeclaration() ? "true" : "false");
     printf ("In Unparse_Type::unparseClassType(): decl->get_isUnNamed()                    = %s \n",decl->get_isUnNamed() ? "true" : "false");

     SgClassDeclaration* defining_decl = isSgClassDeclaration(class_type->get_declaration()->get_definingDeclaration());
     printf ("decl = %p defining_decl = %p \n",decl,defining_decl);
     if (defining_decl != NULL)
        {
          printf ("In Unparse_Type::unparseClassType(): defining_decl->get_isAutonomousDeclaration() = %s \n",defining_decl->get_isAutonomousDeclaration() ? "true" : "false");
          printf ("In Unparse_Type::unparseClassType(): defining_decl->get_isUnNamed()               = %s \n",defining_decl->get_isUnNamed() ? "true" : "false");
        }

     printf ("In Unparse_Type::unparseClassType(): decl = %p = %s decl->get_definition() = %p \n",decl,decl->class_name().c_str(),decl->get_definition());
#endif

     if (decl->get_definition() == NULL)
        {
       // We likely have a forward declaration so get the defining declaration if it is available
       // (likely the first non-defining declaration and the forward declaration are the same).
          ASSERT_not_null(class_type->get_declaration());
          if (decl->get_definingDeclaration() != NULL)
             {
               ASSERT_not_null(decl->get_definingDeclaration());
#if DEBUG_UNPARSE_CLASS_TYPE
               printf ("In Unparse_Type::unparseClassType(): Resetting decl to be the defining declaration from decl = %p to decl = %p \n",decl,decl->get_definingDeclaration());
#endif
#if 1
            // DQ (9/23/2012): Original version of code.
               decl = isSgClassDeclaration(decl->get_definingDeclaration());
               ASSERT_not_null(decl);
               ASSERT_not_null(decl->get_definition());
#else

#error "DEAD CODE!"

            // DQ (9/23/2012): I think that we want to always using the non-defining declaration, since that is the declaration with name 
            // qualification computed for the possible template arguments. Then again, should the name qualification be attached to the 
            // template arguments (for either the defining or nondefining declaration).
               printf ("In Unparse_Type::unparseClassType(): Skipping the reset of decl = %p to be the defining declaration = %p \n",decl,decl->get_definingDeclaration());
#endif
             }
            else
             {
#if DEBUG_UNPARSE_CLASS_TYPE
               printf ("Can't find a class declaration with an attached definition! \n");
#endif
             }
        }

  // DQ (7/28/2013): Added assertion.
     ROSE_ASSERT(decl == decl->get_definingDeclaration() || decl->get_definingDeclaration() == NULL);

  // GB (09/19/2007): This is the defining declaration of the class, it might have preprocessing information attached to it.
     SgClassDeclaration *cDefiningDecl = isSgClassDeclaration(decl->get_definingDeclaration());

#if DEBUG_UNPARSE_CLASS_TYPE
     printf ("In unparseClassType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparseClassType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparseClassType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparseClassType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif
#if DEBUG_UNPARSE_CLASS_TYPE && 0
     curprint ( string("\n/* In unparseClassType: info.isTypeFirstPart()  = ") + ((info.isTypeFirstPart()  == true) ? "true" : "false") + " */ \n ");
     curprint ( string("\n/* In unparseClassType: info.isTypeSecondPart() = ") + ((info.isTypeSecondPart() == true) ? "true" : "false") + " */ \n ");
#endif

  // DQ (10/7/2006): In C (and I think C99), we need the "struct" keyword
  // in places where it is not required for C++.  See test2006_147.C.
  // if (info.isTypeFirstPart() == true)
  // if (info.isTypeFirstPart() == true || (SageInterface::is_C_language() || SageInterface::is_C99_language()) )
     if ( (info.isTypeFirstPart() == true) || (info.isTypeSecondPart() == false) )
        {
       /* print the class specifiers */
       // printf ("I think that for C++ we can skip the class specifier, where for C it is required: print the class specifiers \n");
       // curprint ( "/* I think that for C++ we can skip the class specifier, where for C it is required: info.SkipClassSpecifier() = " + (info.SkipClassSpecifier() ? "true" : "false") + " */ ";

#if 0
          info.display("In unparseClassType(): can we supress the class specifier in an initialization list?");
#endif
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
              // DQ (6/6/2007): Type elaboration goes here.
                 bool useElaboratedType = generateElaboratedType(decl,info);
                 if (useElaboratedType == true)
                    {
                      switch (decl->get_class_type())
                         {
                           case SgClassDeclaration::e_class :
                              {
                                curprint ( "class ");
                                break;
                              }
                           case SgClassDeclaration::e_struct :
                              {
                                curprint ( "struct ");
                                break;
                              }
                           case SgClassDeclaration::e_union :
                              {
                                curprint ( "union ");
                                break;
                              }
                           default:
                              {
                                printf ("Error: default reached in selection of elaborated type \n");
                                ROSE_ASSERT(false);
                                break;
                              }
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
       // DQ (11/22/2004): New code using refactored code using explicitly stored scope to compute the qualified name
       // this version should be more robust in generating correct qualified names when the parent is inconsistant
       // with the explicitly stored scope (which happens in rare cases, but particularly in KULL and for va_list
       // bases typedefed types).
#if DEBUG_UNPARSE_CLASS_TYPE
          printf ("In unparseClassType(): info.PrintName() = %s decl->get_isUnNamed() = %s \n",(info.PrintName() == true) ? "true" : "false",decl->get_isUnNamed() ? "true" : "false");
#endif
       // DQ (7/28/2012): Added support for un-named types in typedefs.
       // SgName nm = decl->get_name();
          SgName nm;

          if (decl->get_isUnNamed() == false || info.PrintName() == true)
             {
               nm = decl->get_name();

#if DEBUG_UNPARSE_CLASS_TYPE
               printf ("In unparseClassType(): nm = %s \n",nm.str());
#endif
             }
            else
             {
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

#if DEBUG_UNPARSE_CLASS_TYPE
          printf ("In unparseClassType: nm = %s \n",nm.str());
#endif
#if DEBUG_UNPARSE_CLASS_TYPE && 0
          curprint ( string("\n/* In unparseClassType: nm = ") + nm.str() + " */ \n ");
#endif
       // DQ (6/27/2006): nm.is_null() is a better test for an empty name, don't output the qualifier for un-named
       // structs.  This is part of the fix for the Red Hat 7.3 gconv problem (see ChangeLog for details).
       // if (nm.str() != NULL)
          if (nm.is_null() == false)
             {
            // if (SageInterface::is_C_language() == true)
               if (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true)
                  {
                    curprint(string(nm.str()) + " ");
                  }
                 else
                  {
#if DEBUG_UNPARSE_CLASS_TYPE && 0
                    curprint ( string("\n/* In unparseClassType: info.forceQualifiedNames() = ") + ((info.forceQualifiedNames() == true) ? "true" : "false") + " */ \n");
                 // curprint ( "\n/* cdecl->get_need_name_qualifier() = " + (cdecl->get_need_name_qualifier() == true ? "true" : "false") + " */ \n";
                    curprint ( string("\n/* decl->get_scope() = ") + decl->get_scope()->class_name() + " */\n ");
                    curprint ( string("\n/* info.get_current_namespace() = ") + ((info.get_current_namespace() != NULL) ? info.get_current_namespace()->class_name() : "no namespace in use") + " */\n ");
                    curprint ( string("\n/* info.get_declstatement_ptr() = ") + ((info.get_declstatement_ptr() != NULL) ? info.get_declstatement_ptr()->class_name() : "no declaration statement being generated") + " */\n ");
                 // curprint ( "\n/* SageInterface::get_name(info.get_declstatement_ptr()) = " + ((info.get_declstatement_ptr() != NULL) ? SageInterface::get_name(info.get_declstatement_ptr()) : "no declaration statement available") + " */\n ");
#endif

                 // info.display("In unparseClassType: The C++ support is more complex and can require qualified names");

#if DEBUG_UNPARSE_CLASS_TYPE && 0
                    curprint ( string("\n/* In unparseClassType: info.get_reference_node_for_qualification() = ") + ((info.get_reference_node_for_qualification() != NULL) ? Rose::StringUtility::numberToString(info.get_reference_node_for_qualification()) : "null") + " */ \n");
                    curprint ( string("\n/* In unparseClassType: info.get_reference_node_for_qualification() = ") + ((info.get_reference_node_for_qualification() != NULL) ? info.get_reference_node_for_qualification()->class_name() : "null") + " */ \n");
                 // curprint("\n/* In unparseFunctionType: needParen = " + StringUtility::numberToString(needParen) + " */ \n");
#endif
                 // DQ (6/25/2011): Fixing name qualifiction to work with unparseToString().  In this case we don't 
                 // have an associated node to reference as a way to lookup the strored name qualification.  In this 
                 // case we return a fully qualified name.
                    if (info.get_reference_node_for_qualification() == NULL)
                       {
#if DEBUG_UNPARSE_CLASS_TYPE
                         printf ("WARNING: In unparseClassType(): info.get_reference_node_for_qualification() == NULL (assuming this is for unparseToString() \n");
#endif
                      // DQ (3/29/2019): In reviewing where we are using the get_qualified_name() function, this
                      // might be OK since it is likely only associated with the unparseToString() function.
                         SgName nameQualifierAndType = class_type->get_qualified_name();
                         curprint(nameQualifierAndType.str());
                       }
                      else
                       {
                      // DQ (6/2/2011): Newest support for name qualification...
#if DEBUG_UNPARSE_CLASS_TYPE
                         printf ("info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif
                         SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());

#if DEBUG_UNPARSE_CLASS_TYPE
                         printf ("nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
#endif

                      // SgName nameQualifier = unp->u_name->generateNameQualifierForType( type , info );
#if DEBUG_UNPARSE_CLASS_TYPE
                         printf ("In unparseClassType: nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
#endif
#if DEBUG_UNPARSE_CLASS_TYPE && 0
                         curprint ( string("\n/* In unparseClassType: nameQualifier (from unp->u_name->generateNameQualifier function) = ") + nameQualifier + " */ \n ");
#endif
                         curprint(nameQualifier.str());

                         SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(decl);
                         if (isSgTemplateInstantiationDecl(decl) != NULL)
                            {
                           // Handle case of class template instantiation (code located in unparse_stmt.C)
#if 0
                              curprint ("/* Calling unparseTemplateName */ \n ");
#endif
#if 0
                              printf ("In unparseClassType: calling unparseTemplateName() for templateInstantiationDeclaration = %p \n",templateInstantiationDeclaration);
#endif
                              SgUnparse_Info ninfo(info);

                           // DQ (5/7/2013): This fixes the test2013_153.C test code.
                              if (ninfo.isTypeFirstPart() == true)
                                 {
#if 0
                                   printf ("In unparseClassType(): resetting isTypeFirstPart() == false \n");
#endif
                                   ninfo.unset_isTypeFirstPart();
                                 }

                              if (ninfo.isTypeSecondPart() == true)
                                 {
#if 0
                                   printf ("In unparseClassType(): resetting isTypeSecondPart() == false \n");
#endif
                                   ninfo.unset_isTypeSecondPart();
                                 }

                           // DQ (5/7/2013): I think these should be false so that the full type will be output.
                              ROSE_ASSERT(ninfo.isTypeFirstPart()  == false);
                              ROSE_ASSERT(ninfo.isTypeSecondPart() == false);

                           // unp->u_exprStmt->unparseTemplateName(templateInstantiationDeclaration,info);
                              unp->u_exprStmt->unparseTemplateName(templateInstantiationDeclaration,ninfo);
#if 0
                              curprint ("/* DONE: Calling unparseTemplateName */ \n ");
#endif
                            }
                           else
                            {
#if DEBUG_UNPARSE_CLASS_TYPE && 0
                              curprint ( string("\n/* In unparseClassType: output tag name = ") + nm.str() + " */ \n ");
#endif
                              curprint ( string(nm.str()) + " ");
#if 0
                              printf ("test 1: class type name: nm = %s \n",nm.str());
#endif
                            }
                       }
                  }
             }
            else
             {
            // DQ (12/3/2017): This is a problem for Cxx11_tests/test2017_31,C, need to debug this case.
#if DEBUG_UNPARSE_CLASS_TYPE
               printf ("info.get_use_generated_name_for_template_arguments() = %s \n",info.get_use_generated_name_for_template_arguments() ? "true" : "false");
#endif
            // DQ (4/28/2017): Where this is un-named type but we are wanting to output a name for a template argument, then we want the generated name,
            // otherwise we want the class definition to be output directly.  So I think we need to trigger this use case via the SgUnparseInfo object.
               if (info.get_use_generated_name_for_template_arguments() == true)
                  {
#if 0
                    printf ("WARNING: info.get_use_generated_name_for_template_arguments() == true \n");
#endif
                 // In this case we need to output the generated name.
#if 0
                    printf ("In unparseClassType(): Detected case of output of un-named class for use in template argument \n");
#endif
                    SgName nm = class_type->get_name();
#if 0
                    printf ("In unparseClassType(): Detected case of output of un-named class for use in template argument: nm = %s \n",nm.str());
#endif
#if 0
                    curprint(" /* in unparseClassType: output un-named type for template argument */ ");
#endif
                    curprint(string(nm.str()) + " ");
                  }
                 else
                  {
                 // DQ (10/23/2012): Added support for types of references to un-named class/struct/unions to always include their definitions.
#if 0
                    printf ("In unparseClassType: This is an un-named class declaration: decl = %p name = %s (need to output its class definition) decl->get_definition() = %p \n",
                         decl,decl->get_name().str(),decl->get_definition());
                    printf ("In unparseClassType: Detected un-named class declaration: Calling  unset_SkipClassDefinition() \n");
#endif

                 // DQ (12/3/2017): This is a problem for Cxx11_tests/test2017_31.C (but with it uncommented, C_tests/test2015_67.c does pass).
                 // DQ (4/4/2015): Comment out this to support test2015_67.c.
                 // info.unset_SkipClassDefinition();
                 // DQ (1/9/2014): Mark Enum and Class declaration handling consistantly (enforced within the unparser now).
                 // info.unset_SkipEnumDefinition();

                 // DQ (12/6/2017): Check if this is a part of a lambda capture.
                    SgClassDeclaration* parentClassDeclaration = isSgClassDeclaration(class_type->get_declaration());
                    ASSERT_not_null(parentClassDeclaration);
                    SgLambdaExp* lambdaExpresssion = isSgLambdaExp(parentClassDeclaration->get_parent());
                    if (lambdaExpresssion != NULL)
                       {
                      // In the case where this is a class representing the capture variables, we don't output the class.
                       }
                      else
                       {
                      // DQ (12/6/2017): test2005_114.C demonstrates where we need to output the class declaration even when the name is generated (the generated name will not be output).
                         info.unset_SkipClassDefinition();
                         info.unset_SkipEnumDefinition();
                       }

#if 0
                    printf ("In unparseClassType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
                    printf ("In unparseClassType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif
                 // DQ (1/9/2014): These should have been setup to be the same.
                    ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());
                  }
             }
        }

#if DEBUG_UNPARSE_CLASS_TYPE
     printf ("In unparseClassType: info.SkipClassDefinition(): test 5: = %s \n",(info.SkipClassDefinition()          == true) ? "true" : "false");
#endif
#if DEBUG_UNPARSE_CLASS_TYPE
     printf ("In unparseClassType: decl->isForward()                   = %s \n",(decl->isForward()                   == true) ? "true" : "false");
     printf ("In unparseClassType: decl->get_isUnNamed()               = %s \n",(decl->get_isUnNamed()               == true) ? "true" : "false");
     printf ("In unparseClassType: decl->get_isAutonomousDeclaration() = %s \n",(decl->get_isAutonomousDeclaration() == true) ? "true" : "false");
#endif
#if DEBUG_UNPARSE_CLASS_TYPE
     printf ("In unparseClassType(): info.isTypeFirstPart()     = %s \n",(info.isTypeFirstPart() == true) ? "true" : "false");
     printf ("In unparseClassType(): info.isTypeSecondPart()    = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
     printf ("In unparseClassType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseClassType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (7/28/2013): If this is an un-named class/struct/union then we have to put out the full definition each time (I think).
  // Note that YardenPragmaPackExample.c requires that (info.isTypeSecondPart() == false) be added.
  // if (info.isTypeFirstPart() == true)
  // if (info.isTypeFirstPart() == true || decl->get_isUnNamed() == true)
  // if (info.isTypeFirstPart() == true)
  // if ( (info.isTypeFirstPart() == true) || (info.isTypeSecondPart() == false && decl->get_isUnNamed() == true) )
  // if ( (info.isTypeFirstPart() == true) || (info.isTypeSecondPart() == false && decl->get_isAutonomousDeclaration() == false) )
     if ( (info.isTypeFirstPart() == true) || (info.isTypeSecondPart() == false && decl->get_isAutonomousDeclaration() == false && info.SkipClassDefinition() == false) )
        {
        // DQ (5/25/2019): Add this case to handle unnamed types used in variable declarations with multiple variables. 
#if 0
           printf("In unparseClassType(): Need to handle case of un-named types used with variable declarations with multiple variables \n");
           curprint("/* In unparseClassType(): Need to handle case of un-named types used with variable declarations with multiple variables */ \n");
#endif
          if ( !info.SkipClassDefinition() )
             {
            // DQ (8/17/2006): Handle the case where the definition does not exist (there may still be a pointer to the type).
               SgClassDefinition* classdefn_stmt = decl->get_definition();
#if DEBUG_UNPARSE_CLASS_TYPE
               printf ("In unparseClassType: for decl = %p = %s we want to output the class definition = %p \n",decl,decl->class_name().c_str(),classdefn_stmt);
#endif
               if (classdefn_stmt != NULL)
                  {
                    SgUnparse_Info ninfo(info);
                    ninfo.unset_SkipSemiColon();

                 // DQ (11/29/2004): Added support for saving context so that qualified names would be computed
                 // properly (using unqualified names instead of qualified names where appropriate (declarations
                 // in a class, for example)).
                    SgNamedType *saved_context = ninfo.get_current_context();

                 // DQ (6/13/2007): Set to null before resetting to non-null value
                    ninfo.set_current_context(NULL);
                    ninfo.set_current_context(class_type);

                 // DQ (6/9/2007): Set the current scope
                    ninfo.set_current_scope(NULL);
                    ninfo.set_current_scope(classdefn_stmt);

                 // curprint ( "\n/* Unparsing class definition within unparseClassType */ \n";

                 // DQ (12/26/2019): If we are supporting multiple files and named types using defining declaration 
                 // in multiple translation units, then we need to use the defining declaration that is associated 
                 // with the correct file (so that it can be unparsed).
                    if (info.useAlternativeDefiningDeclaration() == true)
                       {
                         ASSERT_not_null(info.get_declstatement_associated_with_type());

                         SgClassDeclaration* class_declstatement_associated_with_type = isSgClassDeclaration(info.get_declstatement_associated_with_type());
                         ASSERT_not_null(class_declstatement_associated_with_type);

                      // This should be a defining declaration.
                         ASSERT_not_null(class_declstatement_associated_with_type->get_definition());
#if 0
                         printf ("Reset the declaration to be used in unparsing the defining declaration (muti-file support): class_declstatement_associated_with_type = %p \n",class_declstatement_associated_with_type);
#endif
                      // decl = class_declstatement_associated_with_type;
                      // cDefiningDecl = class_declstatement_associated_with_type;
                         classdefn_stmt = class_declstatement_associated_with_type->get_definition();
                         ASSERT_not_null(classdefn_stmt);
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
#if 0
                 // DQ (1/8/2020): Identifying the location where we need to output the class hierarchy (see Cxx11_tests/test2020_26.C).
                    printf ("In unparseClassType(): Output class hierarchy here! \n");
                    curprint(" /* Output class hierarchy here! */ ");
#endif

                 // DQ (1/8/2020): Support for defining declarations with base classes (called from unparseClassDefnStmt() and unparseClassType() functions).
                 // This supports Cxx_tests/test2020_24.C.
                    ASSERT_not_null(classdefn_stmt);
                    unp->u_exprStmt->unparseClassInheritanceList (classdefn_stmt,info);

                    ninfo.set_isUnsetAccess();
                    curprint("{");
                    if (classdefn_stmt == NULL)
                       {
                         printf ("Error: In unparseClassType(): classdefn_stmt = NULL decl = %p = %s \n",decl,decl->get_name().str());
                       }
                    ASSERT_not_null(classdefn_stmt);
#if DEBUG_UNPARSE_CLASS_TYPE
                    printf ("In unparseClassType: classdefn_stmt = %p classdefn_stmt->get_members().size() = %" PRIuPTR " \n",classdefn_stmt, classdefn_stmt->get_members().size());
#endif
                    SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();
                    while (pp != classdefn_stmt->get_members().end())
                       {
                      // DQ (10/18/2012): If this is in the context of a conditional then the ";" will be supressed.
                      // We could explicitly output a ";" in this case if required.
#if DEBUG_UNPARSE_CLASS_TYPE
                         printf ("In unparseClassType: output member declaration: %p ninfo.isSkipSemiColon() = %s \n",*pp,ninfo.SkipSemiColon() ? "true" : "false");
#endif
#if DEBUG_UNPARSE_CLASS_TYPE
                         printf ("In unparseClassType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
                         printf ("In unparseClassType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition() == true) ? "true" : "false");
#endif
                      // DQ (1/9/2014): These should have been setup to be the same.
                         ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

                         unp->u_exprStmt->unparseStatement((*pp), ninfo);
                         pp++;
                       }
                 // GB (09/18/2007): If the class defining class declaration is unparsed, also unparse its attached
                 // preprocessing info. Putting the "inside" info right here is just a wild guess as to where it might
                 // really belong. Some of the preprocessing info may be in the SgClassDefinition and some in the
                 // SgClassDeclaration, so unparse both.
                    ASSERT_not_null(cDefiningDecl);
                    if (cDefiningDecl->get_definition() != NULL)
                       {
                         unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl->get_definition(), info, PreprocessingInfo::inside);
                       }
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::inside);
#if DEBUG_UNPARSE_CLASS_TYPE
                    curprint(" /* in unparseClassType: output data members */ ");
#endif
                    curprint("}");

                 // DQ (6/13/2007): Set to null before resetting to non-null value
                 // DQ (11/29/2004): Restore context saved above before unparsing declaration.
                    ninfo.set_current_context(NULL);
                    ninfo.set_current_context(saved_context);
                  }
                 else
                  {
#if DEBUG_UNPARSE_CLASS_TYPE
                    printf ("classdefn_stmt not found for decl = %p \n",decl);
#endif
                  }
               
            // GB (09/18/2007): If the class definition is unparsed, also unparse its attached preprocessing info.
               if (cDefiningDecl != NULL)
                  {
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::after);
                  }
             }
            else
             {
#if 0
            // DQ (5/25/2019): Add this case to handle unnamed types used in variable declarations with multiple variables. 
               curprint("/* In unparseClassType(): info.SkipClassDefinition() == true: Need to handle case of un-named types used with variable declarations with multiple variables */ \n");

#if DEBUG_UNPARSE_CLASS_TYPE
               printf ("In unparseClassType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseClassType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
               printf ("In unparseClassType: decl->get_isUnNamed()               = %s \n",(decl->get_isUnNamed()               == true) ? "true" : "false");
#endif
               ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());
               if (info.SkipClassDefinition() == true && decl->get_isUnNamed() == true)
                  {
                    printf ("We need to output the name only in this case \n");
                    curprint("/* In unparseClassType(): We need to output the name only in this case */ \n");
                  }
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
        }
       else
        {
#if 0
       // DQ (5/26/2019): This is fixed by not generaed strings for un-named types.
       // DQ (5/25/2019): Add this case to handle unnamed types used in variable declarations with multiple variables. 
       // if ( (info.isTypeFirstPart() == true) || (info.isTypeSecondPart() == false && decl->get_isAutonomousDeclaration() == false && info.SkipClassDefinition() == false) )
       // if ( (info.isTypeFirstPart() == true) || (info.isTypeSecondPart() == false && decl->get_isAutonomousDeclaration() == true && info.SkipClassDefinition() == true && decl->get_isUnNamed() == true) )
          if ( (info.isTypeFirstPart() == true) || (info.isTypeSecondPart() == false && info.SkipClassDefinition() == true && decl->get_isUnNamed() == true) )
             {
               printf ("We need to output the name only in this case \n");
               curprint("/* In unparseClassType(): We need to output the name only in this case */ \n");
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#endif
        }


//#endif

#if DEBUG_UNPARSE_CLASS_TYPE
     printf ("Leaving Unparse_Type::unparseClassType \n");
     curprint("/* Leaving Unparse_Type::unparseClassType */ \n");
#endif
   }


void
Unparse_Type::unparseEnumType(SgType* type, SgUnparse_Info& info)
   {
     SgEnumType* enum_type = isSgEnumType(type);
     ROSE_ASSERT(enum_type);

#define DEBUG_ENUM_TYPE 0

#if DEBUG_ENUM_TYPE
     printf ("Inside of unparseEnumType(): info.isTypeFirstPart() = %s info.isTypeSecondPart() = %s \n",(info.isTypeFirstPart() == true) ? "true" : "false",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif
#if DEBUG_ENUM_TYPE
     printf ("Inside of unparseEnumType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Inside of unparseEnumType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true)  ? "true" : "false");
     printf ("Inside of unparseEnumType(): info.SkipClassSpecifier()  = %s \n",(info.SkipClassSpecifier() == true) ? "true" : "false");
  // printf ("Inside of unparseEnumType(): info.SkipEnumSpecifier()   = %s \n",(info.SkipEnumSpecifier() == true)  ? "true" : "false");
#endif
#if 0
     info.display("Inside of unparseEnumType(): call to info.display()");
#endif

     SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
     ASSERT_not_null(edecl);

#if DEBUG_ENUM_TYPE
     printf ("Inside of unparseEnumType(): edecl = %p = %s \n",edecl,edecl ? edecl->class_name().c_str() : "");
#endif

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
     if (info.isTypeSecondPart() == false)
        {
          SgClassDefinition *cdefn = NULL;
          SgNamespaceDefinitionStatement* namespaceDefn = NULL;

       // printf ("edecl->isForward()         = %s \n",(edecl->isForward() == true) ? "true" : "false");

       // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
       // to be used check if name qualification is required.
          unp->u_exprStmt->initializeDeclarationsFromParent ( edecl, cdefn, namespaceDefn );

       // printf ("After initializeDeclarationsFromParent: cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);
       // printf ("In unparseEnumType: cdefn = %p \n",cdefn);

       // GB (09/19/2007): If the enum definition is unparsed, also unparse its
       // attached preprocessing info.
          if (info.isTypeFirstPart() == true && info.SkipEnumDefinition() == false)
             {
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(edecl, info, PreprocessingInfo::before);
             }

       // DQ (4/7/2013): We want to skip the class specified (this include the enum specified also) (see test2013_92.C).
       // DQ (7/24/2011): Restrict where enum is used (to avoid output in template arguments after the name qualification).
       // if ( (info.isTypeFirstPart() == true) )
          if ( (info.isTypeFirstPart() == true) && (info.SkipClassSpecifier() == false) )
             {
            // DQ (5/22/2003) Added output of "enum" string
                curprint ("enum ");
#if DEBUG_ENUM_TYPE
                printf ("Inside of unparseEnumType(): output enum keyword \n");
                curprint ("/* enum from unparseEnumType() */ ");
#endif
            // DQ (2/14/2019): Adding support for C++11 scoped enums (syntax is "enum class ").
               if (edecl->get_isScopedEnum() == true)
                  {
                    curprint ("class ");
                  }
             }
            else
             {
#if DEBUG_ENUM_TYPE
               printf ("Inside of unparseEnumType(): DO NOT output enum keyword \n");
#endif
             }

#if DEBUG_ENUM_TYPE
     printf ("In unparseEnumType: info.inTypedefDecl() = %s \n",info.inTypedefDecl() ? "true" : "false");
     printf ("In unparseEnumType: info.inArgList()     = %s \n",info.inArgList() ? "true" : "false");
#endif

       // DQ (9/14/2013): For C language we need to output the "enum" keyword (see test2013_71.c).
          if ( (info.isTypeFirstPart() == false) && (info.SkipClassSpecifier() == false) && (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true) )
             {
            // DQ (1/6/2020): When this is used as a argument we only want to unparse the name (e.g. sizeof opterator).
            // Note: we need this for the C language support.
            // if (info.inArgList() == false)
            //    {
               curprint ("enum ");

            // DQ (2/14/2019): Adding support for C++11 scoped enums (syntax is "enum class ").
               if (edecl->get_isScopedEnum() == true)
                  {
                    curprint ("class ");
                  }
            //    }
             }
#if 0
       // DQ (7/30/2014): Commented out to avoid compiler warning about not being used.
       // DQ (10/16/2004): Handle name qualification the same as in the unparseClassType function (we could factor common code later!)
          SgNamedType *ptype = NULL;
          if (cdefn != NULL)
             {
               ptype = isSgNamedType(cdefn->get_declaration()->get_type());
             }
#endif
#if 0
       // DQ (10/14/2004): If we are going to output the definition (below) then we don't need the qualified name!
          bool definitionWillBeOutput = ( (info.isTypeFirstPart() == true) && !info.SkipClassDefinition() );
       // printf ("definitionWillBeOutput = %s \n",(definitionWillBeOutput == true) ? "true" : "false");

       // DQ (10/14/2004): This code takes the namespace into account when a qualified name is required!
          ROSE_ASSERT(namespaceDefn == NULL || namespaceDefn->get_namespaceDeclaration() != NULL);
          bool outputQualifiedName = ((ptype != NULL) && (info.get_current_context() != ptype)) ||
                                     ((namespaceDefn != NULL) && (info.get_current_namespace() != namespaceDefn->get_namespaceDeclaration()));
          outputQualifiedName = outputQualifiedName && (definitionWillBeOutput == false);
       // printf ("outputQualifiedName = %s \n",(outputQualifiedName == true) ? "true" : "false");
#endif

          if (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true)
             {
            // DQ (10/11/2006): I think that now that we fill in all empty name as a post-processing step, we can assert this now!
               curprint ( enum_type->get_name().getString() + " ");
             }
            else
             {
            // DQ (6/25/2011): Fixing name qualifiction to work with unparseToString().  In this case we don't 
            // have an associated node to reference as a way to lookup the strored name qualification.  In this 
            // case we return a fully qualified name.
               if (info.get_reference_node_for_qualification() == NULL)
                  {
                    SgName nameQualifierAndType = enum_type->get_qualified_name();
#if DEBUG_ENUM_TYPE
                    printf ("NOTE: In unparseEnumType(): info.get_reference_node_for_qualification() == NULL (assuming this is for unparseToString() nameQualifierAndType = %s \n",nameQualifierAndType.str());
#endif
                 // DQ (3/29/2019): In reviewing where we are using the get_qualified_name() function, this
                 // might be OK since it is likely only associated with the unparseToString() function.
                    curprint(nameQualifierAndType.str());
                  }
                 else
                  {
#if DEBUG_ENUM_TYPE
                    printf ("In unparseEnumType(): info.get_reference_node_for_qualification() = %p \n",info.get_reference_node_for_qualification());
                    if (info.get_reference_node_for_qualification() != NULL)
                       {
                         printf (" --- info.get_reference_node_for_qualification() = %s \n",info.get_reference_node_for_qualification()->class_name().c_str());
                       }
#endif
                 // DQ (6/2/2011): Newest support for name qualification...
                    SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
#if DEBUG_ENUM_TYPE
                    printf ("In unparseEnumType(): nameQualifier = %s \n",nameQualifier.str());
#endif
                    curprint (nameQualifier.str());

                 // DQ (7/28/2012): Added support for un-named types in typedefs.
                    SgName nm;
                    if (edecl->get_isUnNamed() == false)
                       {
                         nm = edecl->get_name();
                       }
                      else
                       {
                      // Else if this is a declaration in a variable declaration, then we do want to output a generated name.
                      // We could also mark the declaration for the cases where this is required. See test2012_141.C for this case.
                         if (edecl->get_parent() == NULL)
                            {
                              printf ("WARNING: edecl->get_parent() == NULL: edecl = %p \n",edecl);

                           // DQ (4/23/2017): Debugging Microsoft extensions.
                              edecl->get_file_info()->display("WARNING: edecl->get_parent() == NULL: debug");

                              printf ("edecl->get_firstNondefiningDeclaration() = %p \n",edecl->get_firstNondefiningDeclaration());
                              printf ("edecl->get_definingDeclaration() = %p \n",edecl->get_definingDeclaration());
                            }
                         ASSERT_not_null(edecl->get_parent());
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(edecl->get_parent());
                         if (typedefDeclaration != NULL)
                            {
                              nm = edecl->get_name();
                            }
                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(edecl->get_parent());
                         if (variableDeclaration != NULL)
                            {
                              nm = edecl->get_name();
                            }
                       }

                    if (nm.getString() != "")
                       {
#if DEBUG_ENUM_TYPE
                         printf ("In unparseEnumType(): Output qualifier of current types to the name = %s \n",nm.str());
#endif
                         curprint ( nm.getString() + " ");
                       }
                  }
             }

       // DQ (1/6/2020): When this is used as a argument we only want to unparse the name (e.g. sizeof opterator).
          if (info.inArgList() == false)
             {
            // DQ (2/18/2019): Adding support for C++11 base type specification syntax.
               if (edecl->get_field_type() != NULL)
                  {
                    curprint(" : ");

                 // Make a new SgUnparse_Info object.
                    SgUnparse_Info ninfo(info);
                    unp->u_type->unparseType(edecl->get_field_type(),ninfo);           
                  }
             }
        }


#if DEBUG_ENUM_TYPE
     printf ("In unparseEnumType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseEnumType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition()  == true) ? "true" : "false");
#endif

  // DQ (1/7/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     if (info.isTypeFirstPart() == true)
        {
       // info.display("info before constructing ninfo");
          SgUnparse_Info ninfo(info);

       // don't skip the semicolon in the output of the statement in the class definition
          ninfo.unset_SkipSemiColon();

          ninfo.set_isUnsetAccess();
#if DEBUG_ENUM_TYPE
          printf ("info.SkipEnumDefinition() = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif
          if ( info.SkipEnumDefinition() == false )
             {
               SgUnparse_Info ninfo(info);
               ninfo.set_inEnumDecl();
               SgInitializer *tmp_init = NULL;
               SgName tmp_name;

            // DQ (5/8/2013): Make sure this is a valid pointer.
               if (edecl->get_definingDeclaration() == NULL)
                  {
                    printf ("edecl = %p = %s \n",edecl,edecl->class_name().c_str());
                  }
               ASSERT_not_null(edecl->get_definingDeclaration());

            // DQ (4/22/2013): We need the defining declaration.
               edecl = isSgEnumDeclaration(edecl->get_definingDeclaration());

            // This fails for test2007_140.C.
               ASSERT_not_null(edecl);

            // DQ (6/26/2005): Output the opend and closing braces even if there are no enumerators!
            // This permits support of the empty enum case! "enum x{};"
               curprint ("{");
#if DEBUG_ENUM_TYPE
               printf ("In unparseEnumType(): Output enumerators from edecl = %p \n",edecl);
               printf ("     --- edecl->get_firstNondefiningDeclaration() = %p \n",edecl->get_firstNondefiningDeclaration());
               printf ("     --- edecl->get_definingDeclaration() = %p \n",edecl->get_definingDeclaration());
#endif
               SgInitializedNamePtrList::iterator p = edecl->get_enumerators().begin();
               if (p != edecl->get_enumerators().end())
                  {
                    while (1)
                       {
                         unp->u_exprStmt->unparseAttachedPreprocessingInfo(*p, info, PreprocessingInfo::before);
                         tmp_name=(*p)->get_name();
                         tmp_init=(*p)->get_initializer();
                         curprint ( tmp_name.str());
                         if (tmp_init)
                            {
                              curprint ( "=");
                              unp->u_exprStmt->unparseExpression(tmp_init, ninfo);
                            }
                         p++;
                         if (p != edecl->get_enumerators().end())
                            {
                              curprint ( ",");
                            }
                           else
                            {
                              break;
                            }
                        }
                  // curprint ( "}";
                   }

            // GB (09/18/2007): If the enum definition is unparsed, also unparse its attached preprocessing info.
            // Putting the "inside" info right here is just a wild guess as to where it might really belong.
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(edecl, info, PreprocessingInfo::inside);

            // DQ (6/26/2005): Support for empty enum case!
               curprint ("}");

            // GB (09/18/2007): If the enum definition is unparsed, also unparse its attached preprocessing info.
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(edecl, info, PreprocessingInfo::after);
             }
        }

#if DEBUG_ENUM_TYPE
     printf ("Leaving unparseEnumType(): edecl = %p \n",edecl);
#endif
   }


void
Unparse_Type::unparseTypedefType(SgType* type, SgUnparse_Info& info)
   {
     SgTypedefType* typedef_type = isSgTypedefType(type);
     ASSERT_not_null(typedef_type);

#define DEBUG_TYPEDEF_TYPE 0

#if DEBUG_TYPEDEF_TYPE
     printf ("Inside of Unparse_Type::unparseTypedefType name = %p = %s \n",typedef_type,typedef_type->get_name().str());
  // curprint ( "\n/* Inside of Unparse_Type::unparseTypedefType */ \n";
#endif
#if DEBUG_TYPEDEF_TYPE
     curprint ( string("\n /* info.isWithType()       = ") + ((info.isWithType()       == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* info.SkipBaseType()     = ") + ((info.SkipBaseType()     == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* info.isTypeSecondPart() = ") + ((info.isTypeSecondPart() == true) ? "true" : "false") + " */ \n");
#endif
#if DEBUG_TYPEDEF_TYPE
     printf ("In unparseTypedefType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     if ((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
        {
       /* do nothing */;
#if DEBUG_TYPEDEF_TYPE
          printf ("Inside of Unparse_Type::unparseTypedefType (do nothing) \n");
#endif
       // curprint ( "\n /* Inside of Unparse_Type::unparseTypedefType (do nothing) */ \n");
        }
       else
        {
       // could be a scoped typedef type
       // check if currrent type's parent type is the same as the context type */
       // SgNamedType *ptype = NULL;
#if DEBUG_TYPEDEF_TYPE
          printf ("Inside of Unparse_Type::unparseTypedefType (normal handling) \n");
#endif
       // curprint ( "\n /* Inside of Unparse_Type::unparseTypedefType (normal handling) */ \n";

          SgTypedefDeclaration *tdecl = isSgTypedefDeclaration(typedef_type->get_declaration());
          ASSERT_not_null(tdecl);

       // DQ (10/16/2004): Keep this error checking for now!
          ASSERT_not_null(typedef_type);
          ASSERT_not_null(typedef_type->get_declaration());
#if 0
          printf ("typedef_type->get_declaration() = %p = %s \n",typedef_type->get_declaration(),typedef_type->get_declaration()->sage_class_name());
#endif
       // DQ (10/17/2004): This assertion forced me to set the parents of typedef in the EDG/Sage connection code
       // since I could not figure out why it was not being set in the post processing which sets parents.
          ASSERT_not_null(typedef_type->get_declaration()->get_parent());

#if 0
          SgName qualifiedName = typedef_type->get_qualified_name();

#error "DEAD CODE!"

#if 0
          if ( typedef_type->get_name() == "Zone" )
             {
               printf ("In unparseTypedefType() name = %s qualifiedName = %s \n",typedef_type->get_name().str(),qualifiedName.str());
               printf ("     Zone: unp->u_name->generateNameQualifier( tdecl , info ) = %s \n",unp->u_name->generateNameQualifier( tdecl , info, 1 ).str());
             }
#endif
#if 0
          printf ("typedef_type->get_declaration() = %p = %s  name = %s \n",
               typedef_type->get_declaration(),typedef_type->get_declaration()->sage_class_name(),
               typedef_type->get_name().str());
          printf ("#0 typedef_type->get_name() = %s typedef_type->get_qualified_name() = %s \n",
               typedef_type->get_name().str(),qualifiedName.str());
          printf ("typedef_type->get_declaration()->get_firstNondefiningDeclaration() = %p \n",
               typedef_type->get_declaration()->get_firstNondefiningDeclaration());
          printf ("typedef_type->get_declaration()->get_definingDeclaration() = %p \n",
               typedef_type->get_declaration()->get_definingDeclaration());
#endif

#error "DEAD CODE!"

       // printf ("In unparseTypedefType(): qualifiedName = %s \n",qualifiedName.str());
       // DQ (11/14/2004): It seems that we ALWAY output the qualified name!
       // curprint ( " /* unparse qualified typedef name " + qualifiedName.str() + " */ \n";
          unparseQualifiedNameList(typedef_type->get_qualifiedNameList());
          curprint ( qualifiedName.str() + " ");
#else

          if (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true)
             {
            // DQ (10/11/2006): I think that now that we fill in all enmpty name as a post-processing step, we can assert this now!
               ROSE_ASSERT (typedef_type->get_name().getString() != "");
               curprint ( typedef_type->get_name().getString() + " ");
             }
            else
             {
#if 0
       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
          if (info.SkipQualifiedNames() == false)
             {

#error "DEAD CODE!"

#if 0
            // DQ (10/10/2006): New support for qualified names for types.
            // ASSERT_not_null(info.get_qualifiedNameList());
               if (info.get_qualifiedNameList() != NULL && info.get_qualifiedNameList()->empty() == false)
                  {
                    printf ("Put out the global qualification for SgTypedefType type = %p = %s info.get_qualifiedNameList()->size() = %ld \n",
                         typedef_type,typedef_type->class_name().c_str(),info.get_qualifiedNameList()->size());
                    unparseQualifiedNameList(*(info.get_qualifiedNameList()));
                  }

#error "DEAD CODE!"

                 else
                  {
                 // DQ (10/10/2006): If the qualified list was not built, then only output global qualification if we are currently in a namespace.
                    if (info.get_current_namespace() != NULL)
                       {
                         curprint ( ":: /* default explicit global qualifier for typedef */ ");
                       }
                  }
#endif

#error "DEAD CODE!"

               SgName qualifiedName = typedef_type->get_qualified_name();
               curprint ( qualifiedName.str() + " ");
             }
#else
            // The C++ support is more complex and can require qualified names!

            // DQ (6/22/2011): I don't think we can assert this for anything than internal testing.  The unparseToString tests will fail with this assertion in place.
            // ASSERT_not_null(info.get_reference_node_for_qualification());
            // SgName nameQualifier = unp->u_name->generateNameQualifier( tdecl , info );
            // SgName nameQualifier = unp->u_name->generateNameQualifier( tdecl, info, true );
            // printf ("info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());

            // printf ("In unparseTypedefType(): info.get_current_scope() = %p \n",info.get_current_scope());
#if 0
            // DQ (6/2/2011): Newest support for name qualification...
               SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());

            // printf ("In unparseTypedefType(): nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
            // curprint ( "\n/* nameQualifier (from unp->u_name->generateNameQualifier function) = " + nameQualifier + " */ \n ";

#error "DEAD CODE!"

               curprint ( nameQualifier.str());
               SgName nm = typedef_type->get_name();
               if (nm.getString() != "")
                  {
                 // printf ("Output qualifier of current types to the name = %s \n",nm.str());
                    curprint ( nm.getString() + " ");
                  }
#else

#if 0
            // DQ (4/14/2018): Check if this is associated with the template instantiation.
               SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(tdecl);
               if (templateInstantiationTypedefDeclaration != NULL)
                  {
                    printf ("Part 1: We need to process the template arguments (and name qualification) for this SgTemplateInstantiationTypedefDeclaration \n");
                    curprint ("/* Part 1: We need to process the template arguments to include name qualification */ ");
                  }
               printf ("In unparseTypedefType(): info.get_reference_node_for_qualification() = %p \n",info.get_reference_node_for_qualification());
#endif
            // DQ (6/25/2011): Fixing name qualifiction to work with unparseToString().  In this case we don't 
            // have an associated node to reference as a way to lookup the strored name qualification.  In this 
            // case we return a fully qualified name.
               if (info.get_reference_node_for_qualification() == NULL)
                  {
                 // printf ("WARNING: In unparseTypedefType(): info.get_reference_node_for_qualification() == NULL (assuming this is for unparseToString() \n");
                    SgName nameQualifierAndType = typedef_type->get_qualified_name();
#if 0
                    printf ("In unparseTypedefType(): Output name nameQualifierAndType = %s \n",nameQualifierAndType.str());
#endif
                 // DQ (3/29/2019): In reviewing where we are using the get_qualified_name() function, this
                 // might be OK since it is likely only associated with the unparseToString() function.
                    curprint(nameQualifierAndType.str());
                  }
                 else
                  {
                    SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
                    curprint(nameQualifier.str());

                 // DQ (4/14/2018): This is not the correct way to handle the output of template instantations since this uses the internal name (with unqualified template arguments).

#if 0
                 // DQ (4/15/2018): Original code (which unparsed using the name which would embedd template arguments, but without name qualification).
                    SgName nm = typedef_type->get_name();
                    if (nm.getString() != "")
                       {
#if 0
                         printf ("In unparseTypedefType(): Output qualifier of current types to the name = %s \n",nm.str());
#endif
                         curprint ( nm.getString() + " ");
                       }
#else
                 // DQ (4/15/2018): New code (which unparses the template name (without template arguments, and unparsed the template arguments with name qualification).
                 // DQ (4/2/2018): Adding support for alternative and more sophisticated handling of the function name 
                 // (e.g. with template arguments correctly qualified, etc.).
                    SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(tdecl);
                    if (templateInstantiationTypedefDeclaration != NULL)
                       {
#if 0
                         printf ("Calling unparseTemplateTypedefName() \n");
                         curprint(" /* In unparseTypedefType(): Calling unparseTemplateTypedefName() */ \n");
#endif
                         unparseTemplateTypedefName(templateInstantiationTypedefDeclaration,info);
#if 0
                         printf ("Done: unparseTemplateTypedefName() \n");
                         curprint(" /* In unparseTypedefType(): Done: unparseTemplateTypedefName() */ \n");
#endif
                       }
                      else
                       {
#if 0
                         printf ("typedef_type->get_name() for non-template instantiation typedef = %s \n",typedef_type->get_name().str());
#endif
                      // curprint ( typedef_type->get_name().str());
                         SgName nm = typedef_type->get_name();
                         if (nm.getString() != "")
                            {
#if 0
                              printf ("In unparseTypedefType(): Output qualifier of current types to the name = %s \n",nm.str());
#endif
                              curprint ( nm.getString() + " ");
                            }
                       }
#endif
                  }
#endif
#endif
             }
#endif

#if 0
#if PRINT_DEVELOPER_WARNINGS
          if (tdecl->get_parent() == NULL)
               printf ("Warning: SgTypedefDeclaration does not have a parent (tdecl->get_parent() == NULL) in %s on line %d \n",__FILE__,__LINE__);
#endif
       // ASSERT_not_null(tdecl->get_parent());

#error "DEAD CODE!"

#if 1
          ASSERT_not_null(tdecl->get_parent());
          SgClassDefinition *cdefn = isSgClassDefinition(tdecl->get_parent());
       // printf ("In unparseTypedefType(): cdefn = %p \n",cdefn);
          if (cdefn != NULL)
#else
       // Alternative fix that might help Beata except that it does not enforce the parent pointers
       // so we have to think about this a little more when we work on the Sage III interface to
       // constructing AST fragments directly.
          if (tdecl->get_parent() != NULL && isSgClassDefinition(tdecl->get_parent()))
#endif
             {
               SgClassDefinition *cdefn    = isSgClassDefinition(tdecl->get_parent());
               SgNamedType *ptype = isSgNamedType(cdefn->get_declaration()->get_type());
            // if(!ptype || (info.get_current_context() == ptype))
               if ( (ptype == NULL) || (info.get_current_context() == ptype) )
                  {
                 // printf ("#1 typedef_type->get_name() = %s \n",typedef_type->get_name().str());

#error "DEAD CODE!"

                    curprint ( typedef_type->get_name().str() + " ");
                  }
                 else
                  {
                 // add qualifier of current types to the name
                    SgName nm = cdefn->get_declaration()->get_qualified_name();
                 // if(!nm.is_null())
                    if ( nm.is_null() == false )
                       {
                      // printf ("nm = %s :: typedef_type->get_name() = %s \n",nm.str(),typedef_type->get_name().str());

#error "DEAD CODE!"

                         curprint ( nm.str() + "::" + typedef_type->get_name().str() + " ");
                       }
                      else
                       {
                      // printf ("#2 typedef_type->get_name() = %s \n",typedef_type->get_name().str());
                         curprint ( typedef_type->get_name().str() + " ");
                       }
                  }

#error "DEAD CODE!"

             }
            else
             {
            // printf ("#3 typedef_type->get_name() = %s \n",typedef_type->get_name().str());
               curprint ( typedef_type->get_name().str() + " ");
             }

#error "DEAD CODE!"

#endif


#if 0
       // DQ (4/14/2018): Check if this is associated with the template instantiation.
          SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(tdecl);
          if (templateInstantiationTypedefDeclaration != NULL)
             {
               printf ("We need to process the template arguments (and name qualification) for this SgTemplateInstantiationTypedefDeclaration \n");
               curprint ("/* We need to process the template arguments to include name qualification */ ");
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#endif
        }

#if DEBUG_TYPEDEF_TYPE
     printf ("Leaving Unparse_Type::unparseTypedefType \n");
#endif
#if DEBUG_TYPEDEF_TYPE
     curprint("\n/* Leaving Unparse_Type::unparseTypedefType */ \n");
#endif
   }


void
Unparse_Type::unparseTemplateTypedefName(SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration, SgUnparse_Info& info)
   {
  // DQ (6/21/2011): Generated this function from refactored call to unparseTemplateArgumentList
     ASSERT_not_null(templateInstantiationTypedefDeclaration);

#if 0
     printf ("In unparseTemplateTypedefName(): templateInstantiationTypedefDeclaration->get_name() = %s \n",templateInstantiationTypedefDeclaration->get_name().str());
     printf ("In unparseTemplateTypedefName(): templateInstantiationTypedefDeclaration->get_templateName() = %s \n",templateInstantiationTypedefDeclaration->get_templateName().str());
#endif

     unp->u_exprStmt->curprint(templateInstantiationTypedefDeclaration->get_templateName().str());

  // DQ (4/15/2018): Not clear if this level of information is available for template teypdef instantiations.
  // bool unparseTemplateArguments = templateInstantiationTypedefDeclaration->get_template_argument_list_is_explicit();
     bool unparseTemplateArguments = true;

#if 0
  // DQ (6/29/2013): This controls if the template arguments for the function should be unparsed.
  // printf ("In unparseTemplateFunctionName(): templateInstantiationFunctionDeclaration->get_template_argument_list_is_explicit() = %s \n",
  //      templateInstantiationFunctionDeclaration->get_template_argument_list_is_explicit() ? "true" : "false");
     printf ("In unparseTemplateTypedefName(): name = %s unparse template arguments = %s \n",
          templateInstantiationTypedefDeclaration->get_templateName().str(),unparseTemplateArguments ? "true" : "false");
#endif

  // DQ (6/29/2013): Use the information recorded in the AST as to if this function has been used with 
  // template arguments in the original code.  If so then we always unparse the template arguments, if 
  // not then we never unparse the template arguments.  See test2013_242.C for an example of where this 
  // is significant in the generated code.  Note that this goes a long way toward making the generated
  // code look more like the original input code (where before we have always unparsed template arguments
  // resulting in some very long function calls in the generated code).  Note that if some template
  // arguments are specified and some are not then control over not unparsing template arguments that
  // where not explicit in the original code will be handled seperately in the near future (in the 
  // SgTemplateArgument IR nodes).
  // unparseTemplateArgumentList(templateInstantiationFunctionDeclaration->get_templateArguments(),info);
     if (unparseTemplateArguments == true)
        {
#if 0
          SgTemplateArgumentPtrList & templateArgList = templateInstantiationTypedefDeclaration->get_templateArguments();
          printf ("In unparseTemplateTypedefName(): templateArgList.size() = %zu \n",templateArgList.size());
          for (size_t i = 0; i < templateArgList.size(); i++)
             {
               printf ("--- templateArgList[%zu] = %p \n",i,templateArgList[i]);
             }
#endif
          unp->u_exprStmt->unparseTemplateArgumentList(templateInstantiationTypedefDeclaration->get_templateArguments(),info);
        }
   }












string
Unparse_Type::unparseRestrictKeyword()
   {
  // DQ (12/11/2012): This isolates the logic for the output of the "restrict" keyword for different backend compilers.
     string returnString;

  // DQ (8/29/2005): Added support for classification of back-end compilers (independent of the name invoked to execute them)
     bool usingGcc = false;
     #ifdef USE_CMAKE
        #ifdef CMAKE_COMPILER_IS_GNUCC
           usingGcc = true;
        #endif
     #else
     // DQ (4/16/2016): The clang compiler also uses the GNU form of the restrict keyword.
     // DQ (2/1/2016): Make the behavior of ROSE independent of the exact name of the backend compiler (problem when packages name compilers such as "g++-4.8").
     // string compilerName = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
     // usingGcc = (compilerName == "g++" || compilerName == "gcc" || compilerName == "mpicc" || compilerName == "mpicxx" || compilerName == "mpiicpc");
     // #if BACKEND_CXX_IS_GNU_COMPILER
        #if BACKEND_CXX_IS_GNU_COMPILER || BACKEND_CXX_IS_CLANG_COMPILER
           usingGcc = true;
        #endif
        #if 0
           printf ("Processing restrict keyword: compilerName = %s \n",compilerName.c_str());
        #endif
     #endif

  // Liao 6/11/2008, Preserve the original "restrict" for UPC
  // regardless types of the backend compiler
     if (SageInterface::is_UPC_language() == true )
        {
       // DQ (12/12/2012): We need the white space before and after the keyword (fails for cherokee-util.c).
          returnString = " restrict ";
        }
       else
        {
          if ( usingGcc )
             {
            // GNU uses a string variation on the C99 spelling of the "restrict" keyword
            // DQ (12/12/2012): We need the white space before and after the keyword.
#if 0
               printf ("Using GNU form of restrict keyword! \n");
#endif
               returnString = " __restrict__ ";
             }
            else
             {
            // DQ (12/12/2012): We need the white space before and after the keyword.
#if 0
               printf ("Using non-GNU form of restrict keyword! \n");
#endif
               returnString = " restrict ";
             }
        }

     return returnString;
   }


void Unparse_Type::unparseModifierType(SgType* type, SgUnparse_Info& info)
   {
     SgModifierType* mod_type = isSgModifierType(type);
     ASSERT_not_null(mod_type);

#if 0
     printf ("Top of Unparse_Type::unparseModifierType \n");
  // curprint("\n/* Top of Unparse_Type::unparseModifierType */ \n");
#endif
#if 0
  // mod_type->get_typeModifier().display("called from Unparse_Type::unparseModifierType()");
     printf ("In unparseModifierType(): modifier values (at %p): %s \n",mod_type,mod_type->get_typeModifier().displayString().c_str());
#endif

#if 0
     printf ("In unparseModifierType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparseModifierType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparseModifierType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparseModifierType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

  // Determine if we have to print the base type first (before printing the modifier).
  // This is true in case of a pointer (e.g., int * a) or a reference (e.g., int & a)
     bool btype_first = false;
  // if ( isSgReferenceType(mod_type->get_base_type()) || isSgPointerType(mod_type->get_base_type()) )
     if ( isSgReferenceType(mod_type->get_base_type()) || isSgPointerType(mod_type->get_base_type()) )
        {
          btype_first = true;
        }
       else
        {
       // DQ (6/19/2013): Check for case or base_type being a modifier (comes up in complex templae argument handling for template arguments that are unavailable though typedef references.
          SgModifierType* inner_mod_type = isSgModifierType(mod_type->get_base_type());
#if 0
          printf ("inner_mod_type = %p \n",inner_mod_type);
#endif
          if (inner_mod_type != NULL)
             {
               ASSERT_not_null(inner_mod_type->get_base_type());
#if 0
               printf ("In Unparse_Type::unparseModifierType(): inner_mod_type->get_base_type() = %p = %s \n",inner_mod_type->get_base_type(),inner_mod_type->get_base_type()->class_name().c_str());
#endif
#if 0
               if ( isSgReferenceType(inner_mod_type->get_base_type()) || isSgPointerType(inner_mod_type->get_base_type()) )
                  {
#if 0
                    printf ("In Unparse_Type::unparseModifierType(): Detected rare case of modifier-modifier (due to complex template typedef references to unavailable template instantiations). \n");
#endif
                    btype_first = true;
                  }
#else
            // btype_first = true;
            // printf ("In Unparse_Type::unparseModifierType(): Make recursive call to unparseModifierType \n");
            // unparseModifierType(inner_mod_type,info);
#endif
             }
        }
     

#if 0
     printf ("btype_first            = %s \n",btype_first ? "true" : "false");
     printf ("info.isTypeFirstPart() = %s \n",info.isTypeFirstPart() ? "true" : "false");
#endif

     if (info.isTypeFirstPart())
        {
       // Print the base type if this has to come first
          if (mod_type->get_typeModifier().isOpenclGlobal())
              curprint ( "__global ");
          if (mod_type->get_typeModifier().isOpenclLocal())
              curprint ( "__local ");
          if (mod_type->get_typeModifier().isOpenclConstant())
              curprint ( "__constant ");

          if (btype_first)
               unparseType(mod_type->get_base_type(), info);

          if (mod_type->get_typeModifier().haveAddressSpace()) {
              std::ostringstream outstr;
              outstr << "__attribute__((address_space(" << mod_type->get_typeModifier().get_address_space_value() << ")))";
              curprint ( outstr.str().c_str() );
          }

          if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())
             { curprint ( "const "); }
          if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile())
             { curprint ( "volatile "); }
#if 0
       // DQ (1/19/2019): Cxx_tests/test2019_04.C demonstrates that "const volatile" is output as "volatile"
          printf ("mod_type->get_typeModifier().get_constVolatileModifier().isConst()    = %s \n",mod_type->get_typeModifier().get_constVolatileModifier().isConst()    ? "true" : "false");
          printf ("mod_type->get_typeModifier().get_constVolatileModifier().isVolatile() = %s \n",mod_type->get_typeModifier().get_constVolatileModifier().isVolatile() ? "true" : "false");
          printf ("mod_type->get_typeModifier().isRestrict()                             = %s \n",mod_type->get_typeModifier().isRestrict() ? "true" : "false");
#endif
          if (mod_type->get_typeModifier().isRestrict())
             {
            // DQ (12/11/2012): Newer version of the code refactored.
               curprint(unparseRestrictKeyword());
             }

       // Microsoft extension
       // xxx_unaligned   // Microsoft __unaligned qualifier

       // Support for near and far pointers (a microsoft extension)
       // xxx_near        // near pointer
       // xxx_far         // far pointer

       // Support for UPC
       // Liao, 6/11/2008. Enable faithful unparsing for .upc files
          if (mod_type->get_typeModifier().get_upcModifier().isUPC_Strict())
             { curprint ("strict "); }
          if (mod_type->get_typeModifier().get_upcModifier().isUPC_Relaxed())
             { curprint ("relaxed "); }

          if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
             {
               long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

               if (block_size == 0) // block size empty
                  {
                    curprint ("shared[] ") ;
                  }
               else if (block_size == -1) // block size omitted
                  {
                    curprint ("shared ") ;
                  }
               else if (block_size == -2) // block size is *
                  {
                    curprint ("shared[*] ") ;
                  }
               else
                  {
                    ROSE_ASSERT(block_size > 0);

                    stringstream ss;

                    ss << block_size;

                    curprint ("shared[" + ss.str() + "] ");
                  }
             }

       // Print the base type unless it has been printed up front
          if (!btype_first)
             {
#if 0
               printf ("In Unparse_Type::unparseModifierType(): Calling unparseType on mod_type->get_base_type() = %p = %s \n",mod_type->get_base_type(),mod_type->get_base_type()->class_name().c_str());
#endif
               unparseType(mod_type->get_base_type(), info);
             }
        }
       else
        {
#if 0
          printf ("info.isTypeSecondPart() = %s \n",info.isTypeSecondPart() ? "true" : "false");
#endif
          if (info.isTypeSecondPart())
             {
#if 0
               printf ("In Unparse_Type::unparseModifierType(): Calling unparseType on mod_type->get_base_type() = %p = %s \n",mod_type->get_base_type(),mod_type->get_base_type()->class_name().c_str());
#endif
               unparseType(mod_type->get_base_type(), info);
             }
            else
             {
#if 0
               printf ("In Unparse_Type::unparseModifierType(): Calling unparseType TWICE (1st and 2nd parts) on mod_type = %p = %s \n",mod_type,mod_type->class_name().c_str());
#endif
               SgUnparse_Info ninfo(info);
               ninfo.set_isTypeFirstPart();
               unparseType(mod_type, ninfo);
               ninfo.set_isTypeSecondPart();
               unparseType(mod_type, ninfo);
             }
        }

#if 0
     printf ("Leaving Unparse_Type::unparseModifierType \n");
  // curprint("\n/* Leaving Unparse_Type::unparseModifierType */ \n");
#endif
   }


void
Unparse_Type::unparseFunctionType(SgType* type, SgUnparse_Info& info)
   {
     SgFunctionType* func_type = isSgFunctionType(type);
     ASSERT_not_null(func_type);

#define DEBUG_FUNCTION_TYPE 0

  // DQ (1/8/2014): debugging test2014_25.c.
  // info.unset_isPointerToSomething();

     SgUnparse_Info ninfo(info);
     int needParen = 0;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen = 1;
        }

#if 0
     printf ("In unparseFunctionType(): reset needParen = 0 \n");
     curprint("\n/* In unparseFunctionType: set needParen = 0 */ \n");
#endif

  // needParen = 0;

#if DEBUG_FUNCTION_TYPE
     printf ("In unparseFunctionType(): needParen = %d \n",needParen);
     curprint("\n/* In unparseFunctionType: needParen = " + StringUtility::numberToString(needParen) + " */ \n");
#endif

#if DEBUG_FUNCTION_TYPE
     printf ("In unparseFunctionType(): info.isReferenceToSomething() = %s \n",info.isReferenceToSomething() ? "true" : "false");
     printf ("In unparseFunctionType(): info.isPointerToSomething()   = %s \n",info.isPointerToSomething()   ? "true" : "false");
#endif

     ROSE_ASSERT(info.isReferenceToSomething() == ninfo.isReferenceToSomething());
     ROSE_ASSERT(info.isPointerToSomething()   == ninfo.isPointerToSomething());

  // DQ (10/8/2004): Skip output of class definition for return type! C++ standard does not permit
  // a defining declaration within a return type, function parameter, or sizeof expression.
     ninfo.set_SkipClassDefinition();

  // DQ (1/7/2014): We also need to skip the enum definition (see test2014_24.c).
     ninfo.set_SkipEnumDefinition();


     if (ninfo.isTypeFirstPart())
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
          curprint ( "\n/* In unparseFunctionType: handling first part */ \n");
          curprint ( "\n/* Skipping the first part of the return type! */ \n");
#endif
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
            // DQ (9/21/2004): we don't want this for typedefs of function pointers where the function return type is a pointer
               printf ("Skipping the first part of the return type (in needParen == true case)! \n");
               curprint ( "\n/* Skipping the first part of the return type (in needParen == true case)! */ \n");
#endif
#if OUTPUT_DEBUGGING_UNPARSE_INFO || DEBUG_FUNCTION_TYPE
               curprint ( string("\n/* ") + ninfo.displayString("Skipping the first part of the return type (in needParen == true case)") + " */ \n");
#endif
               unparseType(func_type->get_return_type(), ninfo);
               curprint("(");
            // curprint("/* unparseFunctionType */ (");
             }
            else
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
            // DQ (9/21/2004): we don't want this for typedefs of function pointers where the function return type is a pointer
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
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ( "\n/* In unparseFunctionType: handling second part */ \n");
#endif
               if (needParen)
                  {
#if DEBUG_FUNCTION_TYPE
                    curprint ("/* needParen must be true */ \n ");
#endif
                    curprint(")");
                 // curprint("/* unparseFunctionType */ )");

                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
            // print the arguments
               SgUnparse_Info ninfo2(info);
               ninfo2.unset_SkipBaseType();
               ninfo2.unset_isTypeSecondPart();
               ninfo2.unset_isTypeFirstPart();

            // DQ (3/15/2005): Don't let typedef declarations (or enum or struct definitions) be
            // unparsed in the function parameter list type output (see test2005_16.C).
               ninfo2.set_SkipDefinition();

            // DQ (4/23/2013): Force the fully qualified name to be used instead (leave reference_node_for_qualification == NULL).
            // See test2013_130.C for an example of where this required.
            // DQ (6/12/2011): Added support for name qualification of types (but this is insufficent when types in teh list must be qualified differently).
            // We can use the func_type as the node to associate name qualifications, but this is also not the correct type of node to us since it should
            // be a node that is unique in the AST.  The type name would have to be saved more directly.  This is likely the support that is ALSO required.
            // ninfo2.set_reference_node_for_qualification(func_type);
               ninfo2.set_reference_node_for_qualification(NULL);
               ROSE_ASSERT(ninfo2.get_reference_node_for_qualification() == NULL);
#if DEBUG_FUNCTION_TYPE
               printf ("Setting reference_node_for_qualification to SgFunctionType, but this is not correct where name qualification is required. \n");
#endif
#if DEBUG_FUNCTION_TYPE
               curprint ("/* Output the type arguments (with parenthesis) */ \n ");
#endif
               curprint("(");
            // curprint("/* unparseFunctionType:parameters */ (");

               SgTypePtrList::iterator p = func_type->get_arguments().begin();
               while(p != func_type->get_arguments().end())
                  {
                 // printf ("Output function argument ... \n");
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
                    curprint ( "\n/* In unparseFunctionType(): Output the function type arguments */ \n");
#endif
#if DEBUG_FUNCTION_TYPE
                    printf ("In unparseFunctionType(): calling unparseType(): output function arguments = %p \n",*p);
                    printf ("   --- *p = %p = %s \n",*p,(*p)->class_name().c_str());
                    printf ("   --- ninfo2.isTypeFirstPart()  = %s \n",ninfo2.isTypeFirstPart() ? "true" : "false");
                    printf ("   --- ninfo2.isTypeSecondPart() = %s \n",ninfo2.isTypeSecondPart() ? "true" : "false");
#endif
                    unparseType(*p, ninfo2);

                    p++;

                    if (p != func_type->get_arguments().end())
                       { 
                         curprint(", ");
                       }
                  }

               curprint(")");
            // curprint("/* unparseFunctionType:parameters */ )");

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ("\n/* In unparseFunctionType(): AFTER parenthesis are output */ \n");
#endif
               unparseType(func_type->get_return_type(), info); // catch the 2nd part of the rtype

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ("\n/* Done: In unparseFunctionType(): handling second part */ \n");
#endif
             }
            else
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ("\n/* In unparseFunctionType: recursive call with isTypeFirstPart == true */ \n");
#endif
#if 0
            // DQ (8/19/2012): Temp code while debugging (test2012_192.C).
               printf ("Error: I think this should not be done \n");
               ROSE_ASSERT(false);
#endif
               ninfo.set_isTypeFirstPart();
               unparseType(func_type, ninfo);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ( "\n/* In unparseFunctionType: recursive call with isTypeSecondPart == true */ \n");
#endif
               ninfo.set_isTypeSecondPart();
               unparseType(func_type, ninfo);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ( "\n/* In unparseFunctionType: end of recursive call */ \n");
#endif
             }
        }

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
     printf("Leaving unparseFunctionType() \n");
     curprint("\n/* Leaving unparseFunctionType() */ \n");
#endif
   }


void
Unparse_Type::unparseMemberFunctionType(SgType* type, SgUnparse_Info& info)
   {
#if 0
     printf ("In unparseMemberFunctionType(type = %p (%s))\n", type, type ? type->class_name().c_str() : "");
#endif

     SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(type);
     ASSERT_not_null(mfunc_type);

     SgUnparse_Info ninfo(info);
     int needParen=0;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen=1;
        }

  // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit
  // a defining declaration within a return type, function parameter, or sizeof expression.
     ninfo.set_SkipClassDefinition();

  // DQ (1/13/2014): Set the output of the enum defintion to match that of the class definition (consistancy is now inforced).
     ninfo.set_SkipEnumDefinition();

     if (ninfo.isTypeFirstPart())
        {
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();
#if 0
               printf ("In unparseMemberFunctionType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

               unparseType(mfunc_type->get_return_type(), ninfo);
               curprint ( "(");
             }
            else
             {
#if 0
               printf ("In unparseMemberFunctionType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

               unparseType(mfunc_type->get_return_type(), ninfo);
             }
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
                 // printf ("In unparseMemberFunctionType: output the arguments \n");
                    unparseType(*p, ninfo2);
                    p++;
                    if (p != mfunc_type->get_arguments().end())
                       {
                         curprint ( ", ");
                       }
                  }
               curprint ( ")");
#if 0
               printf ("In unparseMemberFunctionType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

               unparseType(mfunc_type->get_return_type(), info); // catch the 2nd part of the rtype
#if 0
               printf ("In unparseMemberFunctionType(): after unparseType() second part: unparse modifiers \n");
#endif

               if (mfunc_type->isConstFunc()) {
                 curprint (" const");
               }

            // DQ (1/11/2020): Adding missing support for volatile and const-volatile.
               if (mfunc_type->isVolatileFunc()) 
                  {
                 // curprint (" /* adding volatile */ ");
                    curprint (" volatile");
                  }

            // DQ (1/11/2020): Adding support for lvalue reference member function modifiers.
               if (mfunc_type->isLvalueReferenceFunc())
                  {
                    curprint(" &");
                  }

            // DQ (1/11/2020): Adding support for rvalue reference member function modifiers.
               if (mfunc_type->isRvalueReferenceFunc())
                  {
                    curprint(" &&");
                  }
#if 0
               if (mfunc_type->get_ref_qualifiers() == 1) {
                 curprint (" &");
               } else if (mfunc_type->get_ref_qualifiers() == 2) {
                 curprint (" &&");
               }
#endif
             }
            else
             {
               ninfo.set_isTypeFirstPart();
#if 0
               printf ("In unparseMemberFunctionType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

               unparseType(mfunc_type, ninfo);

               ninfo.unset_isTypeFirstPart();
               ninfo.set_isTypeSecondPart();
#if 0
               printf ("In unparseMemberFunctionType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

               unparseType(mfunc_type, ninfo);
             }
        }
   }

void
Unparse_Type::unparseArrayType(SgType* type, SgUnparse_Info& info)
   {
     SgArrayType* array_type = isSgArrayType(type);
     ASSERT_not_null(array_type);

#define DEBUG_ARRAY_TYPE 0

  // different cases to think about
  //    int (*) [10],  int (*var) [20]
  //    int *[10],  int *var[10]
  //    int [10][20], int var[10][20]
  // multidimensional,
  //    int [2][10] is built up as
  //      ArrayType(base_type, 2)
  //        ArrayType(int, 10), because of the front-end

#if 0
     info.display("In unparseArrayType()");
#endif

     bool is_variable_length_array = array_type->get_is_variable_length_array();
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(info.get_decl_stmt());
     bool isFunctionPrototype = ( (functionDeclaration != NULL) && (functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration()) );

#if 0
     printf ("In unparseArrayType(): is_variable_length_array = %s \n",is_variable_length_array ? "true" : "false");
     printf ("In unparseArrayType(): isFunctionPrototype      = %s \n",isFunctionPrototype ? "true" : "false");
#endif

#if DEBUG_ARRAY_TYPE
     string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
     string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
     printf ("\nIn Unparse_Type::unparseArrayType(): type = %p type->class_name() = %s firstPart = %s secondPart = %s \n",type,type->class_name().c_str(),firstPartString.c_str(),secondPartString.c_str());
     printf ("In Unparse_Type::unparseArrayType(): array_type->get_base_type() = %p = %s \n",array_type->get_base_type(),array_type->get_base_type()->class_name().c_str());
#endif

#if 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint("/* In unparseArrayType() */ \n ");
#endif
#if DEBUG_ARRAY_TYPE
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint(string("\n/* Top of unparseArrayType() using generated type name string: ") + type->class_name() + " firstPart " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif

#if DEBUG_ARRAY_TYPE
     printf ("In Unparse_Type::unparseArrayType(): info.isReferenceToSomething() = %s \n",info.isReferenceToSomething() ? "true" : "false");
     printf ("In Unparse_Type::unparseArrayType(): info.isPointerToSomething()   = %s \n",info.isPointerToSomething()   ? "true" : "false");
#endif

#if 0
  // DQ (5/10/2015): Removing this special case now that other parts of the unparsing of
  // typeof operators are more refined.  This is done as part of debugging: test2015_115.c.

  // DQ (4/11/2015): Need to handle "[n]" part of array type (see test2015_83.c).
  // DQ (3/31/2015): We can't use the perenthesis in this case (see test2015_49.c).
  // if (isSgTypeOfType(array_type->get_base_type()) != NULL)
     if (isSgTypeOfType(array_type->get_base_type()) != NULL && info.isTypeFirstPart() == true)
        {
       // This is a special case (similar to that for SgPointerType, but not the same).
#if DEBUG_ARRAY_TYPE
          printf("################ In unparseArrayType(): handle special case of array_type->get_base_type() is SgTypeOfType \n");
#endif
          if (info.isTypeFirstPart() == true)
             {
#if DEBUG_ARRAY_TYPE
               printf ("In Unparse_Type::unparseArrayType(): base_type is SgTypeOfType: info.isTypeFirstPart() == true (unset first and second parts and call unparse on SgTypeOfType base type) \n");
#endif
               SgUnparse_Info ninfo1(info);
               ninfo1.unset_isTypeSecondPart();
               ninfo1.unset_isTypeFirstPart();

               unparseType(array_type->get_base_type(), ninfo1);
             }

#if 0
       // DQ (4/11/2015): This code cannot be executed since we modified the predicate to add "&& info.isTypeFirstPart() == true"
          if (info.isTypeSecondPart() == true)
             {
#if DEBUG_ARRAY_TYPE
               printf ("In Unparse_Type::unparseArrayType(): base_type is SgTypeOfType: info.isTypeSecondPart() == true (nothing implemented) \n");
#endif
             }
#endif
          return;
        }
#else
//   printf ("In Unparse_Type::unparseArrayType(): Commented out special case handling for SgTypeOf base type \n");
#endif

     SgUnparse_Info ninfo(info);
     bool needParen = false;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen = true;

#if 0
       // DQ (3/30/2015): We can't use the perenthesis in this case (see test2015_49.c).
          if (isSgTypeOfType(array_type->get_base_type()) != NULL)
             {
#if 1
               printf("In unparseArrayType(): if isSgTypeOfType(array_type->get_base_type()) != NULL, then set needParen = false \n");
#endif
               needParen = false;
             }
#endif
        }

#if DEBUG_ARRAY_TYPE || 0
     printf ("In unparseArrayType(): needParen = %s \n",(needParen == true) ? "true" : "false");
     curprint(string("/* In  unparseArrayType() needParen = ") + string((needParen == true) ? "true" : "false") + string(" */ \n "));
#endif

     if (ninfo.isTypeFirstPart() == true)
        {
          if(needParen == true)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();
#if DEBUG_ARRAY_TYPE
               printf ("ninfo.isTypeFirstPart() == true: needParen == true: Calling unparseType(array_type->get_base_type(), ninfo); \n");
#endif
               unparseType(array_type->get_base_type(), ninfo);
#if DEBUG_ARRAY_TYPE
               printf ("DONE: ninfo.isTypeFirstPart() == true: needParen == true: Calling unparseType(array_type->get_base_type(), ninfo); \n");
#endif
               curprint("(");
            // curprint(" /* unparseArrayType */ (");
             }
            else
             {
#if DEBUG_ARRAY_TYPE
               printf ("ninfo.isTypeFirstPart() == true: needParen == false: Calling unparseType(array_type->get_base_type(), ninfo); \n");
#endif
               unparseType(array_type->get_base_type(), ninfo);
#if DEBUG_ARRAY_TYPE
               printf ("DONE: ninfo.isTypeFirstPart() == true: needParen == false: Calling unparseType(array_type->get_base_type(), ninfo); \n");
#endif
             }
        }
       else
        {
          if (ninfo.isTypeSecondPart() == true)
             {
               if (needParen == true)
                  {
#if DEBUG_ARRAY_TYPE
                    printf ("ninfo.isTypeSecondPart() == true: needParen == true: output parenthisis and unparse the array index \n");
#endif
                    curprint(")");
                 // curprint(" /* unparseArrayType */ )");
#if 0
#error "DEAD CODE!"
                 // DQ (3/24/2015): Original code.
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
#else
#if 1
                 // DQ (3/24/2015): Original code (also required to fix test2015_21.C).
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
#endif
                 // DQ (3/24/2015): I think we want to unset ninfo (see test2015_30.c).
                    ninfo.unset_isReferenceToSomething();
                    ninfo.unset_isPointerToSomething();
#endif
                  }
                 else
                  {
#if DEBUG_ARRAY_TYPE
                    printf ("ninfo.isTypeSecondPart() == true: needParen == false: unparse the array index \n");
#endif
                  }

               curprint("[");

#if DEBUG_ARRAY_TYPE
            // DQ (6/3/2017): Added more debugging info.
               printf ("##### array_type = %p array_type->get_index() = %p = %s \n",array_type,array_type->get_index(),array_type->get_index() != NULL ? array_type->get_index()->class_name().c_str() : "null");
#endif
               if (array_type->get_index())
                  {
                 // JJW (12/14/2008): There may be types inside the size of an array, and they are not the second part of the type
                    SgUnparse_Info ninfo2(ninfo);
                    ninfo2.unset_isTypeSecondPart();
#if 0
                 // DQ (6/3/2017): I think a fix for multi-dimentional array types may be to disable this feature, 
                 // sense we already build the array type with a SgNullExpression for the index.
                    printf ("Test disabling supressArrayBound in array type \n");
                    ninfo2.unset_supressArrayBound();
#endif
#if 0
                    printf ("In Unparse_Type::unparseArrayType(): ninfo2.isReferenceToSomething() = %s \n",ninfo2.isReferenceToSomething() ? "true" : "false");
                    printf ("In Unparse_Type::unparseArrayType(): ninfo2.isPointerToSomething()   = %s \n",ninfo2.isPointerToSomething()   ? "true" : "false");
#endif
#if 0
                 // DQ (3/24/2015): I think that the index needs to have these be unset!
                    ROSE_ASSERT(ninfo2.isReferenceToSomething() == false);
                    ROSE_ASSERT(ninfo2.isPointerToSomething() == false);
#endif
#if 0
                    printf ("In unparseArrayType(): ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");
                    printf ("In unparseArrayType(): ninfo2.SkipEnumDefinition()  = %s \n",(ninfo2.SkipEnumDefinition() == true) ? "true" : "false");
#endif
                 // DQ (1/9/2014): These should have been setup to be the same.
                    ROSE_ASSERT(ninfo2.SkipClassDefinition() == ninfo2.SkipEnumDefinition());
#if DEBUG_ARRAY_TYPE
                    printf ("In unparseArrayType(): ninfo2.supressArrayBound()  = %s \n",(ninfo2.supressArrayBound() == true) ? "true" : "false");
#endif
                 // DQ (2/2/2014): Allow the array bound to be subressed (e.g. in secondary declarations of array variable using "[]" syntax.
                 // unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                    if (ninfo2.supressArrayBound() == false)
                       {
                      // Unparse the array bound.

                      // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
                      // DQ (2/12/2016): Adding support for variable length arrays.
                      // unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                      // SgExpression* indexExpression = array_type->get_index();

                      // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
                      // SgNullExpression* nullExpression = isSgNullExpression(indexExpression);

                      // DQ (2/14/2016): Since the array type's index is updated after seeing the function definition, we need to always use the VLA syntax for reliabily.
                      // if (nullExpression != NULL && array_type->get_is_variable_length_array() == true)
                         if (is_variable_length_array == true && isFunctionPrototype == true)
                            {
                           // The is the canonical normalized form for a type specified in a function parameter list of a prototype function.
                              curprint("*");
                            }
                           else
                            {
                           // unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                              if (is_variable_length_array == true && isFunctionPrototype == false)
                                 {
#if 0
                                   printf ("We need to output the expression used in the defining declaration's array type \n");
                                   curprint(" /* We need to output the expression used in the defining declaration's array type */ ");
#endif
#if 0
                                   printf ("In unparseArrayType(): array_type->get_index() = %p \n",array_type->get_index());
                                   printf ("In unparseArrayType(): array_type->get_index() = %p = %s \n",array_type->get_index(),array_type->get_index()->class_name().c_str());
#endif
                                   unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                                 }
                             // TV (09/21/2018): ROSE-1391: TODO: That should only happens when array_type->get_index() refers to a symbol that is not accessible
                                else if (array_type->get_number_of_elements() > 0)
                                 {
                                     std::ostringstream oss;
                                     oss << array_type->get_number_of_elements();
                                     curprint(oss.str());
                                 }
                                else
                                 {
                                   unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                                 }
                            }
                       }
                      else
                       {
#if DEBUG_ARRAY_TYPE
                         printf ("In unparseArrayType(): Detected info_for_type.supressArrayBound() == true \n");
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }

               curprint("]");

#if DEBUG_ARRAY_TYPE
               printf ("ninfo.isTypeSecondPart() == true: needParen = %s Calling unparseType(array_type->get_base_type(), ninfo); \n",needParen ? "true" : "false");
#endif
               unparseType(array_type->get_base_type(), info); // second part
#if DEBUG_ARRAY_TYPE
               printf ("DONE: ninfo.isTypeSecondPart() == true: needParen = %s Calling unparseType(array_type->get_base_type(), ninfo); \n",needParen ? "true" : "false");
#endif
             }
            else
             {
#if DEBUG_ARRAY_TYPE
               printf ("Calling unparseType(array_type, ninfo); with ninfo.set_isTypeFirstPart(); \n");
#endif
               ninfo.set_isTypeFirstPart();
               unparseType(array_type, ninfo);

#if DEBUG_ARRAY_TYPE
               printf ("Calling unparseType(array_type, ninfo); with ninfo.set_isTypeSecondPart(); \n");
#endif
               ninfo.set_isTypeSecondPart();
               unparseType(array_type, ninfo);
             }
        }

#if DEBUG_ARRAY_TYPE || 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     printf ("Leaving unparseArrayType(): type = %p \n",type);
     curprint("/* Leaving unparseArrayType() */ \n ");
#endif
   }



void
Unparse_Type::unparseTemplateType(SgType* type, SgUnparse_Info& info)
   {
     SgTemplateType* template_type = isSgTemplateType(type);
     ASSERT_not_null(template_type);

     ROSE_ASSERT(false);
   }

void
Unparse_Type::unparseAutoType(SgType* type, SgUnparse_Info& info)
   {
     SgAutoType* auto_type = isSgAutoType(type);
     ASSERT_not_null(auto_type);
     bool unparse_type = info.isTypeFirstPart() || ( !info.isTypeFirstPart() && !info.isTypeSecondPart() );
     if (unparse_type) {
       curprint("auto ");
     }
   }

#define DEBUG_UNPARSE_NONREAL_TYPE 0

void
Unparse_Type::unparseNonrealType(SgType* type, SgUnparse_Info& info, bool is_first_in_nonreal_chain)
   {

  // TV (03/29/2018): either first part is requested, or neither if called from unparseToString.
     bool unparse_type = info.isTypeFirstPart() || ( !info.isTypeFirstPart() && !info.isTypeSecondPart() );
     if (!unparse_type) return;

     SgNonrealType * nrtype = isSgNonrealType(type);
     ASSERT_not_null(nrtype);

#if DEBUG_UNPARSE_NONREAL_TYPE
     printf("In unparseNonrealType(type = %p): name = %s\n", type, nrtype->get_name().str());
#endif

     SgNonrealDecl * nrdecl = isSgNonrealDecl(nrtype->get_declaration());
     ASSERT_not_null(nrdecl);

     if (nrdecl->get_templateDeclaration() == NULL) {
       SgNode * parent = nrdecl->get_parent();
       ASSERT_not_null(parent);
       SgDeclarationScope * nrscope = isSgDeclarationScope(parent);
       if (nrscope == NULL) {
         printf("FATAL: Found a SgNonrealDecl (%p) whose parent is a %s (%p)\n", nrdecl, parent->class_name().c_str(), parent);
       }
       ASSERT_not_null(nrscope);

       parent = nrscope->get_parent();
       SgNonrealDecl * nrparent_nrscope = isSgNonrealDecl(parent);
#if DEBUG_UNPARSE_NONREAL_TYPE
       printf(" --- nrparent_nrscope = %p (%s)\n", nrparent_nrscope, nrparent_nrscope != NULL ? nrparent_nrscope->class_name().c_str() : NULL);
#endif
       if (nrparent_nrscope != NULL) {
         if (is_first_in_nonreal_chain) curprint("typename ");
         unparseNonrealType(nrparent_nrscope->get_type(), info, false);
         curprint("::");
       }

     } else if (info.get_reference_node_for_qualification()) {
       SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
#if DEBUG_UNPARSE_NONREAL_TYPE
       printf(" --- nameQualifier = %s\n", nameQualifier.str());
#endif
       curprint(nameQualifier.str());
     }

     SgTemplateArgumentPtrList & tpl_args = nrdecl->get_tpl_args();

     // if template argument are provided then the "template" keyword has to be added
  // if (tpl_args.size() > 0) curprint("template ");

     // output the name of the non-real type
     curprint(nrtype->get_name());

     // unparse template argument list
     if (tpl_args.size() > 0) {
#if DEBUG_UNPARSE_NONREAL_TYPE
       printf("  tpl_args.size() = %d\n", tpl_args.size());
#endif
       SgUnparse_Info ninfo(info);
       ninfo.set_SkipClassDefinition();
       ninfo.set_SkipEnumDefinition();
       ninfo.set_SkipClassSpecifier();
       unp->u_exprStmt->unparseTemplateArgumentList(tpl_args, ninfo);
     }

     curprint(" ");
   }

#if 0
void
Unparse_Type::foobar( SgUnparse_Info & info )
   {
  // DQ (5/5/2013): This function forces the instantiation of the required versions of the Unparse_Type::outputType() template function.

     SgInitializedName* initializedName   = NULL;
     SgTemplateArgument* templateArgument = NULL;

     SgType* referenceNodeType = NULL;

     outputType(initializedName,referenceNodeType,info);
     outputType(templateArgument,referenceNodeType,info);

     printf ("This function should not be called \n");
     ROSE_ASSERT(false);
   }
#else
// explicit instantiation of Unparse_Type::outputType
template void Unparse_Type::outputType(SgInitializedName*, SgType* , SgUnparse_Info &);
template void Unparse_Type::outputType(SgTemplateArgument*, SgType*, SgUnparse_Info &);

// DQ (9/4/2013): We need to support this instantiation as part of new support for SgCompoundLiteralExp 
// unparsing (via SgAggregateInitializer unparsing).
template void Unparse_Type::outputType(SgAggregateInitializer*, SgType*, SgUnparse_Info &);
#endif

template <class T>
void
Unparse_Type::outputType( T* referenceNode, SgType* referenceNodeType, SgUnparse_Info & info)
   {
  // DQ (5/4/2013): This code was copied from the function argument processing which does handle the types properly.
  // So this code needs to be refactored.

#define DEBUG_OUTPUT_TYPE 0

#if DEBUG_OUTPUT_TYPE
     printf ("In outputType(): referenceNode     = %p = %s \n",referenceNode,referenceNode->class_name().c_str());
     printf ("In outputType(): referenceNodeType = %p = %s \n",referenceNodeType,referenceNodeType->class_name().c_str());
     curprint(string("\n/* In outputType(): referenceNode = ") +  referenceNode->class_name() + " */ \n");
     curprint(string("\n/* In outputType(): referenceNodeType = ") +  referenceNodeType->class_name() + " */ \n");
#endif

#if 0
  // curprint(string("\n/* In outputType():  */ \n");
     curprint(string("\n/* In outputType(): referenceNode = ") +  referenceNode->class_name() + " */ \n");
     curprint(string("\n/* In outputType(): referenceNodeType = ") +  referenceNodeType->class_name() + " */ \n");
#endif

#if 0
  // DQ (9/10/2014): debugging code!
     if (isSgInitializedName(referenceNode) != NULL && isSgTypeInt(referenceNodeType) != NULL)
        {
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
        }
#endif

     SgUnparse_Info newInfo(info);

  // info.set_isTypeFirstPart();
     newInfo.set_isTypeFirstPart();
  // curprint( "\n/* unparse_helper(): output the 1st part of the type */ \n");

  // DQ (8/6/2007): Skip forcing the output of qualified names now that we have a hidden list mechanism.
  // DQ (10/14/2006): Since function can appear anywhere types referenced in function 
  // declarations have to be fully qualified.  We can't tell from the type if it requires 
  // qualification we would need the type and the function declaration (and then some 
  // analysis).  So fully qualify all function parameter types.  This is a special case
  // (documented in the Unparse_ExprStmt::unp->u_name->generateNameQualifier() member function.
  // info.set_forceQualifiedNames();

  // SgUnparse_Info ninfo_for_type(info);
     SgUnparse_Info ninfo_for_type(newInfo);

#if 1
  // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
  // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
  // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
  // if (initializedName->get_requiresGlobalNameQualificationOnType() == true)
     if (referenceNode->get_requiresGlobalNameQualificationOnType() == true)
        {
       // Output the name qualification for the type in the variable declaration.
       // But we have to do so after any modifiers are output, so in unparseType().
       // printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): This function parameter type requires a global qualifier \n");

       // Note that general qualification of types is separated from the use of globl qualification.
       // ninfo2.set_forceQualifiedNames();
          ninfo_for_type.set_requiresGlobalNameQualification();
        }
#endif

  // DQ (5/12/2011): Added support for newer name qualification implementation.
  // ninfo_for_type.set_name_qualification_length(initializedName->get_name_qualification_length_for_type());
  // ninfo_for_type.set_global_qualification_required(initializedName->get_global_qualification_required_for_type());
  // ninfo_for_type.set_type_elaboration_required(initializedName->get_type_elaboration_required_for_type());

     SgTemplateArgument* templateArgument = isSgTemplateArgument(referenceNode);

     if (templateArgument != NULL)
        {
#if DEBUG_OUTPUT_TYPE
          printf ("In outputType(): BEFORE: templateArgument->get_name_qualification_length_for_type()     = %d \n",templateArgument->get_name_qualification_length_for_type());
          printf ("In outputType(): BEFORE: templateArgument->get_global_qualification_required_for_type() = %s \n",templateArgument->get_global_qualification_required_for_type() ? "true" : "false");
          printf ("In outputType(): BEFORE: templateArgument->get_type_elaboration_required_for_type()     = %s \n",templateArgument->get_type_elaboration_required_for_type() ? "true" : "false");
#endif
       // Transfer values from old variables to the newly added variables (which will be required to support the refactoring into a template of common code.
          templateArgument->set_name_qualification_length_for_type    (templateArgument->get_name_qualification_length());
          templateArgument->set_global_qualification_required_for_type(templateArgument->get_global_qualification_required());
          templateArgument->set_type_elaboration_required_for_type    (templateArgument->get_type_elaboration_required());
#if 0
          printf ("In outputType(): AFTER: templateArgument->get_name_qualification_length_for_type()     = %d \n",templateArgument->get_name_qualification_length_for_type());
          printf ("In outputType(): AFTER: templateArgument->get_global_qualification_required_for_type() = %s \n",templateArgument->get_global_qualification_required_for_type() ? "true" : "false");
          printf ("In outputType(): AFTER: templateArgument->get_type_elaboration_required_for_type()     = %s \n",templateArgument->get_type_elaboration_required_for_type() ? "true" : "false");
#endif
        }
       else
        {
          SgInitializedName* initializedName = isSgInitializedName(referenceNode);
          if (initializedName != NULL)
             {
            // We don't have to transfer any data in this case.
             }
            else
             {
               SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(referenceNode);
               if (aggregateInitializer != NULL)
                  {
                 // We don't have to transfer any data in this case.
#if 0
                    printf ("Nothing to do for this case of referenceNode = %p = %s \n",referenceNode,referenceNode->class_name().c_str());
#endif
                  }
                 else
                  {
                    printf ("ERROR: referenceNode is not a supported type of IR node. referenceNode kind = %s \n",referenceNode->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }
        }

     ninfo_for_type.set_name_qualification_length    (referenceNode->get_name_qualification_length_for_type());
     ninfo_for_type.set_global_qualification_required(referenceNode->get_global_qualification_required_for_type());
     ninfo_for_type.set_type_elaboration_required    (referenceNode->get_type_elaboration_required_for_type());

  // DQ (5/29/2011): We have to set the associated reference node so that the type unparser can get the name qualification if required.
  // ninfo_for_type.set_reference_node_for_qualification(initializedName);
     ninfo_for_type.set_reference_node_for_qualification(referenceNode);

#if 0
     printf ("ninfo_for_type.set_reference_node_for_qualification(referenceNode): referenceNode = %p = %s \n",referenceNode,referenceNode->class_name().c_str());
#endif

  // TV (08/16/2018): enforce global qualification if required through the SgUnparse_Info (to circumvent the info assoc with the reference node)
     if (ninfo_for_type.requiresGlobalNameQualification()) {

#if 0
       printf ("WARNING: Setting the ninfo_for_type.set_reference_node_for_qualification(NULL) \n");
#endif

       ninfo_for_type.set_global_qualification_required(true);
       ninfo_for_type.set_reference_node_for_qualification(NULL);
     }

#if DEBUG_OUTPUT_TYPE
     printf ("In outputType(): ninfo_for_type.SkipClassDefinition() = %s \n",(ninfo_for_type.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In outputType(): ninfo_for_type.SkipEnumDefinition()  = %s \n",(ninfo_for_type.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(ninfo_for_type.SkipClassDefinition() == ninfo_for_type.SkipEnumDefinition());

#if DEBUG_OUTPUT_TYPE
     curprint("\n/* outputType(): output the 1st part of the type */ \n");
#endif

  // unparseType(tmp_type, info);
  // unp->u_type->unparseType(tmp_type, ninfo_for_type);
     unp->u_type->unparseType(referenceNodeType, ninfo_for_type);

#if DEBUG_OUTPUT_TYPE
     curprint("\n/* DONE - outputType(): output the 1st part of the type */ \n");
#endif

     SgInitializedName* initializedName = isSgInitializedName(referenceNode);
     if (initializedName != NULL)
        {
          SgName tmp_name  = initializedName->get_name();
       // DQ (5/4/2013): This would be the name of the variable of the specific type in the function 
       // parameter list, not wanted for the case of template arguments.

       // forward declarations don't necessarily need the name of the argument
       // so we must check if not NULL before adding to chars_on_line
       // This is a more consistant way to handle the NULL string case
       // curprint( "\n/* unparse_helper(): output the name of the type */ \n");
       // curprint(tmp_name.str());
       // curprint( "\n/* In unparseTemplateArgument(): <<< name of type >>> */ \n");
          curprint(tmp_name.str());
        }

  // output the rest of the type
  // info.set_isTypeSecondPart();
     newInfo.set_isTypeSecondPart();

  // info.display("unparse_helper(): output the 2nd part of the type");

  // printf ("unparse_helper(): output the 2nd part of the type \n");
#if DEBUG_OUTPUT_TYPE
     curprint( "\n/* unparse_helper(): output the 2nd part of the type */ \n");
#endif
  // unp->u_type->unparseType(tmp_type, info);
  // unp->u_type->unparseType(templateArgumentType, info);
     unp->u_type->unparseType(referenceNodeType, newInfo);

#if DEBUG_OUTPUT_TYPE
     printf ("DONE: outputType(): \n");
     curprint( "\n/* DONE: outputType(): */ \n");
#endif

#if 0
  // curprint(string("\n/* Leaving outputType() */ \n");
     curprint(string("\n/* Leaving outputType(): referenceNode = ") +  referenceNode->class_name() + " */ \n");
     curprint(string("\n/* Leaving outputType(): referenceNodeType = ") +  referenceNodeType->class_name() + " */ \n");
#endif

   }

