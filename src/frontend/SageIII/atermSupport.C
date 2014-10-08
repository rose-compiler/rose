// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

// Put non-generated Aterm support code here.

#ifdef ROSE_USE_ROSE_ATERM_SUPPORT

using namespace std;
using namespace AtermSupport;

// Note that setting this to true was the original setting.
// #define LAZY_WRAPPING_MACRO true
#define LAZY_WRAPPING_MACRO false

string
AtermSupport::aterm_type_name( ATerm term )
   {
  // Debugging support.

     string s;
     int atermType = ATgetType(term);

     switch (atermType)
        {
          case AT_APPL:        s = "AT_APPL";        break;
          case AT_INT:         s = "AT_INT";         break;
          case AT_REAL:        s = "AT_REAL";        break;
          case AT_LIST:        s = "AT_LIST";        break;
          case AT_PLACEHOLDER: s = "AT_PLACEHOLDER"; break;
          case AT_BLOB:        s = "AT_BLOB";        break;

          default:
             {
               printf ("In graph(ATerm): term = %p: default reached atermType = %d \n",term,atermType);
               ROSE_ASSERT(!"error to reach default in switch!");
             }
        }

     return s;
   }

string AtermSupport::uniqueId(SgNode* n)
   {
  // return SageInterface::generateUniqueName(n, false);
  // return intToHex(n);

     if (n == NULL)
        {
          return "NULL";
        }

     ROSE_ASSERT(n != NULL);

     Sg_File_Info* fileInfo = isSg_File_Info(n);
     if (fileInfo != NULL)
        {
       // This could be how we handle source position information.
        }

  // DQ (9/18/2014): Ignore the different between defining vs. non-defining declarations (in generation of unique names).
  // string returnString = SageInterface::generateUniqueName(n, false);
     string returnString = SageInterface::generateUniqueName(n, true);

#if 0
     printf ("In uniqueId(): n = %p = %s returnString = %s \n",n,n->class_name().c_str(),returnString.c_str());
#endif

  // return SageInterface::generateUniqueName(n, false);
     return returnString;
   }

string AtermSupport::uniqueId(uint64_t n)
   {
  // This supports generation of strings from addresses (mostly so that the SgAsm support will compile).
  // I don't think that Aterms have a data type for unsigned 64-bit integers.

  // return an empty string for now.
     return "";
   }

string AtermSupport::uniqueId(const rose_rva_t & n)
   {
  // This supports generation of strings from rose_rva_t (so that the SgAsm support will compile).
  // I don't think it is clear how to implement this function (but we can worry about the binary analysis 
  // use of Aterms later).

  // return an empty string for now.
     return "";
   }


ATerm AtermSupport::convertFileInfoToAterm(Sg_File_Info* fi) 
   {
     ATerm term = ATmake("Position(<str>, <int>, <int>)", fi->get_filename(), fi->get_line(), fi->get_col());

     return term;
   }

ATerm AtermSupport::convertVectorToAterm(const vector<ATerm>& v, int start, int len) 
   {
     if (len == -1)
          len = v.size() - start;
     ATermList ls = ATmakeList0();
     for (int i = start; i < start + len; ++i)
          ls = ATappend(ls, v[i]);

     return ATmake("<term>", ls);
   }

template <typename Iter>
ATerm AtermSupport::convertRangeToAterm(Iter b, Iter e) 
   {
     ATermList ls = ATmakeList0();
     for (; b != e; ++b) 
        {
          ls = ATappend(ls, *b);
        }

     return ATmake("<term>", ls);
   }

