/* unparse_type.C 
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"
#include "unparser.h"

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS 0
#define OUTPUT_DEBUGGING_UNPARSE_INFO 0

string 
get_type_name(SgType* t)
{
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
          case T_UNSIGNED_LONG_LONG: return "unsigned long long";
          case T_LONG_DOUBLE:        return "long double";
          case T_STRING:             return "char*";
          case T_BOOL:               return "bool";
          case T_COMPLEX:            return "complex";
          case T_DEFAULT:            return "int";
          case T_POINTER:            
                {
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
                        +string(mpointer_type->get_class_of()->get_qualified_name().str())
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
                       string(mpointer_type->get_class_of()->get_qualified_name().str()) + "::*" + ")";
                  }
               }
          case T_NAME:               
              printf ("Error: It should be impossible to call this unparseNameType() function (except directly which should not be done!) \n");
              ROSE_ASSERT(false);
          case T_CLASS:              
              {
                SgClassType* class_type = isSgClassType(t);
                ROSE_ASSERT(class_type != NULL);
                SgClassDeclaration *cdecl = isSgClassDeclaration(class_type->get_declaration());
                SgName nm = cdecl->get_qualified_name();
                if (nm.str() != NULL)
                    return string(nm.str());
                else
                   return string(class_type->get_name().str());
               }
          case T_ENUM:               
               {
                SgEnumType* enum_type = isSgEnumType(t);
                ROSE_ASSERT(enum_type);
                SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
                SgName nm = edecl->get_qualified_name();
                if (nm.str() != NULL) 
                    return string(nm.str());
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
                    res = get_type_name(mod_type->get_base_type());
                    unparse_base = false;
                }
                if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())                     res = res + "const "; 
                if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile()) 
                    res = res + "volatile "; 
                if (mod_type->get_typeModifier().isRestrict()) {
                    if ( (string(CXX_COMPILER_NAME) == "g++") || (string(CXX_COMPILER_NAME) == "gcc") ) 
                       res = res + "__restrict__ ";
                    else
                       res = res + "restrict ";
                 }
                 if (unparse_base)
                    res = res + get_type_name(mod_type->get_base_type());
                  return res;
                }
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
               while (p != mfunc_type->get_arguments().end()) {
                    res = res + get_type_name(*p);
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
  
              string res = get_type_name(array_type->get_base_type()) + "[";
              if (array_type->get_index()) 
                  res = res + array_type->get_index()->unparseToString();
               res = res + "]";
               return res;
             }
          case T_ELLIPSE: return "...";
          default:
             {
               printf("Error: Default case reached in switch: Unknown type \n");
               ROSE_ABORT();
               break;
             }
        }
}

//-----------------------------------------------------------------------------------
//  void Unparser::unparseType
//
//  General function that gets called when unparsing a C++ type. Then it routes
//  to the appropriate function to unparse each C++ type.
//-----------------------------------------------------------------------------------
void
Unparser::unparseType(SgType* type, SgUnparse_Info& info)
   {
     ROSE_ASSERT(type != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
     string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
     printf ("In Unparser::unparseType(): type->sage_class_name() = %s firstPart = %s secondPart = %s \n",
          type->sage_class_name(),firstPartString.c_str(),secondPartString.c_str());
     cur << "\n/* Top of unparseType: sage_class_name() = " << type->sage_class_name()
         << " firstPart  = " << firstPartString << " secondPart = " << secondPartString << " */\n";
#endif

#if 0
     printf ("In Unparser::unparseType(): type->sage_class_name() = %s \n",type->sage_class_name());
     cur << "\n/* Top of unparseType: sage_class_name() = " << type->sage_class_name() << " */ \n";
#endif

     char buffer[512];
     sprintf (buffer,"entering case for %s",type->sage_class_name());
     printDebugInfo(buffer,TRUE);

     switch (type->variant())
        {
          case T_UNKNOWN:            cur << get_type_name(type) << " ";          break;
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
          case T_DEFAULT:            
          case T_ELLIPSE:            
             if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
             /* do nothing */;
             else { cur << get_type_name(type) << " "; }
             break;
          case T_POINTER:            unparsePointerType(type, info);          break;
          case T_MEMBER_POINTER:     unparseMemberPointerType(type, info);    break;
          case T_REFERENCE:          unparseReferenceType(type, info);        break;
          case T_NAME:               unparseNameType(type, info);             break;
          case T_CLASS:              unparseClassType(type, info);            break;
          case T_ENUM:               unparseEnumType(type, info);             break;
#ifndef USE_SAGE3
          case T_STRUCT:             unparseStructType(type, info);           break;
          case T_UNION:              unparseUnionType(type, info);            break;
#endif
          case T_TYPEDEF:            unparseTypedefType(type, info);          break;
          case T_MODIFIER:           unparseModifierType(type, info);         break;
          case T_PARTIAL_FUNCTION:   
          case T_FUNCTION:           unparseFunctionType(type, info);         break;
          case T_MEMBERFUNCTION:     unparseMemberFunctionType(type, info);   break;
          case T_ARRAY:              unparseArrayType(type, info);            break;
          default:
             {
               printf("Error: Default case reached in switch: Unknown type \n");
               ROSE_ABORT();
               break;
             }
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving Unparser::unparseType(): type->sage_class_name() = %s firstPart = %s secondPart = %s \n",
          type->sage_class_name(),firstPartString.c_str(),secondPartString.c_str());
     cur << "\n/* Bottom of unparseType: sage_class_name() = " << type->sage_class_name() 
         << " firstPart  = " << firstPartString << " secondPart = " << secondPartString << " */\n";
#endif
   }



