/* unparse_type.C
 * This C file contains the general function to unparse types as well as
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
  // printf ("t->class_name() = %s \n",t->class_name().c_str());

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
          case T_FLOAT:                   return "float";
          case T_DOUBLE:                  return "double";
          case T_LONG_LONG:               return "long long";
          case T_SIGNED_LONG_LONG:        return "signed long long";
          case T_UNSIGNED_LONG_LONG:      return "unsigned long long";

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
                ROSE_ASSERT(complexType != NULL);
                string returnString = get_type_name(complexType->get_base_type()) + " _Complex";
                return returnString;
             }

       // DQ (8/27/2006): Added require imaginary support to complete the complex support.
          case T_IMAGINARY:
             {
                SgTypeImaginary* imaginaryType = isSgTypeImaginary(t);
                ROSE_ASSERT(imaginaryType != NULL);
                string returnString = get_type_name(imaginaryType->get_base_type());

                bool usingGcc = false;
                #ifdef USE_CMAKE
                  #ifdef CMAKE_COMPILER_IS_GNUCC
                    usingGcc = true;
                  #endif
                #else
                  string backEndCompiler = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
                  usingGcc = (backEndCompiler == "g++" || backEndCompiler == "gcc" || backEndCompiler == "mpicc" || backEndCompiler == "mpicxx");
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
               printf ("Outp case T_DEFAULT: (int) \n");
#endif
               return "int";
             }

          case T_POINTER:
                {
#if 0
                  printf ("Outp case T_POINTER: \n");
#endif
                  SgPointerType* pointer_type = isSgPointerType(t);
                  ROSE_ASSERT(pointer_type != NULL);
                  return get_type_name(pointer_type->get_base_type()) + "*";
                }

          case T_REFERENCE:
                {
                  SgReferenceType* ref_type = isSgReferenceType(t);
                  ROSE_ASSERT(ref_type != NULL);
                  return get_type_name(ref_type->get_base_type()) + "&";
                }

          case T_MEMBER_POINTER:
               {
                  SgPointerMemberType* mpointer_type = isSgPointerMemberType(t);
                  ROSE_ASSERT(mpointer_type != NULL);

                  SgType *btype = mpointer_type->get_base_type();
                  SgMemberFunctionType *ftype = NULL;

                  if ( (ftype = isSgMemberFunctionType(btype)) != NULL) {
                      string res = get_type_name(ftype->get_return_type()) +  "("
                        +get_type_name(mpointer_type->get_class_type())
                        + "::*" + ")" + "(" ;
                      SgTypePtrList::iterator p = ftype->get_arguments().begin();
                      while ( p != ftype->get_arguments().end() )
                       {
                         res = res + get_type_name(*p);
                         p++;
                         if (p != ftype->get_arguments().end()) { res = res + ","; }
                       }
                       return res + ")";
                  }
                  else {
                     return get_type_name(btype) + "(" +
                       get_type_name(mpointer_type->get_class_type()) + "::*" + ")";
                  }
               }

          case T_CLASS:
              {
                SgClassType* class_type = isSgClassType(t);
                ROSE_ASSERT(class_type != NULL);
             // CH (4/7/2010): 'cdecl' is a keywork of MSVC
             // SgClassDeclaration* cdecl;
                SgClassDeclaration* decl = isSgClassDeclaration(class_type->get_declaration());
                SgName nm = decl->get_qualified_name();
#if 0
                printf ("In unparseType(%p): nm = %s \n",t,nm.str());
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
                SgName nm = edecl->get_qualified_name();

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
                    res = get_type_name(mod_type->get_base_type());
                    unparse_base = false;
                }
                if (mod_type->get_typeModifier().isOpenclGlobal())
                    res = "__global " + res;
                if (mod_type->get_typeModifier().isOpenclLocal())
                    res = "__local " + res;
                if (mod_type->get_typeModifier().isOpenclConstant())
                    res = "__constant " + res;
                if (mod_type->get_typeModifier().haveAddressSpace()) {
                    std::ostringstream outstr;
                    outstr << mod_type->get_typeModifier().get_address_space_value(); 
                    res = res + "__attribute__((address_space(" + outstr.str() + ")))";
                }
                if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())
                    res = res + "const ";
                if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile())
                    res = res + "volatile ";
                if (mod_type->get_typeModifier().isRestrict())
                   {
                  // DQ (8/29/2005): Added support for classification of back-end compilers (independent of the name invoked to execute them)

                     bool usingGcc = false;
                     #ifdef USE_CMAKE
                       #ifdef CMAKE_COMPILER_IS_GNUCC
                         usingGcc = true;
                       #endif
                     #else
                       string compilerName = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
                       usingGcc = (compilerName == "g++" || compilerName == "gcc" || compilerName == "mpicc" || compilerName == "mpicxx");
                     #endif

                     if ( usingGcc )
                        res = res + "__restrict__ ";
                     else
                        res = res + "restrict ";
                   }
                 if (unparse_base)
                    res = res + get_type_name(mod_type->get_base_type());
                  return res;
                }
#if 0
          case T_QUALIFIED_NAME:
             {
               SgQualifiedNameType* qualifiedNameType = isSgQualifiedNameType(t);
               ROSE_ASSERT(qualifiedNameType != NULL);
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
                ROSE_ASSERT (func_type != NULL);
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
               ROSE_ASSERT(mfunc_type != NULL);

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
               return res;
             }

          case T_ARRAY:
             {
              SgArrayType* array_type = isSgArrayType(t);
              ROSE_ASSERT(array_type != NULL);

              string res = get_type_name(array_type->get_base_type()) + "[";
              if (array_type->get_index())
                  res = res + array_type->get_index()->unparseToString();
               res = res + "]";
               return res;
             }

          case T_ELLIPSE: return "...";

          default:
             {
               printf("Error: unparse_type.C get_type_name(): Default case reached in switch: Unknown type \n");
               ROSE_ASSERT(false);
               break;
             }
        }
//#endif
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
     ROSE_ASSERT(type != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
     string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
     printf ("In Unparse_Type::unparseType(): type->class_name() = %s firstPart = %s secondPart = %s \n",
             type->class_name().c_str(),firstPartString.c_str(),secondPartString.c_str());
#endif
#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     curprint ( string("\n/* Top of unparseType name ") + type->class_name().c_str()
         + " firstPart " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif

#if 0
     printf ("In Unparse_Type::unparseType(): type = %p = %s \n",type,type->class_name().c_str());
#endif

#if 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint(string("\n/* Top of unparseType: class_name() = ") + type->class_name() + " */ \n");
#endif

