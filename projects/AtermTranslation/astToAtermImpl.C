#include "rose.h"
#include <stdio.h>
#include "aterm1.h"
#include "aterm2.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "atermTranslation.h"

// #include "MergeUtils.h"

using namespace std;

// Note that setting this to true was the original setting.
// #define LAZY_WRAPPING_MACRO true
#define LAZY_WRAPPING_MACRO false

std::string uniqueId(SgNode* n)
   {
  // return SageInterface::generateUniqueName(n, false);
  // return intToHex(n);

     ROSE_ASSERT(n != NULL);

  // DQ (9/18/2014): Ignore the different between defining vs. non-defining declarations (in generation of unique names).
  // string returnString = SageInterface::generateUniqueName(n, false);
     string returnString = SageInterface::generateUniqueName(n, true);

#if 0
     printf ("In uniqueId(): n = %p = %s returnString = %s \n",n,n->class_name().c_str(),returnString.c_str());
#endif

  // return SageInterface::generateUniqueName(n, false);
     return returnString;
   }

ATerm convertFileInfoToAterm(Sg_File_Info* fi) 
   {
     ATerm term = ATmake("Position(<str>, <int>, <int>)", fi->get_filename(), fi->get_line(), fi->get_col());

     return term;
   }

ATerm convertVectorToAterm(const vector<ATerm>& v, int start, int len) 
   {
     if (len == -1)
          len = v.size() - start;
     ATermList ls = ATmakeList0();
     for (int i = start; i < start + len; ++i)
          ls = ATappend(ls, v[i]);

     return ATmake("<term>", ls);
   }

template <typename Iter>
ATerm convertRangeToAterm(Iter b, Iter e) 
   {
     ATermList ls = ATmakeList0();
     for (; b != e; ++b) 
        {
          ls = ATappend(ls, *b);
        }

     return ATmake("<term>", ls);
   }

template <typename Iter>
ATerm convertSgNodeRangeToAterm(Iter b, Iter e) 
   {
     bool lazyWrapping = LAZY_WRAPPING_MACRO;

     if ( (b != e) && (*b != NULL) )
        {
          SgNode* parent = (*b)->get_parent();
#if 0
          if (parent == NULL)
             {
               printf ("warning: parent == NULL: *b = %p = %s \n",*b,(*b)->class_name().c_str());
             }
#endif
       // ROSE_ASSERT(parent != NULL);
#if 0
          printf ("Building an ATerm list for (*b)->get_parent() = %p = %s \n",parent,parent->class_name().c_str());
#endif
        }
       else
        {
#if 0
          printf ("Building an empty list \n");
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
               child_term = convertNodeToAterm(*b);
             }

       // ls = ATappend(ls,convertNodeToAterm(*b));
          ls = ATappend(ls,child_term);
        }

  // return ATmake("<term>", ls);
     ATerm returnTerm = ATmake("<term>", ls);

#if 0
     printf ("In convertSgNodeRangeToAterm(): returnTerm = %p ls = %p \n",returnTerm,ls);
#endif

     return returnTerm;
   }

class NullChecker: public AstBottomUpProcessing<bool> 
   {
     public:
          virtual bool evaluateSynthesizedAttribute(SgNode* node, vector<bool> ls) 
             {
               assert (std::accumulate(ls.begin(), ls.end(), true, std::logical_and<bool>()) || ((cout << node->sage_class_name() << endl), false));
               return true;
             }
   };

string getShortVariantName(VariantT var) 
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


ATerm getTraversalChildrenAsAterm(SgNode* n) 
   {
  // This function is using the same infrastructue used to support the AST traversals, so it
  // is equivalent in what it traverses and thus properly traverses the defined ROSE AST.

     vector<SgNode*> children = n->get_traversalSuccessorContainer();

#if 1
     printf ("In getTraversalChildrenAsAterm(): n = %p = %s children.size() = %zu \n",n,n->class_name().c_str(),children.size());
#endif

     return convertSgNodeRangeToAterm(children.begin(), children.end());
   }