void Unparser::unparsePointerType(SgType* type, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparser::unparsePointerType \n");
  // cur << "\n/* Inside of Unparser::unparsePointerType */\n";

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
  // cur << "\n/* " << info.displayString("unparsePointerType") << " */\n";
#endif

     if (info.isTypeFirstPart() == true)
        {
          unparseType(pointer_type->get_base_type(), info);

       // DQ (9/21/2004): Moved this conditional into this branch (to fix test2004_93.C)
       // DQ (9/21/2004): I think we can assert this, and if so we can simplify the logic below
          ROSE_ASSERT(info.isTypeSecondPart() == false);
       // if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )
#if 0
          printf ("info.isWithType()   = %s \n",info.isWithType()   ? "true" : "false");
          printf ("info.SkipBaseType() = %s \n",info.SkipBaseType() ? "true" : "false");
          cur << "\n/* info.isWithType()           = " << (info.isWithType()   ? "true" : "false") << " */\n";
          cur << "\n/* info.SkipBaseType()         = " << (info.SkipBaseType() ? "true" : "false") << " */\n";
          cur << "\n/* info.isPointerToSomething() = " << (info.isPointerToSomething() ? "true" : "false") << " */\n";
#endif

#if 0
       // DQ (9/21/2004): This is currently an error!
          if ( info.isWithType() && info.SkipBaseType() )
             {
            // Do nothing (don't unparse the "*" using the same logic for which we don't unparse the type name)
            // cur << "\n/* $$$$$ In unparsePointerType: Do Nothing Case for output of type $$$$$  */\n";
             }
            else
             {
            // cur << "\n/* $$$$$ In unparsePointerType: Unparse Type Case for output of type $$$$$  */\n";
            // cur << "* /* pointer */ ";
               cur << "*";
             }
#else
       // if (info.SkipDefinition() == true)
          cur << "*";
#endif
        }
       else
        {
          if (info.isTypeSecondPart() == true)
             {
            // printf ("In Unparser::unparsePointerType(): unparse 2nd part of type \n");
            // cur << "\n/* In Unparser::unparsePointerType(): unparse 2nd part of type */\n";
               unparseType(pointer_type->get_base_type(), info);
            // printf ("DONE: Unparser::unparsePointerType(): unparse 2nd part of type \n");
            // cur << "\n/* DONE: Unparser::unparsePointerType(): unparse 2nd part of type */\n";
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

  // printf ("Leaving of Unparser::unparsePointerType \n");
  // cur << "\n/* Leaving of Unparser::unparsePointerType */\n";
   }

void Unparser::unparseMemberPointerType(SgType* type, SgUnparse_Info& info)
   {
     SgPointerMemberType* mpointer_type = isSgPointerMemberType(type);
     ROSE_ASSERT(mpointer_type != NULL);

  // plain type :  int (P::*)
  // type with name:  int P::* pmi = &X::a;
  // use: obj.*pmi=7;
     SgType *btype = mpointer_type->get_base_type();
     SgMemberFunctionType *ftype = NULL;

  // printf ("In unparseMemberPointerType(): btype = %p = %s \n",btype,(btype != NULL) ? btype->sage_class_name() : "NULL" );
  // cur << "\n/* In unparseMemberPointerType() */\n";

     if ( (ftype = isSgMemberFunctionType(btype)) != NULL)
        {
       /* pointer to member function data */
       // printf ("In unparseMemberPointerType(): pointer to member function data \n");

          if (info.isTypeFirstPart())
             {
               unparseType(ftype->get_return_type(), info); // first part
               cur << "(";
               cur << mpointer_type->get_class_of()->get_qualified_name().str();
               cur << "::*";
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
                    cur << ")";

                 // argument list
                    SgUnparse_Info ninfo(info);
                    ninfo.unset_SkipBaseType();
                    ninfo.unset_isTypeSecondPart();
                    ninfo.unset_isTypeFirstPart();

                 // cur << "\n/* In unparseMemberPointerType(): start of argument list */\n";
                    cur << "(";
	
                    SgTypePtrList::iterator p = ftype->get_arguments().begin();
                    while ( p != ftype->get_arguments().end() )
                       {
                      // printf ("In unparseMemberPointerType: output the arguments \n");
                         unparseType(*p, ninfo);
                         p++;
                         if (p != ftype->get_arguments().end()) { cur << ", "; }
                       }
                    cur << ")"; 
                 // cur << "\n/* In unparseMemberPointerType(): end of argument list */\n";
                    unparseType(ftype->get_return_type(), info); // second part
                  }
                 else
                  {
                 // not called from statement level
                    printf ("What is this 3rd case of neither 1st part nor 2nd part \n");
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
            // DQ (9/16/2004): This appears to be an error, btype should not be unparsed here (of maybe btype is not set properly)!
            // printf ("Handling the first part \n");
            // cur << "\n/* start of btype */\n";
               unparseType(btype, info);
            // cur << "\n/* end of btype */\n";
               cur << "(";
               cur << mpointer_type->get_class_of()->get_qualified_name().str();
               cur << "::*";
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
                 // printf ("Handling the second part \n");
                    cur << ")";
                  }
                 else
                  {
                    printf ("What is this 3rd case of neither 1st part nor 2nd part \n");
                    SgUnparse_Info ninfo(info);   
                    ninfo.set_isTypeFirstPart();
                    unparseType(mpointer_type, ninfo); 
                    ninfo.set_isTypeSecondPart();
                    unparseType(mpointer_type, ninfo); 
                  }
             }
        }

  // cur << "\n/* Leaving unparseMemberPointerType() */\n";
   }

void Unparser::unparseReferenceType(SgType* type, SgUnparse_Info& info)
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
       // cur << "& /* reference */ ";
          cur << "&";
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

void Unparser::unparseNameType(SgType* type, SgUnparse_Info& info)
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
       // cur << named_type->get_name().str() << " ";
          printf ("In unparseNameType(): named_type->get_qualified_name() = %s \n",named_type->get_qualified_name().str());
          cur << named_type->get_qualified_name().str() << " "; 
        }
   }