template <typename Iter>
ATerm AtermSupport::convertSgNodeRangeToAterm(Iter b, Iter e) 
   {
     bool lazyWrapping = LAZY_WRAPPING_MACRO;

     ROSE_ASSERT(lazyWrapping == false);

     if ( (b != e) && (*b != NULL) )
        {
#if 0
          SgNode* parent = (*b)->get_parent();
          if (parent == NULL)
             {
               printf ("warning: parent == NULL: *b = %p = %s \n",*b,(*b)->class_name().c_str());
             }
#endif
       // ROSE_ASSERT(parent != NULL);
#if 0
          printf ("In AtermSupport::convertSgNodeRangeToAterm(): Building an ATerm list for (*b)->get_parent() = %p = %s \n",parent,(parent != NULL) ? parent->class_name().c_str() : "NULL");
#endif
        }
       else
        {
#if 0
          printf ("In AtermSupport::convertSgNodeRangeToAterm(): Building an empty list \n");
#endif
        }

     ATermList ls = ATmakeList0();
     for (; b != e; ++b) 
        {
          ATerm child_term = NULL;
          if (lazyWrapping == true)
             {
               child_term = ATmake("lazyWrap(<str>)","lazyWrap");
             }
            else
             {
#if 0
               printf ("In AtermSupport::convertSgNodeRangeToAterm(): Calling convertNodeToAterm() *b = %p = %s\n",*b,(*b != NULL) ? (*b)->class_name().c_str() : "NULL");
#endif
               child_term = convertNodeToAterm(*b);

#if 0
            // I think we are generating invalid aterms in some cases.
               int atermKind = ATgetType(child_term);
               printf ("In AtermSupport::convertSgNodeRangeToAterm(): child_term: atermKind = %d = %s \n",atermKind,aterm_type_name(child_term).c_str());
#endif
#if 0
               printf ("In AtermSupport::convertSgNodeRangeToAterm(): DONE: Calling convertNodeToAterm() *b = %p = %s \n",*b,(*b != NULL) ? (*b)->class_name().c_str() : "NULL");
#endif
#if 0
               cout << "In AtermSupport::convertSgNodeRangeToAterm(): DONE: output child_term: -> " << ATwriteToString(child_term) << endl;
#endif
             }

#if 0
          printf ("In AtermSupport::convertSgNodeRangeToAterm(): Calling ATappend() \n");
#endif
       // ls = ATappend(ls,convertNodeToAterm(*b));
          ls = ATappend(ls,child_term);

#if 0
       // I think we are generating invalid aterms in some cases.
          int atermKind = ATgetType(ls);
       // printf ("In AtermSupport::convertSgNodeRangeToAterm(): ATappend(): atermKind = %d = %s \n",atermKind,aterm_type_name(ls).c_str());
          printf ("In AtermSupport::convertSgNodeRangeToAterm(): ATappend(): atermKind = %d \n",atermKind);
#endif
#if 0
          printf ("In AtermSupport::convertSgNodeRangeToAterm(): DONE: Calling ATappend() \n");
#endif
        }

#if 0
     printf ("In AtermSupport::convertSgNodeRangeToAterm(): building aterm to return \n");
#endif

  // return ATmake("<term>", ls);
     ATerm returnTerm = ATmake("<term>", ls);

#if 0
  // I think we are generating invalid aterms in some cases.
     int atermKind = ATgetType(returnTerm);
     printf ("In AtermSupport::convertSgNodeRangeToAterm(): atermKind = %d = %s \n",atermKind,aterm_type_name(returnTerm).c_str());
#endif
#if 0
     printf ("In AtermSupport::convertSgNodeRangeToAterm(): returnTerm = %p ls = %p \n",returnTerm,ls);
#endif
#if 0
     cout << "AtermSupport::convertSgNodeRangeToAterm(): returning the aterm -> " << ATwriteToString(returnTerm) << endl;
#endif

     return returnTerm;
   }