ATerm convertNodeToAterm(SgNode* n) 
   {
  // These will ultimately become functions generated by ROSETTA, 
  // but until then this is a simpler implementation to manage.

  // What is needed to build the ROSE AST from ATerms:
  // 1) Many declarations require the scope so the scope must be saved within the generated aterm.
  //    The scope is not part of the AST defined traversal but is auxilery data in some IR nodes 
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

  // This variable controls the use of lazy wrapping of ROSE AST nodes into ATerms.
     bool lazyWrapping = LAZY_WRAPPING_MACRO;

     if (n == NULL)
        {
#if 0
          printf ("convertNodeToAterm(): n = %p = %s \n",n,"NULL");
#endif
          return ATmake("NULL");
        }

     ROSE_ASSERT(n != NULL);
#if 0
     printf ("convertNodeToAterm(): n = %p = %s \n",n,n->class_name().c_str());
#endif

     printf ("This function is no longer called! \n");
     ROSE_ASSERT(false);

     ATerm term;

#if 1
  // This is the newest ROSE builtin ATerm support.
     term = AtermSupport::convertNodeToAterm(n);
#endif

#if 0
  // Start of large commented out section

     switch (n->variantT())
        {
       // case V_SgFile:
          case V_SgSourceFile:
             {
            // Special case needed to include file name
            // term = ATmake("File(<str>, <term>)", isSgFile(n)->getFileName(), convertNodeToAterm(isSgFile(n)->get_root()));
            // term = ATmake("File(<str>, <term>)", isSgSourceFile(n)->getFileName().c_str(), convertNodeToAterm(isSgSourceFile(n)->get_globalScope()));
               ATerm child_term = NULL;
               if (lazyWrapping == true)
                  {
                 // Build a node to recognize as a terminal in the lazily wrapped ATerm.
                 // When we see this as a child we know that the parent has to be force 
                 // to interpret its child aterms.  This should be handled in the 
                 // ATgetArgument() function.
                    child_term = ATmake("lazyWrap(<str>)","lazyWrap");
                  }
                 else
                  {
                    child_term = convertNodeToAterm(isSgSourceFile(n)->get_globalScope());
                  }
               term = ATmake("File(<str>, <term>)",isSgSourceFile(n)->getFileName().c_str(),child_term);
               break;
             }

          case V_SgPlusPlusOp:
          case V_SgMinusMinusOp:
             {
            // Special cases needed to include prefix/postfix status
               ATerm child_term = NULL;
               if (lazyWrapping == true)
                  {
                    child_term = ATmake("lazyWrap(<str>)","lazyWrap");
                  }
                 else
                  {
                    child_term = convertNodeToAterm(isSgUnaryOp(n)->get_operand());
                  }
            // term = ATmake("<appl(<appl>, <term>)>", getShortVariantName((VariantT)(n->variantT())).c_str(), 
            //               (isSgUnaryOp(n)->get_mode() == SgUnaryOp::prefix ? "Prefix" : isSgUnaryOp(n)->get_mode() == SgUnaryOp::postfix ? "Postfix" : "Unknown"),convertNodeToAterm(isSgUnaryOp(n)->get_operand()));
               term = ATmake("<appl(<appl>, <term>)>", getShortVariantName((VariantT)(n->variantT())).c_str(), 
                             (isSgUnaryOp(n)->get_mode() == SgUnaryOp::prefix ? "Prefix" : isSgUnaryOp(n)->get_mode() == SgUnaryOp::postfix ? "Postfix" : "Unknown"), child_term);
               break;
             }
#if 0
          case V_SgExpressionRoot:
            // Special case to remove this node
               term = convertNodeToAterm(isSgExpressionRoot(n)->get_operand());
               break;
#endif
          case V_SgCastExp:
             {
            // Special case needed to include type
               ATerm child_term_1 = NULL;
               ATerm child_term_2 = NULL;
               if (lazyWrapping == true)
                  {
                    child_term_1 = ATmake("lazyWrap(<str>)","lazyWrap");
                    child_term_2 = ATmake("lazyWrap(<str>)","lazyWrap");
                  }
                 else
                  {
                    child_term_1 = convertNodeToAterm(isSgUnaryOp(n)->get_operand());
                    child_term_2 = convertNodeToAterm(isSgCastExp(n)->get_type());
                  }
            // term = ATmake("Cast(<term>, <term>)>",convertNodeToAterm(isSgUnaryOp(n)->get_operand()),convertNodeToAterm(isSgCastExp(n)->get_type()));
               term = ATmake("Cast(<term>, <term>)>",child_term_1,child_term_2);
               break;
             }

          case V_SgVarRefExp:
            // Special case needed to include id
               term = ATmake("Var(<str>)",uniqueId(isSgVarRefExp(n)->get_symbol()->get_declaration()).c_str());
               break;

          case V_SgFunctionRefExp:
            // Special case needed to include id
               term = ATmake("Func(<str>)",uniqueId(isSgFunctionRefExp(n)->get_symbol()->get_declaration()).c_str());
               break;

          case V_SgIntVal:
            // Special case needed to include value
               term = ATmake("IntC(<int>)", isSgIntVal(n)->get_value());
               break;

          case V_SgUnsignedIntVal:
               term = ATmake("UnsignedIntC(<int>)", isSgUnsignedIntVal(n)->get_value());
               break;

          case V_SgUnsignedLongVal: 
             {
               ostringstream s;
               s << isSgUnsignedLongVal(n)->get_value();
               term = ATmake("UnsignedLongC(<str>)", s.str().c_str());
               break;
             }

          case V_SgUnsignedLongLongIntVal: 
             {
               ostringstream s;
               s << isSgUnsignedLongLongIntVal(n)->get_value();
               term = ATmake("UnsignedLongLongC(<str>)", s.str().c_str());
               break;
             }

          case V_SgDoubleVal:
               term = ATmake("DoubleC(<real>)", isSgDoubleVal(n)->get_value());
               break;

          case V_SgInitializedName:
             {
            // Works around double initname problem
               SgInitializer* initializer = isSgInitializedName(n)->get_initializer();
               const SgName& name = isSgInitializedName(n)->get_name();
               SgType* type = isSgInitializedName(n)->get_type();

               ROSE_ASSERT(type != NULL);
#if 0
               printf ("convertNodeToAterm(): case V_SgInitializedName: name = %s initializer = %p type = %p = %s \n",name.str(),initializer,type,type->class_name().c_str());
#endif
            // Works around fact that ... is not really an initname and shouldn't be a type either
               if (isSgTypeEllipse(type))
                  {
                    term = ATmake("Ellipses");
                  }
                 else
                  {
                    std::string uniqueIdString = uniqueId(n);

                    ATerm child_term_1 = NULL;
                    ATerm child_term_2 = NULL;
                    if (lazyWrapping == true)
                       {
                         child_term_1 = ATmake("lazyWrap(<str>)","lazyWrap");
                         child_term_2 = ATmake("lazyWrap(<str>)","lazyWrap");
                       }
                      else
                       {
                         child_term_1 = convertNodeToAterm(type);
                         child_term_2 = convertNodeToAterm(initializer);
                       }
#if 0
                    printf ("uniqueIdString = %s \n",uniqueIdString.c_str());
                    printf ("Calling generate ATerm for SgInitializedName->get_name() name = %s \n",name.str());
                    ATerm name_aterm = ATmake("Name(<str>)",name.str());
                 // ATerm name_aterm = ATmake(name.str());
                    printf ("Calling convertNodeToAterm(type) \n");
                    ATerm type_aterm = convertNodeToAterm(type);
                    printf ("Calling convertNodeToAterm(initializer) \n");
#endif
#if 0
                    printf ("Calling ATmake() \n");
#endif
#if 1
                 // term = ATmake("InitName(<str>, <term>, <term>) {[id, <str>]}",(name.str() ? name.str() : ""),convertNodeToAterm(type),convertNodeToAterm(initializer),uniqueId(n).c_str());
                    term = ATmake("InitName(<str>, <term>, <term>) {[id, <str>]}",(name.str() ? name.str() : ""),child_term_1,child_term_2);
#else
                    ATerm initializer_aterm = convertNodeToAterm(initializer);
                    term = ATmake("InitName(<term>,<term>)",
                                  //(name.str() ? name.str() : ""), 
                                  // name_aterm,
                                    type_aterm, 
                                    initializer_aterm
                                 // uniqueId(n).c_str());
                                 // uniqueIdString.c_str());
                                    );
#endif
#if 0
                    printf ("Calling ATsetAnnotation() \n");
#endif
#if 1
                    term = ATsetAnnotation(term, ATmake("id"), ATmake("<str>", uniqueId(n).c_str()));
#else
                    printf ("In convertNodeToAterm(): Skipping annotation for uniqueId for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
#if 0
                    printf ("DONE: Calling ATsetAnnotation() \n");
#endif
                  }

               break;
             }

          case V_SgFunctionDeclaration: 
             {
            // Special case needed to include name
               SgFunctionDeclaration* fd = isSgFunctionDeclaration(n);
               ATerm child_term_1 = NULL;
               ATerm child_term_2 = NULL;
               ATerm child_term_3 = NULL;
               if (lazyWrapping == true)
                  {
                    child_term_1 = ATmake("lazyWrap(<str>)","lazyWrap");
                    child_term_2 = ATmake("lazyWrap(<str>)","lazyWrap");
                    child_term_3 = ATmake("lazyWrap(<str>)","lazyWrap");
                  }
                 else
                  {
                    child_term_1 = convertNodeToAterm(fd->get_orig_return_type());
                    child_term_2 = convertSgNodeRangeToAterm(fd->get_args().begin(),fd->get_args().end());
                    child_term_3 = convertNodeToAterm(fd->get_definition());
                  }

            // term = ATmake("Function(<str>, <term>, <term>, <term>)",fd->get_name().str(),convertNodeToAterm(fd->get_orig_return_type()),convertSgNodeRangeToAterm(fd->get_args().begin(),fd->get_args().end()),convertNodeToAterm(fd->get_definition()));
               term = ATmake("Function(<str>, <term>, <term>, <term>)",fd->get_name().str(),child_term_1,child_term_2,child_term_3);
#if 1
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
#else
               printf ("In convertNodeToAterm(): Skipping annotation for uniqueId for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
               break;
             }

          case V_SgMemberFunctionDeclaration:
             {
            // This is mostly a copy of the case for SgFunctionDeclaration
            // Special case needed to include name
               SgMemberFunctionDeclaration* fd = isSgMemberFunctionDeclaration(n);
               ATerm child_term_1 = NULL;
               ATerm child_term_2 = NULL;
               ATerm child_term_3 = NULL;
               if (lazyWrapping == true)
                  {
                    child_term_1 = ATmake("lazyWrap(<str>)","lazyWrap");
                    child_term_2 = ATmake("lazyWrap(<str>)","lazyWrap");
                    child_term_3 = ATmake("lazyWrap(<str>)","lazyWrap");
                  }
                 else
                  {
                    child_term_1 = convertNodeToAterm(fd->get_orig_return_type());
                    child_term_2 = convertSgNodeRangeToAterm(fd->get_args().begin(),fd->get_args().end());
                    child_term_3 = convertNodeToAterm(fd->get_definition());
                  }

            // term = ATmake("Function(<str>, <term>, <term>, <term>)",fd->get_name().str(),convertNodeToAterm(fd->get_orig_return_type()),convertSgNodeRangeToAterm(fd->get_args().begin(),fd->get_args().end()),convertNodeToAterm(fd->get_definition()));
               term = ATmake("MemberFunction(<str>, <term>, <term>, <term>)",fd->get_name().str(),child_term_1,child_term_2,child_term_3);
#if 1
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
#else
               printf ("In convertNodeToAterm(): Skipping annotation for uniqueId for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
               break;
             }

          case V_SgClassDeclaration: 
             {
            // Special case needed to distinguish forward/full definitions and to
            // include class name
               SgClassDeclaration* decl = isSgClassDeclaration(n);
               assert (decl);
               SgName sname = decl->get_name();
               const char* name = sname.str();
            // Suggestion: have a field named local_definition in each class
            // declaration that is 0 whenever the current declaration doesn't
            // have a definition attached, even if there is another declaration
            // which does have a definition attached.
               SgClassDefinition* defn = decl->get_definition();
            // cout << "defn = 0x" << hex << defn << endl << dec;
               if (decl->isForward())
                    defn = 0;

               SgClassDeclaration* nondefiningDeclaration = isSgClassDeclaration(decl->get_firstNondefiningDeclaration());
               ROSE_ASSERT(nondefiningDeclaration != NULL);
            // decl = nondefiningDeclaration;

               if (defn != NULL)
                  {
                 // Will be simpler when SgName becomes string
                    ATerm child_term = NULL;
                    if (lazyWrapping == true)
                       {
                         child_term = ATmake("lazyWrap(<str>)","lazyWrap");
                       }
                      else
                       {
                         child_term = convertNodeToAterm(defn);
                       }

                 // term = ATmake("Class(<str>, <term>)", (name ? name : ""), convertNodeToAterm(defn));
                    term = ATmake("Class(<str>, <term>)", (name ? name : ""), child_term);

                 // printf ("In convertNodeToAterm(): building aterm for SgClassDeclaration: adding annotation: uniqueId for term = %p node = %p = %s uniqueId(n) = %s \n",term,n,n->class_name().c_str(),uniqueId(n).c_str());
                    printf ("In convertNodeToAterm(): building aterm for SgClassDeclaration: adding annotation: uniqueId for term = %p node = %p = %s uniqueId(n) = %s \n",
                         term,nondefiningDeclaration,nondefiningDeclaration->class_name().c_str(),uniqueId(nondefiningDeclaration).c_str());
#if 1
                 // term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>",uniqueId(n).c_str()));
                    term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>",uniqueId(nondefiningDeclaration).c_str()));
#else
                    printf ("In convertNodeToAterm(): Skipping annotation for uniqueId for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
                  }
                 else
                  {
#if 1
                 // We want to also save the scope.
                    SgScopeStatement* rose_scope = decl->get_scope();
                    ROSE_ASSERT(rose_scope != NULL);

                 // SgName mangled_name = rose_scope->get_mangled_name();
                    SgName mangled_name = uniqueId(rose_scope).c_str();

                    printf ("Build an ATerm for the scope: mangled_name = %s \n",mangled_name.str());

                 // ATerm scope = ATmake("ScopeRef(<term>)",convertNodeToAterm(rose_scope));
                    ATerm scope = ATmake("ScopeRef(<str>)",mangled_name.str());

                    printf ("DONE: Build an ATerm for the scope! \n");
#endif
                 // term = ATmake("ClassFwd(<str>)", (name ? name : ""));
                    term = ATmake("ClassFwd(<str>, <term>)", (name ? name : ""),scope);
#if 1
                 // term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>",uniqueId(n).c_str()));
                    term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>",uniqueId(nondefiningDeclaration).c_str()));
#else
                    printf ("In convertNodeToAterm(): Skipping annotation for uniqueId for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
                  }

               break;
             }

          case V_SgEnumDeclaration: 
             {
            // Special case to include enum name and enumerator names which are not traversal children
               SgName sname = isSgEnumDeclaration(n)->get_name();
               const char* name = sname.str();
               const SgInitializedNamePtrList & enumerators = isSgEnumDeclaration(n)->get_enumerators();
               term = ATmake("Enum(<str>, <term>)", (name ? name : "{anonymous}"), convertSgNodeRangeToAterm(enumerators.begin(),enumerators.end()));
#if 1
               term = ATsetAnnotation(term, ATmake("id"), ATmake("<str>", uniqueId(n).c_str()));
#else
               printf ("In convertNodeToAterm(): Skipping annotation for uniqueId for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
               break;
             }

          case V_SgPointerType: 
             {
            // Special case because types can't be traversed, becasue they can't 
            // be traversed, we don't really need to support wrapping these...
               ATerm child_term = NULL;
               if (lazyWrapping == true)
                  {
                    child_term = ATmake("lazyWrap(<str>)","lazyWrap");
                  }
                 else
                  {
                    SgType* type = isSgPointerType(n)->get_base_type();
                    child_term = convertNodeToAterm(type);
                  }

            // SgType* type = isSgPointerType(n)->get_base_type();
            // ATerm t = convertNodeToAterm(type);
            // term = ATmake("Pointer(<term>)", t);
               term = ATmake("Pointer(<term>)", child_term);
               break;
             }

          case V_SgReferenceType: 
             {
            // Special case because types can't be traversed, skip support for lazy wrapping.
               SgType* type = isSgReferenceType(n)->get_base_type();
               ATerm t = convertNodeToAterm(type);
               term = ATmake("Reference(<term>)", t);
               break;
             }

          case V_SgModifierType: 
             {
            // Special case for type traversal and to prettify modifier names
               SgType* type = isSgModifierType(n)->get_base_type();
               SgTypeModifier& modifier = isSgModifierType(n)->get_typeModifier();
               SgConstVolatileModifier& cvmod = modifier.get_constVolatileModifier();
               term = convertNodeToAterm(type);

            // Wrap as a const or volatile modifier.
               if (cvmod.isConst())
                    term = ATmake("Const(<term>)", term);
               if (cvmod.isVolatile())
                    term = ATmake("Volatile(<term>)", term);

               break;
             }

          case V_SgArrayType: 
             {
            // Special case because types can't be traversed yet, and to get length
               SgType* type = isSgArrayType(n)->get_base_type();
               ATerm t = convertNodeToAterm(type);
#if 0
               printf ("isSgArrayType(n)->get_index() = %p \n",isSgArrayType(n)->get_index());
               printf ("n->get_traversalSuccessorContainer().size() = %zu \n",n->get_traversalSuccessorContainer().size());
#endif
            // I think we just want the index in this case.
            // term = ATmake("Array(<term>, <term>)", t, (isSgArrayType(n)->get_index() ? convertNodeToAterm((n->get_traversalSuccessorContainer())[4]) : ATmake("<str>", "NULL")));
               term = ATmake("Array(<term>, <term>)", t, (isSgArrayType(n)->get_index() ? convertNodeToAterm(isSgArrayType(n)->get_index()) : ATmake("<str>", "NULL")));
               assert (term);
               break;
             }

          case V_SgFunctionType: 
             {
            // Special case to allow argument list to be traversed, skip support for lazy wrapping
               SgFunctionType* ft = isSgFunctionType(n);

               ATerm ret       = NULL;
               ATerm args_list = NULL;
               if (lazyWrapping == true)
                  {
                    ret       = ATmake("lazyWrap(<str>)","lazyWrap");
                    args_list = ATmake("lazyWrap(<str>)","lazyWrap");
                  }
                 else
                  {
                    ret       = convertNodeToAterm(ft->get_return_type());
                    args_list = convertSgNodeRangeToAterm(ft->get_arguments().begin(),ft->get_arguments().end());
                  }

            // ATerm ret = convertNodeToAterm(ft->get_return_type());
            // ATerm args_list = convertSgNodeRangeToAterm(ft->get_arguments().begin(),ft->get_arguments().end());
               term = ATmake("FunctionType(<term>, <term>)", ret, args_list);
               break;
             }

          case V_SgEnumType:
             {
            // Special cases to optionally put in type definition instead of reference
               SgNamedType* nt = isSgNamedType(n);
               assert (nt);
               SgName sname = nt->get_name();
            // char* name = sname.str();
               SgDeclarationStatement* decl = nt->get_declaration();
               assert (decl);

            // Use the first nondefining declaration as a general rule.
               if (decl->get_firstNondefiningDeclaration() != NULL)
                    decl = decl->get_firstNondefiningDeclaration();
               ROSE_ASSERT(decl != NULL);

               ROSE_ASSERT(lazyWrapping == false);

            // term = ATmake("ClassType(<term>)",(nt->get_autonomous_declaration() || !defn ? ATmake("id(<str>)", uniqueId(decl).c_str()) : convertNodeToAterm(nt->get_declaration())));
               term = ATmake("EnumType(<term>)",(nt->get_autonomous_declaration() ? ATmake("id(<str>)", uniqueId(decl).c_str()) : convertNodeToAterm(nt->get_declaration())));
               break;
             }

          case V_SgClassType:
             {
            // We want to store the reference to the declaration (so that we can build the 
            // type in the conversion of Aterm -> ROSE AST), and a name to use to reference 
            // the type if it has been previously built.

            // Special cases to optionally put in type definition instead of reference
               SgNamedType* nt = isSgNamedType(n);
               assert (nt);
               SgName sname = nt->get_name();
            // char* name = sname.str();

               SgDeclarationStatement* decl = nt->get_declaration();
               assert (decl != NULL);

            // Use the first nondefining declaration as a general rule.
               if (decl->get_firstNondefiningDeclaration() != NULL)
                    decl = decl->get_firstNondefiningDeclaration();
               ROSE_ASSERT(decl != NULL);

            // SgClassDefinition* defn = isSgClassDeclaration(decl) ? isSgClassDeclaration(decl)->get_definition() : NULL;

               ROSE_ASSERT(lazyWrapping == false);

            // This field is not used in ROSE any more and is set by default to be true.
               ROSE_ASSERT(nt->get_autonomous_declaration() == true);

            // printf ("Building ATerm for SgClassType: uniqueId(decl) = %s nt->get_autonomous_declaration() = %s defn = %p \n",uniqueId(decl).c_str(),nt->get_autonomous_declaration() ? "true" : "false",defn);
               printf ("Building ATerm for SgClassType: uniqueId(decl) = %s \n",uniqueId(decl).c_str());

            // term = ATmake("ClassType(<term>)",(nt->get_autonomous_declaration() || !defn ? ATmake("id(<str>)", uniqueId(decl).c_str()) : convertNodeToAterm(nt->get_declaration())));
               term = ATmake("ClassType(<term>)",ATmake("id(<str>)", uniqueId(decl).c_str()));
               break;
             }

          case V_SgTypedefType:
             {
            // printf ("WARNING: mixing SgEnumType, SgClassType, and SgTypedefType to build a common aterm kind (should be seperatd for each case). \n");

            // Special cases to optionally put in type definition instead of reference
               SgNamedType* nt = isSgNamedType(n);
               assert (nt);
               SgName sname = nt->get_name();
            // char* name = sname.str();
               SgDeclarationStatement* decl = nt->get_declaration();
               assert (decl);

            // Use the first nondefining declaration as a general rule.
               if (decl->get_firstNondefiningDeclaration() != NULL)
                    decl = decl->get_firstNondefiningDeclaration();
               ROSE_ASSERT(decl != NULL);

               SgClassDefinition* defn = isSgClassDeclaration(decl) ? isSgClassDeclaration(decl)->get_definition() : NULL;

               ROSE_ASSERT(lazyWrapping == false);

               printf ("Building ATerm for SgTypedefType: uniqueId(decl) = %s nt->get_autonomous_declaration() = %s defn = %p \n",uniqueId(decl).c_str(),nt->get_autonomous_declaration() ? "true" : "false",defn);
#if 0
               if (lazyWrapping == true)
                  {
                 // Take advandate of the runtime determination to build the aterm, but if required build it as a wrapped node.
                    term = ATmake("Type(<term>)",(nt->get_autonomous_declaration() || !defn ? ATmake("id(<str>)", uniqueId(decl).c_str()) : ATmake("lazyWrap(<str>)","lazyWrap") ));
                  }
                 else
                  {
                    term = ATmake("Type(<term>)",(nt->get_autonomous_declaration() || !defn ? ATmake("id(<str>)", uniqueId(decl).c_str()) : convertNodeToAterm(nt->get_declaration())));
                 // term = ATmake("Type(<term>)",(nt->get_autonomous_declaration() || !defn ? ATmake("id(<str>)", "no_name") : convertNodeToAterm(nt->get_declaration())));
                  }
#else
            // term = ATmake("TypedefType(<term>)",(nt->get_autonomous_declaration() || !defn ? ATmake("id(<str>)", uniqueId(decl).c_str()) : convertNodeToAterm(nt->get_declaration())));
               term = ATmake("TypedefType(<term>)", convertNodeToAterm(nt->get_declaration()));
#endif
            // term = ATmake("Type(<term>)",(nt->get_autonomous_declaration() || !defn ? ATmake("id(<str>)", uniqueId(decl).c_str()) : convertNodeToAterm(nt->get_declaration())));
               break;
             }

          case V_SgLabelStatement: 
             {
            // Special case to put in label id
               const char* name = isSgLabelStatement(n)->get_name().str();
               term = ATmake("Label(<str>)", (name ? name : ""));
#if 0
               term = ATsetAnnotation(term, ATmake("id"), ATmake("<str>", uniqueId(n).c_str()));
#else
               printf ("In convertNodeToAterm(): Skipping annotation for uniqueId for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
               break;
             }

          case V_SgGotoStatement: 
             {
            // Special case to put in label id
               term = ATmake("Goto(<str>)", uniqueId(isSgGotoStatement(n)->get_label()).c_str());
               break;
             }

          case V_SgTypedefDeclaration: 
             {
            // Special case to put in typedef name
               const SgName& name = isSgTypedefDeclaration(n)->get_name();
               SgType* type = isSgTypedefDeclaration(n)->get_base_type();

               bool outputTypeDefinition = isSgTypedefDeclaration(n)->get_typedefBaseTypeContainsDefiningDeclaration();

               printf ("In case V_SgTypedefDeclaration: outputTypeDefinition = %s \n",outputTypeDefinition ? "true" : "false");

            // Where the typedef included the defining declaration then we need to included that as the aterm instead of the associated class type.
               if (outputTypeDefinition == true)
                  {
                    SgNamedType* namedType = isSgNamedType(type);
                    ROSE_ASSERT(namedType != NULL);
                    SgDeclarationStatement* decl = namedType->get_declaration();
                    ROSE_ASSERT(decl != NULL);

                    term = ATmake("Typedef(<str>, <term>)", (name.str() ? name.str() : ""), convertNodeToAterm(decl));
#if 0
                    printf ("This case is not handled yet! \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {

               term = ATmake("Typedef(<str>, <term>)", (name.str() ? name.str() : ""), convertNodeToAterm(type));
#if 1
               term = ATsetAnnotation(term, ATmake("id"), ATmake("<str>", uniqueId(n).c_str()));
#else
               printf ("In convertNodeToAterm(): Skipping annotation for uniqueId for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
                  }
               break;
             }

          case V_SgTemplateDeclaration: 
             {
            // Traversal doesn't work for these
               SgTemplateDeclaration* td = isSgTemplateDeclaration(n);
               ROSE_ASSERT (td);
            // SgTemplateParameterPtrListPtr paramsPtr = td->get_templateParameters();
            // SgTemplateParameterPtrList & paramsPtr = td->get_templateParameters();
            // SgTemplateParameterPtrList params = paramsPtr ? *paramsPtr : SgTemplateParameterPtrList();
               SgTemplateParameterPtrList & params = td->get_templateParameters();
               string templateKindString;
               switch (td->get_template_kind()) 
                  {
                    case SgTemplateDeclaration::e_template_none:
                         templateKindString = "None"; 
                         break;
                    case SgTemplateDeclaration::e_template_class:
                         templateKindString = "Class"; 
                         break;
                    case SgTemplateDeclaration::e_template_m_class:
                         templateKindString = "MemberClass"; 
                         break;
                    case SgTemplateDeclaration::e_template_function:
                         templateKindString = "Function"; 
                         break;
                    case SgTemplateDeclaration::e_template_m_function:
                         templateKindString = "MemberFunction"; 
                         break;
                    case SgTemplateDeclaration::e_template_m_data:
                         templateKindString = "MemberData"; 
                         break;
                    default: 
                         templateKindString = "Unknown"; 
                         break;
                  }

               term = ATmake("TemplateDeclaration(<appl>, <str>, <term>, <str>)",templateKindString.c_str(),td->get_name().str(),convertSgNodeRangeToAterm(params.begin(), params.end()),td->get_string().str());
               break;
             }

          case V_SgTemplateInstantiationDecl: 
             {
            // Traversal doesn't work for these
               SgTemplateInstantiationDecl* td = isSgTemplateInstantiationDecl(n);
               ROSE_ASSERT (td);
            // SgTemplateArgumentPtrListPtr argsPtr = td->get_templateArguments();
            // SgTemplateArgumentPtrList args = argsPtr ? *argsPtr : SgTemplateArgumentPtrList();
               SgTemplateArgumentPtrList & args = td->get_templateArguments();
               term = ATmake("TemplateInstantiationDecl(<str>, <term>)", td->get_templateDeclaration()->get_name().str(), convertSgNodeRangeToAterm(args.begin(), args.end()));
               break;
             }

          case V_SgTemplateParameter: 
             {
            // Traversal doesn't work for these
               SgTemplateParameter* tp = isSgTemplateParameter(n);
               ROSE_ASSERT (tp);
               switch (tp->get_parameterType()) 
                  {
                    case SgTemplateParameter::parameter_undefined: 
                       {
                         term = ATmake("Undefined");
                         break;
                       }

                    case SgTemplateParameter::type_parameter: 
                       {
                         term = ATmake("Type(<term>)",convertNodeToAterm(tp->get_defaultTypeParameter()));
                         break;
                       }

                    case SgTemplateParameter::nontype_parameter: 
                       {
                         term = ATmake("Nontype(<term>, <term>)",convertNodeToAterm(tp->get_type()),convertNodeToAterm(tp->get_defaultExpressionParameter()));
                         break;
                       }

                    case SgTemplateParameter::template_parameter:
                       {
                         term = ATmake("Template");
                         break;
                       }

                    default: 
                         term = ATmake("Unknown"); 
                         break;
                  }
               break;
             }

          case V_SgTemplateArgument: 
             {
            // Traversal doesn't work for these
               SgTemplateArgument* ta = isSgTemplateArgument(n);
               ROSE_ASSERT (ta);
               switch (ta->get_argumentType()) 
                  {
                    case SgTemplateArgument::argument_undefined:
                         term = ATmake("Undefined");
                         break;

                    case SgTemplateArgument::type_argument:
                         term = ATmake("Type(<term>)",convertNodeToAterm(ta->get_type()));
                         break;

                    case SgTemplateArgument::nontype_argument:
                         term = ATmake("Nontype(<term>)", convertNodeToAterm(ta->get_expression()));
                         break;

                 // case SgTemplateArgument::template_argument:
                 //      term = ATmake("Template");
                 //      break;

                    default: term = ATmake("Unknown"); break;
                  }
               break;
             }

       // DQ (9/17/2014): Added new case to address where we were using lazy wrapping previously.
          case V_SgVariableDeclaration:
             {
            // Using the default case for the moment.
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n);
               SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
               ROSE_ASSERT(initializedName != NULL);
               
            // const SgName& name = "x"; // isSgVariableDeclaration(n)->get_name();
            // SgType* type = isSgVariableDeclaration(n)->get_type();
            // ROSE_ASSERT(type != NULL);
            // term = ATmake("VarDecl(<term>)",convertNodeToAterm(initializedName));
               term = ATmake("VarDecl(<term>)", convertSgNodeRangeToAterm(variableDeclaration->get_variables().begin(),variableDeclaration->get_variables().end()));

            // DQ (10/26/2013): This is required to avoid having ATerms be shared and interfering with the maximal sharing of ATerms (especially when we are using a lazy evaluation).
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
#if 0
               printf ("Found correct form of VarDecl \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // Handle these explicitly instead of via the default case.
       // case V_SgBasicBlock:
       // case V_SgGlobal:
          case V_SgFunctionDefinition:
       // case V_SgClassDefinition:
             {
               term = ATmake("<appl(<list>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));

            // DQ (10/26/2013): This is required to avoid having ATerms be shared and interfering with the maximal sharing of ATerms (especially when we are using a lazy evaluation).
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
               break;
             }
#endif
#if 0
       // Handle these explicitly instead of via the default case.
          case V_SgCtorInitializerList:
          case V_SgFunctionParameterList:
          case V_SgGlobal:
          case V_SgBasicBlock:
          case V_SgClassDefinition:
          case V_SgFunctionDefinition:
             {
            // This kind of ATerm can support arity greater than 255 (so SgGlobal must be handled via this kind of ATerm).
               term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));

            // DQ (10/26/2013): This is required to avoid having ATerms be shared and interfering with the maximal sharing of ATerms (especially when we are using a lazy evaluation).
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
               break;
             }
#endif

       // DQ (9/30/2014): Seperated case to support zero length containers.
          case V_SgGlobal:
          case V_SgClassDefinition:
          case V_SgTemplateClassDefinition:
          case V_SgNamespaceDefinitionStatement:
          case V_SgTemplateInstantiationDefn:
          case V_SgBasicBlock:

          case V_SgFunctionParameterList:
          case V_SgCtorInitializerList:
          case V_SgCatchStatementSeq:
          case V_SgForInitStatement:
          case V_SgExprListExp:
          case V_SgMemberFunctionRefExp:
          case V_SgTemplateParameterVal:
          case V_SgNamespaceAliasDeclarationStatement:
          case V_SgUsingDeclarationStatement:
          case V_SgUsingDirectiveStatement:
             {
            // Adding the scope.
            // term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));
#if 1
               SgName mangled_name;
               SgScopeStatement* scope = isSgScopeStatement(n);
               if (scope != NULL)
                  {
                 // Build a reference to the scope.
                    ATerm scope_term = ATmake("ScopeRef(<term>)",term);

                 // With the added scope information.
                    term = ATmake("<appl(<str>, <term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),uniqueId(n).c_str(),getTraversalChildrenAsAterm(n));
                  }
                 else
                  {
                 // Without the added scope information.
                    term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));
                  }
#else
            // Without the added scope information.
               term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));
#endif

            // printf ("DEFAULT: TRUE: Size of list for %s = %zu \n",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n).size());
               int number_of_containers        = AstTests::numSuccContainers(n);
               int number_of_single_successors = AstTests::numSingleSuccs(n);
               printf ("DEFAULT: TRUE: Size of list for %s number_of_containers = %d number_of_single_successors = %d \n",getShortVariantName((VariantT)(n->variantT())).c_str(),number_of_containers,number_of_single_successors);

            // I would like to modify the list above to only apply when there is a valid list or where
            // the list is enpty (in which case the number_of_containers incorrectly will evaluate to 0).
               ROSE_ASSERT( (number_of_containers == 0 || number_of_containers == 1) && number_of_single_successors == 0);

            // DQ (10/26/2013): This is required to avoid having ATerms be shared and interfering with the maximal sharing of ATerms (especially when we are using a lazy evaluation).
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
               break;
             }

#if 0
       // TRUE CASE:
       // Added this case to avoid processing via the default case.
       // case V_SgCtorInitializerList:
       // case V_SgFunctionParameterList:
       // case V_SgGlobal:
       // case V_SgBasicBlock:
       // case V_SgClassDefinition:
       // case V_SgTemplateClassDefinition:
       // case V_SgFunctionDefinition:
       // case V_SgTemplateFunctionDefinition:
       // case V_SgNamespaceDefinitionStatement:
       // case V_SgThisExp:
       // case V_SgExprListExp:
       // case V_SgStringVal:
       // case V_SgBoolValExp:
       // case V_SgLongIntVal:
       // case V_SgCharVal:
       // case V_SgFloatVal:
       // case V_SgLongDoubleVal:
       // case V_SgLongLongIntVal:
       // case V_SgWcharVal:
       // case V_SgNullExpression:
       // case V_SgBreakStmt:
       // case V_SgEnumVal:
       // case V_SgMemberFunctionRefExp:
       // case V_SgCatchStatementSeq:
       // case V_SgForInitStatement:
       // case V_SgTemplateInstantiationDefn:
       // case V_SgTemplateParameterVal:
       // case V_SgNamespaceAliasDeclarationStatement:
       // case V_SgUsingDeclarationStatement:
       // case V_SgUsingDirectiveStatement:
       // case V_SgTemplateFunctionRefExp:
       // case V_SgTemplateMemberFunctionRefExp:
       // case V_SgNullStatement:
       // case V_SgTypeTraitBuiltinOperator:
       // case V_SgPseudoDestructorRefExp:
       // case V_SgAsmStmt:
             {
               term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));

            // printf ("DEFAULT: TRUE: Size of list for %s = %zu \n",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n).size());
               int number_of_containers        = AstTests::numSuccContainers(n);
               int number_of_single_successors = AstTests::numSingleSuccs(n);
               printf ("DEFAULT: TRUE: Size of list for %s number_of_containers = %d number_of_single_successors = %d \n",getShortVariantName((VariantT)(n->variantT())).c_str(),number_of_containers,number_of_single_successors);

            // I would like to modify the list above to only apply when there is a valid list.
               ROSE_ASSERT(number_of_containers == 1);

            // DQ (10/26/2013): This is required to avoid having ATerms be shared and interfering with the maximal sharing of ATerms (especially when we are using a lazy evaluation).
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
               break;
             }
#endif
#if 1
       // Moved (from the TRUE cases above) to match the constraint that number_of_containers == 0 && number_of_single_successors == 0.
          case V_SgThisExp:
          case V_SgStringVal:
          case V_SgBoolValExp:
          case V_SgLongIntVal:
          case V_SgCharVal:
          case V_SgFloatVal:
          case V_SgLongDoubleVal:
          case V_SgLongLongIntVal:
          case V_SgWcharVal:
          case V_SgNullExpression:
          case V_SgBreakStmt:
          case V_SgEnumVal:
          case V_SgTemplateFunctionRefExp:
          case V_SgTemplateMemberFunctionRefExp:
          case V_SgNullStatement:
          case V_SgTypeTraitBuiltinOperator:
          case V_SgPseudoDestructorRefExp:
          case V_SgAsmStmt:
          case V_SgPragma:
             {
               term = ATmake("<appl(<list>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));

            // printf ("DEFAULT: FALSE: Size of list for %s = %zu \n",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n).size());
               int number_of_containers        = AstTests::numSuccContainers(n);
               int number_of_single_successors = AstTests::numSingleSuccs(n);
               printf ("DEFAULT: FALSE: Size of list for %s number_of_containers = %d number_of_single_successors = %d \n",getShortVariantName((VariantT)(n->variantT())).c_str(),number_of_containers,number_of_single_successors);
               ROSE_ASSERT(number_of_containers == 0 && number_of_single_successors == 0);

            // DQ (10/26/2013): This is required to avoid having ATerms be shared and interfering with the maximal sharing of ATerms (especially when we are using a lazy evaluation).
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
               break;
             }
#endif

       // Moved to match the constraint that number_of_containers == 0.
          case V_SgFunctionDefinition:
          case V_SgTemplateFunctionDefinition:

       // FALSE CASE:
       // Handle these explicitly instead of via the default case.
          case V_SgReturnStmt:
          case V_SgAssignInitializer:
          case V_SgConstructorInitializer:
          case V_SgAggregateInitializer:
          case V_SgExprStatement:
          case V_SgEqualityOp:
          case V_SgFunctionCallExp:
          case V_SgConditionalExp:
          case V_SgArrowExp:
          case V_SgAssignOp:
          case V_SgDotExp:
          case V_SgNotEqualOp:
          case V_SgIfStmt:
          case V_SgDefaultOptionStmt:
          case V_SgSwitchStatement:
          case V_SgCaseOptionStmt:
          case V_SgAddOp:
          case V_SgGreaterThanOp:
          case V_SgNotOp:
          case V_SgOrOp:
          case V_SgDeleteExp:
          case V_SgSizeOfOp:
          case V_SgMultiplyOp:
          case V_SgSubtractOp:
          case V_SgMinusOp:
          case V_SgVarArgStartOp:
          case V_SgVarArgOp:
          case V_SgVarArgEndOp:
          case V_SgAddressOfOp:
          case V_SgNewExp:
          case V_SgPointerDerefExp:
          case V_SgBitComplementOp:
          case V_SgUnaryAddOp:
          case V_SgThrowOp:
          case V_SgCatchOptionStmt:
          case V_SgTryStmt:
          case V_SgForStatement:
          case V_SgNamespaceDeclarationStatement:
          case V_SgTemplateClassDeclaration:
          case V_SgTemplateMemberFunctionDeclaration:
          case V_SgTemplateFunctionDeclaration:
          case V_SgTemplateVariableDeclaration:
       // case V_SgPragma:
          case V_SgTemplateInstantiationMemberFunctionDecl:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationDirectiveStatement:
          case V_SgPragmaDeclaration:
          case V_SgWhileStmt:
          case V_SgDoWhileStmt:
          case V_SgStatementExpression:
             {
               term = ATmake("<appl(<list>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));

            // printf ("DEFAULT: FALSE: Size of list for %s = %zu \n",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n).size());
               int number_of_containers        = AstTests::numSuccContainers(n);
               int number_of_single_successors = AstTests::numSingleSuccs(n);
               printf ("DEFAULT: FALSE: Size of list for %s number_of_containers = %d number_of_single_successors = %d \n",getShortVariantName((VariantT)(n->variantT())).c_str(),number_of_containers,number_of_single_successors);
               ROSE_ASSERT(number_of_containers == 0);
               ROSE_ASSERT(number_of_single_successors > 0);

            // DQ (10/26/2013): This is required to avoid having ATerms be shared and interfering with the maximal sharing of ATerms (especially when we are using a lazy evaluation).
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
               break;
             }

          default:
             {
            // This is a container (e.g. SgGlobal, other scope, etc.).
#if 0
               printf ("In convertNodeToAterm(): default: Processing the default case of container: n = %p = %s \n",n,n->class_name().c_str());
#endif
               bool isContainer = (AstTests::numSuccContainers(n) == 1) || (!isSgType(n) && (n->get_traversalSuccessorContainer().size() == 0));
#if 0
               printf ("In convertNodeToAterm(): default: isContainer = %s \n",isContainer ? "true" : "false");
#endif
               term = ATmake((isContainer ? "<appl(<term>)>" : "<appl(<list>)>"), getShortVariantName((VariantT)(n->variantT())).c_str(),(isSgType(n) ? ATmake("[]") : getTraversalChildrenAsAterm(n)));
            // Special case for types is because of traversal problems

            // DQ (10/26/2013): This is required to avoid having ATerms be shared and interfering with the maximal sharing of ATerms (especially when we are using a lazy evaluation).
               term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(n).c_str()));
#if 0
               printf ("In convertNodeToAterm(): default: n = %p = %s uniqueId(n) = %s \n",n,n->class_name().c_str(),uniqueId(n).c_str());
#endif
#if 1
            // Allow types not handled explicitly above to be handled by the default.
               if (isSgType(n) == NULL)
                  {
#if 0
                    printf ("In convertNodeToAterm(): default: Processing the default case of container: n = %p = %s \n",n,n->class_name().c_str());
                    printf ("In convertNodeToAterm(): default: isContainer = %s \n",isContainer ? "true" : "false");
                    printf ("In convertNodeToAterm(): default: n = %p = %s uniqueId(n) = %s \n",n,n->class_name().c_str(),uniqueId(n).c_str());
#endif
                    if (isSgBinaryOp(n) == NULL)
                       {
                         printf ("We would like to avoid processing IR nodes using the default case n = %p = %s isContainer = %s uniqueId = %s \n",n,n->class_name().c_str(),isContainer ? "true" : "false",uniqueId(n).c_str());
                         ROSE_ASSERT(false);
                       }
                  }
#endif
               break;
             }
        }

#if 0
     printf ("Base of switch statement in convertNodeToAterm(): n = %p = %s \n",n,n->class_name().c_str());
#endif

     assert (term != NULL);

  // This added the pointer to the ROSE IR AST node to the aterm as an annotation (using an ATerm mechanism)
     term = ATsetAnnotation(term, ATmake("ptr"), pointerAsAterm(n));

#if 0
     if (n->get_file_info() != NULL)
        {
       // Add the source position information as a annotation.
          term = ATsetAnnotation(term, ATmake("location"),convertFileInfoToAterm(n->get_file_info()));
        }

     if (isSgExpression(n))
        {
       // Add the type information as a annotation.
          term = ATsetAnnotation(term, ATmake("type"), convertNodeToAterm(isSgExpression(n)->get_type()));
        }
#else
#if 0
     printf ("In convertNodeToAterm(): Skipping annocation for type and source position information for term = %p node = %p = %s \n",term,n,n->class_name().c_str());
#endif
#endif

#if 0
     printf ("Leaving convertNodeToAterm(): n = %p = %s \n",n,n->class_name().c_str());
#endif
#if 0
     printf ("--- n->class_name() = %s ATwriteToString(term) = %s \n",n->class_name().c_str(),ATwriteToString(term));
#endif

// End of large commented out section
#endif
  // cout << n->sage_class_name() << " -> " << ATwriteToString(term) << endl;
     return term;
   }
