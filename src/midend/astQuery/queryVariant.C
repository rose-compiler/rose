//#include "rose.h"

// include file for transformation specification support
// include "specification.h"
// include "globalTraverse.h"

// include "query.h"

// string class used if compiler does not contain a C++ string class
// include <roseString.h>

#include "nodeQuery.h"
#define DEBUG_NODEQUERY 0 
// #include "arrayTransformationSupport.h"

// This is where we specify that types should be traversed
#define TRAVERSE_TYPES 1

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


namespace NodeQuery{

void
printNodeList ( const Rose_STL_Container<SgNode*> & localList )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector
     int counter = 0;
     printf ("Output node list: \n");
     for (Rose_STL_Container<SgNode*>::const_iterator i = localList.begin(); i != localList.end(); i++)
        {
       // printf ("Adding node to list! \n");
          printf ("   list element #%d = %s \n",counter,(*i)->sage_class_name());
          counter++;
        }
   }



/* BEGIN INTERFACE NAMESPACE NODEQUERY2 */


void
pushNewNode ( NodeQuerySynthesizedAttributeType & nodeList, 
              const VariantVector & targetVariantVector,
              SgNode * astNode )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector

  // Allow input of a NULL pointer but don't add it to the list
     if (astNode != NULL)
        {
          for (vector<VariantT>::const_iterator i = targetVariantVector.begin(); i != targetVariantVector.end(); i++)
             {
            // printf ("Loop over target node vector: node = %s \n",getVariantName(*i).c_str());
               if (astNode->variantT() == *i)
                  {
                 // printf ("Adding node to list! \n");
                    nodeList.push_back(astNode);
                  }
             }
        }
   }

void
mergeList ( NodeQuerySynthesizedAttributeType & nodeList, const Rose_STL_Container<SgNode*> & localList )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector
     unsigned localListSize = localList.size();
     unsigned nodeListSize  = nodeList.size();
     for (Rose_STL_Container<SgNode*>::const_iterator i = localList.begin(); i != localList.end(); i++)
        {
       // printf ("Adding node to list (%s) \n",(*i)->sage_class_name());
          nodeList.push_back(*i);
        }
     ROSE_ASSERT (nodeList.size() == nodeListSize+localListSize);
   }