string
AtermSupport::getShortVariantName(VariantT var) 
   {
     switch (var) 
        {
          case V_SgAddOp:                 return "Add";
          case V_SgMultiplyOp:            return "Multiply";
          case V_SgDivideOp:              return "Divide";
          case V_SgBitOrOp:               return "BitOr";
          case V_SgBitXorOp:              return "BitXor";
          case V_SgBitAndOp:              return "BitAnd";
          case V_SgLshiftOp:              return "LeftShift";
          case V_SgRshiftOp:              return "RightShift";
          case V_SgModOp:                 return "Mod";
          case V_SgBitComplementOp:       return "Complement";
          case V_SgLessThanOp:            return "Less";
          case V_SgLessOrEqualOp:         return "LessEqual";
          case V_SgGreaterThanOp:         return "Greater";
          case V_SgGreaterOrEqualOp:      return "GreaterEqual";
          case V_SgEqualityOp:            return "Equal";
          case V_SgNotEqualOp:            return "NotEqual";
          case V_SgAssignOp:              return "Assign";
          case V_SgPlusAssignOp:          return "AddAssign";
          case V_SgPlusPlusOp:            return "Increment";
          case V_SgPntrArrRefExp:         return "Index";
          case V_SgFunctionCallExp:       return "Call";
          case V_SgVariableDeclaration:   return "VarDecl";
          case V_SgForStatement:          return "For";
          case V_SgIfStmt:                return "If";
          case V_SgWhileStmt:             return "While";
          case V_SgReturnStmt:            return "Return";
          case V_SgSwitchStatement:       return "Switch";
          case V_SgCaseOptionStmt:        return "Case";
          case V_SgDefaultOptionStmt:     return "Default";
          case V_SgExprStatement:         return "ExprStmt";
          case V_SgTypeInt:               return "Int";
          case V_SgTypeLong:              return "Long";
          case V_SgTypeLongLong:          return "LongLong";
          case V_SgTypeShort:             return "Short";
          case V_SgTypeChar:              return "Char";
          case V_SgTypeSignedChar:        return "SignedChar";
          case V_SgTypeUnsignedChar:      return "UnsignedChar";
          case V_SgTypeUnsignedInt:       return "UnsignedInt";
          case V_SgTypeUnsignedLong:      return "UnsignedLong";
          case V_SgTypeUnsignedLongLong:  return "UnsignedLongLong";
          case V_SgTypeUnsignedShort:     return "UnsignedShort";
          case V_SgTypeBool:              return "Bool";
          case V_SgTypeVoid:              return "Void";
          case V_SgTypeFloat:             return "Float";
          case V_SgTypeDouble:            return "Double";
          case V_SgTypeLongDouble:        return "LongDouble";
          case V_SgTypeDefault:           return "DefaultType";
          case V_SgTypeEllipse:           return "EllipsesType";
          case V_SgAssignInitializer:     return "AssignInit";
          case V_SgCastExp:               return "Cast";
          case V_SgGlobal:                return "Global";
          case V_SgFunctionParameterList: return "ParameterList";
          case V_SgProject:               return "Project";
          case V_SgForInitStatement:      return "ForInit";
          case V_SgExprListExp:           return "ExprList";
          case V_SgClassDefinition:       return "ClassDefinition";
          case V_SgFunctionDefinition:    return "FunctionDefinition";
          case V_SgBasicBlock:            return "Block";
          case V_SgNullStatement:         return "NullStmt";

          default:                        return getVariantName(var);
        }
   }


ATerm AtermSupport::getTraversalChildrenAsAterm(SgNode* n) 
   {
  // This function is using the same infrastructue used to support the AST traversals, so it
  // is equivalent in what it traverses and thus properly traverses the defined ROSE AST.

     vector<SgNode*> children = n->get_traversalSuccessorContainer();

#if 0
     printf ("In AtermSupport::getTraversalChildrenAsAterm(): n = %p = %s children.size() = %zu \n",n,n->class_name().c_str(),children.size());
     for (vector<SgNode*>::iterator i = children.begin(); i != children.end(); i++)
        {
          SgNode* child = *i;
          printf ("   --- child = %p = %s \n",child,(child != NULL) ? child->class_name().c_str() : "NULL");
        }
#endif

  // return convertSgNodeRangeToAterm(children.begin(), children.end());
     ATerm term = convertSgNodeRangeToAterm(children.begin(), children.end());

  // I think we are generating invalid aterms in some cases.
#if 0
     int atermKind = ATgetType(term);
     printf ("In AtermSupport::getTraversalChildrenAsAterm(): atermKind = %d = %s \n",atermKind,aterm_type_name(term).c_str());
#endif
#if 0
     printf ("In AtermSupport::getTraversalChildrenAsAterm(): returning the aterm \n");
#endif
#if 0
     cout << "AtermSupport::getTraversalChildrenAsAterm(): returning the aterm -> " << ATwriteToString(term) << endl;
#endif

     return term;
   }