void
Unparser::unparseClassType(SgType* type, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparser::unparseClassType \n");

     SgClassType* class_type = isSgClassType(type);
     ROSE_ASSERT(class_type != NULL);

     SgClassDeclaration *cdecl = isSgClassDeclaration(class_type->get_declaration());
     ROSE_ASSERT(cdecl != NULL);

#if 0
     printf ("info.isWithType() = %s \n",(info.isWithType() == true) ? "true" : "false");
     printf ("info.SkipBaseType() = %s \n",(info.SkipBaseType() == true) ? "true" : "false");
     printf ("info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     if (info.isTypeFirstPart() == true)
        {
       /* print the class specifiers */
       // printf ("print the class specifiers \n");
          if(!info.SkipClassSpecifier())
             {
               switch (cdecl->get_class_type())
                  {
                    case SgClassDeclaration::e_class :
                       {
                         cur << "class ";
                         break;
                       }
                    case SgClassDeclaration::e_struct :
                       {
                         cur << "struct ";
                         break;
                       }
                    case SgClassDeclaration::e_union :
                       {
                         cur << "union ";
                         break;
                       }
                    default:
                       {
                         printf ("Error: default reached \n");
                         ROSE_ASSERT(false);
                         break;
                       }
                  }
             }
        }

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
     if (info.isTypeSecondPart() == false)
        {
#if 0
       // DQ (10/7/2004): We need this logic to avoid using the qualified name which for some 
       // declarations would be computed incorrectly.  We could alternatively save additional 
       // information from EDG as to when the qualified name is required!
          SgClassDefinition *cdefn = cdecl->get_definition();
#else
       // DQ (10/14/2004): Or this could be a declaration in a namespace
          SgClassDefinition*              cdefn         = NULL;
          SgNamespaceDefinitionStatement* namespaceDefn = NULL;

          SgClassDeclaration *nondefiningClassDeclaration = isSgClassDeclaration(class_type->get_declaration());
          ROSE_ASSERT(nondefiningClassDeclaration != NULL);
       // printf ("nondefiningClassDeclaration = %p = %s \n",nondefiningClassDeclaration,nondefiningClassDeclaration->sage_class_name());
          SgClassDefinition  *nondefiningClassDefinition  = nondefiningClassDeclaration->get_definition();
          ROSE_ASSERT(nondefiningClassDefinition != NULL);
       // printf ("nondefiningClassDefinition = %p = %s \n",nondefiningClassDefinition,nondefiningClassDefinition->sage_class_name());
          SgClassDeclaration *definingClassDeclaration    = isSgClassDeclaration(nondefiningClassDefinition->get_declaration());
          ROSE_ASSERT(definingClassDeclaration != NULL);
       // printf ("definingClassDeclaration = %p = %s \n",definingClassDeclaration,definingClassDeclaration->sage_class_name());

          SgTemplateInstantiationDecl *instantiatedTemplateClass = isSgTemplateInstantiationDecl(definingClassDeclaration);

          SgDeclarationStatement* declaration = NULL;

       // printf ("instantiatedTemplateClass = %p \n",instantiatedTemplateClass);
          if (instantiatedTemplateClass != NULL)
             {
               ROSE_ASSERT(instantiatedTemplateClass != NULL);
            // printf ("instantiatedTemplateClass = %p = %s \n",instantiatedTemplateClass,instantiatedTemplateClass->sage_class_name());

               SgTemplateDeclaration *templateDeclaration = instantiatedTemplateClass->get_templateDeclaration();
               ROSE_ASSERT(templateDeclaration != NULL);
            // printf ("templateDeclaration = %p = %s \n",templateDeclaration,templateDeclaration->sage_class_name());
               declaration = templateDeclaration;
             }
            else
             {
               declaration = definingClassDeclaration;
             }

          ROSE_ASSERT(declaration != NULL);

       // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
       // to be used check if name qualification is required.
          initializeDeclarationsFromParent ( declaration, cdefn, namespaceDefn );

       // printf ("After initializeDeclarationsFromParent: cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);
#endif

          SgType* ptype = NULL;

       // DQ (10/10/2004): Added test to see if it is always true
       // ROSE_ASSERT(cdefn != NULL);
       // ROSE_ASSERT( (cdefn != NULL) || (namespaceDefn != NULL) );

          if (cdefn != NULL)
             {
               ptype = isSgNamedType(cdefn->get_declaration()->get_type());
#if 0
               printf ("cdefn = %p = %s ptype = %p = %s \n",cdefn,cdefn->sage_class_name(),ptype,ptype->sage_class_name());
               cdecl->get_file_info()->display("Location of cdecl Declaration");
               cdefn->get_declaration()->get_file_info()->display("Location of cdefn->get_declaration() Declaration");
#endif
             }
#if 0
          if (info.get_current_context() != NULL)
             {
               printf ("info.get_current_context() = %p = %s \n",info.get_current_context(),
                    info.get_current_context()->sage_class_name());
               info.get_current_context()->get_declaration()->get_file_info()->display("Location of Declaration");
             }
            else
             {
               printf ("info.get_current_context() == NULL \n");
             }
#endif

       // DQ (10/8/2004): I had to make this (ptype != NULL) to make sure that the get_qualified_name()
       // function would not be called for IR nodes where the parent of the class declaration was not 
       // set consistantly.  It seems likely that this might be a bug in the future!
       // DQ (10/8/2004): If the parent nodes are not set then avoid calling 
       // get_qualified_name() (which requires valid parent pointers).
       // if ( (ptype == NULL) || (info.get_current_context() == ptype) )
       // if ( (ptype == NULL) || (info.get_current_context() == ptype) || (cdecl->get_parent() == NULL) )
       // if ( (ptype == NULL) || (info.get_current_context() == ptype) || (isSgScopeStatement(cdecl->get_parent()) == NULL) )

       // DQ 10/10/2004): This version with (ptype != NULL) works in the sense that it allows 
       // the EDG/Sage connection code to avoid calling the get_qualified_name() function, but 
       // sometimes the qualified name is required so it is ultimately note correct.
       // if ( (ptype != NULL) || (info.get_current_context() == ptype) )
       // if ( (ptype == NULL) || (info.get_current_context() == ptype) )
       // bool outputQualifiedName = ( (ptype == NULL) || (info.get_current_context() == ptype) );

       // DQ (10/14/2004): If we are going to output the definition (below) then we don't need the qualified name!
          bool definitionWillBeOutput = ( (info.isTypeFirstPart() == true) && !info.SkipClassDefinition() );
#if 0
          bool outputQualifiedName = ( (ptype == NULL) || (info.get_current_context() == ptype) ) && (definitionWillBeOutput == false);
#else
       // DQ (10/14/2004): This code take the namespace into account when a qualified name is required!
          ROSE_ASSERT(namespaceDefn == NULL || namespaceDefn->get_namespaceDeclaration() != NULL);
          bool outputQualifiedName = ((ptype != NULL) && (info.get_current_context() != ptype)) ||
                                     ((namespaceDefn != NULL) && (info.get_current_namespace() != namespaceDefn->get_namespaceDeclaration()));
          outputQualifiedName = outputQualifiedName && (definitionWillBeOutput == false);
#endif
       // printf ("outputQualifiedName = %s \n",(outputQualifiedName == true) ? "true" : "false");

          if ( outputQualifiedName == false )
             {
            // (10/15/2001): Bugfix
            // Note that the name of the class is allowed to be empty in the case of: 
            //      typedef struct <no tag required here> { int x; } y;
               if (class_type->get_name().str() != NULL)
                  {
                 // printf ("class_type->get_name().str() = %s \n",class_type->get_name().str());
                    cur << class_type->get_name().str() << " ";
                  }
                 else
                  {
                 // printf ("class_type->get_name().str() == NULL \n");
                  }
             }
            else
             {
            // add qualifier of current types to the name
               SgName nm = cdecl->get_qualified_name();
               if (nm.str() != NULL)
                  {
                 // printf ("Output qualifier of current types to the name = %s \n",nm.str());
                    cur << nm.str() << " ";
                  }
#if 0
                 else
                  {
                    printf ("tag name in class type declaration = %p = %s is NULL \n",cdecl,cdecl->sage_class_name());
                    cdecl->get_file_info()->display("Location of problematic node in source code");
                  }
#endif
            // ROSE_ASSERT(nm != "var21::iceland");
             }
        }

#if 0
     printf ("info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("cdecl->isForward()         = %s \n",(cdecl->isForward() == true) ? "true" : "false");
#endif

     if (info.isTypeFirstPart() == true)
        {
          if ( !info.SkipClassDefinition() )
             {
               SgUnparse_Info ninfo(info);
               ninfo.unset_SkipSemiColon();

            // cur << "\n/* Unparsing class definition within unparseClassType */\n";

               ninfo.set_isUnsetAccess();
               cur << "{";
               SgClassDefinition* classdefn_stmt = cdecl->get_definition();
            // printf ("In unparseClassType: classdefn_stmt = %p \n",classdefn_stmt);
               ROSE_ASSERT(classdefn_stmt != NULL);
               SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();
               while (pp != classdefn_stmt->get_members().end())
                  {
                    unparseStatement((*pp), ninfo);
                    pp++;
                  }
               cur << "}";
             }
        }
   }

#if 0
// DQ (10/7/2004): Older version (try to rewrite to make it simpler!)
void
Unparser::unparseClassType(SgType* type, SgUnparse_Info& info)
   {
     printf ("Inside of Unparser::unparseClassType \n");

     SgClassType* class_type = isSgClassType(type);
     ROSE_ASSERT(class_type != NULL);

  // DQ (10/16/2001): Moved this block of code outside of:
  // ***** "if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())" below

  // check if currrent type's parent type is the same as the context type */
     SgNamedType *ptype       = NULL;
     SgClassDefinition *cdefn = NULL;

     SgClassDeclaration *cdecl = isSgClassDeclaration(class_type->get_declaration());
     ROSE_ASSERT(cdecl != NULL);
     SgClassDeclaration *ndecl = cdecl->get_definition()->get_declaration();
     ROSE_ASSERT(ndecl != NULL);

     printf ("cdecl = %p ndecl = %p \n",cdecl,ndecl);

#if 0
     if (cdecl != ndecl)
        {
       // this is not the defining declaration
          printf ("This is NOT the defining declaration \n");
       // cur << "\n/* In unparseClassType(): this is NOT the defining declaration */\n";

       // if (!ndecl->isFriend())
          if (!ndecl->get_declarationModifier().isFriend())
             {
#if PRINT_DEVELOPER_WARNINGS
               if (ndecl->get_parent() == NULL)
                  {
                    printf ("Warning in unparseClassType: ndecl->parent not set yet, OK if building AST, cdecl is a %s \n",cdecl->sage_class_name());
                    ndecl->get_file_info()->display("Location in source file: ndecl");
                    cdecl->get_file_info()->display("Location in source file: cdecl");
                  }
#endif
            // ROSE_ASSERT (ndecl->get_parent() != NULL);
               cdefn = isSgClassDefinition(ndecl->get_parent());
             }
       // else, do nothing, this is default to global scope
        }
       else
#endif
        {
       // printf ("This IS the defining declaration \n");
       // cur << "\n/* In unparseClassType(): this IS the defining declaration */\n";

       // printf ("This will be the defining declaration if info.SkipClassDefinition() = %s == true \n",(info.SkipClassDefinition() == true) ? "true" : "false");

#if 1
          printf ("cdecl = %p cdecl->isForward() = %s \n",cdecl,(cdecl->isForward() == true) ? "true" : "false");
          printf ("cdecl = %p cdecl->get_declarationModifier().isFriend() = %s \n",
               cdecl,(cdecl->get_declarationModifier().isFriend() == true) ? "true" : "false");
#endif

       // if (!cdecl->isFriend() && !cdecl->isForward())
          if (!cdecl->get_declarationModifier().isFriend() && !cdecl->isForward())
             {
            // ck if defined in a var decl
            // printf ("Check parent to see if defined in a variable declaration \n");

               int v = GLOBAL_STMT;

#if PRINT_DEVELOPER_WARNINGS
            // DQ (9/24/2004): Allow this to be NULL since parents are not set and this function is called from 
            // the EDG/Sage III connection cdoe to generate template names (which maybe could be done in post 
            // processing if required, not sure about that?) Print out warning for now!
            // DQ (9/10/2004): Now that we have fixed parents to never by types of symbols can we live without this assertion!
               if (cdecl->get_parent() == NULL)
                  {
                    printf ("Warning in unparseClassType: cdecl->parent not set yet, OK if building AST, cdecl is a %s \n",cdecl->sage_class_name());
                    cdecl->get_file_info()->display("location of cdecl");
                  }
#endif
            // ROSE_ASSERT (cdecl->get_parent() != NULL);
               SgStatement *cparent = isSgStatement(cdecl->get_parent());
               if (cparent != NULL)
                    v = cparent->variant();

               if (v == VAR_DECL_STMT || v == TYPEDEF_STMT)
                  {
                 // printf ("In unparseClassType: parent IR node is either a VAR_DECL_STMT or a TYPEDEF_STMT \n");
                    ROSE_ASSERT (cparent != NULL);
                    ROSE_ASSERT (cparent->get_parent() != NULL);
#if 0
                 // It is used often system header files, this can be uncommented to see examples where it is used!
                 // DQ (9/9/2004): I'm not clear how this code is used, if it ever is used!
                    printf ("ERROR: I'm not clear how this code is used, if it ever is used! \n");
                    printf ("Location is source code being compiled is: \n");
                    ROSE_ASSERT(cdecl != NULL);
                    cdecl->get_file_info()->display("cdecl ERROR: I'm not clear how this code is used, if it ever is used!");
                    cparent->get_file_info()->display("cparent ERROR: I'm not clear how this code is used, if it ever is used!");
                 // ROSE_ASSERT(false);
#endif

                 // DQ (9/9/2004): Uncomment to test
                 // DQ (7/19/2004) comment out to test
                 // cdefn = isSgClassDefinition(cparent->get_parent());
                  }
                 else
                  {
                 // printf ("is NEITHER a VAR_DECL_STMT NOR a TYPEDEF_STMT \n");
                    if ( v == CLASS_DEFN_STMT)
                       {
                      // printf ("is a SgClassDefinition (CLASS_DEFN_STMT) \n");
                         cdefn = isSgClassDefinition(cparent);
                       }
                  }
             }
        }

     if (cdefn != NULL)
        {
       // printf ("initializing ptype: cdefn = %p \n",cdefn);
          ptype = isSgNamedType(cdefn->get_declaration()->get_type());
        }

  // ***** end of blocked moved outside of conditional below
#if 0
     printf ("info.isWithType() = %s \n",(info.isWithType() == true) ? "true" : "false");
     printf ("info.SkipBaseType() = %s \n",(info.SkipBaseType() == true) ? "true" : "false");
     printf ("info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif
     if ( (info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart() )
        {
       /* do nothing */;

       // Output the class definition as part of the 2nd part
       // printf ("In unparseClassType: Output the class definition as part of the 2nd part ... \n");

#if 0
          if (class_type != NULL)
               printf ("class_type->get_autonomous_declaration() = %s \n",(class_type->get_autonomous_declaration()) ? "TRUE" : "FALSE");
            else
               printf ("class_type == NULL \n");
#endif

       // DQ (3/25/2003)
       // Do avoid infinite recursion in processing:
       // struct Xtag { Xtag* next; } X;
       // by detecting the use of structure tags that don't require unparsing the structures
       // All structure tags are places into a list and the list is searched, if the tag is found
       // then unparsing the type representing the tag is only unparsed to generate the type name
       // only (not the full class definition).
          bool structureTagBeingProcessed = true;
          if ( (class_type != NULL) &&
               (class_type->get_autonomous_declaration() == FALSE) )
             {
            // DQ (9/22/2004): Set to true if found in list, false if NOT found in list
            // Since the first reference in not in the list, the structure will be unparsed 
            // mistakenly as a defining declaration the first time it is references.  C++ 
            // constructions like:
            //      typedef struct foobar *(*foobarPointer)();
            // Can thus be unparsed mistakenly as:
            //      typedef struct foobar *(*foobarPointer)(){};
            // which is an error!

               structureTagBeingProcessed = find(info.getStructureTagList().begin(),
                                                 info.getStructureTagList().end(),class_type)
                                            != info.getStructureTagList().end();
             }

#if 0
          printf ("cdecl->isForward() = %s \n",(cdecl->isForward() == true) ? "true" : "false");
          printf ("structureTagBeingProcessed = %s \n",(structureTagBeingProcessed) ? "true" : "false");
          cur << "\n/* structureTagBeingProcessed = " << ((structureTagBeingProcessed) ? "true" : "false") << "*/\n";
#endif

       // DQ (3/20/2003): This fixes the uparsing of the class definition within "typedef struct { int a; } A;"
       // if ( info.isTypeSecondPart() && !class_type->get_autonomous_declaration() )
#if 0
          printf ("info.isTypeSecondPart()                  = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
          printf ("class_type->get_autonomous_declaration() = %s \n",(class_type->get_autonomous_declaration() == true) ? "true" : "false");
          printf ("info.SkipClassDefinition()               = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
          printf ("cdecl->isForward()                       = %s \n",(cdecl->isForward() == true) ? "true" : "false");
#endif
       // DQ (10/5/2004): Support for explicit setting of defining declarations
       // DQ (9/22/2004): Try to use isForward flag to make nondefining-declarations
       // if ( info.isTypeSecondPart() && !class_type->get_autonomous_declaration() && !info.SkipClassDefinition() && !cdecl->isForward())
          if ( info.isTypeSecondPart() && !class_type->get_autonomous_declaration() && !info.SkipClassDefinition())
             {
            // Copy the tag info so that we can handle recursive use of tags as in:
            // "typedef struct Xtag { Xtag* pointer; } X;
            // SgUnparse_Info ninfo;
            // SgUnparse_Info testAbilityToCatchError_info;

            // info.display("info before constructing ninfo");
               SgUnparse_Info ninfo(info);

            // don't skip the semicolon in the output of the statement in the class definition
               ninfo.unset_SkipSemiColon();

            // This is static so we don't have ot copy it
            // ninfo.getStructureTagList() = info.getStructureTagList();

            // Pass on the structure tag list to the new ninfo object
            // ninfo.getStructureTagList() = info.getStructureTagList();

            // cur << "\n/* Unparsing class definition within unparseClassType */\n";

               ninfo.set_isUnsetAccess();

               cur << "{";

               SgClassDefinition* classdefn_stmt = cdecl->get_definition();
            // printf ("In unparseClassType: classdefn_stmt = %p \n",classdefn_stmt);
               ROSE_ASSERT(classdefn_stmt != NULL);
               SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();

               while (pp != classdefn_stmt->get_members().end())
                  {
                    unparseStatement((*pp), ninfo);
                    pp++;
                  }

                  cur << "}";
             }
#if 0
            else
             {
               printf ("!info.isTypeSecondPart() || class_type->get_autonomous_declaration() \n");
               if (info.SkipClassDefinition())
                    printf ("Skipped output of class definition in unparseClassType \n");
             }
#endif
        }
       else
        {
       /* print the class specifiers */
       // printf ("print the class specifiers \n");
          if(!info.SkipClassSpecifier())
             {
               switch (cdecl->get_class_type())
                  {
                 // DQ (2/28/2004): Comment this out for now but consider adding later
                 // case SgClassDeclaration::e_template_parameter :
                    case SgClassDeclaration::e_class :
                       {
                      // DQ (2/7/2004): I wonder why this asymetry is here???
                      // Qing pointed out that this was required and I agree.
                      // DQ (5/30/2004): This is required for test2004_17.C to be unparsed properly
                      // options handled through the opt class should not apply here so I have 
                      // recommented out the asymetry.
#if 0
                      // DQ (5/29/2004): Qing pointed out that this was required and I agree
                      // (as an asymetry, it should be fixed differently!)  I have not seen 
                      // where or why she requires it.
                         if (opt.get_class_opt())
                            {
                              cur << "class ";
                            }
#else
                      // printf ("Skipping conditional controling use of class keyword \n");
                         cur << "class ";
#endif
                         break;
                       }
                    case SgClassDeclaration::e_struct :
                       {
                         cur << "struct ";
                         break;
                       }
                    case SgClassDeclaration::e_union :
                       {
                         cur << "union ";
                         break;
                       }
                  }
             }

#if PRINT_DEVELOPER_WARNINGS
//        printf ("This code in unparseClassType requires the same logic as in unparseClassDeclaration to handle namespaces \n");
#endif

          if( !ptype || (info.get_current_context() == ptype) )
             {
            // (10/15/2001): Bugfix
            // Note that the name of the class is allowed to be empty in the case of: 
            //      typedef struct <no tag required here> { int x; } y;
               if (class_type->get_name().str() != NULL)
                  {
                 // printf ("class_type->get_name().str() = %s \n",class_type->get_name().str());
                    cur << class_type->get_name().str() << " ";
                  }
                 else
                  {
                 // printf ("class_type->get_name().str() == NULL \n");
                  }
             }
            else
             {
            // add qualifier of current types to the name
            // printf ("Output qualifier of current types to the name \n");
               SgName nm = cdecl->get_qualified_name();
               cur << nm.str() << " ";
             }
        } // end of if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart()) conditional
   }
// DQ (10/7/2004): Older code commented out for now while we try to write a shorter version!
#endif

void
Unparser::unparseEnumType(SgType* type, SgUnparse_Info& info)
   {
     SgEnumType* enum_type = isSgEnumType(type);
     ROSE_ASSERT(enum_type);

  // printf ("Inside of unparseEnumType() \n");

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
     if (info.isTypeSecondPart() == false)
        {
          SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
          SgClassDefinition *cdefn = NULL;
          SgNamespaceDefinitionStatement* namespaceDefn = NULL;

          ROSE_ASSERT(edecl != NULL);

       // printf ("edecl->isForward()         = %s \n",(edecl->isForward() == true) ? "true" : "false");

#if 0
       // ck if defined in var decl
          int v = GLOBAL_STMT;
       // SgStatement *eparent=edecl->get_parent();
          ROSE_ASSERT (edecl->get_parent() != NULL);
          SgStatement *eparent = isSgStatement(edecl->get_parent());
          if (eparent)
               v = eparent->variant();
          if (v == VAR_DECL_STMT || v == TYPEDEF_STMT)
             {
            // printf ("Parent of declaration is a VAR_DECL_STMT || TYPEDEF_STMT \n");
               ROSE_ASSERT (eparent->get_parent() != NULL);
               cdefn = isSgClassDefinition(eparent->get_parent());
             }
            else
             {
            // printf ("Parent of declaration is NOT a VAR_DECL_STMT || TYPEDEF_STMT \n");
               if(v == CLASS_DEFN_STMT)
                  {
                 // printf ("Parent of declaration is a CLASS_DEFN_STMT \n");
                    cdefn = isSgClassDefinition(eparent);
                  }
             }
#else
       // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
       // to be used check if name qualification is required.
          initializeDeclarationsFromParent ( edecl, cdefn, namespaceDefn );

       // printf ("After initializeDeclarationsFromParent: cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);
#endif

       // printf ("In unparseEnumType: cdefn = %p \n",cdefn);

       // DQ (5/22/2003) Added output of "enum" string
          cur << "enum ";

#if 0
          if(cdefn != NULL)
             {
               SgNamedType *ptype = isSgNamedType(cdefn->get_declaration()->get_type());

            // DQ (10/8/2004): If the parent nodes are not set then avoid calling 
            // get_qualified_name() (which requires valid parent pointers).
            // if ( (ptype == NULL) || (info.get_current_context() == ptype) )
               if ( (ptype == NULL) || (info.get_current_context() == ptype) || (edecl->get_parent() == NULL) )
                  {
                 // cur << enum_type->get_name().str() << " ";
                    if (enum_type->get_name().str() != NULL)
                         cur << enum_type->get_name().str() << " ";
                  }
                 else
                  { 
                 // add qualifier of current types to the name
                    SgName nm = cdefn->get_declaration()->get_qualified_name();
                    if(!nm.is_null())
                       {
                         cur << nm.str() << "::" << enum_type->get_name().str() << " ";
                       }
                      else
                       {
                         cur << enum_type->get_name().str() << " ";
                       }
                  }
             }
            else
             {
               cur << enum_type->get_name().str() << " ";
             }
#else
       // DQ (10/16/2004): Handle name qualification the same as in the unparseClassType function (we could factor common code later!)
          SgNamedType *ptype = NULL;
          if (cdefn != NULL)
             {
               ptype = isSgNamedType(cdefn->get_declaration()->get_type());
             }

       // DQ (10/14/2004): If we are going to output the definition (below) then we don't need the qualified name!
          bool definitionWillBeOutput = ( (info.isTypeFirstPart() == true) && !info.SkipClassDefinition() );
#if 0
          bool outputQualifiedName = ( (ptype == NULL) || (info.get_current_context() == ptype) ) && (definitionWillBeOutput == false);
#else
       // DQ (10/14/2004): This code take the namespace into account when a qualified name is required!
          ROSE_ASSERT(namespaceDefn == NULL || namespaceDefn->get_namespaceDeclaration() != NULL);
          bool outputQualifiedName = ((ptype != NULL) && (info.get_current_context() != ptype)) ||
                                     ((namespaceDefn != NULL) && (info.get_current_namespace() != namespaceDefn->get_namespaceDeclaration()));
          outputQualifiedName = outputQualifiedName && (definitionWillBeOutput == false);
#endif
       // printf ("outputQualifiedName = %s \n",(outputQualifiedName == true) ? "true" : "false");

          if ( outputQualifiedName == false )
             {
            // (10/15/2001): Bugfix
            // Note that the name of the class is allowed to be empty in the case of: 
            //      typedef struct <no tag required here> { int x; } y;
               if (enum_type->get_name().str() != NULL)
                  {
                 // printf ("enum_type->get_name().str() = %s \n",enum_type->get_name().str());
                    cur << enum_type->get_name().str() << " ";
                  }
                 else
                  {
                 // printf ("enum_type->get_name().str() == NULL \n");
                  }
             }
            else
             {
            // add qualifier of current types to the name
               SgName nm = edecl->get_qualified_name();
               if (nm.str() != NULL)
                  {
                 // printf ("Output qualifier of current types to the name = %s \n",nm.str());
                    cur << nm.str() << " ";
                  }
             }
#endif
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

         if ( info.SkipEnumDefinition() == false)
            {
              SgUnparse_Info ninfo(info);
              ninfo.set_inEnumDecl();
              SgInitializer *tmp_init = NULL;
              SgName tmp_name;

              SgEnumDeclaration *enum_stmt = isSgEnumDeclaration(enum_type->get_declaration());
              ROSE_ASSERT(enum_stmt != NULL);

              SgInitializedNamePtrList::iterator p = enum_stmt->get_enumerators().begin();

              if (p != enum_stmt->get_enumerators().end())
                 {
                   cur << "{"; 
                   while (1)
                      {
                        tmp_name=(*p)->get_name();
                        tmp_init=(*p)->get_initializer();
                        cur << tmp_name.str();
                        if(tmp_init)
                           {
                             cur << "=";
                             unparseExpression(tmp_init, ninfo);
                           }
                        p++;
                        if (p != enum_stmt->get_enumerators().end())
                           {
                             cur << ",";
                           }
                          else
                             break; 
                       }
                    cur << "}";
                  }
             }
        }
   }

#if 0
// DQ (10/8/2004): Old code which is being rewritten to be shorter, simpler and better!
// We now explicitly mark where definitions are to be output so the complex logic that 
// used to be required (and which was wrong) can now be replaced with the newer simpler
// system.  This is handled similar as to the class type unparsing (which was rewritten 
// yesterday).
void
Unparser::unparseEnumType(SgType* type, SgUnparse_Info& info)
   {
     SgEnumType* enum_type = isSgEnumType(type);
     ROSE_ASSERT(enum_type);

  // printf ("Inside of unparseEnumType() \n");

     if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
        {
       /* do nothing */;
       // printf ("In unparseEnumType: do nothing case! \n");

       // Output the class definition as part of the 2nd part

#if 0
          if (enum_type != NULL)
               printf ("enum_type->get_autonomous_declaration() = %s \n",(enum_type->get_autonomous_declaration()) ? "TRUE" : "FALSE");
            else
               printf ("enum_type == NULL \n");
#endif

       // DQ (3/25/2003)
       // Do avoid infinite recursion in processing:
       // struct Xtag { Xtag* next; } X;
       // by detecting the use of structure tags that don't require unparsing the structures
       // All structure tags are places into a list and the list is searched, if the tag is found
       // then unparsing the type representing the tag is only unparsed to generate the type name
       // only (not the full class definition).
          bool structureTagBeingProcessed = true;
          if ( (enum_type != NULL) &&
               (enum_type->get_autonomous_declaration() == FALSE) )
             {
               structureTagBeingProcessed = find(info.getStructureTagList().begin(),
                                                 info.getStructureTagList().end(),enum_type)
                                            != info.getStructureTagList().end();
             }

       // printf ("structureTagBeingProcessed = %s \n",(structureTagBeingProcessed) ? "TRUE" : "FALSE");

       // DQ (3/20/2003): This fixes the uparsing of the class definition within "typedef struct { int a; } A;"
       // if ( info.isTypeSecondPart() && !class_type->get_autonomous_declaration() )
          if ( info.isTypeSecondPart() && !enum_type->get_autonomous_declaration() && !info.SkipEnumDefinition())
             {
            // Copy the tag info so that we can handle recursive use of tags as in:
            // "typedef struct Xtag { Xtag* pointer; } X;
            // SgUnparse_Info ninfo;
            // SgUnparse_Info testAbilityToCatchError_info;

            // info.display("info before constructing ninfo");
               SgUnparse_Info ninfo(info);

            // don't skip the semicolon in the output of the statement in the class definition
               ninfo.unset_SkipSemiColon();

            // This is static so we don't have ot copy it
            // ninfo.getStructureTagList() = info.getStructureTagList();

            // Pass on the structure tag list to the new ninfo object
            // ninfo.getStructureTagList() = info.getStructureTagList();

               ninfo.set_isUnsetAccess();


#if 1
            // cur << "/* OUTPUT THE ENUM DECLARATION */";
               if (!info.SkipEnumDefinition())
                  {
                    SgUnparse_Info ninfo(info);
                    ninfo.set_inEnumDecl();
                    SgInitializer *tmp_init = NULL;
                    SgName tmp_name;

//             SgClassDefinition* classdefn_stmt = cdecl->get_definition();
//             SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();
//             SgClassDeclaration *cdecl = isSgClassDeclaration(class_type->get_declaration());

                    SgEnumDeclaration *enum_stmt = isSgEnumDeclaration(enum_type->get_declaration());
                    ROSE_ASSERT(enum_stmt != NULL);

                    SgInitializedNamePtrList::iterator p = enum_stmt->get_enumerators().begin();

                    if (p != enum_stmt->get_enumerators().end())
                       {
                         cur << "{"; 
                         while (1)
                            {
                              tmp_name=(*p)->get_name();
                              tmp_init=(*p)->get_initializer();
                              cur << tmp_name.str();
                              if(tmp_init)
                                 {
                                   cur << "=";
                                   unparseExpression(tmp_init, ninfo);
                                 }
                              p++;
                              if (p != enum_stmt->get_enumerators().end())
                                 {
                                   cur << ",";
                                 }
                                else
                                   break; 
                            }
                         cur << "}";
                       }
                  }
#else
               cur << "{";
               SgClassDefinition* classdefn_stmt = cdecl->get_definition();
               SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();

               while(pp != classdefn_stmt->get_members().end())
                  {
                    unparseStatement((*pp), ninfo);
                    pp++;
                  }
                  cur << "}";
#endif
             }
#if 0
            else
             {
               if (info.SkipEnumDefinition())
                    printf ("Skipped output of enum definition in unparseEnumType \n");
             }
#endif
        }
       else
        {
       // printf ("NOT  ((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart()) \n");

       // could be a scoped enum type
       // check if currrent type's parent type is the same as the context type */
       // SgNamedType *ptype=0;
    
          SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
          SgClassDefinition *cdefn = NULL;

       // ck if defined in var decl
          int v = GLOBAL_STMT;
       // SgStatement *eparent=edecl->get_parent();
          ROSE_ASSERT (edecl->get_parent() != NULL);
          SgStatement *eparent = isSgStatement(edecl->get_parent());
          if (eparent)
               v = eparent->variant();
          if (v == VAR_DECL_STMT || v == TYPEDEF_STMT)
             {
            // printf ("Parent of declaration is a VAR_DECL_STMT || TYPEDEF_STMT \n");
               ROSE_ASSERT (eparent->get_parent() != NULL);
               cdefn = isSgClassDefinition(eparent->get_parent());
             }
            else
             {
            // printf ("Parent of declaration is NOT a VAR_DECL_STMT || TYPEDEF_STMT \n");
               if(v == CLASS_DEFN_STMT)
                  {
                 // printf ("Parent of declaration is a CLASS_DEFN_STMT \n");
                    cdefn = isSgClassDefinition(eparent);
                  }
             }

       // printf ("cdefn is a %s pointer \n",(cdefn != NULL) ? "VALID" : "NULL");

       // DQ (5/22/2003) Added output of "enum" string
          cur << "enum ";

          if(cdefn != NULL)
             {
               SgNamedType *ptype=isSgNamedType(cdefn->get_declaration()->get_type());
               if(!ptype || (info.get_current_context() == ptype))
                  {
                 // cur << enum_type->get_name().str() << " ";
                    if (enum_type->get_name().str() != NULL)
                         cur << enum_type->get_name().str() << " ";
                  }
                 else
                  { 
                 // add qualifier of current types to the name
                    SgName nm = cdefn->get_declaration()->get_qualified_name();
                    if(!nm.is_null())
                       {
                         cur << nm.str() << "::" << enum_type->get_name().str() << " ";
                       }
                      else
                       {
                         cur << enum_type->get_name().str() << " ";
                       }
                  }
             }
            else
             {
               cur << enum_type->get_name().str() << " ";
             }
        }
   }
// DQ (10/8/2004): Older code commented out for now while we try to write a shorter version!
#endif

#if 0
// No longer a part of SAGE 3
void
Unparser::unparseStructType(SgType* type, SgUnparse_Info& info)
   {
     printf ("Unparser::unparseStructType not implemented! \n");
     ROSE_ABORT();
   }

// No longer a part of SAGE 3
void Unparser::unparseUnionType(SgType* type, SgUnparse_Info& info)
   {
     printf ("unparsing of unions: Sorry, not implemented! \n");
     ROSE_ABORT();
   }
#endif

void
Unparser::unparseTypedefType(SgType* type, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparser::unparseTypedefType \n");
  // cur << "\n/* Inside of Unparser::unparseTypedefType */\n";

     SgTypedefType* typedef_type = isSgTypedefType(type);
     ROSE_ASSERT(typedef_type != NULL);

     if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart()) 
        {
       /* do nothing */;
       // printf ("Inside of Unparser::unparseTypedefType (do nothing) \n");
        }
       else
        {
       // could be a scoped typedef type
       // check if currrent type's parent type is the same as the context type */
       // SgNamedType *ptype = NULL;

          SgTypedefDeclaration *tdecl = isSgTypedefDeclaration(typedef_type->get_declaration());
          ROSE_ASSERT (tdecl != NULL);

#if 1
       // DQ (10/16/2004): Keep this error checking for now!
          ROSE_ASSERT(typedef_type != NULL);
          ROSE_ASSERT(typedef_type->get_declaration() != NULL);
#if 0
          printf ("typedef_type->get_declaration() = %p = %s \n",
               typedef_type->get_declaration(),typedef_type->get_declaration()->sage_class_name());
#endif
       // DQ (10/17/2004): This assertion force me to set the parents of typedef in the EDG/Sage connection code 
       // since I could not figure out why it was not being set in the post processing which sets parents.
          ROSE_ASSERT(typedef_type->get_declaration()->get_parent() != NULL);
#if 0
          printf ("typedef_type->get_declaration() = %p = %s  name = %s \n",
               typedef_type->get_declaration(),typedef_type->get_declaration()->sage_class_name(),
               typedef_type->get_name().str());
          printf ("#0 typedef_type->get_name() = %s typedef_type->get_qualified_name() = %s \n",
               typedef_type->get_name().str(),typedef_type->get_qualified_name().str());
#endif
#endif
          cur << typedef_type->get_qualified_name().str() << " ";

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
                    cur << typedef_type->get_name().str() << " ";
                  }
                 else
                  {
                 // add qualifier of current types to the name
                    SgName nm = cdefn->get_declaration()->get_qualified_name();
                 // if(!nm.is_null())
                    if ( nm.is_null() == false )
                       {
                      // printf ("nm = %s :: typedef_type->get_name() = %s \n",nm.str(),typedef_type->get_name().str());
                         cur << nm.str() << "::" << typedef_type->get_name().str() << " ";
                       }
                      else
                       {
                      // printf ("#2 typedef_type->get_name() = %s \n",typedef_type->get_name().str());
                         cur << typedef_type->get_name().str() << " ";
                       }
                  }
             }
            else
             {
            // printf ("#3 typedef_type->get_name() = %s \n",typedef_type->get_name().str());
               cur << typedef_type->get_name().str() << " ";
             }
#endif
        }

  // printf ("Leaving Unparser::unparseTypedefType \n");
  // cur << "\n/* Leaving Unparser::unparseTypedefType */\n";
   }

void Unparser::unparseModifierType(SgType* type, SgUnparse_Info& info)
   {
     SgModifierType* mod_type = isSgModifierType(type);
     ROSE_ASSERT(mod_type != NULL);

  // printf ("Top of Unparser::unparseModifierType \n");
  // cur << "\n/* Top of Unparser::unparseModifierType */\n";

#if 0
  // mod_type->get_typeModifier().display("called from Unparser::unparseModifierType()");
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

          if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())    { cur << "const "; }
          if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile()) { cur << "volatile "; }

       // Previous support for CC++ (no longer supported in SAGE III)
       // if (mod_type->isSync())     { cur << "sync "; }
       // if (mod_type->isGlobal())   { cur << "global "; }

          if (mod_type->get_typeModifier().isRestrict())
             {
               if ( (string(CXX_COMPILER_NAME) == "g++") || (string(CXX_COMPILER_NAME) == "gcc") )
                  {
                 // GNU uses a string variation on the C99 spelling of the "restrict" keyword
                    cur << "__restrict__ ";
                  }
                 else
                  {
                    cur << "restrict ";
                  }
             }

       // Microsoft extension
       // xxx_unaligned   // Microsoft __unaligned qualifier

       // Support for near and far pointers (a microsoft extension)
       // xxx_near        // near pointer
       // xxx_far         // far pointer

       // Support for UPC
          if (mod_type->get_typeModifier().get_upcModifier().isUPC_Shared())
             { cur << "/* shared: upc not supported by vendor compiler (ignored) */ "; }
          if (mod_type->get_typeModifier().get_upcModifier().isUPC_Strict())
             { cur << "/* strict: upc not supported by vendor compiler (ignored) */ "; }
          if (mod_type->get_typeModifier().get_upcModifier().isUPC_Relaxed())
             { cur << "/* relaxed: upc not supported by vendor compiler (ignored) */ "; }

#if 0
          printf ("SgModifierType::m_restrict = %d \n",SgModifierType::m_restrict);
          printf ("SgModifierType::m_shared   = %d \n",SgModifierType::m_shared);
          printf ("SgModifierType::m_strict   = %d \n",SgModifierType::m_strict);
          printf ("SgModifierType::m_relaxed  = %d \n",SgModifierType::m_relaxed);
#endif

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

  // printf ("Leaving Unparser::unparseModifierType \n");
  // cur << "\n/* Leaving Unparser::unparseModifierType */\n";
   }

void
Unparser::unparseFunctionType(SgType* type, SgUnparse_Info& info)
   {
     SgFunctionType* func_type = isSgFunctionType(type);
     ROSE_ASSERT (func_type != NULL);

     SgUnparse_Info ninfo(info);
     int needParen = 0;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen=1;
        }

  // DQ (10/8/2004): Skip output of class definition for return type! C++ standard does not permit 
  // a defining declaration within a return type, function parameter, or sizeof expression. 
     ninfo.set_SkipClassDefinition();

     if (ninfo.isTypeFirstPart())
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          cur << "\n/* In unparseFunctionType: handling first part */\n";
          cur << "\n/* Skipping the first part of the return type! */\n";
#endif
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
            // DQ (9/21/2004): we don't want this for typedefs of function pointers where the function return type is a pointer
               printf ("Skipping the first part of the return type (in needParen == true case)! \n");
               cur << "\n/* Skipping the first part of the return type (in needParen == true case)! */\n";
#endif
#if OUTPUT_DEBUGGING_UNPARSE_INFO
               cur << "\n/* " << ninfo.displayString("Skipping the first part of the return type (in needParen == true case)") << " */\n";
#endif
               unparseType(func_type->get_return_type(), ninfo);
               cur << "(";
             }
            else
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
            // DQ (9/21/2004): we don't want this for typedefs of function pointers where the function return type is a pointer
               printf ("Skipping the first part of the return type (in needParen == false case)! \n");
               cur << "\n/* Skipping the first part of the return type (in needParen == false case)! */\n";