#if 0
// DQ (4/7/2004): Added to support more general lookup of data in the AST (vector of variants)
NodeQuerySynthesizedAttributeType
querySolverGrammarElementFromVariantVector ( 
   SgNode * astNode, 
   VariantVector targetVariantVector )
   {
  // This function extracts type nodes that would not be traversed so that they can
  // accumulated to a list.  The specific nodes collected into the list is controlled
  // by targetVariantVector.

     ROSE_ASSERT (astNode != NULL);
     NodeQuerySynthesizedAttributeType returnNodeList;

  // DQ (8/26/2004):
  // This is where we place the root of subtrees that require a visit flag mechanism.  
  // Instead of attaching the visit flag to each IR node (as we used to do in initial work
  // on the traversal (by Markus S.)), we build a list of IR nodes which would generate
  // cycles if we traversed them more than once.  This permits us to visit them at least
  // once which is what was a problem when we removed the visit flags initially. This 
  // fixes the island problem, regions of the AST which skipped being traversed in earlier 
  // work (see ROSE/proposals/islandsOfUntraversedAST.txt).
     Rose_STL_Container<SgNode*> nodesToVisitTraverseOnlyOnce;

#if 0
     int lineNumber = -1;
     string fileName = "no file";
     if (astNode->get_file_info() != NULL)
        {
          lineNumber = astNode->get_file_info()->get_line();
          fileName   = astNode->get_file_info()->get_filename();
        }
     printf ("Type Query: astNode = %s at %d of %s \n",astNode->sage_class_name(),lineNumber,fileName.c_str());
#endif
#if 0
     if (isSgType(astNode) != NULL)
        {
          printf ("Found a type: astNode = %s \n",astNode->sage_class_name());
        }
#endif

  // VariantVector::printVariantVector(targetVariantVector);

  // Push the current node onto the list if it is one of the variants in the vector of variants
  // printf ("Calling pushNewNode at TOP of querySolverGrammarElementFromVariantVector \n");
     pushNewNode (returnNodeList,targetVariantVector,astNode);
  // printf ("DONE: Calling pushNewNode at TOP of querySolverGrammarElementFromVariantVector \n");

     switch (astNode->variantT())
        {
       // Alternatively, we could have just handled the case of a SgInitializedName more directly!
          case V_SgVariableDeclaration:
             {
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNode);
               ROSE_ASSERT (variableDeclaration != NULL);
               SgInitializedNamePtrList & varList = variableDeclaration->get_variables();
               SgInitializedNamePtrList::iterator i = varList.begin();
               while (i != varList.end())
                  {
#if 0
                    SgName name = (*i)->get_name();
                    if (name.str() != NULL)
                         printf ("variable name = %s \n",name.str());
                      else
                         printf ("variable name = NULL \n");
#endif
                    ROSE_ASSERT ((*i)->get_type() != NULL);
                    pushNewNode (returnNodeList,targetVariantVector,(*i)->get_type());
                    ROSE_ASSERT ((*i)->get_type() != NULL);

#if 0
                 // DQ (7/20/2004): These are traversed when we traverse the SgInitializedName objects
                 // Specified with DEF2TYPE_TRAVERSAL
                    SgType* type = (*i)->get_type();
                    ROSE_ASSERT (type != NULL);
                    printf ("Added traversal of the variable type (%s) \n",type->sage_class_name());
                    list<SgNode*> localList = NodeQuery::querySubTree (type,targetVariantVector);
                    mergeList (returnNodeList,localList);
#endif
                 // move to next variable in this declaration statement
                    i++;
                  }
               break;
             }

       // Template for possible additional cases
          case V_SgVariableDefinition:
             {
            // printf ("Get the type out of the SgVariableDefinition (not implemented) \n");
               SgVariableDefinition* x = isSgVariableDefinition(astNode);
               ROSE_ASSERT (x != NULL);
               ROSE_ASSERT (x->get_vardefn() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_vardefn());
               break;
             }

          case V_SgTypedefDeclaration:
             {
            // printf ("Get the type out of the SgTypedefDeclaration \n");
               SgTypedefDeclaration* x = isSgTypedefDeclaration(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type());
               pushNewNode (returnNodeList,targetVariantVector,x->get_base_type());

#if 0
            // DQ (7/19/2004): Added nested traversal of a typedef's declaration
            // to traversl AST islands (previously not traversed).
               printf ("Added traversal of the SgTypedefDeclaration's declaration \n");
               SgDeclarationStatement* declaration = x->get_declaration();
            // ROSE_ASSERT (declaration != NULL);
               if (declaration != NULL)
                  {
                 // DQ (8/26/2004): Added support for breaking cycles AND still traversing 
                 // islands of the AST hidden within typedefs.
                    if (nodesToVisitTraverseOnlyOnce.find(declaration) == nodesToVisitTraverseOnlyOnce.end() )
                       {
                         nodesToVisitTraverseOnlyOnce.push_back(declaration);
                         list<SgNode*> localList = NodeQuery::querySubTree (declaration,targetVariantVector);
                         mergeList (returnNodeList,localList);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT (x->get_name().str() != NULL);
                    printf ("Found a NULL declaration field in a SgTypedefDeclaration (%s) \n",x->get_name().str());
                  }
#endif

#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               SgType* type = x->get_type();
               ROSE_ASSERT (type != NULL);
               printf ("Added traversal of the typedef type (%s) \n",type->sage_class_name());
               list<SgNode*> localList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,localList);
#endif

#if 0
            // Not specified with DEF2TYPE_TRAVERSAL
               SgType* baseType = x->get_base_type();
               ROSE_ASSERT (baseType != NULL);
               printf ("Added traversal of the typedef type (%s) \n",baseType->sage_class_name());
               list<SgNode*> localList = NodeQuery::querySubTree (baseType,targetVariantVector);
               mergeList (returnNodeList,localList);
#endif
               break;
             }

          case V_SgClassDeclaration:
             {
            // printf ("Get the type out of the SgClassDeclaration \n");
               SgClassDeclaration* x = isSgClassDeclaration(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type());

#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgClassDeclaration's type \n");
               SgClassType* type = x->get_type();
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif

            // DQ: This is traversed directly by the traversal we don't have to form a nested traversal here.
            // printf ("Added traversal of the SgClassDeclaration's definition \n");
            // SgClassDefinition* definition = x->get_definition();
            // list<SgNode*> localList = NodeQuery::querySubTree (definition,targetVariantVector);
            // mergeList (returnNodeList,localList);
               break;
             }

          case V_SgEnumDeclaration:
             {
            // Note that no modifiers are allowed for enum values!
               SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(astNode);
               ROSE_ASSERT (enumDeclaration != NULL);
               SgInitializedNamePtrList & enumVarList = enumDeclaration->get_enumerators();
            // printf ("enumVarList.size() = %zu \n",enumVarList.size());
               SgInitializedNamePtrList::iterator i = enumVarList.begin();
               while (i != enumVarList.end())
                  {
#if 0
                    SgName name = (*i)->get_name();
                    if (name.str() != NULL)
                         printf ("enum variable name = %s \n",name.str());
                      else
                         printf ("enum variable name = NULL \n");
#endif
                    pushNewNode (returnNodeList,targetVariantVector,(*i)->get_type());
                    i++;
                  }

            // Specified with DEF2TYPE_TRAVERSAL
            // printf ("Added traversal of the SgEnumDeclaration's type \n");
               SgEnumType* type = enumDeclaration->get_type();
               ROSE_ASSERT (type != NULL);
// DQ (7/20/2004): This appears to be a runtime error ???
//             list<SgNode*> localList = NodeQuery::querySubTree (type,targetVariantVector);
//             mergeList (returnNodeList,localList);
               break;
             }

          case V_SgCtorInitializerList:
             {
            // Note that no modifiers are allowed for enum values!
               SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(astNode);
               ROSE_ASSERT (ctorInitializerList != NULL);
               SgInitializedNamePtrList & ctorList = ctorInitializerList->get_ctors();
            // printf ("ctorList.size() = %zu \n",ctorList.size());
               SgInitializedNamePtrList::iterator i = ctorList.begin();
               while (i != ctorList.end())
                  {
#if 0
                    SgName name = (*i)->get_name();
                    if (name.str() != NULL)
                         printf ("enum variable name = %s \n",name.str());
                      else
                         printf ("enum variable name = NULL \n");
#endif
                    pushNewNode (returnNodeList,targetVariantVector,(*i)->get_type());
                    i++;
                  }
               break;
             }


          case V_SgMemberFunctionDeclaration:
          case V_SgFunctionDeclaration:
             {
               SgFunctionDeclaration* x = isSgFunctionDeclaration(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type());
               pushNewNode (returnNodeList,targetVariantVector,x->get_orig_return_type());

#if 0
            // This is just the interface for an interior SgFunctionParameterList 
            // object (which we also traverse).  So get the arguments from that class.
               SgInitializedNamePtrList & parameterList = x->get_args();
               SgInitializedNamePtrList::iterator i = parameterList.begin();
               while (i != parameterList.end())
                  {
#if 0
                    SgName name = (*i)->get_name();
                    if (name.str() != NULL)
                         printf ("(SgFunctionDeclaration) function parameter name = %s \n",name.str());
                      else
                         printf ("(SgFunctionDeclaration) function parameter name = NULL \n");
#endif
                    pushNewNode (returnNodeList,targetVariantVector,(*i)->get_type());
                    i++;
                  }
#endif

#if 0
            // Specified with DEF2TYPE_TRAVERSAL
            // printf ("Added traversal of the function type \n");
               SgFunctionType* functionType = x->get_type();
               ROSE_ASSERT(functionType != NULL);
               list<SgNode*> functionTypeList = NodeQuery::querySubTree (functionType,targetVariantVector);
               mergeList (returnNodeList,functionTypeList);
#endif
#if 0
// DQ (7/20/2004): This causes a cycle for "class A { A foo(); }", I think
            // Specified with DEF2TYPE_TRAVERSAL
            // printf ("Added traversal of the return type \n");
               SgType* returnType = x->get_orig_return_type();
               ROSE_ASSERT(returnType != NULL);
               list<SgNode*> returnTypeList = NodeQuery::querySubTree (returnType,targetVariantVector);
               mergeList (returnNodeList,returnTypeList);
#endif
               break;
             }

          case V_SgFunctionParameterList:
             {
               SgFunctionParameterList* functionParameters = isSgFunctionParameterList(astNode);
               ROSE_ASSERT (functionParameters != NULL);
               SgInitializedNamePtrList & parameterList = functionParameters->get_args();
               SgInitializedNamePtrList::iterator i = parameterList.begin();
               while (i != parameterList.end())
                  {
#if 0
                    SgName name = (*i)->get_name();
                    if (name.str() != NULL)
                         printf ("(SgFunctionParameterList) function parameter name = %s \n",name.str());
                      else
                         printf ("(SgFunctionParameterList) function parameter name = NULL \n");
#endif
                    pushNewNode (returnNodeList,targetVariantVector,(*i)->get_type());
                    i++;
                  }
               break;
             }

           case V_SgInitializedName:
             {
            // printf ("Get the type out of the SgInitializedName \n");
               SgInitializedName* initializedName = isSgInitializedName(astNode);
               ROSE_ASSERT (initializedName != NULL);
               pushNewNode (returnNodeList,targetVariantVector,initializedName->get_type());

#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgInitializedName type \n");
               SgType* type = initializedName->get_type();
               if (type != NULL)
                  {
                    ROSE_ASSERT(type != NULL);
                    list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
                    mergeList (returnNodeList,typeList);
                  }
                 else
                  {
                    printf ("nodeQuery types initializedName->get_type() == NULL \n");
                  }
#endif
               break;
             }

          case V_SgTypedefSeq:
             {
            // printf ("Get the type out of the SgTypedefSeq \n");
               SgTypedefSeq* x = isSgTypedefSeq(astNode);
               ROSE_ASSERT (x != NULL);
               SgTypePtrList & typedefList = x->get_typedefs();
               SgTypePtrList::iterator i = typedefList.begin();
               while (i != typedefList.end())
                  {
                    pushNewNode (returnNodeList,targetVariantVector,*i);

                 // Specified with DEF2TYPE_TRAVERSAL
                 // printf ("Added traversal of the SgTypedefSeq typedef list element \n");
                    ROSE_ASSERT( (*i) != NULL);
// DQ (7/20/2004): This appears to be a runtime error ???
//                  list<SgNode*> typeList = NodeQuery::querySubTree (*i,targetVariantVector);
//                  mergeList (returnNodeList,typeList);

                    i++;
                  }
               break;
             }

          case V_SgFunctionParameterTypeList:
             {
            // printf ("Get the type out of the SgFunctionParameterTypeList \n");
               SgFunctionParameterTypeList* x = isSgFunctionParameterTypeList(astNode);
               ROSE_ASSERT (x != NULL);
               SgTypePtrList & argumentList = x->get_arguments();
               SgTypePtrList::iterator i = argumentList.begin();
               while (i != argumentList.end())
                  {
                    pushNewNode (returnNodeList,targetVariantVector,*i);
#if 0
                 // DQ (7/20/2004): Skip the traversal of the types in a list of function parametr types
                 // By definition they should have been seen before since the type can't be defined in 
                 // the function parameter (C++ has rules about that).

                 // Specified with DEF2TYPE_TRAVERSAL
                 // printf ("Added traversal of the SgFunctionParameterTypeList type list element \n");
                    ROSE_ASSERT( (*i) != NULL);
                    list<SgNode*> typeList = NodeQuery::querySubTree (*i,targetVariantVector);
                    mergeList (returnNodeList,typeList);
#endif
                    i++;
                  }
               break;
             }

          case V_SgTemplateParameter:
             {
            // printf ("Get the type out of the SgTemplateParameter (not implemented) \n");
               SgTemplateParameter* templateParameter = isSgTemplateParameter(astNode);
               ROSE_ASSERT (templateParameter != NULL);
               pushNewNode (returnNodeList,targetVariantVector,templateParameter->get_type());
               pushNewNode (returnNodeList,targetVariantVector,templateParameter->get_defaultExpressionParameter());
               break;
             }

          case V_SgTemplateArgument:
             {
            // printf ("Get the type out of the SgTemplateArgument (not implemented) \n");
               SgTemplateArgument* x = isSgTemplateArgument(astNode);
               ROSE_ASSERT (x != NULL);
               ROSE_ASSERT (x->get_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type());
               break;
             }

          case V_SgMemberFunctionType:
          case V_SgFunctionType:
             {
            // printf ("Get the type out of the SgFunctionType (not implemented) \n");
               SgFunctionType* x = isSgFunctionType(astNode);
               ROSE_ASSERT (x != NULL);

#if 0
            // DQ (9/17/2004): Old code
               ROSE_ASSERT (x->get_return_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_return_type());
               ROSE_ASSERT (x->get_orig_return_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_orig_return_type());
#endif

#if 0
            // DQ (9/17/2004): This can't work since the modifier information is not present in the function type (should it be?)
               if ( x->get_specialFunctionModifier().isConstructor() ||
                    x->get_specialFunctionModifier().isDestructor()  ||
                    x->get_specialFunctionModifier().isConversion() )
                  {
                 // Handle special case of constructors, destructors, and conversion 
                 // operators not having a return type defined internally!
                    ROSE_ASSERT (x->get_return_type()      == NULL);
                    ROSE_ASSERT (x->get_orig_return_type() == NULL);
                  }
                 else
                  {
                    ROSE_ASSERT (x->get_return_type() != NULL);
                    pushNewNode (returnNodeList,targetVariantVector,x->get_return_type());
                    ROSE_ASSERT (x->get_orig_return_type() != NULL);
                    pushNewNode (returnNodeList,targetVariantVector,x->get_orig_return_type());
                  }
#else
                 // DQ (9/17/2004): New code (can't assume these are valid pointers for member 
                 // function types used with constructors, destructors, and conversion operators
                    if (x->get_return_type() != NULL)
                         pushNewNode (returnNodeList,targetVariantVector,x->get_return_type());
                    if (x->get_orig_return_type() != NULL)
                         pushNewNode (returnNodeList,targetVariantVector,x->get_orig_return_type());
#endif

               SgTypePtrList & argList = x->get_arguments();
               SgTypePtrList::iterator i = argList.begin();
               while (i != argList.end())
                  {
                    pushNewNode (returnNodeList,targetVariantVector,*i);
                    i++;
                  }
               break;
             }

          case V_SgPointerMemberType:
          case V_SgPointerType:
             {
            // printf ("Get the type out of the SgPointerType (not implemented) \n");
               SgPointerType* x = isSgPointerType(astNode);
               ROSE_ASSERT (x != NULL);
               ROSE_ASSERT (x->get_base_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_base_type());

// DQ (7/25/2004): This seems to generate a cycle
// #if TRAVERSE_TYPES
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
            // printf ("Added traversal of the function type \n");
               SgType* baseType = x->get_base_type();
               ROSE_ASSERT(baseType != NULL);
// DQ (7/20/2004): This appears to be a runtime error ???
               list<SgNode*> baseTypeList = NodeQuery::querySubTree (baseType,targetVariantVector);
               mergeList (returnNodeList,baseTypeList);
#endif
               break;
             }

          case V_SgArrayType:
             {
            // printf ("Get the type out of the SgFunctionType (not implemented) \n");
               SgArrayType* x = isSgArrayType(astNode);
               ROSE_ASSERT (x != NULL);
               ROSE_ASSERT (x->get_base_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_base_type());

// DQ (7/25/2004): This seems to generate a cycle
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the array type \n");
               SgType* baseType = x->get_base_type();
               ROSE_ASSERT(baseType != NULL);
// DQ (7/20/2004): This appears to be a runtime error ???
               list<SgNode*> baseTypeList = NodeQuery::querySubTree (baseType,targetVariantVector);
               mergeList (returnNodeList,baseTypeList);
#endif
               break;
             }

          case V_SgReferenceType:
             {
            // printf ("Get the type out of the SgReferenceType \n");
               SgReferenceType* x = isSgReferenceType(astNode);
               ROSE_ASSERT (x != NULL);
               ROSE_ASSERT (x->get_base_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_base_type());

// DQ (7/25/2004): This seems to generate a cycle
// #if TRAVERSE_TYPES
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the array type \n");
               SgType* baseType = x->get_base_type();
               ROSE_ASSERT(baseType != NULL);
// DQ (7/20/2004): This appears to be a runtime error ???
               list<SgNode*> baseTypeList = NodeQuery::querySubTree (baseType,targetVariantVector);
               mergeList (returnNodeList,baseTypeList);
#endif
               break;
             }

          case V_SgModifierType:
             {
            // printf ("Get the type out of the SgModifierType \n");
               SgModifierType* x = isSgModifierType(astNode);
               ROSE_ASSERT (x != NULL);
               ROSE_ASSERT (x->get_base_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_base_type());
               break;
             }

          case V_SgTypeEllipse:
             {
               printf ("Note: Nothing to do for SgTypeEllipse (astNode = %s) \n",astNode->sage_class_name());
               break;
             }

          case V_SgPartialFunctionType:
       // case V_SgMemberFunctionType:
       // case V_SgFunctionType:
       // case V_SgPointerType:
       // case V_SgPointerMemberType:
          case V_SgType:

          case V_SgTypeUnknown:
          case V_SgTypeDefault:
       // case V_SgReferenceType:
          case V_SgTemplateType:
       // case V_SgModifierType:
          case V_SgPartialFunctionModifierType:
       // case V_SgTypeEllipse:
       // case V_SgUnknownMemberFunctionType:
             {
               printf ("Error: Type case not implemented! (astNode = %s) \n",astNode->sage_class_name());
               ROSE_ASSERT (false);
               break;
             }

          case V_SgTypeChar:
          case V_SgTypeSignedChar:
          case V_SgTypeUnsignedChar:
          case V_SgTypeShort:
          case V_SgTypeSignedShort:
          case V_SgTypeUnsignedShort:
          case V_SgTypeInt:
          case V_SgTypeSignedInt:
          case V_SgTypeUnsignedInt:
          case V_SgTypeLong:
          case V_SgTypeSignedLong:
          case V_SgTypeUnsignedLong:
          case V_SgTypeVoid:
          case V_SgTypeGlobalVoid:
          case V_SgTypeWchar:
          case V_SgTypeFloat:
          case V_SgTypeDouble:
          case V_SgTypeLongLong:
          case V_SgTypeUnsignedLongLong:
          case V_SgTypeLongDouble:
          case V_SgTypeString:
          case V_SgTypeBool:
          case V_SgTypeComplex:
          case V_SgTypeImaginary:
             {
            // printf ("Note: nothing to implement for these cases \n");
               break;
             }

          case V_SgNamedType:
          case V_SgEnumType:
          case V_SgTypedefType:
          case V_SgClassType:
             {
               SgNamedType* x = isSgNamedType(astNode);
               ROSE_ASSERT (x != NULL);
#if 0
// DQ (7/20/2004): This may cause a cycle for "class A { A foo(); }" ???
            // printf ("Added traversal of the declaration in named type \n");
               SgDeclarationStatement* declaration = x->get_declaration();
               list<SgNode*> localList = NodeQuery::querySubTree (declaration,targetVariantVector);
               mergeList (returnNodeList,localList);
#endif

            // printf ("Named Type case not implemented! \n");
            // ROSE_ASSERT (false);
               break;
             }

          case V_SgExpression:
          case V_SgExprListExp:
          case V_SgVarRefExp:
          case V_SgClassNameRefExp:
       // case V_SgFunctionRefExp:
       // case V_SgMemberFunctionRefExp:
       // case V_SgFunctionCallExp:
       // case V_SgSizeOfOp:
       // case V_SgVarArgStartOp:
       // case V_SgVarArgStartOneOperandOp:
       // case V_SgVarArgOp:
       // case V_SgVarArgEndOp:
       // case V_SgVarArgCopyOp:
       // case V_SgTypeIdOp:
       // case V_SgConditionalExp:
       // case V_SgNewExp:
          case V_SgDeleteExp:
          case V_SgThisExp:
       // case V_SgRefExp:
          case V_SgAggregateInitializer:
          case V_SgConstructorInitializer:
       // case V_SgAssignInitializer:
          case V_SgExpressionRoot:
          case V_SgMinusOp:
          case V_SgUnaryAddOp:
          case V_SgNotOp:
          case V_SgPointerDerefExp:
          case V_SgAddressOfOp:
          case V_SgMinusMinusOp:
          case V_SgPlusPlusOp:
          case V_SgBitComplementOp:
          case V_SgCastExp:
          case V_SgThrowOp:
          case V_SgArrowExp:
          case V_SgDotExp:
          case V_SgDotStarOp:
          case V_SgArrowStarOp:
          case V_SgEqualityOp:
          case V_SgLessThanOp:
          case V_SgGreaterThanOp:
          case V_SgNotEqualOp:
          case V_SgLessOrEqualOp:
          case V_SgGreaterOrEqualOp:
          case V_SgAddOp:
          case V_SgSubtractOp:
          case V_SgMultiplyOp:
          case V_SgDivideOp:
          case V_SgIntegerDivideOp:
          case V_SgModOp:
          case V_SgAndOp:
          case V_SgOrOp:
          case V_SgBitXorOp:
          case V_SgBitAndOp:
          case V_SgBitOrOp:
          case V_SgCommaOpExp:
          case V_SgLshiftOp:
          case V_SgRshiftOp:
          case V_SgPntrArrRefExp:
          case V_SgScopeOp:
          case V_SgAssignOp:
          case V_SgPlusAssignOp:
          case V_SgMinusAssignOp:
          case V_SgAndAssignOp:
          case V_SgIorAssignOp:
          case V_SgMultAssignOp:
          case V_SgDivAssignOp:
          case V_SgModAssignOp:
          case V_SgXorAssignOp:
          case V_SgLshiftAssignOp:
          case V_SgRshiftAssignOp:
          case V_SgBoolValExp:
          case V_SgStringVal:
          case V_SgShortVal:
          case V_SgCharVal:
          case V_SgUnsignedCharVal:
          case V_SgWcharVal:
          case V_SgUnsignedShortVal:
          case V_SgIntVal:
          case V_SgEnumVal:
          case V_SgUnsignedIntVal:
          case V_SgLongIntVal:
          case V_SgLongLongIntVal:
          case V_SgUnsignedLongLongIntVal:
          case V_SgUnsignedLongVal:
          case V_SgFloatVal:
          case V_SgDoubleVal:
          case V_SgLongDoubleVal:
          case V_SgComplexVal:
             {
            // printf ("Get the type out of the SgExpression (not implemented) \n");
            // printf ("-- We need to cast the astNode to each different expression type and then call get_type() \n");

               SgExpression* x = isSgExpression(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): debugging a problem
               if (x->get_type() == NULL)
                  {
                    SgNode* p = x->get_parent();
                    printf ("Error: get_type() returns NULL for x = %p = %s (parent = %p = %s) \n",x,x->class_name().c_str(),p,p->class_name().c_str());
                    SgUnaryOp* unaryOp = isSgUnaryOp(x);
                    SgExpression* operand = unaryOp->get_operand();
                    ROSE_ASSERT(operand != NULL);
                    ROSE_ASSERT(operand->get_type() != NULL);
                    printf ("operand->get_type() = %s \n",operand->get_type()->class_name().c_str());
                 // x->set_type(operand->get_type());
                 // unaryOp->set_type(operand->get_type());
                  }
               ROSE_ASSERT (x->get_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type());

#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the Expression Values (%s) \n",x->sage_class_name());
               SgType* type = x->get_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

       // DQ (7/26/2004): SgUnaryOp is not a terminal in the grammar so it is never traversed
          case V_SgUnaryOp:
             {
            // printf ("Get the type out of the SgUnaryOp \n");
               SgUnaryOp* x = isSgUnaryOp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());

#if TRAVERSE_TYPES
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgUnaryOp type \n");
#if 0
            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
#endif
               break;
             }

       // DQ (7/26/2004): SgUnaryOp is not a terminal in the grammar so it is never traversed
          case V_SgBinaryOp:
             {
            // printf ("Get the type out of the SgBinaryOp \n");
               SgBinaryOp* x = isSgBinaryOp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());

#if TRAVERSE_TYPES
            // Specified with DEF2TYPE_TRAVERSAL
            // printf ("Added traversal of the SgUnaryOp type \n");
#if 0
            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
#endif
               break;
             }

          case V_SgFunctionRefExp:
             {
            // printf ("Get the type out of the SgFunctionRefExp \n");
               SgFunctionRefExp* x = isSgFunctionRefExp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_function_type());