ATerm AtermSupport::convertNodeToAterm(SgNode* n) 
   {
  // What is needed to build the ROSE AST from ATerms:
  // 1) Many declarations require the scope so the scope must be saved within the generated aterm.
  //    The scope is not part of the AST defined traversal but is auxilary data in some IR nodes 
  //    (as required).  Scopes must be constructed as aterms with save unique names to support 
  //    their being saved into maps when generated the ROSE AST from the aterms.  All IR nodes
  //    requiring scope data, must then use the string (as a key) and the map of saved scopes to
  //    look up the scopes (as needed).
  // 2) Types are not in the defined AST (but are present as annotations, formally; and as pointers 
  //    to IR nodes in the implementation of the ROSE AST).  We need to be able to find the associated
  //    types (e.g. when building variable declarations).  Types need to be saved with a unique
  //    string that we can use as a key to save the type and also look up the type as needed.
  //    Some types are easy to build from aterms, e.g. primative types only require a name.
  //    Other types require the declaration to support their construction and then a name
  //    to use if they were previously constructed.  A simpler solution would be to always use the 
  //    uniquely generated identifier for the declaration and they use a seperate map in the
  //    construction of the IR nodes from the aterm; a map of types.

  // 3) Most other kinds or IR nodes should have a more straight forward implementation.

      ATerm term;

     if (n == NULL)
        {
#if 0
          printf ("In AtermSupport::convertNodeToAterm(): n == NULL \n");
#endif
          term = ATmake("NULL");

#if 0
       // I think we are generating invalid aterms in some cases.
          int atermKind = ATgetType(term);
          printf ("In AtermSupport::convertNodeToAterm(SgNode* n): NULL ATERM: atermKind = %d = %s \n",atermKind,aterm_type_name(term).c_str());
#endif
#if 0
          cout << "AtermSupport::convertNodeToAterm(): null aterm (no annotations): NULL -> " << ATwriteToString(term) << endl;
#endif
        }
       else
        {
          ROSE_ASSERT(n != NULL);

#if 0
          printf ("In AtermSupport::convertNodeToAterm(SgNode* n): n = %p = %s \n",n,n->class_name().c_str());
#endif

          term = n->generate_ATerm();

#if 0
       // I think we are generating invalid aterms in some cases.
          int atermKind = ATgetType(term);
          printf ("In AtermSupport::convertNodeToAterm(SgNode* n): n = %p = %s atermKind = %d = %s \n",n,n->class_name().c_str(),atermKind,aterm_type_name(term).c_str());
#endif
#if 0
          cout << "AtermSupport::convertNodeToAterm(): before annotations: " << n->class_name() << " -> " << ATwriteToString(term) << endl;
#endif
#if 1
          n->generate_ATerm_Annotation(term);
#if 0
          cout << "AtermSupport::convertNodeToAterm(): after annotations: " << n->class_name() << " -> " << ATwriteToString(term) << endl;
#endif
#endif
#if 0
          printf ("Leaving AtermSupport::convertNodeToAterm(SgNode* n): n = %p = %s \n",n,n->class_name().c_str());
#endif
        }

     return term;
   }



ATerm AtermSupport::getAtermAnnotation(ATerm term, char* annotation_name ) 
   {
     ROSE_ASSERT(annotation_name != NULL);

     ATerm idannot = ATgetAnnotation(term, ATmake(annotation_name));
     if (idannot)
        {
#if 1
          printf ("Found an annotation \n");
#endif
          char* unique_string = NULL;
       // if (ATmatch("id(<str>)", &unique_string))
          if (ATmatch(idannot,"<str>", &unique_string))
             {
               printf ("unique_string = %s \n",unique_string);
#if 0
               updateDeclarationMap(unique_string,decl);
#if 0
            // Not clear if I need to build the type directly here, I now think we need this step as well.
            // ROSE_ASSERT(decl->get_type() != NULL);

               SgClassType* classType = new SgClassType(decl);
            // updateTypeMap(str,classType);
               updateTypeMap(unique_string,classType);
#endif
               decl->set_type(classType);
               ROSE_ASSERT(decl->get_type() != NULL);
#endif
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
            // If the annotation is not a string it might be an integer.
               int unique_integer = -1;
               if (ATmatch(idannot,"<int>", &unique_integer))
                  {
                    printf ("unique_integer = %d \n",unique_integer);

                    printf ("integer aterm annotations not implemented yet! \n");
                    ROSE_ASSERT(false);
                  }
                 else
                  {
                    printf ("Unknown annotation: annotation_name = %s \n",annotation_name);
                    ROSE_ASSERT(false);
                  }
             }
        }
       else
        {
          printf ("Error: annotation required: annotation_name = %s (needed to define key for updateDeclarationMap()) \n",annotation_name);
          ROSE_ASSERT(false);
        }
   }