#if 0
     printf ("In unparseType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition()  == true) ? "true" : "false");
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

#if 0
     printf ("In unparseType(): nodeReferenceToType = %p = %s \n",nodeReferenceToType,(nodeReferenceToType != NULL) ? nodeReferenceToType->class_name().c_str() : "null");
#endif

     if (nodeReferenceToType != NULL)
        {
#if 0
          printf ("rrrrrrrrrrrr In unparseType() output type generated name: nodeReferenceToType = %p = %s SgNode::get_globalTypeNameMap().size() = %zu \n",nodeReferenceToType,nodeReferenceToType->class_name().c_str(),SgNode::get_globalTypeNameMap().size());
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
#if 0
               printf ("ssssssssssssssss Found type name in SgNode::get_globalTypeNameMap() typeNameString = %s for nodeReferenceToType = %p = %s \n",typeNameString.c_str(),nodeReferenceToType,nodeReferenceToType->class_name().c_str());
#endif
             }
        }

#if 0
     printf ("In unparseType(): usingGeneratedNameQualifiedTypeNameString = %s \n",usingGeneratedNameQualifiedTypeNameString ? "true" : "false");
#endif
#if 0
     curprint ("\n /* In unparseType(): usingGeneratedNameQualifiedTypeNameString = " + string(usingGeneratedNameQualifiedTypeNameString ? "true" : "false") + " */ \n");
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
               case T_FLOAT:
               case T_DOUBLE:
               case T_LONG_LONG:
               case T_UNSIGNED_LONG_LONG:
               case T_SIGNED_LONG_LONG:

            // DQ (3/24/2014): Added support for 128-bit integers.
               case T_SIGNED_128BIT_INTEGER:
               case T_UNSIGNED_128BIT_INTEGER:

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

               default:
                  {
                    printf("Error: Unparse_Type::unparseType(): Default case reached in switch: Unknown type %p = %s \n",type,type->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
                  }
             }
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     printf ("Leaving Unparse_Type::unparseType(): type->sage_class_name() = %s firstPart = %s secondPart = %s \n",
          type->sage_class_name(),firstPartString.c_str(),secondPartString.c_str());
     curprint ( string("\n/* Bottom of unparseType name ") + type->sage_class_name()
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
     ROSE_ASSERT(decltype_node != NULL);

     ROSE_ASSERT(decltype_node->get_base_expression() != NULL);

     if (info.isTypeFirstPart() == true)
        {
          SgFunctionParameterRefExp* functionParameterRefExp = isSgFunctionParameterRefExp(decltype_node->get_base_expression());
          if (functionParameterRefExp != NULL)
             {
            // In this case just use the type directly.
               ROSE_ASSERT(decltype_node->get_base_type() != NULL);
#if 1
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
     ROSE_ASSERT (qualifiedNameType != NULL);

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

     ROSE_ASSERT(internalType != NULL);
  // ROSE_ASSERT(ninfo.get_qualifiedNameList() != NULL);

  // printf ("Processing a SgQualifiedNameType IR node base_type = %p = %s qualified name list size = %ld \n",
  //      type,type->class_name().c_str(),qualifiedNameType->get_qualifiedNameList().size());

  // Call unparseType on the base type with a modified SgUnparse_Info
     unparseType(internalType,ninfo);
#endif
   }
#endif


void Unparse_Type::unparsePointerType(SgType* type, SgUnparse_Info& info)
   {
#if 0
     printf("Inside of Unparse_Type::unparsePointerType \n");
     curprint("\n/* Inside of Unparse_Type::unparsePointerType */ \n");
#endif

#if 0
     info.display("Inside of Unparse_Type::unparsePointerType");
#endif

#if 0
     printf ("In unparsePointerType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     SgPointerType* pointer_type = isSgPointerType(type);
     ROSE_ASSERT(pointer_type != NULL);

#if 0
     printf ("In unparsePointerType(): isSgReferenceType(pointer_type->get_base_type())      = %s \n",(isSgReferenceType(pointer_type->get_base_type())      != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgPointerType(pointer_type->get_base_type())        = %s \n",(isSgPointerType(pointer_type->get_base_type())        != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgArrayType(pointer_type->get_base_type())          = %s \n",(isSgArrayType(pointer_type->get_base_type())          != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgFunctionType(pointer_type->get_base_type())       = %s \n",(isSgFunctionType(pointer_type->get_base_type())       != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgMemberFunctionType(pointer_type->get_base_type()) = %s \n",(isSgMemberFunctionType(pointer_type->get_base_type()) != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgModifierType(pointer_type->get_base_type())       = %s \n",(isSgModifierType(pointer_type->get_base_type())       != NULL) ? "true" : "false");
#endif

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
#if 0
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
#if 0
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
#if 0
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
#endif
        }
       else
        {
          if (info.isTypeSecondPart() == true)
             {
#if 0
               printf("In Unparse_Type::unparsePointerType(): unparse 2nd part of type \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): unparse 2nd part of type */ \n");
#endif
               unparseType(pointer_type->get_base_type(), info);
#if 0
               printf("DONE: Unparse_Type::unparsePointerType(): unparse 2nd part of type \n");
               curprint("\n/* DONE: Unparse_Type::unparsePointerType(): unparse 2nd part of type */ \n");
#endif
             }
            else
             {
            // DQ (11/27/2004): I think that this is important for unparing functions or function pointers
               SgUnparse_Info ninfo(info);
               ninfo.set_isTypeFirstPart();
#if 0
               printf("In Unparse_Type::unparsePointerType(): (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): (call on base type: part 1) */ \n");
#endif
               unparseType(pointer_type, ninfo);
               ninfo.set_isTypeSecondPart();
#if 0
               printf("In Unparse_Type::unparsePointerType(): (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): (call on base type: part 2) */ \n");
#endif
               unparseType(pointer_type, ninfo);
             }
        }

#if 0
     printf("Leaving of Unparse_Type::unparsePointerType \n");
     curprint("\n /* Leaving of Unparse_Type::unparsePointerType */ \n");
#endif
   }


void Unparse_Type::unparseMemberPointerType(SgType* type, SgUnparse_Info& info)
   {
     SgPointerMemberType* mpointer_type = isSgPointerMemberType(type);
     ROSE_ASSERT(mpointer_type != NULL);

#if 0
     printf ("In unparseMemberPointerType: mpointer_type = %p \n",mpointer_type);
#endif

  // plain type :  int (P::*)
  // type with name:  int P::* pmi = &X::a;
  // use: obj.*pmi=7;
     SgType *btype = mpointer_type->get_base_type();
     SgMemberFunctionType *ftype = NULL;

#if 0
     printf ("In unparseMemberPointerType(): btype = %p = %s \n",btype,(btype != NULL) ? btype->class_name().c_str() : "NULL" );
     curprint("\n/* In unparseMemberPointerType() */ \n");
#endif

     if ( (ftype = isSgMemberFunctionType(btype)) != NULL)
        {
       /* pointer to member function data */
       // printf ("In unparseMemberPointerType(): pointer to member function data \n");

          if (info.isTypeFirstPart())
             {
               unparseType(ftype->get_return_type(), info); // first part
               curprint ( "(");
            // curprint ( "\n/* mpointer_type->get_class_of() = " + mpointer_type->get_class_of()->sage_class_name() + " */ \n";
               curprint ( get_type_name(mpointer_type->get_class_type()) );
               curprint ( "::*");
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
                    curprint(")");

                 // argument list
                    SgUnparse_Info ninfo(info);
                    ninfo.unset_SkipBaseType();
                    ninfo.unset_isTypeSecondPart();
                    ninfo.unset_isTypeFirstPart();
#if 0
                    curprint("\n/* In unparseMemberPointerType(): start of argument list */ \n");
#endif
                    curprint("(");

                    SgTypePtrList::iterator p = ftype->get_arguments().begin();
                    while ( p != ftype->get_arguments().end() )
                       {
#if 0
                         printf ("In unparseMemberPointerType: output the arguments *p = %p = %s \n",*p,(*p)->class_name().c_str());
#endif
#if 0
                         curprint("\n/* In unparseMemberPointerType(): output function argument type */ \n");
#endif
                         unparseType(*p, ninfo);
#if 0
                         curprint("\n/* In unparseMemberPointerType(): DONE: output function argument type */ \n");
#endif

                         p++;
                         if (p != ftype->get_arguments().end()) { curprint ( ", "); }
                       }
                    curprint(")");
                 // curprint("\n/* In unparseMemberPointerType(): end of argument list */ \n";

                    unparseType(ftype->get_return_type(), info); // second part

                 // Liao, 2/27/2009, add "const" specifier to fix bug 327
                    if (ftype->isConstFunc())
                       {
                         curprint(" const ");
                       }
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
#if 0
          printf ("In unparseMemberPointerType(): pointer to member data \n");
#endif
          if (info.isTypeFirstPart())
             {
            // DQ (9/16/2004): This appears to be an error, btype should not be unparsed here (of maybe btype is not set properly)!
            // printf ("Handling the first part \n");
            // curprint ( "\n/* start of btype */ \n";
               unparseType(btype, info);
            // curprint ( "\n/* end of btype */ \n";
               curprint ( "(");
               curprint ( get_type_name(mpointer_type->get_class_type()) );
               curprint ( "::*");
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
#if 0
                    printf ("Handling the second part \n");
#endif
                    curprint(")");

                 // DQ (8/19/2014): Handle array types (see test2014_129.C).
                    SgArrayType* arrayType = isSgArrayType(btype);
                    if (arrayType != NULL)
                       {
#if 0
                         printf ("Handling the array type \n");
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
                    SgUnparse_Info ninfo(info);
                    ninfo.set_isTypeFirstPart();
                    unparseType(mpointer_type, ninfo);
                    ninfo.set_isTypeSecondPart();
                    unparseType(mpointer_type, ninfo);
                  }
             }
        }

#if 0
     printf ("Leaving unparseMemberPointerType() \n");
     curprint("\n/* Leaving unparseMemberPointerType() */ \n");
#endif
   }


void Unparse_Type::unparseReferenceType(SgType* type, SgUnparse_Info& info)
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

void Unparse_Type::unparseRvalueReferenceType(SgType* type, SgUnparse_Info& info)
   {
     SgRvalueReferenceType* rvalue_ref_type = isSgRvalueReferenceType(type);
     ROSE_ASSERT(rvalue_ref_type != NULL);

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
     ROSE_ASSERT(named_type != NULL);

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
#if 0
     printf ("Inside of Unparse_Type::unparseClassType type = %p \n",type);
     curprint("/* Inside of Unparse_Type::unparseClassType */ \n");
#endif

#if 0
     info.display("Inside of Unparse_Type::unparseClassType");
#endif

#if 0
     printf ("In unparseClassType(): ninfo.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseClassType(): ninfo.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif
  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());


  // CH (4/7/2010): This issue is because of using a MSVC keyword 'cdecl' as a variable name

     SgClassType* class_type = isSgClassType(type);
     ROSE_ASSERT(class_type != NULL);

  // DQ (6/22/2006): test2006_76.C demonstrates a problem with this code
  // SgClassDeclaration *cdecl = isSgClassDeclaration(class_type->get_declaration());
     SgClassDeclaration *decl = isSgClassDeclaration(class_type->get_declaration());
     ROSE_ASSERT(decl != NULL);

  // DQ (7/28/2013): Added assertion.
     ROSE_ASSERT(decl == decl->get_firstNondefiningDeclaration());

#if 0
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
          ROSE_ASSERT(class_type->get_declaration() != NULL);
          if (decl->get_definingDeclaration() != NULL)
             {
               ROSE_ASSERT(decl->get_definingDeclaration() != NULL);
#if 1
#if 0
               printf ("In Unparse_Type::unparseClassType(): Resetting decl to be the defining declaration from decl = %p to decl = %p \n",decl,decl->get_definingDeclaration());
#endif
            // DQ (9/23/2012): Original version of code.
               decl = isSgClassDeclaration(decl->get_definingDeclaration());
               ROSE_ASSERT(decl != NULL);
               ROSE_ASSERT(decl->get_definition() != NULL);
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
#if 0
               printf ("Can't find a class declaration with an attached definition! \n");
#endif
             }
        }

  // DQ (7/28/2013): Added assertion.
     ROSE_ASSERT(decl == decl->get_definingDeclaration() || decl->get_definingDeclaration() == NULL);

  // GB (09/19/2007): This is the defining declaration of the class, it might have preprocessing information attached to it.
     SgClassDeclaration *cDefiningDecl = isSgClassDeclaration(decl->get_definingDeclaration());

#if 0
     printf ("In unparseClassType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparseClassType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparseClassType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparseClassType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
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
          if(!info.SkipClassSpecifier())
             {
            // GB (09/18/2007): If the class definition is unparsed, also unparse its
            // attached preprocessing info.
               if (cDefiningDecl != NULL && !info.SkipClassDefinition())
                  {
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::before);
                  }
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

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
     if (info.isTypeSecondPart() == false)
        {
       // DQ (11/22/2004): New code using refactored code using explicitly stored scope to compute the qualified name
       // this version should be more robust in generating correct qualified names when the parent is inconsistant
       // with the explicitly stored scope (which happens in rare cases, but particularly in KULL and for va_list
       // bases typedefed types).
#if 0
          printf ("In unparseClassType(): info.PrintName() = %s decl->get_isUnNamed() = %s \n",(info.PrintName() == true) ? "true" : "false",decl->get_isUnNamed() ? "true" : "false");
#endif
       // DQ (7/28/2012): Added support for un-named types in typedefs.
       // SgName nm = decl->get_name();
          SgName nm;
#if 1
          if (decl->get_isUnNamed() == false || info.PrintName() == true)
             {
               nm = decl->get_name();
#if 0
               printf ("In unparseClassType(): nm = %s \n",nm.str());
#endif
             }
#else
          if (decl->get_isUnNamed() == false)
             {
               nm = decl->get_name();
             }
            else

#error "DEAD CODE!"

             {
            // Else if this is a declaration in a variable declaration, then we do want to output a generated name.
            // We could also mark the declaration for the cases where this is required. See test2012_141.C for this case.
               ROSE_ASSERT(decl->get_parent() != NULL);
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(decl->get_parent());
               if (variableDeclaration != NULL)
                  {
                    nm = decl->get_name();
                  }
             }
#endif
       // printf ("In unparseClassType: nm = %s \n",nm.str());

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
#if 0
                    curprint ( string("\n/* In unparseClassType: info.forceQualifiedNames() = ") + ((info.forceQualifiedNames() == true) ? "true" : "false") + " */ \n");
                 // curprint ( "\n/* cdecl->get_need_name_qualifier() = " + (cdecl->get_need_name_qualifier() == true ? "true" : "false") + " */ \n";
                    curprint ( string("\n/* decl->get_scope() = ") + decl->get_scope()->class_name() + " */\n ");
                    curprint ( string("\n/* info.get_current_namespace() = ") + ((info.get_current_namespace() != NULL) ? info.get_current_namespace()->class_name() : "no namespace in use") + " */\n ");
                    curprint ( string("\n/* info.get_declstatement_ptr() = ") + ((info.get_declstatement_ptr() != NULL) ? info.get_declstatement_ptr()->class_name() : "no declaration statement being generated") + " */\n ");
                 // curprint ( "\n/* SageInterface::get_name(info.get_declstatement_ptr()) = " + ((info.get_declstatement_ptr() != NULL) ? SageInterface::get_name(info.get_declstatement_ptr()) : "no declaration statement available") + " */\n ");
#endif

                 // info.display("In unparseClassType: The C++ support is more complex and can require qualified names");
#if 0
                 // The C++ support is more complex and can require qualified names!
                 // DQ (5/29/2011): Newest support for name qualification...
                    SgName nameQualifier;

                 // DQ (6/22/2011): I don't think we can assert this for anything than internal testing.  The unparseToString tests will fail with this assertion in place.
                 // ROSE_ASSERT(info.get_reference_node_for_qualification() != NULL);
                    if (info.get_reference_node_for_qualification() != NULL)
                       {
                      // printf ("In unparseClassType: info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());

#error "DEAD CODE!"
                      // DQ (6/2/2011): Newest support for name qualification...
                         nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
                       }
                      else
                       {
                      // printf ("In unparseClassType: info.get_reference_node_for_qualification() == NULL \n");
                       }

                 // SgName nameQualifier = unp->u_name->generateNameQualifierForType( type , info );
#if 0
                    printf ("In unparseClassType: nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
                 // curprint ( string("\n/* In unparseClassType: nameQualifier (from unp->u_name->generateNameQualifier function) = ") + nameQualifier + " */ \n ");
#endif
                    curprint ( nameQualifier.str());

                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(decl);
                    if (isSgTemplateInstantiationDecl(decl) != NULL)
                       {
                      // Handle case of class template instantiation (code located in unparse_stmt.C)
                         unp->u_exprStmt->unparseTemplateName(templateInstantiationDeclaration,info);
                       }
#error "DEAD CODE!"
                      else
                       {
                         curprint ( string(nm.str()) + " ");
                      // printf ("class type name: nm = %s \n",nm.str());
                       }
#else
                 // DQ (6/25/2011): Fixing name qualifiction to work with unparseToString().  In this case we don't 
                 // have an associated node to reference as a way to lookup the strored name qualification.  In this 
                 // case we return a fully qualified name.
                    if (info.get_reference_node_for_qualification() == NULL)
                       {
#if 0
                         printf ("WARNING: In unparseClassType(): info.get_reference_node_for_qualification() == NULL (assuming this is for unparseToString() \n");
#endif
                         SgName nameQualifierAndType = class_type->get_qualified_name();
                         curprint(nameQualifierAndType.str());
                       }
                      else
                       {
                      // DQ (6/2/2011): Newest support for name qualification...
#if 0
                         printf ("info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif
                         SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
#if 0
                         printf ("nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
#endif
#if 0
                      // printf ("nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
                      // curprint ("\n/* nameQualifier (from unp->u_name->generateNameQualifier function) = " + nameQualifier + " */ \n ";
                         curprint (nameQualifier.str());
                         SgName nm = class_type->get_name();

#error "DEAD CODE!"
                         if (nm.getString() != "")
                            {
                           // printf ("Output qualifier of current types to the name = %s \n",nm.str());
                              curprint ( nm.getString() + " ");
                            }
#endif
                      // SgName nameQualifier = unp->u_name->generateNameQualifierForType( type , info );
#if 0
                         printf ("In unparseClassType: nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
                      // curprint ( string("\n/* In unparseClassType: nameQualifier (from unp->u_name->generateNameQualifier function) = ") + nameQualifier + " */ \n ");
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
                              curprint ( string(nm.str()) + " ");
#if 0
                              printf ("class type name: nm = %s \n",nm.str());
#endif
                            }
                       }
#endif

                  }
             }
            else
             {
            // DQ (10/23/2012): Added support for types of references to un-named class/struct/unions to always include their definitions.
#if 0
               printf ("In unparseClassType: This is an un-named class declaration: decl = %p name = %s (need to output its class definition) decl->get_definition() = %p \n",decl,decl->get_name().str(),decl->get_definition());
               printf ("In unparseClassType: Detected un-nmed class declaration: Calling  unset_SkipClassDefinition() \n");
#endif
               info.unset_SkipClassDefinition();

            // DQ (1/9/2014): Mark Enum and Class declaration handling consistantly (enforces within the unparser now).
               info.unset_SkipEnumDefinition();
#if 0
               printf ("In unparseClassType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseClassType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif
            // DQ (1/9/2014): These should have been setup to be the same.
               ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());
             }
        }

#if 0
     printf ("In unparseClassType: info.SkipClassDefinition()          = %s \n",(info.SkipClassDefinition()          == true) ? "true" : "false");
     printf ("In unparseClassType: decl->isForward()                   = %s \n",(decl->isForward()                   == true) ? "true" : "false");
     printf ("In unparseClassType: decl->get_isUnNamed()               = %s \n",(decl->get_isUnNamed()               == true) ? "true" : "false");
     printf ("In unparseClassType: decl->get_isAutonomousDeclaration() = %s \n",(decl->get_isAutonomousDeclaration() == true) ? "true" : "false");
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
          if ( !info.SkipClassDefinition() )
             {
            // DQ (8/17/2006): Handle the case where the definition does not exist (there may still be a pointer to the type).
               SgClassDefinition* classdefn_stmt = decl->get_definition();
#if 0
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

                    ninfo.set_isUnsetAccess();
                    curprint("{");
                    if (classdefn_stmt == NULL)
                       {
                         printf ("Error: In unparseClassType(): classdefn_stmt = NULL decl = %p = %s \n",decl,decl->get_name().str());
                       }
                    ROSE_ASSERT(classdefn_stmt != NULL);
#if 0
                    printf ("In unparseClassType: classdefn_stmt = %p classdefn_stmt->get_members().size() = %zu \n",classdefn_stmt, classdefn_stmt->get_members().size());
#endif
                    SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();
                    while (pp != classdefn_stmt->get_members().end())
                       {
                      // DQ (10/18/2012): If this is in the context of a conditional then the ";" will be supressed.
                      // We could explicitly output a ";" in this case if required.
#if 0
                         printf ("In unparseClassType: output member declaration: %p ninfo.isSkipSemiColon() = %s \n",*pp,ninfo.SkipSemiColon() ? "true" : "false");
#endif
#if 0
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
                    if (cDefiningDecl->get_definition() != NULL)
                       {
                         unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl->get_definition(), info, PreprocessingInfo::inside);
                       }
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::inside);

                    curprint("}");

                 // DQ (6/13/2007): Set to null before resetting to non-null value
                 // DQ (11/29/2004): Restore context saved above before unparsing declaration.
                    ninfo.set_current_context(NULL);
                    ninfo.set_current_context(saved_context);
                  }
                 else
                  {
#if 0
                    printf ("classdefn_stmt not found for decl = %p \n",decl);
#endif
                  }
               
            // GB (09/18/2007): If the class definition is unparsed, also unparse its attached preprocessing info.
               if (cDefiningDecl != NULL)
                  {
                    unp->u_exprStmt->unparseAttachedPreprocessingInfo(cDefiningDecl, info, PreprocessingInfo::after);
                  }
             }
        }
//#endif

#if 0
     printf ("Leaving Unparse_Type::unparseClassType \n");
     curprint("/* Leaving Unparse_Type::unparseClassType */ \n");
#endif
   }


void
Unparse_Type::unparseEnumType(SgType* type, SgUnparse_Info& info)
   {
     SgEnumType* enum_type = isSgEnumType(type);
     ROSE_ASSERT(enum_type);

#if 0
     printf ("Inside of unparseEnumType(): info.isTypeFirstPart() = %s info.isTypeSecondPart() = %s \n",(info.isTypeFirstPart() == true) ? "true" : "false",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif
#if 0
     printf ("Inside of unparseEnumType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Inside of unparseEnumType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true)  ? "true" : "false");
#endif
#if 0
     info.display("Inside of unparseEnumType(): call to info.display()");
#endif

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
     if (info.isTypeSecondPart() == false)
        {
          SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
          SgClassDefinition *cdefn = NULL;
          SgNamespaceDefinitionStatement* namespaceDefn = NULL;
#if 0
          printf ("Inside of unparseEnumType(): edecl = %p = %s \n",edecl,edecl->class_name().c_str());
#endif
          ROSE_ASSERT(edecl != NULL);

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
#if 0
                printf ("Inside of unparseEnumType(): output enum keyword \n");
                curprint ("/* enum from unparseEnumType() */ ");
#endif
             }
            else
             {
#if 0
               printf ("Inside of unparseEnumType(): DO NOT output enum keyword \n");
#endif
             }

       // DQ (9/14/2013): For C language we need to output the "enum" keyword (see test2013_71.c).
          if ( (info.isTypeFirstPart() == false) && (info.SkipClassSpecifier() == false) && (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true) )
             {
               curprint ("enum ");
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
#if 0
          if ( outputQualifiedName == false )
             {
            // (10/15/2001): Bugfix
            // Note that the name of the class is allowed to be empty in the case of:
            //      typedef struct <no tag required here> { int x; } y;
               if (enum_type->get_name().str() != NULL)
                  {
                 // printf ("enum_type->get_name().str() = %s \n",enum_type->get_name().str());
                    curprint ( enum_type->get_name().str() + " ");
                  }
                 else
                  {
                 // printf ("enum_type->get_name().str() == NULL \n");
                  }
             }
            else
             {
            // add qualifier of current types to the name
            // The C++ support is more complex and can require qualified names!
               SgName nameQualifier = unp->u_name->generateNameQualifier( edecl , info );

            // printf ("nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
            // curprint ( "\n/* nameQualifier (from unp->u_name->generateNameQualifier function) = " + nameQualifier + " */ \n ";
               curprint ( nameQualifier.str());
               SgName nm = enum_type->get_name();

               if (nm.str() != NULL)
                  {
                 // printf ("Output qualifier of current types to the name = %s \n",nm.str());
                    curprint ( nm.str() + " ");
                  }
             }
#else
#if 0
            // The C++ support is more complex and can require qualified names!
               SgName nameQualifier = unp->u_name->generateNameQualifier( edecl , info );
#else
            // DQ (6/22/2011): I don't think we can assert this for anything than internal testing.  The unparseToString tests will fail with this assertion in place.
            // ROSE_ASSERT(info.get_reference_node_for_qualification() != NULL);

#if 1
#else
            // DQ (5/29/2011): Newest support for name qualification...
               SgName nameQualifier;

#error "DEAD CODE!"

               printf ("info.get_reference_node_for_qualification() = %s \n",info.get_reference_node_for_qualification()->class_name().c_str());
               SgInitializedName* initializedName = isSgInitializedName(info.get_reference_node_for_qualification());
            // ROSE_ASSERT(initializedName != NULL);
               if (initializedName != NULL)
                  {
                    nameQualifier = initializedName->get_qualified_name_prefix_for_type();
                  }
                 else
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(info.get_reference_node_for_qualification());
                 // ROSE_ASSERT(initializedName != NULL);

#error "DEAD CODE!"

                    if (typedefDeclaration != NULL)
                       {
                         nameQualifier = typedefDeclaration->get_qualified_name_prefix_for_base_type();
                       }
                      else
                       {
                         ROSE_ASSERT(info.get_reference_node_for_qualification() != NULL);
                         printf ("Sorry not supported, info.get_reference_node_for_qualification() = %s \n",info.get_reference_node_for_qualification()->class_name().c_str());
                       }
                  }
#endif
#endif

#if 0
            // DQ (6/2/2011): Newest support for name qualification...
               SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
            // printf ("nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
            // curprint ( "\n/* nameQualifier (from unp->u_name->generateNameQualifier function) = " + nameQualifier + " */ \n ";
               curprint ( nameQualifier.str());
               SgName nm = enum_type->get_name();

#error "DEAD CODE!"

               if (nm.getString() != "")
                  {
                 // printf ("Output qualifier of current types to the name = %s \n",nm.str());
                    curprint ( nm.getString() + " ");
                  }
#else
            // DQ (6/25/2011): Fixing name qualifiction to work with unparseToString().  In this case we don't 
            // have an associated node to reference as a way to lookup the strored name qualification.  In this 
            // case we return a fully qualified name.
               if (info.get_reference_node_for_qualification() == NULL)
                  {
                    SgName nameQualifierAndType = enum_type->get_qualified_name();
#if 0
                    printf ("WARNING: In unparseEnumType(): info.get_reference_node_for_qualification() == NULL (assuming this is for unparseToString() nameQualifierAndType = %s \n",nameQualifierAndType.str());
#endif
                    curprint(nameQualifierAndType.str());
                  }
                 else
                  {

                 // DQ (6/2/2011): Newest support for name qualification...
                    SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
#if 0
                    printf ("In unparseEnumType(): nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
                 // printf ("nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
                    curprint ("\n/* nameQualifier (from unp->u_name->generateNameQualifier function) = " + nameQualifier + " */ \n ");
#endif
                    curprint (nameQualifier.str());
#if 1
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
                         ROSE_ASSERT(edecl->get_parent() != NULL);
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
#else

#error "DEAD CODE!"

                    SgName nm = enum_type->get_name();
#endif

                    if (nm.getString() != "")
                       {
#if 0
                         printf ("In unparseEnumType(): Output qualifier of current types to the name = %s \n",nm.str());
#endif
                         curprint ( nm.getString() + " ");
                       }
                  }
#endif
#endif
             }
        }

#if 0
     printf ("In unparseEnumType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseEnumType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
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
#if 0
         printf ("info.SkipEnumDefinition() = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif
         if ( info.SkipEnumDefinition() == false)
            {
              SgUnparse_Info ninfo(info);
              ninfo.set_inEnumDecl();
              SgInitializer *tmp_init = NULL;
              SgName tmp_name;

              SgEnumDeclaration* enum_stmt = isSgEnumDeclaration(enum_type->get_declaration());
              ROSE_ASSERT(enum_stmt != NULL);

           // DQ (5/8/2013): Make sure this is a valid pointer.
              if (enum_stmt->get_definingDeclaration() == NULL)
                 {
                   printf ("enum_stmt = %p = %s \n",enum_stmt,enum_stmt->class_name().c_str());
                 }
              ROSE_ASSERT(enum_stmt->get_definingDeclaration() != NULL);

           // DQ (4/22/2013): We need the defining declaration.
              enum_stmt = isSgEnumDeclaration(enum_stmt->get_definingDeclaration());

           // This fails for test2007_140.C.
              ROSE_ASSERT(enum_stmt != NULL);

           // DQ (6/26/2005): Output the opend and closing braces even if there are no enumerators!
           // This permits support of the empty enum case! "enum x{};"
              curprint ("{");
#if 0
              printf ("In unparseEnumType(): Output enumerators from enum_stmt = %p \n",enum_stmt);
              printf ("     --- enum_stmt->get_firstNondefiningDeclaration() = %p \n",enum_stmt->get_firstNondefiningDeclaration());
              printf ("     --- enum_stmt->get_definingDeclaration() = %p \n",enum_stmt->get_definingDeclaration());
#endif
              SgInitializedNamePtrList::iterator p = enum_stmt->get_enumerators().begin();
              if (p != enum_stmt->get_enumerators().end())
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
                        if (p != enum_stmt->get_enumerators().end())
                           {
                             curprint ( ",");
                           }
                          else
                             break;
                       }
                 // curprint ( "}";
                  }

            // GB (09/18/2007): If the enum definition is unparsed, also unparse its attached preprocessing info.
            // Putting the "inside" info right here is just a wild guess as to where it might really belong.
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(enum_stmt, info, PreprocessingInfo::inside);

            // DQ (6/26/2005): Support for empty enum case!
               curprint ("}");

            // GB (09/18/2007): If the enum definition is unparsed, also unparse its attached preprocessing info.
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(enum_stmt, info, PreprocessingInfo::after);
             }
        }
   }


void
Unparse_Type::unparseTypedefType(SgType* type, SgUnparse_Info& info)
   {
     SgTypedefType* typedef_type = isSgTypedefType(type);
     ROSE_ASSERT(typedef_type != NULL);

#if 0
     printf ("Inside of Unparse_Type::unparseTypedefType name = %p = %s \n",typedef_type,typedef_type->get_name().str());
  // curprint ( "\n/* Inside of Unparse_Type::unparseTypedefType */ \n";
#endif
#if 0
     curprint ( string("\n /* info.isWithType()       = ") + ((info.isWithType()       == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* info.SkipBaseType()     = ") + ((info.SkipBaseType()     == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* info.isTypeSecondPart() = ") + ((info.isTypeSecondPart() == true) ? "true" : "false") + " */ \n");
#endif
#if 0
     printf ("In unparseTypedefType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     if ((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
        {
       /* do nothing */;
#if 0
          printf ("Inside of Unparse_Type::unparseTypedefType (do nothing) \n");
#endif
       // curprint ( "\n /* Inside of Unparse_Type::unparseTypedefType (do nothing) */ \n");
        }
       else
        {
       // could be a scoped typedef type
       // check if currrent type's parent type is the same as the context type */
       // SgNamedType *ptype = NULL;
#if 0
          printf ("Inside of Unparse_Type::unparseTypedefType (normal handling) \n");
#endif
       // curprint ( "\n /* Inside of Unparse_Type::unparseTypedefType (normal handling) */ \n";

          SgTypedefDeclaration *tdecl = isSgTypedefDeclaration(typedef_type->get_declaration());
          ROSE_ASSERT (tdecl != NULL);

       // DQ (10/16/2004): Keep this error checking for now!
          ROSE_ASSERT(typedef_type != NULL);
          ROSE_ASSERT(typedef_type->get_declaration() != NULL);
#if 0
          printf ("typedef_type->get_declaration() = %p = %s \n",typedef_type->get_declaration(),typedef_type->get_declaration()->sage_class_name());
#endif
       // DQ (10/17/2004): This assertion forced me to set the parents of typedef in the EDG/Sage connection code
       // since I could not figure out why it was not being set in the post processing which sets parents.
          ROSE_ASSERT(typedef_type->get_declaration()->get_parent() != NULL);

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
            // ROSE_ASSERT(info.get_qualifiedNameList() != NULL);
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
            // ROSE_ASSERT(info.get_reference_node_for_qualification() != NULL);
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
                    curprint(nameQualifierAndType.str());
                  }
                 else
                  {
                    SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
                    curprint(nameQualifier.str());
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
#endif
             }
#endif

#if 0
#if PRINT_DEVELOPER_WARNINGS
          if (tdecl->get_parent() == NULL)
               printf ("Warning: SgTypedefDeclaration does not have a parent (tdecl->get_parent() == NULL) in %s on line %d \n",__FILE__,__LINE__);
#endif
       // ROSE_ASSERT (tdecl->get_parent() != NULL);

#if 1
          ROSE_ASSERT(tdecl->get_parent() != NULL);
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
                         curprint ( nm.str() + "::" + typedef_type->get_name().str() + " ");
                       }
                      else
                       {
                      // printf ("#2 typedef_type->get_name() = %s \n",typedef_type->get_name().str());
                         curprint ( typedef_type->get_name().str() + " ");
                       }
                  }
             }
            else
             {
            // printf ("#3 typedef_type->get_name() = %s \n",typedef_type->get_name().str());
               curprint ( typedef_type->get_name().str() + " ");
             }
#endif
        }

#if 0
     printf ("Leaving Unparse_Type::unparseTypedefType \n");
  // curprint ( "\n/* Leaving Unparse_Type::unparseTypedefType */ \n";
#endif
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
    string compilerName = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
    usingGcc = (compilerName == "g++" || compilerName == "gcc" || compilerName == "mpicc" || compilerName == "mpicxx");
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
               returnString = " __restrict__ ";
             }
            else
             {
            // DQ (12/12/2012): We need the white space before and after the keyword.
               returnString = " restrict ";
             }
        }

     return returnString;
   }


void Unparse_Type::unparseModifierType(SgType* type, SgUnparse_Info& info)
   {
     SgModifierType* mod_type = isSgModifierType(type);
     ROSE_ASSERT(mod_type != NULL);

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
               ROSE_ASSERT(inner_mod_type->get_base_type() != NULL);
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
          if (btype_first)
               unparseType(mod_type->get_base_type(), info);

          if (mod_type->get_typeModifier().isOpenclGlobal())
              curprint ( "__global ");
          if (mod_type->get_typeModifier().isOpenclLocal())
              curprint ( "__local ");
          if (mod_type->get_typeModifier().isOpenclConstant())
              curprint ( "__constant ");
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
          printf ("mod_type->get_typeModifier().isRestrict() = %s \n",mod_type->get_typeModifier().isRestrict() ? "true" : "false");
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
     ROSE_ASSERT (func_type != NULL);

  // DQ (1/8/2014): debugging test2014_25.c.
  // info.unset_isPointerToSomething();

     SgUnparse_Info ninfo(info);
     int needParen = 0;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen = 1;
        }

#if 0
     printf ("In unparseFunctionType(): needParen = %d \n",needParen);
     curprint("\n/* In unparseFunctionType: needParen = " + StringUtility::numberToString(needParen) + " */ \n");
#endif

#if 0
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
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( "\n/* In unparseFunctionType: handling first part */ \n");
          curprint ( "\n/* Skipping the first part of the return type! */ \n");
#endif
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
            // DQ (9/21/2004): we don't want this for typedefs of function pointers where the function return type is a pointer
               printf ("Skipping the first part of the return type (in needParen == true case)! \n");
               curprint ( "\n/* Skipping the first part of the return type (in needParen == true case)! */ \n");
#endif
#if OUTPUT_DEBUGGING_UNPARSE_INFO
               curprint ( string("\n/* ") + ninfo.displayString("Skipping the first part of the return type (in needParen == true case)") + " */ \n");
#endif
               unparseType(func_type->get_return_type(), ninfo);
               curprint("(");
            // curprint("/* unparseFunctionType */ (");
             }
            else
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
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
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               curprint ( "\n/* In unparseFunctionType: handling second part */ \n");
#endif
               if (needParen)
                  {
#if 0
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
#if 0
               printf ("Setting reference_node_for_qualification to SgFunctionType, but this is not correct where name qualification is required. \n");
#endif
#if 0
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
#if 0
                    printf ("In unparseFunctionType(): calling unparseType(): output function arguments = %p \n",*p);
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

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               curprint ("\n/* In unparseFunctionType(): AFTER parenthesis are output */ \n");
#endif
               unparseType(func_type->get_return_type(), info); // catch the 2nd part of the rtype
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               curprint ("\n/* Done: In unparseFunctionType(): handling second part */ \n");
#endif
             }
            else
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               curprint ("\n/* In unparseFunctionType: recursive call with isTypeFirstPart == true */ \n");
#endif
#if 0
            // DQ (8/19/2012): Temp code while debugging (test2012_192.C).
               printf ("Error: I think this should not be done \n");
               ROSE_ASSERT(false);