#endif
               unparseType(func_type->get_return_type(), ninfo);
             }
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               cur << "\n/* In unparseFunctionType: handling second part */\n";
#endif
               if (needParen)
                  {
                    cur << ")";
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
            // print the arguments
               SgUnparse_Info ninfo2(info); 
               ninfo2.unset_SkipBaseType();
               ninfo2.unset_isTypeSecondPart();
               ninfo2.unset_isTypeFirstPart();

               cur << "(";
               SgTypePtrList::iterator p = func_type->get_arguments().begin();
               while(p != func_type->get_arguments().end())
                  {
                 // printf ("Output function argument ... \n");
                    unparseType(*p, ninfo2);
                    p++;
                    if (p != func_type->get_arguments().end())
                       { cur << ", "; }
                  }
               cur << ")";
               unparseType(func_type->get_return_type(), info); // catch the 2nd part of the rtype
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
               cur << "\n/* Done: In unparseFunctionType: handling second part */\n";
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

void Unparser::unparseMemberFunctionType(SgType* type, SgUnparse_Info& info)
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

     if (ninfo.isTypeFirstPart())
        {
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();
               unparseType(mfunc_type->get_return_type(), ninfo);
               cur << "(";
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
                    cur << ")";
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
            // print the arguments
               SgUnparse_Info ninfo2(info); 
               ninfo2.unset_SkipBaseType();
               ninfo2.unset_isTypeFirstPart();
               ninfo2.unset_isTypeSecondPart();
    
               cur << "(";
               SgTypePtrList::iterator p = mfunc_type->get_arguments().begin();
               while (p != mfunc_type->get_arguments().end())
                  {
                    printf ("In unparseMemberFunctionType: output the arguments \n");
                    unparseType(*p, ninfo2);
                    p++;
                    if (p != mfunc_type->get_arguments().end())
                       {
                         cur << ", ";
                       }
                  }
               cur << ")";
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

void Unparser::unparseArrayType(SgType* type, SgUnparse_Info& info) {
  SgArrayType* array_type = isSgArrayType(type);
  ROSE_ASSERT(array_type != NULL);
  
  // different cases to think about
  // int (*) [10],  int (*var) [20]
  // int *[10],  int *var[10]
  // int [10][20], int var[10][20]
  // multidimensional,
  //  int [2][10] is built up as
  //    ArrayType(base_type, 2)
  //      ArrayType(int, 10), because of the front-end

  SgUnparse_Info ninfo(info);
  int needParen=0;
  if(ninfo.isReferenceToSomething() || ninfo.isPointerToSomething()) {
    needParen=1;
  }

  if(ninfo.isTypeFirstPart()) {
    if(needParen) {
      ninfo.unset_isReferenceToSomething();
      ninfo.unset_isPointerToSomething();
      unparseType(array_type->get_base_type(), ninfo);
      cur << "(";
    } else unparseType(array_type->get_base_type(), ninfo);
  } else if(ninfo.isTypeSecondPart()) {
    if(needParen) { 
      cur << ")";
      info.unset_isReferenceToSomething();
      info.unset_isPointerToSomething();
    }
    cur << "[";
    if (array_type->get_index()) unparseExpression(array_type->get_index(), info); //get_index() returns an expr
    cur << "]";
    unparseType(array_type->get_base_type(), info);//second part
  } else {
    ninfo.set_isTypeFirstPart();
    unparseType(array_type, ninfo);
    ninfo.set_isTypeSecondPart();
    unparseType(array_type, ninfo);
  }
}