vector<ATerm> AtermSupport::getAtermList(ATerm ls) 
   {
     ATerm a, b;
     vector<ATerm> result;
     while (true) 
        {
          if (ATmatch(ls, "[]"))
               return result;
            else 
               if (ATmatch(ls, "[<term>, <list>]", &a, &b)) 
                  {
                    result.push_back(a);
                    ls = b;
                  } 
                 else
                    ROSE_ASSERT (!"getAtermList");
        }
   }


string 
AtermSupport::getAtermStringAttribute(ATerm term, const string & annotationName ) 
   {
  // This gets the string from an attribut where the know the type is a string.
  // It is used to assign data members where the data member is a string (e.g. SgName in a SgClassDeclaration).

     string returnString;

     ATerm idannot = ATgetAnnotation(term, ATmake(annotationName.c_str()));
     if (idannot)
        {
#if 1
          printf ("Found an annotation: annotationName = %s \n",annotationName.c_str());
#endif
          char* id = NULL;
          if (ATmatch(idannot, "<str>", &id))
             {
#if 0
                ROSE_ASSERT(result != NULL);
                printf ("Setting targetLocations map: id = %s result = %p = %s \n",id,result,result->class_name().c_str());
#endif
                returnString = id;
             }
        }

     return returnString;
   }

#if 0
// DQ (10/7/2014): Not celar if we need this function.

string 
AtermSupport::processAtermAttribute(ATerm term) 
   {
  // This function processes the attribute where it is a associated with a SgType, SgDeclaration, or SgInitializedName.

     ATerm idannot = ATgetAnnotation(term, ATmake("id"));
     if (idannot)
        {
#if 1
          printf ("Found an annotation \n");
#endif
          char* id;
          if (ATmatch(idannot, "<str>", &id))
             {
#if 1
                ROSE_ASSERT(result != NULL);
                printf ("Setting targetLocations map: id = %s result = %p = %s \n",id,result,result->class_name().c_str());
#endif
            // Organize as seperate translation maps to simplify the debugging (also allows the same key to be used for types and declarations, etc.).
            // targetLocations[id] = result;
               SgScopeStatement*       scope           = isSgScopeStatement(result);
               SgDeclarationStatement* declaration     = isSgDeclarationStatement(result);
               SgType*                 type            = isSgType(result);
               SgInitializedName*      initializedName = isSgInitializedName(result);
               if (scope != NULL)
                  {
                    updateScopeMap(id,scope);
                  }
                 else
                  {
                    if (declaration != NULL)
                       {
                         updateDeclarationMap(id,declaration);
                       }
                      else
                       {
                         if (type != NULL)
                            {
                              updateTypeMap(id,type);
                            }
                           else
                            {
                              if (initializedName != NULL)
                                 {
                                   updateInitializedNameMap(id,initializedName);
                                 }
                                else
                                 {
                                   printf ("Not updated in translation map: result = %p = %s \n",result,result->class_name().c_str());
                                 }
                            }
                       }
                  }
             }
            else
             {
               ROSE_ASSERT (!"id annotation has incorrect format");
             }
        }
       else
        {
#if 1
          printf ("No annotation found! \n");
#endif
        }
#endif

// endif for ROSE_USE_ROSE_ATERM_SUPPORT
#endif