#if TRAVERSE_TYPES
            // Specified with DEF2TYPE_TRAVERSAL
            // printf ("Added traversal of the SgFunctionRefExp type \n");
#if 0
            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
               SgType* type = x->get_function_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
#endif
               break;
             }

          case V_SgMemberFunctionRefExp:
             {
            // printf ("Get the type out of the SgMemberFunctionRefExp \n");
               SgMemberFunctionRefExp* x = isSgMemberFunctionRefExp(astNode);
               ROSE_ASSERT (x != NULL);
#if 0
            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_function_type());

#if TRAVERSE_TYPES
            // Specified with DEF2TYPE_TRAVERSAL
            // printf ("Added traversal of the SgMemberFunctionRefExp type \n");
               SgType* type = x->get_function_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
#endif
               break;
             }

          case V_SgFunctionCallExp:
             {
            // printf ("Get the type out of the SgFunctionCallExp \n");
               SgFunctionCallExp* x = isSgFunctionCallExp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgFunctionCallExp type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgSizeOfOp:
             {
            // printf ("Get the type out of the SgSizeOfOp \n");
               SgSizeOfOp* x = isSgSizeOfOp(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_operand_type());

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgSizeOfOp type \n");
               SgType* type1 = x->get_operand_type();
               ROSE_ASSERT(type1 != NULL);
               list<SgNode*> typeList1 = NodeQuery::querySubTree (type1,targetVariantVector);
               mergeList (returnNodeList,typeList1);

            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgSizeOfOp type \n");
               SgType* type2 = x->get_expression_type();
               ROSE_ASSERT(type2 != NULL);
               list<SgNode*> typeList2 = NodeQuery::querySubTree (type2,targetVariantVector);
               mergeList (returnNodeList,typeList2);
#endif
               break;
             }

          case V_SgTypeIdOp:
             {
            // printf ("Get the type out of the SgSizeOfOp \n");
               SgTypeIdOp* x = isSgTypeIdOp(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_operand_type());

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgTypeIdOp type \n");
               SgType* type1 = x->get_operand_type();
               ROSE_ASSERT(type1 != NULL);
               list<SgNode*> typeList1 = NodeQuery::querySubTree (type1,targetVariantVector);
               mergeList (returnNodeList,typeList1);

            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgTypeIdOp type \n");
               SgType* type2 = x->get_expression_type();
               ROSE_ASSERT(type2 != NULL);
               list<SgNode*> typeList2 = NodeQuery::querySubTree (type2,targetVariantVector);
               mergeList (returnNodeList,typeList2);
#endif
               break;
             }

          case V_SgVarArgStartOp:
             {
            // printf ("Get the type out of the SgVarArgStartOp \n");
               SgVarArgStartOp* x = isSgVarArgStartOp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgVarArgStartOp type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgVarArgStartOneOperandOp:
             {
            // printf ("Get the type out of the SgVarArgStartOneOperandOp \n");
               SgVarArgStartOneOperandOp* x = isSgVarArgStartOneOperandOp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgVarArgStartOneOperandOp type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgVarArgOp:
             {
            // printf ("Get the type out of the SgVarArgOp \n");
               SgVarArgOp* x = isSgVarArgOp(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgVarArgOp type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgVarArgEndOp:
             {
            // printf ("Get the type out of the SgVarArgEndOp \n");
               SgVarArgEndOp* x = isSgVarArgEndOp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgVarArgEndOp type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgVarArgCopyOp:
             {
            // printf ("Get the type out of the SgVarArgCopyOp \n");
               SgVarArgCopyOp* x = isSgVarArgCopyOp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgVarArgCopyOp type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgConditionalExp:
             {
            // printf ("Get the type out of the SgConditionalExp \n");
               SgConditionalExp* x = isSgConditionalExp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgConditionalExp type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgNewExp:
             {
            // printf ("Get the type out of the SgNewExp \n");
               SgNewExp* x = isSgNewExp(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());

#if TRAVERSE_TYPES
            // Specified with DEF2TYPE_TRAVERSAL
#if 0
            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // printf ("Added traversal of the SgNewExp type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
#endif
               break;
             }

          case V_SgRefExp:
             {
            // printf ("Get the type out of the SgRefExp \n");
               SgRefExp* x = isSgRefExp(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type_name());

#if TRAVERSE_TYPES
            // Specified with DEF2TYPE_TRAVERSAL
            // printf ("Added traversal of the SgRefExp type \n");
               SgType* type = x->get_type_name();
               ROSE_ASSERT(type != NULL);
               Rose_STL_Container<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgAssignInitializer:
             {
            // printf ("Get the type out of the SgAssignInitializer \n");
               SgAssignInitializer* x = isSgAssignInitializer(astNode);
               ROSE_ASSERT (x != NULL);

            // DQ (1/14/2006): The type is no longer stored in expressions (and is computed instead).
            // pushNewNode (returnNodeList,targetVariantVector,x->get_expression_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgAssignInitializer type \n");
               SgType* type = x->get_expression_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

       // Symbols are not traversed!
          case V_SgVariableSymbol: // contains a SgInitializedName object
          case V_SgEnumFieldSymbol:// contains a SgInitializedName object
          case V_SgNamespaceSymbol:// contains a SgInitializedName object
             {
               printf ("Error: Symbols are not traversed! astNode = %s \n",astNode->sage_class_name());
               ROSE_ASSERT (false);
               break;
             }

          case V_SgFunctionTypeSymbol:
             {
               printf ("Get the type out of the SgFunctionTypeSymbol \n");
               SgFunctionTypeSymbol* x = isSgFunctionTypeSymbol(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgFunctionTypeSymbol type \n");
               SgType* type = x->get_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgDefaultSymbol:
             {
               printf ("Get the type out of the SgDefaultSymbol \n");
               SgDefaultSymbol* x = isSgDefaultSymbol(astNode);
               ROSE_ASSERT (x != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type());
#if 0
            // Specified with DEF2TYPE_TRAVERSAL
               printf ("Added traversal of the SgDefaultSymbol type \n");
               SgType* type = x->get_type();
               ROSE_ASSERT(type != NULL);
               list<SgNode*> typeList = NodeQuery::querySubTree (type,targetVariantVector);
               mergeList (returnNodeList,typeList);
#endif
               break;
             }

          case V_SgClassSymbol:
          case V_SgTemplateSymbol:
          case V_SgEnumSymbol:
          case V_SgTypedefSymbol:
          case V_SgMemberFunctionSymbol:
          case V_SgLabelSymbol:
             {
               printf ("Error: Symbols are not traversed! \n");
               ROSE_ASSERT (false);
               break;
             }

          case V_SgSymbol:
             {
            // I hope we don't have to cast to each different type of symbol before calling get_type()
            // printf ("Get the type out of the SgSymbol (not implemented) \n");
               SgSymbol* x = isSgSymbol(astNode);
               ROSE_ASSERT (x != NULL);
               ROSE_ASSERT (x->get_type() != NULL);
               pushNewNode (returnNodeList,targetVariantVector,x->get_type());

               printf ("Error: Symbols are not traversed! \n");
               ROSE_ASSERT (false);
               break;
             }

          case V_SgClassDefinition:
             {
//             printf ("Note: nothing to do for SgClassDefinition astNode = %s \n",astNode->sage_class_name());
               break;
             }

          case V_SgModifier:
          case V_SgBitAttribute:
          case V_SgAttribute:
          case V_SgSupport:
//        case V_SgPartialFunctionType:
//        case V_SgMemberFunctionType:
//        case V_SgFunctionType:
//        case V_SgPointerType:
//        case V_SgNamedType:
//        case V_SgType:
          case V_SgInitializer:
//        case V_SgUnaryOp:
//        case V_SgBinaryOp:
//        case V_SgValueExp:
//        case V_SgExpression:
//        case V_SgClassDeclaration:
//        case V_SgClassDefinition:
          case V_SgScopeStatement:
//        case V_SgMemberFunctionDeclaration:
//        case V_SgFunctionDeclaration:
          case V_SgDeclarationStatement:
          case V_SgStatement:
          case V_SgFunctionSymbol:
//        case V_SgSymbol:
          case V_SgLocatedNode:
          case V_SgNode:
          case V_SgSymbolTable:
//        case V_SgInitializedName:
          case V_SgName:
          case V_SgPragma:
          case V_SgModifierNodes:
          case V_SgConstVolatileModifier:
          case V_SgStorageModifier:
          case V_SgAccessModifier:
          case V_SgFunctionModifier:
          case V_SgUPC_AccessModifier:
          case V_SgLinkageModifier:
          case V_SgSpecialFunctionModifier:
          case V_SgTypeModifier:
          case V_SgElaboratedTypeModifier:
          case V_SgBaseClassModifier:
          case V_SgDeclarationModifier:
          case V_Sg_File_Info:
       // case V_SgFile:
          case V_SgSourceFile:
          case V_SgBinaryFile:
          case V_SgProject:
          case V_SgOptions:
          case V_SgUnparse_Info:
          case V_SgFuncDecl_attr:
          case V_SgClassDecl_attr:
//        case V_SgTypedefSeq:
//        case V_SgFunctionParameterTypeList:
//        case V_SgTemplateParameter:
//        case V_SgTemplateArgument:
          case V_SgBaseClass:
//        case V_SgTypeUnknown:
//        case V_SgTypeChar:
//        case V_SgTypeSignedChar:
//        case V_SgTypeUnsignedChar:
//        case V_SgTypeShort:
//        case V_SgTypeSignedShort:
//        case V_SgTypeUnsignedShort:
//        case V_SgTypeInt:
//        case V_SgTypeSignedInt:
//        case V_SgTypeUnsignedInt:
//        case V_SgTypeLong:
//        case V_SgTypeSignedLong:
//        case V_SgTypeUnsignedLong:
//        case V_SgTypeVoid:
//        case V_SgTypeGlobalVoid:
//        case V_SgTypeWchar:
//        case V_SgTypeFloat:
//        case V_SgTypeDouble:
//        case V_SgTypeLongLong:
//        case V_SgTypeUnsignedLongLong:
//        case V_SgTypeLongDouble:
//        case V_SgTypeString:
//        case V_SgTypeBool:
//        case V_SgTypeComplex:
//        case V_SgTypeDefault:
//        case V_SgPointerMemberType:
//        case V_SgReferenceType:
//        case V_SgClassType:
//        case V_SgTemplateType:
//        case V_SgEnumType:
//        case V_SgTypedefType:
//        case V_SgModifierType:
//        case V_SgPartialFunctionModifierType:
//        case V_SgArrayType:
//        case V_SgTypeEllipse:
//        case V_SgUnknownMemberFunctionType:
//        case V_SgExprListExp:
//        case V_SgVarRefExp:
//        case V_SgClassNameRefExp:
//        case V_SgFunctionRefExp:
//        case V_SgMemberFunctionRefExp:
//        case V_SgFunctionCallExp:
//        case V_SgSizeOfOp:
//        case V_SgVarArgStartOp:
//        case V_SgVarArgStartOneOperandOp:
//        case V_SgVarArgOp:
//        case V_SgVarArgEndOp:
//        case V_SgVarArgCopyOp:
//        case V_SgTypeIdOp:
//        case V_SgConditionalExp:
//        case V_SgNewExp:
//        case V_SgDeleteExp:
//        case V_SgThisExp:
//        case V_SgRefExp:
//        case V_SgAggregateInitializer:
//        case V_SgConstructorInitializer:
//        case V_SgAssignInitializer:
//        case V_SgExpressionRoot:
//        case V_SgMinusOp:
//        case V_SgUnaryAddOp:
//        case V_SgNotOp:
//        case V_SgPointerDerefExp:
//        case V_SgAddressOfOp:
//        case V_SgMinusMinusOp:
//        case V_SgPlusPlusOp:
//        case V_SgBitComplementOp:
//        case V_SgCastExp:
//        case V_SgThrowOp:
//        case V_SgArrowExp:
//        case V_SgDotExp:
//        case V_SgDotStarOp:
//        case V_SgArrowStarOp:
//        case V_SgEqualityOp:
//        case V_SgLessThanOp:
//        case V_SgGreaterThanOp:
//        case V_SgNotEqualOp:
//        case V_SgLessOrEqualOp:
//        case V_SgGreaterOrEqualOp:
//        case V_SgAddOp:
//        case V_SgSubtractOp:
//        case V_SgMultiplyOp:
//        case V_SgDivideOp:
//        case V_SgIntegerDivideOp:
//        case V_SgModOp:
//        case V_SgAndOp:
//        case V_SgOrOp:
//        case V_SgBitXorOp:
//        case V_SgBitAndOp:
//        case V_SgBitOrOp:
//        case V_SgCommaOpExp:
//        case V_SgLshiftOp:
//        case V_SgRshiftOp:
//        case V_SgPntrArrRefExp:
//        case V_SgScopeOp:
//        case V_SgAssignOp:
//        case V_SgPlusAssignOp:
//        case V_SgMinusAssignOp:
//        case V_SgAndAssignOp:
//        case V_SgIorAssignOp:
//        case V_SgMultAssignOp:
//        case V_SgDivAssignOp:
//        case V_SgModAssignOp:
//        case V_SgXorAssignOp:
//        case V_SgLshiftAssignOp:
//        case V_SgRshiftAssignOp:
//        case V_SgBoolValExp:
//        case V_SgStringVal:
//        case V_SgShortVal:
//        case V_SgCharVal:
//        case V_SgUnsignedCharVal:
//        case V_SgWcharVal:
//        case V_SgUnsignedShortVal:
//        case V_SgIntVal:
//        case V_SgEnumVal:
//        case V_SgUnsignedIntVal:
//        case V_SgLongIntVal:
//        case V_SgLongLongIntVal:
//        case V_SgUnsignedLongLongIntVal:
//        case V_SgUnsignedLongVal:
//        case V_SgFloatVal:
//        case V_SgDoubleVal:
//        case V_SgLongDoubleVal:
          case V_SgForStatement:
          case V_SgForInitStatement:
          case V_SgCatchStatementSeq:
//        case V_SgFunctionParameterList:
//        case V_SgCtorInitializerList:
          case V_SgBasicBlock:
          case V_SgGlobal:
          case V_SgIfStmt:
          case V_SgFunctionDefinition:
          case V_SgWhileStmt:
          case V_SgDoWhileStmt:
          case V_SgSwitchStatement:
          case V_SgCatchOptionStmt:
//        case V_SgVariableDeclaration:
//        case V_SgVariableDefinition:
//        case V_SgEnumDeclaration:
          case V_SgAsmStmt:
//        case V_SgTypedefDeclaration:
          case V_SgFunctionTypeTable:
          case V_SgExprStatement:
          case V_SgLabelStatement:
          case V_SgCaseOptionStmt:
          case V_SgTryStmt:
          case V_SgDefaultOptionStmt:
          case V_SgBreakStmt:
          case V_SgContinueStmt:
          case V_SgReturnStmt:
          case V_SgGotoStatement:
          case V_SgSpawnStmt:
          case V_SgPragmaDeclaration:
          case V_SgTemplateDeclaration:
          case V_SgTemplateInstantiationDecl:
          case V_SgTemplateInstantiationDefn:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
          case V_SgClinkageStartStatement:
          case V_SgNamespaceDeclarationStatement:
          case V_SgNamespaceAliasDeclarationStatement:
          case V_SgNamespaceDefinitionStatement:
          case V_SgUsingDeclarationStatement:
          case V_SgUsingDirectiveStatement:
          case V_SgTemplateInstantiationDirectiveStatement:
//        case V_SgVariableSymbol:
//        case V_SgFunctionTypeSymbol:
//        case V_SgClassSymbol:
//        case V_SgTemplateSymbol:
//        case V_SgEnumSymbol:
//        case V_SgEnumFieldSymbol:
//        case V_SgTypedefSymbol:
//        case V_SgMemberFunctionSymbol:
//        case V_SgLabelSymbol:
//        case V_SgDefaultSymbol:
          case V_SgNullStatement:
          case V_SgVariantStatement:
          case V_SgNullExpression:
          case V_SgVariantExpression:
          case V_SgStatementExpression:
          case V_SgAsmOp:

       // DQ (1/9/2008): The binary IR nodes must be handled here as well!
          case V_SgAsmFile:
          case V_SgAsmBlock:
          case V_SgAsmMemoryReferenceExpression:
          case V_SgAsmRegisterReferenceExpression:
          case V_SgAsmx86RegisterReferenceExpression:
          case V_SgAsmArmRegisterReferenceExpression:
          case V_SgAsmWordValueExpression:
          case V_SgAsmDoubleWordValueExpression:
          case V_SgAsmQuadWordValueExpression:
          case V_SgAsmByteValueExpression:
          case V_SgAsmInstruction:
          case V_SgAsmx86Instruction:
          case V_SgAsmArmInstruction:
          case V_SgAsmOperandList:
          case V_SgAsmBinaryAdd:
          case V_SgAsmFunctionDeclaration:
          case V_SgAsmBinaryMultiply:

#if USE_OLD_BINARY_EXECUTABLE_IR_NODES
  // DQ (8/2/2008): these might be required for now, but we need to be removed later!
          case V_SgAsmProgramHeaderList:
          case V_SgAsmSectionHeaderList:
#endif

       // Liao, 12/13/2007, new fortran nodes, put in ignoring list temporarily
          case V_SgWriteStatement:
          case V_SgProcedureHeaderStatement:
          case V_SgAttributeSpecificationStatement:
          case V_SgFortranDo:
          case V_SgReadStatement:
          case V_SgArithmeticIfStatement:
          case V_SgPrintStatement:
          case V_SgFormatStatement:
          case V_SgImplicitStatement:
          case V_SgExponentiationOp:
          case V_SgDerivedTypeStatement:
          case V_SgNamelistStatement:
          case V_SgModuleStatement:
          case V_SgEquivalenceStatement:
          case V_SgCommonBlock:
          case V_SgInterfaceStatement:
          case V_SgWhereStatement:
          case V_SgElseWhereStatement:
          case V_SgSubscriptExpression:
          case V_SgImpliedDo:
          case V_SgOpenStatement:
          case V_SgCloseStatement:
          case V_SgImportStatement:
          case V_SgFlushStatement:
          case V_SgRewindStatement:
          case V_SgWaitStatement:
          case V_SgAssociateStatement:
          case V_SgContainsStatement:
          case V_SgEntryStatement:
          case V_SgForAllStatement:
          case V_SgBlockDataStatement:
          case V_SgProgramHeaderStatement:
          case V_SgStopOrPauseStatement:
          case V_SgConcatenationOp:
          case V_SgComputedGotoStatement:
          case V_SgUseStatement:
          case V_SgBackspaceStatement:
          case V_SgLabelRefExp:
          case V_SgActualArgumentExpression:
          case V_SgAsteriskShapeExp:
          case V_SgDesignatedInitializer:
         // Liao, 8/28/2008, ignore  UPC nodes for now
          case V_SgUpcLocalsizeofExpression:
          case V_SgUpcBlocksizeofExpression:
          case V_SgUpcElemsizeofExpression:
          case V_SgUpcNotifyStatement:
          case V_SgUpcWaitStatement:
          case V_SgUpcBarrierStatement:
          case V_SgUpcFenceStatement:
          case V_SgUpcForAllStatement:
          case V_SgUpcThreads:
          case V_SgUpcMythread:
             {
            // printf ("Error unimplemented case in switch: ignoring astNode = %s \n",astNode->sage_class_name());
            // ROSE_ASSERT(false);
               break;
             }

          default:
             {
               printf ("Error default case in querySolverGrammarElementFromVariantVector() switch: ignoring astNode = %s \n",astNode->sage_class_name());
	         // ROSE_ASSERT(false);
               break;
             }
        }

     return returnNodeList;
   } /* End function querySolverUnionFields() */

#else 

// DQ (4/7/2004): Added to support more general lookup of data in the AST (vector of variants)
NodeQuerySynthesizedAttributeType
querySolverGrammarElementFromVariantVector ( 
   SgNode * astNode, 
   VariantVector targetVariantVector )
   {
  // This function extracts type nodes that would not be traversed so that they can
  // accumulated to a list.  The specific nodes collected into the list is controlled
  // by targetVariantVector.

     ROSE_ASSERT (astNode != NULL);
     NodeQuerySynthesizedAttributeType returnNodeList;

     Rose_STL_Container<SgNode*> nodesToVisitTraverseOnlyOnce;

     pushNewNode (returnNodeList,targetVariantVector,astNode);

     vector<SgNode*>               succContainer      = astNode->get_traversalSuccessorContainer();
     vector<pair<SgNode*,string> > allNodesInSubtree  = astNode->returnDataMemberPointers();

     if( succContainer.size() != allNodesInSubtree.size() )
     for(vector<pair<SgNode*,string> >::iterator iItr = allNodesInSubtree.begin(); iItr!= allNodesInSubtree.end();
         ++iItr )
       if( isSgType(iItr->first) != NULL  )
         if(std::find(succContainer.begin(),succContainer.end(),iItr->first) == succContainer.end() )
           pushNewNode (returnNodeList,targetVariantVector,iItr->first);

     
     return returnNodeList;
   } /* End function querySolverUnionFields() */
#endif

}//END NAMESPACE NODEQUERY