#endif
               ninfo.set_isTypeFirstPart();
               unparseType(func_type, ninfo);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               curprint ( "\n/* In unparseFunctionType: recursive call with isTypeSecondPart == true */ \n");
#endif
               ninfo.set_isTypeSecondPart();
               unparseType(func_type, ninfo);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               curprint ( "\n/* In unparseFunctionType: end of recursive call */ \n");
#endif
             }
        }

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
     curprint("\n/* Leaving unparseFunctionType() */ \n");
#endif
   }


void
Unparse_Type::unparseMemberFunctionType(SgType* type, SgUnparse_Info& info)
   {
     SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(type);
     ROSE_ASSERT(mfunc_type != NULL);

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
     ROSE_ASSERT(array_type != NULL);

  // different cases to think about
  //    int (*) [10],  int (*var) [20]
  //    int *[10],  int *var[10]
  //    int [10][20], int var[10][20]
  // multidimensional,
  //    int [2][10] is built up as
  //      ArrayType(base_type, 2)
  //        ArrayType(int, 10), because of the front-end

#if 0
     string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
     string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
     printf ("In Unparse_Type::unparseArrayType(): type = %p type->class_name() = %s firstPart = %s secondPart = %s \n",type,type->class_name().c_str(),firstPartString.c_str(),secondPartString.c_str());
#endif

#if 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint("/* In unparseArrayType() */ \n ");
#endif
#if 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint(string("\n/* Top of unparseArrayType() using generated type name string: ") + type->class_name() + " firstPart " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif

#if 0
     printf ("In Unparse_Type::unparseArrayType(): info.isReferenceToSomething() = %s \n",info.isReferenceToSomething() ? "true" : "false");
     printf ("In Unparse_Type::unparseArrayType(): info.isPointerToSomething()   = %s \n",info.isPointerToSomething()   ? "true" : "false");
#endif

     SgUnparse_Info ninfo(info);
     bool needParen = false;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen = true;
        }

#if 0
     printf ("In unparseArrayType(): needParen = %s \n",(needParen == true) ? "true" : "false");
#endif

     if (ninfo.isTypeFirstPart() == true)
        {
          if(needParen == true)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();
               unparseType(array_type->get_base_type(), ninfo);
               curprint("(");
            // curprint(" /* unparseArrayType */ (");
             }
            else
             {
               unparseType(array_type->get_base_type(), ninfo);
             }
        }
       else
        {
          if (ninfo.isTypeSecondPart() == true)
             {
               if (needParen == true)
                  {
                    curprint(")");
                 // curprint(" /* unparseArrayType */ )");
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
               curprint("[");
               if (array_type->get_index())
                  {
                 // JJW (12/14/2008): There may be types inside the size of an array, and they are not the second part of the type
                    SgUnparse_Info ninfo2(ninfo);
                    ninfo2.unset_isTypeSecondPart();
#if 0
                    printf ("In unparseArrayType(): ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");
                    printf ("In unparseArrayType(): ninfo2.SkipEnumDefinition()  = %s \n",(ninfo2.SkipEnumDefinition() == true) ? "true" : "false");
#endif
                 // DQ (1/9/2014): These should have been setup to be the same.
                    ROSE_ASSERT(ninfo2.SkipClassDefinition() == ninfo2.SkipEnumDefinition());
#if 0
                    printf ("In unparseArrayType(): ninfo2.supressArrayBound()  = %s \n",(ninfo2.supressArrayBound() == true) ? "true" : "false");
#endif
                 // DQ (2/2/2014): Allow the array bound to be subressed (e.g. in secondary declarations of array variable using "[]" syntax.
                 // unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                    if (ninfo2.supressArrayBound() == false)
                       {
                      // Unparse the array bound.
                         unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                       }
                      else
                       {
#if 0
                         printf ("In unparseArrayType(): Detected info_for_type.supressArrayBound() == true \n");
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
               curprint("]");
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

#if 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint("/* Leaving unparseArrayType() */ \n ");
#endif
   }



void
Unparse_Type::unparseTemplateType(SgType* type, SgUnparse_Info& info)
   {
  // This has to be able to select the kind of type being used (likely a template parameter, and unparse it by name).
  // I think that this is non-trivial, since the type might be more than just a name...
  // I am unclear if it can be something that has a first and second part such as some of the other types above (e.g. SgArrayType).

     SgTemplateType* template_type = isSgTemplateType(type);
     ROSE_ASSERT(template_type != NULL);

#if 0
  // DQ (8/25/2012): This allows everything to work, but likely with some internal problems that are not being noticed.
     SgName name = "int ";
#else
  // This is the code that we want to use, but it is not working.
     SgName name = template_type->get_name();
#endif

#if 0
     printf ("In unparseTemplateType(): Unparsing the SgTemplateType as name = %s \n",name.str());
#endif

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
     string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
     printf ("In Unparse_Type::unparseTemplateType(): type->class_name() = %s firstPart = %s secondPart = %s \n",type->class_name().c_str(),firstPartString.c_str(),secondPartString.c_str());
#endif

  // For now just unparse a simple string that will at least be a correct type.
  // curprint("unparse_template_type ");
#if 1
  // DQ (8/25/2012): This was a problem for the output ofr types called from different locations.
     if ( (info.isTypeFirstPart() == false) && (info.isTypeSecondPart() == false) )
        {
       // This is the case where this is called from unparseToString. So we need to output something.
          curprint(name);
        }
       else
        {
       // This is the case where it is called from within the unparser.
          if (info.isTypeSecondPart() == true)
             {
               curprint(name);
             }
        }
#else
  // Older version of code...

#error "DEAD CODE!"

#if 0
  // DQ (8/23/2012): Only out put anything for the 2nd part fo the type.
  // This avoids output of the type twice (which is something I have not tracked down, but appears to be happening).
     curprint("int ");
#else
  // if (info.isTypeFirstPart() == true)
     if (info.isTypeSecondPart() == true)
        {
          curprint("int ");
        }
#endif
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
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
#if 0
     printf ("In unparseTemplateArgument(): BEFORE: templateArgument->get_name_qualification_length_for_type()     = %d \n",templateArgument->get_name_qualification_length_for_type());
     printf ("In unparseTemplateArgument(): BEFORE: templateArgument->get_global_qualification_required_for_type() = %s \n",templateArgument->get_global_qualification_required_for_type() ? "true" : "false");
     printf ("In unparseTemplateArgument(): BEFORE: templateArgument->get_type_elaboration_required_for_type()     = %s \n",templateArgument->get_type_elaboration_required_for_type() ? "true" : "false");
#endif

     SgTemplateArgument* templateArgument = isSgTemplateArgument(referenceNode);
     if (templateArgument != NULL)
        {
       // Transfer values from old variables to the newly added variables (which will be required to support the refactoring into a template of common code.
          templateArgument->set_name_qualification_length_for_type    (templateArgument->get_name_qualification_length());
          templateArgument->set_global_qualification_required_for_type(templateArgument->get_global_qualification_required());
          templateArgument->set_type_elaboration_required_for_type    (templateArgument->get_type_elaboration_required());
#if 0
          printf ("In unparseTemplateArgument(): AFTER: templateArgument->get_name_qualification_length_for_type()     = %d \n",templateArgument->get_name_qualification_length_for_type());
          printf ("In unparseTemplateArgument(): AFTER: templateArgument->get_global_qualification_required_for_type() = %s \n",templateArgument->get_global_qualification_required_for_type() ? "true" : "false");
          printf ("In unparseTemplateArgument(): AFTER: templateArgument->get_type_elaboration_required_for_type()     = %s \n",templateArgument->get_type_elaboration_required_for_type() ? "true" : "false");
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
     printf ("In outputType(): ninfo_for_type.SkipClassDefinition() = %s \n",(ninfo_for_type.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In outputType(): ninfo_for_type.SkipEnumDefinition()  = %s \n",(ninfo_for_type.SkipEnumDefinition() == true) ? "true" : "false");
#endif
  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(ninfo_for_type.SkipClassDefinition() == ninfo_for_type.SkipEnumDefinition());

  // unparseType(tmp_type, info);
  // unp->u_type->unparseType(tmp_type, ninfo_for_type);
     unp->u_type->unparseType(referenceNodeType, ninfo_for_type);

  // curprint( "\n/* DONE - unparse_helper(): output the 1st part of the type */ \n");

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
  // curprint( "\n/* unparse_helper(): output the 2nd part of the type */ \n");
  // unp->u_type->unparseType(tmp_type, info);
  // unp->u_type->unparseType(templateArgumentType, info);
     unp->u_type->unparseType(referenceNodeType, newInfo);

  // printf ("DONE: unparse_helper(): output the 2nd part of the type \n");
  // curprint( "\n/* DONE: unparse_helper(): output the 2nd part of the type */ \n");
   }

