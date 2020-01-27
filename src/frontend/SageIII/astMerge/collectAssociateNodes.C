#ifndef COLLECT_ASSOCIATE_NODES_C
#define COLLECT_ASSOCIATE_NODES_C

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

using namespace std;

void
addAssociatedNodes( SgType* type, set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted )
   {
  // Marking the associated IR nodes for a type is more complex because we want to mark the things in the type as well.
  // In the case of the SgTypedefSeq it is not shared between types (currently) and so it is marked to be deleted with the 
  // type (the delete operator for the types will call automatically call the delete operator for the SgTypedefSeq member pointer).
  // This is also the case with the SgFunctionParameterList (but this case is handled directly).

     ROSE_ASSERT(type != NULL);

  // The semantics of this function should be that it at least includes the input
     nodeList.insert(type);

  // Make sure this IR node was not previously deleted (the finalDeleteSet is non-empty in the 
  // final step of calling getSetOfFrontendSpecificNodes() to build the graph but skip drawing 
  // the front-end specific IR nodes to simplify the graph)).
     if (finalDeleteSet.find(type) != finalDeleteSet.end())
        {
          printf ("ERROR: addAssociatedNodes(SgType* type): adding previously deleted SgType to nodeList type = %p = %s = %s \n",type,type->class_name().c_str(),SageInterface::get_name(type).c_str());
        }
  // ROSE_ASSERT(finalDeleteSet.find(type) == finalDeleteSet.end());

#if 0
     printf ("addAssociatedNodes(SgType* type): type = %p = %s = %s \n",type,type->class_name().c_str(),SageInterface::get_name(type).c_str());
#endif
  // printf ("addAssociatedNodes(SgType* type): type->get_mangled() = %s \n",type->get_mangled().str());

     switch(type->variantT())
        {
          case V_SgFunctionType:
          case V_SgMemberFunctionType:
             {
               SgFunctionType* functionType = isSgFunctionType(type);
               ROSE_ASSERT(functionType != NULL);
               nodeList.insert(functionType->get_return_type());
               addAssociatedNodes(functionType->get_return_type(),nodeList,markMemberNodesDefinedToBeDeleted);

               nodeList.insert(functionType->get_orig_return_type());
               addAssociatedNodes(functionType->get_orig_return_type(),nodeList,markMemberNodesDefinedToBeDeleted);

               SgTypePtrList::iterator parameter = functionType->get_arguments().begin();
               while (parameter != functionType->get_arguments().end())
                  {
                    ROSE_ASSERT(*parameter != NULL);
                    nodeList.insert(*parameter);
                    addAssociatedNodes(*parameter,nodeList,markMemberNodesDefinedToBeDeleted);
                    parameter++;
                  }

               ROSE_ASSERT(functionType->get_argument_list() != NULL);
               if (functionType->get_argument_list() != NULL)
                  {
                    nodeList.insert(functionType->get_argument_list());
                    ROSE_ASSERT(finalDeleteSet.find(functionType->get_argument_list()) == finalDeleteSet.end());
                  }
#if 0
            // I am unclear where these are used and why. Though I have seen them use on SgCharType IR nodes.
               nodeList.insert(functionType->get_ref_to());
               nodeList.insert(functionType->get_ptr_to());
#endif
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }

          case V_SgTypedefType:
             {
               SgTypedefType* typedefType = isSgTypedefType(type);
               ROSE_ASSERT(typedefType != NULL);

            // printf ("Processing typedefType by calling addAssociatedNodes(typedefType->get_declaration(),,) \n");
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(typedefType->get_declaration());
               ROSE_ASSERT(typedefDeclaration != NULL);

            // DQ (2/8/2007): Add the associated declaration (since this function might 
            // have been visited from somewhere else then the typedefDeclaration).
               nodeList.insert(typedefDeclaration);


            // Can this cause recursion?
            // addAssociatedNodes(typedefDeclaration->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);

            // DQ (6/30/2018): Detect a break cycles in typedefs (debugging test2018_118.C).
               static std::set<SgTypedefDeclaration*> typedefDeclarationSet;
               if (typedefDeclarationSet.find(typedefDeclaration) == typedefDeclarationSet.end())
                  {
                    typedefDeclarationSet.insert(typedefDeclaration);

                 // Can this cause recursion?
                    addAssociatedNodes(typedefDeclaration->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
                  }
                 else
                  {
#if 0
                    printf ("In addAssociatedNodes: typedefDeclaration was previously seen: typedefDeclaration = %p = %s \n",typedefDeclaration,typedefDeclaration->class_name().c_str());
#endif
                  }

            // DQ (6/23/2010): We need to include the type defined by the typedef as well.
            // addAssociatedNodes(typedefDeclaration->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);

            // This should be redundant since we come from the SgTypedefType (and does cause recursion).
            // However we could test to see if the declaration is in the nodeList and only call it if not!
            // addAssociatedNodes(typedefDeclaration->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);
               if (nodeList.find(typedefDeclaration) == nodeList.end())
                  {
                    addAssociatedNodes(typedefDeclaration,nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }

       // DQ (2/8/2007): Added this case because it might fix a bug in handling iostream (where after delete the declaration is NULL).
          case V_SgEnumType:
             {
               SgEnumType* enumType = isSgEnumType(type);
               ROSE_ASSERT(enumType != NULL);

            // printf ("Processing typedefType by calling addAssociatedNodes(typedefType->get_declaration(),,) \n");
               SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(enumType->get_declaration());
               ROSE_ASSERT(enumDeclaration != NULL);

            // DQ (2/8/2007): Add the associated declaration (since this function might 
            // have been visited from somewhere else then the typedefDeclaration).
               nodeList.insert(enumDeclaration);

            // This should be redundant since we come from the SgEnumType (and should cause recursion).
            // However we could test to see if the declaration is in the nodeList and only call it if not!
            // addAssociatedNodes(enumDeclaration->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);
               if (nodeList.find(enumDeclaration) == nodeList.end())
                  {
                    addAssociatedNodes(enumDeclaration,nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }

       // DQ (2/8/2007): Added this case because it might fix a bug in handling iostream (where after delete the declaration is NULL).
          case V_SgClassType:
             {
               SgClassType* classType = isSgClassType(type);
               ROSE_ASSERT(classType != NULL);

            // printf ("Processing typedefType by calling addAssociatedNodes(typedefType->get_declaration(),,) \n");
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               ROSE_ASSERT(classDeclaration != NULL);

            // DQ (2/8/2007): Add the associated declaration (since this function might 
            // have been visited from somewhere else then the typedefDeclaration).
               nodeList.insert(classDeclaration);

            // This should be redundant since we come from the SgClassType (and should cause recursion).
            // However we could test to see if the declaration is in the nodeList and only call it if not!
            // addAssociatedNodes(classDeclaration->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);
               if (nodeList.find(classDeclaration) == nodeList.end())
                  {
                    addAssociatedNodes(classDeclaration,nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }

          case V_SgArrayType:
             {
               SgArrayType* arrayType = isSgArrayType(type);
               ROSE_ASSERT(arrayType != NULL);
               if (arrayType->get_base_type() != NULL)
                  {
                    addAssociatedNodes(arrayType->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
                    ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
                  }
            // Could this be a more complex expression than just SgValue!
               if (arrayType->get_index() != NULL)
                  {
                    nodeList.insert(arrayType->get_index());
                    ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
                  }
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }

          case V_SgPointerType:
          case V_SgPointerMemberType:
             {
               SgPointerType* pointerType = isSgPointerType(type);
               ROSE_ASSERT(pointerType != NULL);
               if (pointerType->get_base_type() != NULL)
                  {
                    nodeList.insert(pointerType->get_base_type());
                    addAssociatedNodes(pointerType->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }

          case V_SgReferenceType:
             {
               SgReferenceType* referenceType = isSgReferenceType(type);
               ROSE_ASSERT(referenceType != NULL);
               if (referenceType->get_base_type() != NULL)
                  {
                    nodeList.insert(referenceType->get_base_type());
                    addAssociatedNodes(referenceType->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }

          case V_SgModifierType:
             {
               SgModifierType* modifierType = isSgModifierType(type);
               ROSE_ASSERT(modifierType != NULL);
               if (modifierType->get_base_type() != NULL)
                  {
                    nodeList.insert(modifierType->get_base_type());
                    addAssociatedNodes(modifierType->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }
             

        case V_SgTypeMatrix:
          {
            SgTypeMatrix *matrixType = isSgTypeMatrix(type);
            ROSE_ASSERT(matrixType != NULL);

            if(matrixType->get_base_type() != NULL)
              {
                nodeList.insert(matrixType->get_base_type());
                addAssociatedNodes(matrixType->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
              }
            
            ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
            break;
          }

        case V_SgTypeTuple:
          {
            SgTypeTuple *tupleType = isSgTypeTuple(type);
            ROSE_ASSERT(tupleType != NULL);

            SgTypePtrList typeList = tupleType->get_types();

            for(SgTypePtrList::iterator it = typeList.begin(); it != typeList.end(); it++)
              {
                if(*it != NULL)
                  {
                    nodeList.insert(*it);
                    addAssociatedNodes(*it, nodeList, markMemberNodesDefinedToBeDeleted);
                  }
              }
            
            ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
            break;
          }
          
       // DQ (9/5/2011): Added support for SgJavaParameterizedType.
          case V_SgJavaParameterizedType:
             {
               SgJavaParameterizedType* javaParameterizedType = isSgJavaParameterizedType(type);
               ROSE_ASSERT(javaParameterizedType != NULL);
               if (javaParameterizedType->get_raw_type() != NULL)
                  {
                    nodeList.insert(javaParameterizedType->get_raw_type());
                    addAssociatedNodes(javaParameterizedType->get_raw_type(),nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());

               if (javaParameterizedType->get_type_list() != NULL)
                  {
                    SgTemplateParameterList* type_list = javaParameterizedType->get_type_list();
                    for (size_t i = 0; i < type_list->get_args().size(); i++)
                       {
                         SgType* argumentType = NULL;
                         SgTemplateParameter* templateParameter = type_list->get_args()[i];
                         ROSE_ASSERT(templateParameter != NULL);
                         if (templateParameter->get_parameterType() == SgTemplateParameter::type_parameter)
                            {
                              if (templateParameter->get_type() != NULL)
                                 {
                                   argumentType = templateParameter->get_type();
                                 }
                                else
                                 {
                                // Do we need to support the default type when the type is not explicit.
                                 }
                            }
                           else
                            {
                           // This was not a type parameter (but it might be a template declaration or something work paying attention to).
                            }

                      // There are a number of way in which the argumentType can be set (but maybe a restricted set of ways for Java).
                         if (argumentType != NULL)
                            {
                              nodeList.insert(argumentType);
                              addAssociatedNodes(argumentType,nodeList,markMemberNodesDefinedToBeDeleted);
                            }
                           else
                            {
                           // It might be that this branch should be an error for Java. But likely caught elsewhere in ROSE.
                            }
                       }
                  }

               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());

               break;
             }

          case V_SgQualifiedNameType:
       // case V_SgTemplateType:
          case V_SgPartialFunctionModifierType:
          // case V_SgUnknownMemberFunctionType:
             {
            // Trap these cases ...
               printf ("Unimplemented case reached for type = %p = %s \n",type,type->class_name().c_str());
               ROSE_ASSERT(false);
               break;
             }

       // DQ (9/6/2016): Added support for new type now referenced as a result of using new automated generation of builtin functions for ROSE.
          case V_SgTypeSigned128bitInteger:
          case V_SgTypeUnsigned128bitInteger:

       // DQ (2/2/2011): Unclear if there is anything to do here for this type (any associated IR nodes would have been visited already).
          case V_SgTypeLabel:

       // DQ (1/6/2009): Added support for SgTypeSignedLongLong (only an error on 32 bit systems, not 64 bit systems).
          case V_SgTypeSignedLongLong:

       // These don't have types hidden internally (but they do have declarations)
       // case V_SgClassType:
       // case V_SgEnumType:

       // DQ (9/1/2012): Added more template support.
          case V_SgTemplateType:

       // DQ (8/2/2014): Added C++11 SgDeclType support.
          case V_SgDeclType:

       // DQ (3/29/2015): Added support for GNU C language extension typeof.
          case V_SgTypeOfType:

       // DQ (1/21/2018): Added support for C++11 language type.
          case V_SgRvalueReferenceType:

          case V_SgJovialTableType:

       // These are primative types
          case V_SgJavaWildcardType:
          case V_SgTypeBool:
          case V_SgTypeChar:
          case V_SgTypeComplex:
          case V_SgTypeDefault:
          case V_SgTypeDouble:
          case V_SgTypeEllipse:
          case V_SgTypeFloat:
          case V_SgTypeGlobalVoid:
          case V_SgTypeImaginary:
          case V_SgTypeInt:
          case V_SgTypeLong:
          case V_SgTypeLongDouble:
          case V_SgTypeLongLong:
          case V_SgTypeShort:
          case V_SgTypeSignedChar:
          case V_SgTypeSignedInt:
          case V_SgTypeSignedLong:
          case V_SgTypeSignedShort:
          case V_SgTypeString:
          case V_SgTypeUnknown:
          case V_SgTypeUnsignedChar:
          case V_SgTypeUnsignedInt:
          case V_SgTypeUnsignedLong:
          case V_SgTypeUnsignedLongLong:
          case V_SgTypeUnsignedShort:
          case V_SgTypeVoid:
          case V_SgTypeWchar:
          case V_SgTypeCAFTeam:
       // Allow this as an IR node into the AST.
          case V_SgPartialFunctionType:
       // TV (04/16/2018): Ignore non-real type for now
          case V_SgNonrealType:
          case V_SgAutoType:
             {
            // Ignore these cases (they contain no base types)...
               nodeList.insert(type);
               ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
               break;
             }

          default:
             {
               printf ("Error in addAssociatedNodes( SgType* type): default case reached for type = %p = %s \n",type,type->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }
#if 1
  // DQ (3/27/2007): Added support for previously computed and stored references to types.
     if (type->get_ref_to() != NULL)
        {
          nodeList.insert(type->get_ref_to());
       // addAssociatedNodes(type->get_ref_to(),nodeList,markMemberNodesDefinedToBeDeleted);
          ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
        }

  // DQ (3/27/2007): Added support for previously computed and stored references to types.
     if (type->get_ptr_to() != NULL)
        {
          nodeList.insert(type->get_ptr_to());
       // addAssociatedNodes(type->get_ptr_to(),nodeList,markMemberNodesDefinedToBeDeleted);
          ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
        }
#else
     printf ("This causes a segmentation fault, but it should be turned on, I think! \n");
#endif

     if (type->get_modifiers() != NULL)
        {
          nodeList.insert(type->get_modifiers());
          ROSE_ASSERT(finalDeleteSet.find(type->get_modifiers()) == finalDeleteSet.end());
          ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
        }

  // These are deleted as part of the parent type (never shared)
  // We want to selectively mark them because we want to graph them but never delete them!
     if (markMemberNodesDefinedToBeDeleted == true )
        {
          nodeList.insert(type->get_typedefs());
          ROSE_ASSERT(finalDeleteSet.find(type->get_typedefs()) == finalDeleteSet.end());
          ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());
        }

     ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());

  // printf ("Leaving addAssociatedNodes(SgType* type): nodeList.size() = %ld \n",nodeList.size());
   }

void
addAssociatedNodes ( SgExpression* expression, set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted )
   {
  // Just call the AST traversal base mechanism to collect the expressions
  // set<SgNode*> requiredNodes = buildRequiredNodeList(expression);

  // Build a local traversal of the AST
     class RequiredIRNodes
        : public SgSimpleProcessing
        {
       // The IR nodes accumulated to requiredNodesList will be emilinated from the delete list (if there is overlap)
          public:
            // list of nodes required in AST (these will be eliminated from the delete list)
               std::set<SgNode*> requiredNodesSet;

          public:
               RequiredIRNodes() {};

           //! Required traversal function
               void visit (SgNode* node) 
                  {
                 // collect all IR nodes visited
                    requiredNodesSet.insert(node);
                  }
        };

     RequiredIRNodes t;
     t.traverse(expression,preorder);

  // Insert the list of associated expressions into the nodeList
     nodeList.insert(t.requiredNodesSet.begin(),t.requiredNodesSet.end());
  // printf ("Skipping the handling of expressions in addAssociatedNodes ( SgExpression* expression ) \n");

     ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());

  // printf ("Leaving addAssociatedNodes(SgExpresion* type): nodeList.size() = %ld \n",nodeList.size());
   }


void
addAssociatedNodes ( SgNode* node, set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted, SgNode* matchingNodeInMergedAST /* default argument */ )
   {
  // Take the input list and add any additional IR nodes that should be associated 
  // (but not have been in the AST traversal used to build the initial (input) list).

  // DQ (11/3/2012): Added assertion.
     ROSE_ASSERT(node != NULL);

  // This list will be merged with the input list (returned by reference).
  // set<SgNode*> nodeList;
#if 0
     Sg_File_Info* fileInfo = node->get_file_info();
     printf ("addAssociatedNodes(): node = %p = %s = %s = %s matchingNodeInMergedAST = %p at file = %s \n",
          node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),SageInterface::generateUniqueName(node,false).c_str(),matchingNodeInMergedAST,(fileInfo != NULL) ? fileInfo->get_raw_filename().c_str() : "NULL");
#endif

  // Include the current IR node in this set of associated IR nodes
  // (to avoid having to add it explicitly before or after this function call).
     nodeList.insert(node);
     if (finalDeleteSet.find(node) != finalDeleteSet.end())
        {
          Sg_File_Info* fileInfo = node->get_file_info();
          printf ("addAssociatedNodes(): adding previously deleted IR node to nodeList node = %p = %s = %s at file = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),(fileInfo != NULL) ? fileInfo->get_raw_filename().c_str() : "NULL");
          if (fileInfo != NULL)
               fileInfo->display("addAssociatedNodes(): adding previously deleted IR node to nodeList: debug");
        }
     ROSE_ASSERT(finalDeleteSet.find(node) == finalDeleteSet.end());

     if (isSgExpression(node) != NULL)
        {
          return;
        }

     if (isSgJavaMemberValuePair(node) != NULL)
        {
          return;
        }

  // DQ (1/20/2007): Some IR nodes should force related nodes to be removed (SgFunctionSymbol IR nodes, etc.)
     switch(node->variantT())
        {
       // DQ (9/1/2012): The template function and member function declaration is derived from the SgFunctionDeclaration.
          case V_SgTemplateFunctionDeclaration:
          case V_SgTemplateMemberFunctionDeclaration:

          case V_SgFunctionDeclaration:
          case V_SgProgramHeaderStatement:
          case V_SgProcedureHeaderStatement:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
             // This is the function declaration that will be used in the merged AST!
             // We want to use it to get the symbol so that it can also be shared!
             // printf ("##### Don't forget the share the symbols in the symbol tables #####\n");
             // ROSE_ASSERT(isSgFunctionDeclaration(matchingNodeInMergedAST) != NULL);

               SgFunctionDeclaration*       functionDeclaration       = isSgFunctionDeclaration(node);
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(node);

               ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);
               nodeList.insert(functionDeclaration->get_parameterList());

            // Add associated nodes so that they will be excluded as well
               SgFunctionType* functionType = functionDeclaration->get_type();
               ROSE_ASSERT(functionType != NULL);
#if 0
               printf ("addAssociatedNodes(): functionDeclaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),SageInterface::get_name(functionDeclaration).c_str());
               printf ("addAssociatedNodes(): functionDeclaration = %p firstNondefiningDeclaration = %p definingDeclaration = %p \n",functionDeclaration,functionDeclaration->get_firstNondefiningDeclaration(),functionDeclaration->get_definingDeclaration());
#endif
#if 0
               printf ("addAssociatedNodes(): functionDeclaration mangled name = %s matchingNodeInMergedAST = %p \n",functionDeclaration->get_mangled_name().str(),matchingNodeInMergedAST);
#endif
#if 0
               functionDeclaration->get_startOfConstruct()->display("addAssociatedNodes(): functionDeclaration");
#endif
               addAssociatedNodes(functionType,nodeList,markMemberNodesDefinedToBeDeleted);

            // printf ("addAssociatedNodes(): functionType = %p = %s = %s \n",functionType,functionType->class_name().c_str(),SageInterface::get_name(functionType).c_str());

            // Initially the list is empty so we can't assert this!
            // ROSE_ASSERT(finalDeleteSet.empty() == false);
               ROSE_ASSERT(finalDeleteSet.find(functionDeclaration) == finalDeleteSet.end());
               ROSE_ASSERT(finalDeleteSet.find(functionType) == finalDeleteSet.end());

               ROSE_ASSERT(functionType != NULL);
            // printf ("functionType->get_return_type()->get_mangled() = %s \n",functionType->get_return_type()->get_mangled().str());
            // printf ("functionType->get_mangled_type() = %s \n",functionType->get_mangled().str());

            // Include the function type from the function type table
            // nodeList.insert(SgNode::get_globalFunctionTypeTable()->lookup_function_type(functionType->get_mangled()));
               ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
               ROSE_ASSERT(SgNode::get_globalFunctionTypeTable()->get_function_type_table() != NULL);
            // SgFunctionTypeSymbol* functionTypeSymbol = SgNode::get_globalFunctionTypeTable()->get_function_type_table()->findfunctype(functionType->get_mangled());
               SgFunctionTypeSymbol* functionTypeSymbol = SgNode::get_globalFunctionTypeTable()->get_function_type_table()->find_function_type(functionType->get_mangled_type());

            // DQ (2/6/2007): Some builtin functions (e.g. __builtin_expect()) generate a function type that can't be found
               if (functionTypeSymbol == NULL)
                  {
#if 0
                 // DQ (10/20/2012): Commented out this output spew, not all functions require a definition (see test2012_57.c, for example).
                    printf ("Note: no function type was found for functionDeclaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),SageInterface::get_name(functionDeclaration).c_str());
#if 1
                    printf ("Error: functionType->get_mangled() = %s not found in symbol table \n",functionType->get_mangled().str());
                    printf ("functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());
                    printf ("Location of problem function declaration: \n");
                    functionDeclaration->get_startOfConstruct()->display("Location of problem function declaration: debug");
                    printf ("SgNode::get_globalFunctionTypeTable() = %p get_function_type_table()->size() = %d \n",SgNode::get_globalFunctionTypeTable(),SgNode::get_globalFunctionTypeTable()->get_function_type_table()->size());
#endif
#if 0
                 // DQ (6/8/2010): Commented out as a test...
                    ROSE_ASSERT(functionDeclaration->get_startOfConstruct()->isCompilerGenerated() == true);
#else
                    printf ("Commented out test (assertion) of functionDeclaration->get_startOfConstruct()->isCompilerGenerated() == true \n");
#endif
#endif
                  }
                 else
                  {
                 // DQ (2/3/2007): Member function types are not put into the global function type table
                    ROSE_ASSERT(functionTypeSymbol != NULL);
                    nodeList.insert(functionTypeSymbol);
                  }

            // Don't process the SgFunctionSymbol unless we are referencing different scopes
               if (memberFunctionDeclaration != NULL)
                  {
                 // If this is a member function then we have a CtorInitializerList
                    ROSE_ASSERT(memberFunctionDeclaration->get_CtorInitializerList() != NULL);
                 // nodeList.insert(memberFunctionDeclaration->get_CtorInitializerList());

                 // DQ (2/17/2007): We have to visit the list if SgInitializedName objects!
                    addAssociatedNodes(memberFunctionDeclaration->get_CtorInitializerList(),nodeList,markMemberNodesDefinedToBeDeleted);

                 // DQ (2/18/2007): Add the template declaration
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionDeclaration);
                    if (templateInstantiationMemberFunctionDeclaration != NULL)
                       {
#if 0
                         ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration() != NULL);
                         addAssociatedNodes(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration(),nodeList,markMemberNodesDefinedToBeDeleted);
#else
                      // DQ (11/28/2012): This new version of code allows the associated templateDeclaration to be NULL.
                      // (I think this is required for the newer more general template support, or perhaps it is an error 
                      // that needs to be addessed in the AST construction).
                         if (templateInstantiationMemberFunctionDeclaration->get_templateDeclaration() != NULL)
                            {
                              addAssociatedNodes(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration(),nodeList,markMemberNodesDefinedToBeDeleted);
                            }
                           else
                            {
                              printf ("WARNING: For templateInstantiationMemberFunctionDeclaration = %p templateInstantiationMemberFunctionDeclaration->get_templateDeclaration() == NULL \n", templateInstantiationMemberFunctionDeclaration);
                            }
#endif
                       }
                  }

            // DQ (2/18/2007): Add the template declaration
               SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(functionDeclaration);
               if (templateInstantiationFunctionDeclaration != NULL)
                  {
                 // DQ (11/3/2012): Added assertion.
                    if (templateInstantiationFunctionDeclaration->get_templateDeclaration() == NULL)
                       {
                         printf ("ERROR: templateInstantiationFunctionDeclaration->get_templateDeclaration() == NULL: templateInstantiationFunctionDeclaration = %p \n",templateInstantiationFunctionDeclaration);
                       }
                 // ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateDeclaration() != NULL);

                 // DQ (11/3/2012): This is failing with the new source code position implementation.  Skipp adding nodes that we cant reference...
                 // addAssociatedNodes(templateInstantiationFunctionDeclaration->get_templateDeclaration(),nodeList,markMemberNodesDefinedToBeDeleted);
                    if (templateInstantiationFunctionDeclaration->get_templateDeclaration() != NULL)
                       {
                         addAssociatedNodes(templateInstantiationFunctionDeclaration->get_templateDeclaration(),nodeList,markMemberNodesDefinedToBeDeleted);
                       }
                  }

            // Note that this step might remove a symbol that is shared, but I think not!
               SgScopeStatement* scopeStatement = functionDeclaration->get_scope();
               ROSE_ASSERT(scopeStatement != NULL);
            // printf ("addAssociatedNodes(): Looking for function symbol in scope = %p = %s for %s \n",scopeStatement,scopeStatement->class_name().c_str(),functionDeclaration->get_name().str());
            // SgFunctionSymbol* functionSymbol = scopeStatement->lookup_function_symbol(functionDeclaration->get_name());

            // DQ (2/14/2007): Don't get any symbol, get the exact symbol that we require.
               SgSymbol* symbol = functionDeclaration->get_symbol_from_symbol_table();
               SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
#if 0
               printf ("functionSymbol = %p \n",functionSymbol);
#endif
               if (functionSymbol == NULL)
                  {
                 // This is likely a declaration of a pointer to a function, the type should have been put 
                 // into the global function type symbol table. But there is no associated function symbol 
                 // since the function declaration built to define the function type (required for the 
                 // function pointer) is compiler generated only internally and not externally accessable).
                 // This is also common for template functions defined in template classes (though this may be a bug in ROSE).

                 // It can also be the function declaration that will be deleted from merged AST (and thus us not reference from anywhere in the AST)
                 // This appears to be the case where this message is output after the AST fixup.
#if 0
                    bool inDeleteSet                   = (finalDeleteSet.find(functionDeclaration) != finalDeleteSet.end());
                    bool isDefiningDeclaration         = functionDeclaration == functionDeclaration->get_definingDeclaration();
                    bool isFirstNondefiningDeclaration = functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration();
                    printf ("Note: functionSymbol not found using name for functionDeclaration = %p = %s = %s isDefiningDeclaration = %s isFirstNondefiningDeclaration = %s inDeleteSet = %s \n",
                         functionDeclaration,functionDeclaration->class_name().c_str(),SageInterface::get_name(functionDeclaration).c_str(),
                         isDefiningDeclaration ? "true" : "false",isFirstNondefiningDeclaration ? "true" : "false",inDeleteSet ? "true" : "false");
                 // functionDeclaration->get_startOfConstruct()->display("error: debug");
#endif
                 // DQ (2/3/2007): Temp code (but I am sure we can't leave this in place!)
                 // ROSE_ASSERT(false);
                  }
                 else
                  {
                 // DQ (2/7/2007): Add the functionSymbol to the list
                    nodeList.insert(functionSymbol);

#ifndef NDEBUG
                 // SgSymbolTable* scopeSymbolTable = scopeStatement->get_symbol_table();

                 // ROSE_ASSERT(scopeSymbolTable != NULL);
                 // ROSE_ASSERT(scopeSymbolTable->get_table() != NULL);
#endif
#if 0
                    printf ("scopeStatement = %p scopeSymbolTable = %p \n",scopeStatement,scopeSymbolTable);
#endif
                 // This is the function declaration that will be used in the merged AST!
                 // We want to use it to get the symbol so that the symbol can also be shared!
                 // if (matchingNodeInMergedAST != NULL)
                    SgFunctionDeclaration* matchingNodeInMergedAST_functionDeclaration = isSgFunctionDeclaration(matchingNodeInMergedAST);
                    if (matchingNodeInMergedAST_functionDeclaration != NULL && functionDeclaration->get_scope() != matchingNodeInMergedAST_functionDeclaration->get_scope())
                       {
                      // DQ (1/24/2007): New support in the symbol table for removal of entries.
                      // printf ("Removing the associated function symbol from scopeSymbolTable = %p \n",scopeSymbolTable);
                      // scopeSymbolTable->remove(functionDeclaration->get_name());
                         SgSymbol* symbol = functionDeclaration->get_symbol_from_symbol_table();
                         if (symbol == NULL)
                            {
                              printf ("Error: symbol not found in symbol table for %s \n",functionDeclaration->get_name().str());
                              functionDeclaration->get_startOfConstruct()->display("Error: functionDeclaration located at:");
                              printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                           // SageInterface::outputLocalSymbolTables(classDeclaration->get_scope());
                              functionDeclaration->get_scope()->get_symbol_table()->print("Error: symbol not found in symbol table");
                              printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                            }
                         ROSE_ASSERT(symbol != NULL);
                         ROSE_ASSERT(isSgFunctionSymbol(symbol) != NULL);
                         ROSE_ASSERT(functionDeclaration->get_symbol_from_symbol_table() != NULL);
#if 0
                         printf ("remove symbol: functionDeclaration->get_symbol_from_symbol_table() = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
#if 0
                         printf ("Skipping scopeSymbolTable->remove(symbol = %p = %s); \n",symbol,symbol->class_name().c_str());
#endif
                      // scopeSymbolTable->remove(symbol);

                      // ROSE_ASSERT(functionDeclaration->get_symbol_from_symbol_table() != NULL);
                      // scopeSymbolTable->remove(functionDeclaration->get_symbol_from_symbol_table());

                         ROSE_ASSERT(isSgFunctionDeclaration(matchingNodeInMergedAST) != NULL);
                         SgFunctionDeclaration* sharedFunctionDeclaration = isSgFunctionDeclaration(matchingNodeInMergedAST);
                         SgScopeStatement* sharedFunctionScope = isSgScopeStatement(sharedFunctionDeclaration->get_scope());

                      // printf ("get the symbol for the shared function = %s \n",sharedFunctionDeclaration->get_name().str());
                         ROSE_ASSERT(sharedFunctionScope != NULL);
                      // printf ("sharedFunctionScope = %p = %s \n",sharedFunctionScope,sharedFunctionScope->class_name().c_str());
                      // SgFunctionSymbol* sharedFunctionSymbol = sharedFunctionScope->lookup_function_symbol(sharedFunctionDeclaration->get_name());
                         SgSymbol* sharedSymbol = sharedFunctionDeclaration->get_symbol_from_symbol_table();
                         SgFunctionSymbol* sharedFunctionSymbol = isSgFunctionSymbol(sharedSymbol);
                         if (sharedFunctionSymbol != NULL)
                            {
#if 0
                              printf ("insert symbol: sharedFunctionDeclaration->get_symbol_from_symbol_table() = %p = %s \n",
                                   sharedFunctionSymbol,sharedFunctionSymbol->class_name().c_str());
#endif
                              SgSymbolTable* sharedScopeSymbolTable = sharedFunctionScope->get_symbol_table();

                              ROSE_ASSERT(sharedScopeSymbolTable != NULL);
                              ROSE_ASSERT(sharedScopeSymbolTable->get_table() != NULL);

                           // Insert the shared symbol
#if 0
                              printf ("Insert symbol %p into symbol table %p in scope = %p = %s \n",
                                   sharedFunctionSymbol,sharedFunctionScope->get_symbol_table(),sharedFunctionScope,sharedFunctionScope->class_name().c_str());
#endif
                           // ROSE_ASSERT(sharedFunctionScope->symbol_exists(sharedFunctionSymbol) == false);
                              if (sharedFunctionScope->symbol_exists(sharedFunctionSymbol) == false)
                                 {
#if 1
                                // DQ (7/4/2010): Test use of alternative approach (ignoring some types of symbols...
                                   bool symbolAlreadyPresentLookupName = scopeStatement->symbol_exists(sharedFunctionDeclaration->get_name(),sharedFunctionSymbol);
                                   if (symbolAlreadyPresentLookupName == true)
                                      {
                                     // The symbol is not present but the name associated with the symbol is present.
                                     // This can happend for function symbols such as "__default_function_pointer_name" 
                                     // used to consistantly handle function pointers.

                                        printf ("AST MERGE Function symbol handling in addAssociatedNodes(): Ignoring case of sharedFunctionDeclaration->get_name() = %s \n",sharedFunctionDeclaration->get_name().str());
                                      }
                                     else
                                      {
                                     // sharedFunctionScope->insert_symbol(sharedFunctionDeclaration->get_name(),sharedFunctionSymbol);
                                        scopeStatement->insert_symbol(sharedFunctionDeclaration->get_name(),sharedFunctionSymbol);

                                     // Reset the parent of the symbol (for member functions the versions form either file being merged might be used 
                                     // (because the class definitions may be merged ahead of the member functions and thus the symbols can have a 
                                     // parent set to the symbol table that is eliminated).
                                        sharedFunctionSymbol->set_parent(sharedScopeSymbolTable);
                                      }
#else
                                // DQ (7/4/2010): This is an error for the test-read-medium makefile rule in astFileIOTests.
                                // sharedFunctionScope->insert_symbol(sharedFunctionDeclaration->get_name(),sharedFunctionSymbol);
                                   scopeStatement->insert_symbol(sharedFunctionDeclaration->get_name(),sharedFunctionSymbol);

                                // Reset the parent of the symbol (for member functions the versions form either file being merged might be used 
                                // (because the class definitions may be merged ahead of the member functions and thus the symbols can have a 
                                // parent set to the symbol table that is eliminated).
                                   sharedFunctionSymbol->set_parent(sharedScopeSymbolTable);
#endif
                                 }
                                else
                                 {
#if 0
                                   printf ("Note: sharedFunctionSymbol = %p sharedFunctionDeclaration %p = %s exists in the symbol table already \n",
                                        sharedFunctionSymbol,sharedFunctionDeclaration,sharedFunctionDeclaration->get_name().str());
#endif
                                 }
                            }
                           else
                            {
#if 0
                              printf ("Note: sharedFunctionSymbol == NULL: sharedFunctionDeclaration->get_name() = %s \n",sharedFunctionDeclaration->get_name().str());
#endif
                           // sharedFunctionDeclaration->get_startOfConstruct()->display("Error sharedFunctionDeclaration located at:");
                            }
                      // ROSE_ASSERT(sharedFunctionSymbol != NULL);

                      // PC (10/7/2009): merge definingDeclaration pointers
                           if (matchingNodeInMergedAST_functionDeclaration->get_definingDeclaration() == NULL)
                              {
                                matchingNodeInMergedAST_functionDeclaration->set_definingDeclaration(functionDeclaration->get_definingDeclaration());
                              }
                       }

                    nodeList.insert(functionSymbol);
                    ROSE_ASSERT(finalDeleteSet.find(functionSymbol) == finalDeleteSet.end());
                  }

               nodeList.insert(functionType);
               break;
             }

          case V_SgInitializedName:
             {
               SgInitializedName* initializedName = isSgInitializedName(node);
            // This function returns by reference (unusual for ROSE, adn this should be fixed to be uniform).
               nodeList.insert(&(initializedName->get_storageModifier()));
               if (initializedName->get_initptr() != NULL)
                  {
                    nodeList.insert(initializedName->get_initptr());
                    addAssociatedNodes(initializedName->get_initptr(),nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               nodeList.insert(initializedName->get_type());
               addAssociatedNodes(initializedName->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);

               ROSE_ASSERT(finalDeleteSet.find(initializedName->get_type()) == finalDeleteSet.end());

            // DQ (2/17/2007): This appears to be a problem for orphaned IR nodes!
               if (initializedName->get_scope() == NULL)
                  {
                    printf ("Error: initializedName->get_scope() == NULL name = %p = %s at: \n",initializedName,initializedName->get_name().str());
                    initializedName->get_file_info()->display("Error: initializedName->get_scope() == NULL");

                 // DQ (4/15/2010): Added return to support debugging...must be removed...
                    return;
                  }
               ROSE_ASSERT(initializedName->get_scope() != NULL);

            // DQ (3/2/2014): This might be a SgInitializedName that is a parameter to a non-defining 
            // function declaration that was not deleted when we deleted the AST.  If so then this 
            // will be NULL. See tests/nonsmoke/functional/roseTests/astSnippetTests/specimen2014_03.c.
               if (initializedName->get_scope()->get_symbol_table() == NULL)
                  {
                    printf ("initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
                    printf ("initializedName->get_scope() = %p = %s \n",initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());

                 // DQ (3/2/2014): Check for a previously delete IR node as part of testing the snippet injection mechanism.
                    if (initializedName->get_scope()->variantT() == V_SgNode)
                       {
                      // This is a previously deleted IR node and we need not track down associated node for it.
                         printf ("Warning: This is a previously deleted IR node and we need not track down associated node for it (snippet injection support). \n");
                         return;
                       }

                    initializedName->get_file_info()->display("error: debug");
                  }
               ROSE_ASSERT(initializedName->get_scope()->get_symbol_table() != NULL);

            // DQ (2/16/2007): Get the exact symbol we are looking for not just one with the same name!
            // SgSymbol* symbol = scopeStatement->lookup_variable_symbol(initializedName->get_name());
               SgSymbol* symbol = initializedName->get_symbol_from_symbol_table();

               if (symbol != NULL)
                  {
                    SgVariableSymbol*  variableSymbol = isSgVariableSymbol(symbol);
                    SgEnumFieldSymbol* enumFieldSymbol = isSgEnumFieldSymbol(symbol);

                 // DQ (2/2/2011): Added support for SgLabelSymbol which can also have a SgInitializedName 
                 // IR node as a child (and thus be the symbol associated with a SgInitializedName).
                    SgLabelSymbol*     labelSymbol     = isSgLabelSymbol(symbol);

                 // if (variableSymbol == NULL && enumFieldSymbol == NULL)
                    if (variableSymbol == NULL && enumFieldSymbol == NULL && labelSymbol == NULL)
                       {
                         printf ("symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                         initializedName->get_startOfConstruct()->display("Error: initializedName located at");
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                         SageInterface::outputLocalSymbolTables(initializedName->get_scope());
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                       }
                 // ROSE_ASSERT(variableSymbol != NULL || enumFieldSymbol != NULL);
                    ROSE_ASSERT(variableSymbol != NULL || enumFieldSymbol != NULL || labelSymbol != NULL);

                    nodeList.insert(symbol);
                    ROSE_ASSERT(finalDeleteSet.find(symbol) == finalDeleteSet.end());
                  }
                 else
                  {
                    SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(initializedName->get_parent());
                    if (functionParameterList != NULL)
                       {
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionParameterList->get_parent());
                         ROSE_ASSERT (functionDeclaration != NULL);
                         if (functionDeclaration == functionDeclaration->get_definingDeclaration())
                            {
                           // This happens in examples such as: "inline bool __check_singular_aux(const void*) { return false; }" (from debug.h in the g++ system header files)
#if 0
                              printf ("No symbol found for initializedName = \"%s\" in parameter list of defining declaration = %p = %s \n",
                                   initializedName->get_name().str(),functionDeclaration,functionDeclaration->get_name().str());
#endif
                           // functionDeclaration->get_file_info()->display("No symbol found for initializedName in parameter list of defining declaration");
                            }
                       }
                      else
                       {
                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedName->get_parent());
                         if (variableDeclaration != NULL)
                            {
#if 0
                              bool scopesMatch = initializedName->get_scope() == variableDeclaration->get_scope();
                           // DQ (10/22/2016): Suppress this output because it is a moderate issue in the mergeAST_tests directory.
                              printf ("No symbol found for initializedName = %s in SgVariableDeclaration = %s \n",initializedName->get_name().str(),scopesMatch ? "true" : "false");
#endif
                            }
                           else
                            {
                           // SgInitializedName object in SgCtorInitializerList are not put into symbol tables so avoid output of messages about them.
                              SgCtorInitializerList* ctorList = isSgCtorInitializerList(initializedName->get_parent());
                              if (ctorList == NULL)
                                 {
#if PRINT_DEVELOPER_WARNINGS
                                // DQ (10/22/2007): This only traps cases such as the use of "__PRETTY_FUNCTION__" (which is compiler generated)
                                   printf ("No symbol found for initializedName = %s parent = %s \n",initializedName->get_name().str(),initializedName->get_parent()->class_name().c_str());
#endif
                                 }
                            }
                       }
                  }

            // DQ (2/17/2007): Make sure that these are from different scopes!
            // Handle the associated SgVariableSymbol (do we have to verify this is part of a declaration?)
            // if (matchingNodeInMergedAST != NULL)
               SgInitializedName* matchingNodeInMergedAST_initializedName = isSgInitializedName(matchingNodeInMergedAST);
               if (matchingNodeInMergedAST_initializedName != NULL && initializedName->get_scope() != matchingNodeInMergedAST_initializedName->get_scope())
                  {
                    SgScopeStatement* scopeStatement = initializedName->get_scope();
                    ROSE_ASSERT(scopeStatement != NULL);
                 // printf ("addAssociatedNodes(): Looking for function symbol in scope = %p = %s for %s \n",scopeStatement,scopeStatement->class_name().c_str(),initializedName->get_name().str());

                 // SgSymbolTable* scopeSymbolTable = scopeStatement->get_symbol_table();

                 // DQ (1/24/2007): New support in the symbol table for removal of entries.
                 // printf ("Removing the associated enum symbol from scopeSymbolTable = %p \n",scopeSymbolTable);
                 // scopeSymbolTable->remove(initializedName->get_name());
                    SgSymbol* symbol = initializedName->get_symbol_from_symbol_table();

                 // Note that named function parameters are not in the symbol table so this is sometime NULL
                    if (symbol == NULL)
                       {
                      // Skip output of message for case where we know that the symbol is not in the symbol table.
                         if (isSgFunctionParameterList(initializedName->get_parent()) == NULL && isSgCtorInitializerList(initializedName->get_parent()) == NULL)
                              printf ("Error: symbol not found in symbol table for %s parent = %p = %s \n",initializedName->get_name().str(),initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
#if 0
                         initializedName->get_startOfConstruct()->display("Error: functionDeclaration located at: debug");
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                      // SageInterface::outputLocalSymbolTables(classDeclaration->get_scope());
                         initializedName->get_scope()->get_symbol_table()->print("Error: symbol not found in symbol table");
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
#endif
                       }
                      else
                       {
                         ROSE_ASSERT(symbol != NULL);
                      // ROSE_ASSERT(isSgVariableSymbol(symbol) != NULL);
                      // ROSE_ASSERT(initializedName->get_symbol_from_symbol_table() != NULL);

                         if (SgProject::get_verbose() > 0)
                              printf ("Skipping scopeSymbolTable->remove(symbol = %p = %s); \n",symbol,symbol->class_name().c_str());

                      // scopeSymbolTable->remove(symbol);
                      // ROSE_ASSERT(initializedName->get_symbol_from_symbol_table() != NULL);
                      // scopeSymbolTable->remove(initializedName->get_symbol_from_symbol_table());
                       }

                    ROSE_ASSERT(isSgInitializedName(matchingNodeInMergedAST) != NULL);
                    SgInitializedName* sharedInitializedName = isSgInitializedName(matchingNodeInMergedAST);
                    SgScopeStatement* sharedInitializedNameScope = isSgScopeStatement(sharedInitializedName->get_scope());

                 // printf ("get the symbol for the shared initializedName = %s \n",sharedInitializedName->get_name().str());
                    ROSE_ASSERT(sharedInitializedNameScope != NULL);
                 // printf ("sharedInitializedNameScope = %p = %s \n",sharedInitializedNameScope,sharedInitializedNameScope->class_name().c_str());

                 // DQ (2/16/2007): Get the exact symbol we are looking for not just one with the same name!
                 // SgSymbol* sharedSymbol = sharedInitializedNameScope->lookup_var_symbol(sharedInitializedName->get_name());
                    SgSymbol* sharedSymbol = sharedInitializedName->get_symbol_from_symbol_table();

                 // The case of "void foo(int);" will cause an initialized name to be built but now symbol will be defined, and so sharedSymbol == NULL.
                    if (sharedSymbol != NULL)
                       {
                         SgVariableSymbol* sharedVariableSymbol = isSgVariableSymbol(sharedSymbol);
                      // ROSE_ASSERT(sharedVariableSymbol != NULL);
                         if (sharedVariableSymbol != NULL)
                            {
                              SgSymbolTable* sharedScopeSymbolTable = sharedInitializedNameScope->get_symbol_table();

                              ROSE_ASSERT(sharedScopeSymbolTable != NULL);
                              ROSE_ASSERT(sharedScopeSymbolTable->get_table() != NULL);

                           // Insert the shared symbol
                           // printf ("Insert symbol %p into symbol table %p in scope = %p \n",sharedVariableSymbol,sharedInitializedNameScope->get_symbol_table(),sharedInitializedNameScope);
                              if (scopeStatement->symbol_exists(sharedInitializedName->get_name(),sharedVariableSymbol) == false)
                                 {
                                   scopeStatement->insert_symbol(sharedInitializedName->get_name(),sharedVariableSymbol);
                                 }
                                else
                                 {
                                   printf ("Warning: likely a sharedInitializedName of a Ctor list, but already present in symbol table (symbol table fixed up by previous reference to this SgInitializedName = %p = %s \n",
                                        sharedInitializedName,sharedInitializedName->get_name().str());
                                 }

                           // Reset the parent of the symbol (for member functions the versions form either file being merged might be used 
                           // (because the class definitions may be merged ahead of the member functions and thus the symbols can have a 
                           // parent set to the symbol table that is eliminated).
                              sharedVariableSymbol->set_parent(sharedScopeSymbolTable);
                            }

                      // printf ("Exiting as part of test! \n");
                      // ROSE_ASSERT(false);
                       }
                  }

               break;
             }

       // DQ (11/3/2014): Adding support for templated typedef.
          case V_SgTemplateTypedefDeclaration:

       // DQ (11/5/2014): Adding support for templated typedef.
          case V_SgTemplateInstantiationTypedefDeclaration:

          case V_SgTypedefDeclaration:
             {
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
               ROSE_ASSERT(typedefDeclaration != NULL);

            // ROSE_ASSERT(initializedName->get_storageModifier() != NULL);
            // printf ("In addAssociatedNodes(): Adding typedefDeclaration->get_type() = %p = %s to nodeList \n",typedefDeclaration->get_type(),typedefDeclaration->get_type()->class_name().c_str());
               nodeList.insert(typedefDeclaration->get_type());
               addAssociatedNodes(typedefDeclaration->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);

            // printf ("In addAssociatedNodes(): Adding typedefDeclaration->get_base_type() = %p = %s to nodeList \n",typedefDeclaration->get_base_type(),typedefDeclaration->get_base_type()->class_name().c_str());
               nodeList.insert(typedefDeclaration->get_base_type());
               addAssociatedNodes(typedefDeclaration->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);

               SgScopeStatement* scopeStatement = typedefDeclaration->get_scope();
               ROSE_ASSERT(scopeStatement != NULL);
            // printf ("addAssociatedNodes(): Looking for typedef symbol in scope = %p for %s \n",scopeStatement,typedefDeclaration->get_name().str());

            // DQ (1/30/2007): We must use the lookup_typedef_symbol() member function since there
            // could be more than one type of symbol with a specific name (see test2007_24.C).
            // SgSymbol* symbol = scopeStatement->lookup_symbol(typedefDeclaration->get_name());
            // SgSymbol* symbol = scopeStatement->lookup_typedef_symbol(typedefDeclaration->get_name());
            // DQ (2/14/2007): Don't get any symbol, get the exact symbol that we require.
               SgSymbol* symbol = typedefDeclaration->get_symbol_from_symbol_table();

               if (symbol != NULL)
                  {
                    SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                    if (typedefSymbol == NULL)
                       {
                         printf ("symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                         typedefDeclaration->get_startOfConstruct()->display("Error: typedefDeclaration located at");
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                         SageInterface::outputLocalSymbolTables(typedefDeclaration->get_scope());
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                       }
                    ROSE_ASSERT(typedefSymbol != NULL);

                 // Also add the associated SgTypedefSymbol
                    nodeList.insert(typedefSymbol);
                    ROSE_ASSERT(finalDeleteSet.find(typedefSymbol) == finalDeleteSet.end());
               // }

                 // DQ (2/19/2007): Do we require this?
                 // if (matchingNodeInMergedAST != NULL)
                    SgTypedefDeclaration* matchingNodeInMergedAST_typedefDeclaration = isSgTypedefDeclaration(matchingNodeInMergedAST);
                    if (matchingNodeInMergedAST_typedefDeclaration != NULL && typedefDeclaration->get_scope() != matchingNodeInMergedAST_typedefDeclaration->get_scope())
                       {
                      // SgSymbolTable* scopeSymbolTable = scopeStatement->get_symbol_table();

#if 0
                      // DQ (1/24/2007): New support in the symbol table for removal of entries.
                         SgTypedefSymbol* oldTypedefSymbol = isSgTypedefSymbol(typedefDeclaration->get_symbol_from_symbol_table());
                         printf ("addAssociatedNodes(): Removing the associated typedef symbol = %p from scopeSymbolTable = %p for %s \n",oldTypedefSymbol,scopeSymbolTable,typedefDeclaration->get_name().str());
                         printf ("typedefDeclaration->get_scope() = %p = %s \n",typedefDeclaration->get_scope(),typedefDeclaration->get_scope()->class_name().c_str());
#endif
                         ROSE_ASSERT(typedefDeclaration->get_symbol_from_symbol_table() != NULL);


                         if (SgProject::get_verbose() > 0)
                              printf ("Skipping scopeSymbolTable->remove(symbol = %p = %s); \n",symbol,symbol->class_name().c_str());

                      // scopeSymbolTable->remove(typedefDeclaration->get_symbol_from_symbol_table());

                         ROSE_ASSERT(isSgTypedefDeclaration(matchingNodeInMergedAST) != NULL);
                         SgTypedefDeclaration* sharedTypedefDeclaration = isSgTypedefDeclaration(matchingNodeInMergedAST);
                         SgScopeStatement* sharedTypedefScope = isSgScopeStatement(sharedTypedefDeclaration->get_scope());

                      // printf ("get the symbol for the shared typedef = %s \n",sharedTypedefDeclaration->get_name().str());
                         ROSE_ASSERT(sharedTypedefScope != NULL);
                      // printf ("sharedTypedefScope = %p = %s \n",sharedTypedefScope,sharedTypedefScope->class_name().c_str());

                      // DQ (2/16/2007): Get the exact symbol we are looking for not just one with the same name!
                      // SgSymbol* sharedSymbol = sharedTypedefScope->lookup_typedef_symbol(sharedTypedefDeclaration->get_name());
                         SgSymbol* sharedSymbol = sharedTypedefDeclaration->get_symbol_from_symbol_table();

                         if (sharedSymbol != NULL)
                            {
                              SgTypedefSymbol* sharedTypedefSymbol = isSgTypedefSymbol(sharedSymbol);
                              ROSE_ASSERT(sharedTypedefSymbol != NULL);

                              SgSymbolTable* sharedScopeSymbolTable = sharedTypedefScope->get_symbol_table();

                              ROSE_ASSERT(sharedScopeSymbolTable != NULL);
                              ROSE_ASSERT(sharedScopeSymbolTable->get_table() != NULL);

                           // Insert the shared symbol
                           // printf ("Insert sharedTypedefSymbol %p into symbol table %p in scope = %p \n",sharedTypedefSymbol,sharedTypedefScope->get_symbol_table(),sharedTypedefScope);
                              scopeStatement->insert_symbol(sharedTypedefDeclaration->get_name(),sharedTypedefSymbol);

                           // Reset the parent of the symbol (for member functions the versions form either file being merged might be used 
                           // (because the class definitions may be merged ahead of the member functions and thus the symbols can have a 
                           // parent set to the symbol table that is eliminated).
                              sharedTypedefSymbol->set_parent(sharedScopeSymbolTable);
                            }
                           else
                            {
                              printf ("Note that sharedTypedefScope->lookup_typedefsymbol(sharedTypedefDeclaration->get_name()) == NULL \n");
                            }
                       }
#if 1
                  }
                 else
                  {
                 // printf ("Note that scopeStatement->lookup_typedef_symbol(typedefDeclaration->get_name()) == NULL \n");
                  }
#endif
               break;
             }

          case V_SgFunctionParameterList:
             {
               SgFunctionParameterList* parameterList = isSgFunctionParameterList(node);
               SgInitializedNamePtrList::iterator i = parameterList->get_args().begin();
               while (i != parameterList->get_args().end())
                  {
                    nodeList.insert(*i);
                    addAssociatedNodes(*i,nodeList,markMemberNodesDefinedToBeDeleted);
                    i++;
                  }
               break;
             }

          case V_SgNamespaceDeclarationStatement:
             {
               SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(node);

               nodeList.insert(namespaceDeclaration);

            // DQ (2/20/2009): Added assertion.
               if (namespaceDeclaration->get_parent() == NULL)
                  {
                    printf ("ERROR: namespaceDeclaration->get_parent() == NULL in addAssociatedNodes \n");
                    break;
                  }
               ROSE_ASSERT(namespaceDeclaration->get_parent() != NULL);

               SgScopeStatement* scopeStatement = namespaceDeclaration->get_scope();
               ROSE_ASSERT(scopeStatement != NULL);
            // printf ("addAssociatedNodes(): Looking for typedef symbol in scope = %p for %s \n",scopeStatement,namespaceDeclaration->get_name().str());

            // DQ (2/14/2007): Don't get any symbol, get the exact symbol that we require.
            // SgSymbol* symbol = scopeStatement->lookup_namespace_symbol(namespaceDeclaration->get_name());
               SgSymbol* symbol = namespaceDeclaration->get_symbol_from_symbol_table();

               if (symbol != NULL)
                  {
                    ROSE_ASSERT(symbol != NULL);
                    SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);
                    ROSE_ASSERT(namespaceSymbol != NULL);

                 // Also add the associated SgNamespaceSymbol
                    nodeList.insert(namespaceSymbol);
                  }
                 else
                  {
                 // Since namespaces are reintrant, only the first one has an entry in the symbol table 
                 // and there could be many other declarations for the same namespace.
                 // printf ("Note: symbol not found for namespaceDeclaration = %p = %s \n",namespaceDeclaration,SageInterface::get_name(namespaceDeclaration).c_str());
                  }

            // Also include the SgSymbolTable contained in the SgNamespaceDefinitionStatement (if it exists)
               SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(namespaceDeclaration->get_definition());
               if (namespaceDefinition != NULL)
                  {
                 // DQ (5/21/2013): We want to restrict the access to the symbol table so that we can support new namespace symbol table handling.
                 // This function was added to as a friend function to the SgScopeStatment IR node to support the AST merge capability.
                    SgSymbolTable* scopeSymbolTableFromNamespace = namespaceDefinition->get_symbol_table();
                    nodeList.insert(scopeSymbolTableFromNamespace);
                  }

               if (matchingNodeInMergedAST != NULL)
                  {
                 // DQ (1/24/2007): New support in the symbol table for removal of entries.
                 // SgSymbolTable* scopeSymbolTable = scopeStatement->get_symbol_table();
                 // printf ("Removing the associated typedef symbol from scopeSymbolTable = %p \n",scopeSymbolTable);
                    ROSE_ASSERT(namespaceDeclaration->get_symbol_from_symbol_table() != NULL);

                    if (SgProject::get_verbose() > 0)
                         printf ("Skipping scopeSymbolTable->remove(symbol = %p = %s); \n",symbol,symbol->class_name().c_str());

                 // scopeSymbolTable->remove(namespaceDeclaration->get_symbol_from_symbol_table());

                    ROSE_ASSERT(isSgNamespaceDeclarationStatement(matchingNodeInMergedAST) != NULL);
                    SgNamespaceDeclarationStatement* sharedNamespaceDeclaration = isSgNamespaceDeclarationStatement(matchingNodeInMergedAST);
                    SgScopeStatement* sharedNamespaceScope = isSgScopeStatement(sharedNamespaceDeclaration->get_scope());

                 // printf ("get the symbol for the shared namespace = %s \n",sharedNamespaceDeclaration->get_name().str());
                    ROSE_ASSERT(sharedNamespaceScope != NULL);
                 // printf ("sharedNamespaceScope = %p = %s \n",sharedNamespaceScope,sharedNamespaceScope->class_name().c_str());

                 // DQ (2/16/2007): Get the exact symbol we are looking for not just one with the same name!
                 // SgSymbol* sharedSymbol = sharedNamespaceScope->lookup_namespace_symbol(sharedNamespaceDeclaration->get_name());
                    SgSymbol* sharedSymbol = sharedNamespaceDeclaration->get_symbol_from_symbol_table();

                    ROSE_ASSERT(sharedSymbol != NULL);
                    SgNamespaceSymbol* sharedNamespaceSymbol = isSgNamespaceSymbol(sharedSymbol);
                    ROSE_ASSERT(sharedNamespaceSymbol != NULL);

                    SgSymbolTable* sharedScopeSymbolTable = sharedNamespaceScope->get_symbol_table();

                    ROSE_ASSERT(sharedScopeSymbolTable != NULL);
                    ROSE_ASSERT(sharedScopeSymbolTable->get_table() != NULL);

                 // Insert the shared symbol
                 // printf ("Insert symbol %p into symbol table %p in scope = %p \n",sharedNamespaceSymbol,sharedNamespaceScope->get_symbol_table(),sharedNamespaceScope);
                    scopeStatement->insert_symbol(sharedNamespaceDeclaration->get_name(),sharedNamespaceSymbol);

                 // Reset the parent of the symbol (for member functions the versions form either file being merged might be used 
                 // (because the class definitions may be merged ahead of the member functions and thus the symbols can have a 
                 // parent set to the symbol table that is eliminated).
                    sharedNamespaceSymbol->set_parent(sharedScopeSymbolTable);
                  }
               break;
             }

       // DQ (9/1/2012): The template class declaration is derived from the SgClassDeclaration.
          case V_SgTemplateClassDeclaration:

          case V_SgJavaPackageDeclaration:
          case V_SgClassDeclaration:
          case V_SgDerivedTypeStatement:
          case V_SgJovialTableStatement:
       // DQ (2/10/2007): Added case for SgTemplateInstantiationDecl
          case V_SgTemplateInstantiationDecl:
             {
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
               SgClassDeclaration* matchingNodeInMergedAST_classDeclaration = isSgClassDeclaration(matchingNodeInMergedAST);
#if 0
               printf ("addAssociatedNodes(): classDeclaration = %p = %s = %s in scope = %p = %s matchingNodeInMergedAST = %p \n",
                    classDeclaration,classDeclaration->class_name().c_str(),SageInterface::get_name(classDeclaration).c_str(),
                    classDeclaration->get_scope(),classDeclaration->get_scope()->class_name().c_str(),matchingNodeInMergedAST_classDeclaration);
               classDeclaration->get_startOfConstruct()->display("classDeclaration: debug");

               if (matchingNodeInMergedAST_classDeclaration != NULL)
                    printf ("matchingNodeInMergedAST = %p in scope = %p \n",matchingNodeInMergedAST_classDeclaration,matchingNodeInMergedAST_classDeclaration->get_scope());
#endif
            // This will include classDeclaration->get_type() in the nodeList
               ROSE_ASSERT(classDeclaration->get_type() != NULL);
               addAssociatedNodes(classDeclaration->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);

            // DQ (2/18/2007): Add the template declaration
               SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(classDeclaration);
               if (templateInstantiationDeclaration != NULL)
                  {
                 // DQ (2/28/2015): Only make recursive call when using a valid node.
                 // addAssociatedNodes(templateInstantiationDeclaration->get_templateDeclaration(),nodeList,markMemberNodesDefinedToBeDeleted);
                    if (templateInstantiationDeclaration->get_templateDeclaration() != NULL)
                       {
                         addAssociatedNodes(templateInstantiationDeclaration->get_templateDeclaration(),nodeList,markMemberNodesDefinedToBeDeleted);
                       }
                      else
                       {
                      // DQ (2/28/2015): Make this at least a warning for now.
                         printf ("Warning: templateInstantiationDeclaration->get_templateDeclaration() == NULL \n");
                       }
                  }

            // Might want to traverse the base class list!

               SgScopeStatement* scopeStatement = classDeclaration->get_scope();
               ROSE_ASSERT(scopeStatement != NULL);
#if 0
               printf ("addAssociatedNodes(): Looking for class symbol in scope = %p for classDeclaration = %p = \"%s\" \n",
                    scopeStatement,classDeclaration,classDeclaration->get_name().str());
#endif
            // DQ (2/14/2007): Don't get any symbol, get the exact symbol that we require.
            // SgSymbol* symbol = scopeStatement->lookup_class_symbol(classDeclaration->get_name());
               SgSymbol* symbol = classDeclaration->get_symbol_from_symbol_table();

            // Note that symbol can be NULL if "struct X *Xptr;" did not have a previous declaration of X.
            // In this case we don't want to process this declaration.
            // printf ("addAssociatedNodes(): symbol = %p \n",symbol);
               if (symbol != NULL)
                  {
                    SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                    ROSE_ASSERT(classSymbol != NULL);
#if 0
                    printf ("addAssociatedNodes(): symbol is a SgClassSymbol = %p with declaration = %p = %s = %s \n",
                         classSymbol,classSymbol->get_declaration(),classSymbol->get_declaration()->class_name().c_str(),
                         SageInterface::get_name(classSymbol->get_declaration()).c_str());
                    printf ("classSymbol->get_declaration()->get_definingDeclaration()         = %p \n",classSymbol->get_declaration()->get_definingDeclaration());
                    printf ("classSymbol->get_declaration()->get_firstNondefiningDeclaration() = %p \n",classSymbol->get_declaration()->get_firstNondefiningDeclaration());
#endif
                 // Also add the associated SgTypedefSymbol
                    nodeList.insert(classSymbol);

                 // DQ (2/15/2007): Added extra IR nodes to the list!
                    nodeList.insert(classSymbol->get_declaration());
                 // nodeList.insert(classSymbol->get_declaration()->get_definingDeclaration());
                 // nodeList.insert(classSymbol->get_declaration()->get_firstNondefiningDeclaration());

                 // Don't process the SgClassSymbol unless we are referencing different scopes
                 // if (matchingNodeInMergedAST != NULL)
                 // if (matchingNodeInMergedAST_classDeclaration != NULL && matchingNodeInMergedAST_classDeclaration == matchingNodeInMergedAST_classDeclaration->get_firstNondefiningDeclaration())
                    if (matchingNodeInMergedAST_classDeclaration != NULL && classDeclaration->get_scope() != matchingNodeInMergedAST_classDeclaration->get_scope())
                       {
                      // If we are merging a SgClassDeclaration or SgTemplateInstantiationDecl then there will be typically 
                      // more than one IR nodes associated with each SgClassSymbol, merge then one of then should inable the 
                      // SgClassSymbol in the old AST to be removed.

                      // DQ (5/21/2013): We would like to remove the direct access to the symbol table.
                         SgSymbolTable* scopeSymbolTable = scopeStatement->get_symbol_table();

                      // DQ (1/24/2007): New support in the symbol table for removal of entries.
#if 0
                         printf ("addAssociatedNodes(): Removing the associated class symbol from scopeSymbolTable = %p for classDeclaration = %p = %s \n",
                              scopeSymbolTable,classDeclaration,classDeclaration->get_name().str());
#endif
                         SgSymbol* symbol = classDeclaration->get_symbol_from_symbol_table();
                         if (symbol == NULL)
                            {
#if 0
                              printf ("Error: symbol not found in symbol table for classDeclaration = %p = %s \n",classDeclaration,classDeclaration->get_name().str());
                              printf ("classDeclaration->get_definingDeclaration()                  = %p \n",classDeclaration->get_definingDeclaration());
                              printf ("classDeclaration->get_firstNondefiningDeclaration()          = %p \n",classDeclaration->get_firstNondefiningDeclaration());

                              classDeclaration->get_startOfConstruct()->display("Error: classDeclaration located at:");
                              printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                           // SageInterface::outputLocalSymbolTables(classDeclaration->get_scope());
                              classDeclaration->get_scope()->get_symbol_table()->print("Error: symbol not found in symbol table");
                              printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
#endif
                           // DQ (2/11/2007): We did'nt find the symbol and the classDeclaration == classSymbol->get_declaration(), 
                           // then there must be a problem!
                              ROSE_ASSERT(classDeclaration != classSymbol->get_declaration());
                            }
                           else
                            {
                           // In processing a previous defining or non defining declaration the symbol could have been removed so this is not an error!
                              ROSE_ASSERT(symbol != NULL);
                              ROSE_ASSERT(isSgClassSymbol(symbol) != NULL);
                              ROSE_ASSERT(classDeclaration->get_symbol_from_symbol_table() != NULL);
#if 0
                              printf ("Removing symbol = %p = %s = %s with basis classDeclaration = %p = %s = %s from symbol table %p in scope = %p = %s \n",
                                   symbol,symbol->class_name().c_str(),SageInterface::get_name(symbol).c_str(),
                                   classDeclaration,classDeclaration->class_name().c_str(),SageInterface::get_name(classDeclaration).c_str(),
                                   scopeSymbolTable,scopeStatement,scopeStatement->class_name().c_str());
#endif
                              if (SgProject::get_verbose() > 0)
                                   printf ("Skipping scopeSymbolTable->remove(symbol = %p = %s); \n",symbol,symbol->class_name().c_str());

                           // scopeSymbolTable->remove(symbol);
                            }

                         SgClassDeclaration* sharedClassDeclaration = matchingNodeInMergedAST_classDeclaration;
                         SgScopeStatement* sharedClassScope = isSgScopeStatement(sharedClassDeclaration->get_scope());

                         ROSE_ASSERT(sharedClassScope != NULL);
                         ROSE_ASSERT(sharedClassScope != scopeStatement);
#if 0
                         printf ("get the symbol for the sharedClassDeclaration = %p = %s \n",sharedClassDeclaration,sharedClassDeclaration->get_name().str());
                         printf ("sharedClassDeclaration->get_definingDeclaration()                  = %p \n",sharedClassDeclaration->get_definingDeclaration());
                         printf ("sharedClassDeclaration->get_firstNondefiningDeclaration()          = %p \n",sharedClassDeclaration->get_firstNondefiningDeclaration());
#endif
                         ROSE_ASSERT(sharedClassScope != NULL);
                      // printf ("sharedClassScope = %p = %s \n",sharedClassScope,sharedClassScope->class_name().c_str());

                      // DQ (2/16/2007): Get the exact symbol we are looking for not just one with the same name!
                      // SgSymbol* sharedSymbol = sharedClassScope->lookup_class_symbol(sharedClassDeclaration->get_name());
                         SgSymbol* sharedSymbol = sharedClassDeclaration->get_symbol_from_symbol_table();

                         if (sharedSymbol != NULL)
                            {
                              SgClassSymbol* sharedClassSymbol = isSgClassSymbol(sharedSymbol);
                              ROSE_ASSERT(sharedClassSymbol != NULL);

                              SgSymbolTable* sharedScopeSymbolTable = sharedClassScope->get_symbol_table();

                              ROSE_ASSERT(sharedScopeSymbolTable != NULL);
                              ROSE_ASSERT(sharedScopeSymbolTable->get_table() != NULL);

                           // Insert the shared symbol
                           // printf ("Check if symbol for sharedClassDeclaration %p is already in symbol table in scope = %p \n",sharedClassDeclaration,sharedClassScope);

                           // DQ (2/16/2007): Get the exact symbol we are looking for not just one with the same name!
                           // if (scopeStatement->lookup_class_symbol(sharedClassDeclaration->get_name()) == NULL)
                           // if (sharedClassDeclaration->get_symbol_from_symbol_table() == NULL)

                           // DQ (2/19/2007): We would like to try to avoid calling such an expensive operator!
                           // if (scopeSymbolTable->exists(sharedSymbol) == false)
                              if (scopeSymbolTable->exists(sharedClassDeclaration->get_name(),sharedSymbol) == false)
                                 {
                                // printf ("Insert symbol %p into symbol table %p in scope = %p \n",sharedClassSymbol,sharedClassScope->get_symbol_table(),sharedClassScope);
                                // scopeSymbolTable->insert(sharedClassDeclaration->get_name(),sharedClassSymbol);
                                   scopeStatement->insert_symbol(sharedClassDeclaration->get_name(),sharedClassSymbol);
                                 }

                           // Reset the parent of the symbol (for member functions the versions form either file being merged might be used 
                           // (because the class definitions may be merged ahead of the member functions and thus the symbols can have a 
                           // parent set to the symbol table that is eliminated).
                              sharedClassSymbol->set_parent(sharedScopeSymbolTable);
                            }
                           else
                            {
                              printf ("Note that sharedClassDeclaration->get_symbol_from_symbol_table() == NULL \n");
                            }

                        // PC (10/26/2009): merge definingDeclaration pointers
                           if (matchingNodeInMergedAST_classDeclaration->get_definingDeclaration() == NULL)
                              {
                                matchingNodeInMergedAST_classDeclaration->set_definingDeclaration(classDeclaration->get_definingDeclaration());
                              }

                      // printf ("addAssociatedNodes(): Exiting as part of test! (case V_SgClassDeclaration) \n");
                      // ROSE_ASSERT(false);
                       }
                  }
#if 1
                 else
                  {
                 // This is the interesting case to output a message about.  We might want a post-processing phase on the symbol 
                 // tables that make sure that all symbols reference there first non-defining declaration were available. For classes
                 // the first non-defining declaration should always be avaiable.

                 // This is also the case for class declarations that are to be deleted as part of the AST merge (when reported after the AST fixup).
#if 0
                    bool isDefiningDeclaration         = classDeclaration == classDeclaration->get_definingDeclaration();
                    bool isFirstNondefiningDeclaration = classDeclaration == classDeclaration->get_firstNondefiningDeclaration();
                    if ( isDefiningDeclaration == false ) // && isFirstNondefiningDeclaration == true ||
                       {
                      // These are the more interesting cases since we generally expect that the SgClassSymbol will contain a reference to the firstNondefiningDeclaration.
                         printf ("Note that for classDeclaration = %p get_name() = %s classDeclaration->get_symbol_from_symbol_table() == NULL isDefiningDeclaration = %s isFirstNondefiningDeclaration = %s \n",
                                 classDeclaration,classDeclaration->get_name().str(),isDefiningDeclaration ? "true" : "false",isFirstNondefiningDeclaration ? "true" : "false");
                       }
#endif
#if 0
                    printf ("addAssociatedNodes(): Looking for class symbol in scope = %p for classDeclaration = %p = %s \n",scopeStatement,classDeclaration,classDeclaration->get_name().str());
                    classDeclaration->get_startOfConstruct()->display("Error: enumDeclaration located at:");
                    printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                    SageInterface::outputLocalSymbolTables(classDeclaration->get_scope());
                    printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                    ROSE_ASSERT(false);
#endif
                  }
#endif
               break;
             }
           
          case V_SgEnumDeclaration:
             {
               SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(node);

            // ROSE_ASSERT(initializedName->get_storageModifier() != NULL);
               nodeList.insert(enumDeclaration->get_type());
               addAssociatedNodes(enumDeclaration->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);

               SgScopeStatement* scopeStatement = enumDeclaration->get_scope();
               ROSE_ASSERT(scopeStatement != NULL);
#if 0
               printf ("addAssociatedNodes(): Looking for enum symbol in scope = %p for enumDeclaration = %p = %s matchingNodeInMergedAST = %p \n",
                    scopeStatement,enumDeclaration,enumDeclaration->get_name().str(),matchingNodeInMergedAST);
#endif
            // SgSymbol* symbol = scopeStatement->lookup_enum_symbol(enumDeclaration->get_name());
               SgSymbol* symbol = enumDeclaration->get_symbol_from_symbol_table();

            // DQ (2/7/2007): If this is a post processing phase (after the fixup using the replacement 
            // map then we will have looked into the wrong scope so ignore that we don't find the symbol!)
            // For this reason we might want to communicate what phases the addAssociatedNodes() function 
            // is being used.
               if (symbol == NULL)
                  {
                 // printf ("symbol not found (likely that declaration is in the list that we will removed as part of merge \n");
                 // printf ("addAssociatedNodes(): Looking for enum symbol in scope = %p for enumDeclaration = %p = %s \n",scopeStatement,enumDeclaration,enumDeclaration->get_name().str());
#if 0
                    enumDeclaration->get_startOfConstruct()->display("Error: enumDeclaration located at:");
                    printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                    SageInterface::outputLocalSymbolTables(enumDeclaration->get_scope());
                    printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
#endif
                  }
                 else
                  {
                 // ROSE_ASSERT(symbol != NULL);
                    SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                    ROSE_ASSERT(enumSymbol != NULL);

                 // Also add the associated SgEnumSymbol
                    nodeList.insert(enumSymbol);
                  }

            // DQ (2/11/2007): We don't fixup the symbol table handling of matching symbols in the same scope. This case is similar to "enum{}; enum{};" a somewhat pathological case!
            // if (matchingNodeInMergedAST != NULL)
               if (matchingNodeInMergedAST != NULL && isSgEnumDeclaration(matchingNodeInMergedAST)->get_scope() != scopeStatement)
                  {
                 // These should not be the same of we will screw up the symbol handling
                    ROSE_ASSERT(matchingNodeInMergedAST != enumDeclaration);
                    ROSE_ASSERT(isSgEnumDeclaration(matchingNodeInMergedAST) != NULL);
                    ROSE_ASSERT(isSgEnumDeclaration(matchingNodeInMergedAST)->get_scope() != enumDeclaration->get_scope());

                 // SgSymbolTable* scopeSymbolTable = scopeStatement->get_symbol_table();

                 // DQ (1/24/2007): New support in the symbol table for removal of entries.
                    SgSymbol* removeSymbol = enumDeclaration->get_symbol_from_symbol_table();
#if 0
                    printf ("Removing the associated enum symbol = %p from scopeSymbolTable = %p enumDeclaration = %p = %s \n",removeSymbol,scopeSymbolTable,enumDeclaration,enumDeclaration->get_name().str());
#endif

                 // DQ (5/30/2007): Now that we permit forward enum declaration (even before the defininf declarations),
                 // we have to deal with the symbol having been previously delete from the symbol table when another of the
                 // associated enum declarations processed it.  So we have to allow this to be NULL.
                    if (removeSymbol != NULL)
                       {
                         ROSE_ASSERT(removeSymbol != NULL);

                         if (SgProject::get_verbose() > 0)
                              printf ("Skipping scopeSymbolTable->remove(symbol = %p = %s); \n",symbol,symbol->class_name().c_str());

                      // scopeSymbolTable->remove(removeSymbol);
                       }

                    ROSE_ASSERT(isSgEnumDeclaration(matchingNodeInMergedAST) != NULL);
                    SgEnumDeclaration* sharedEnumDeclaration = isSgEnumDeclaration(matchingNodeInMergedAST);
                    SgScopeStatement* sharedEnumScope = isSgScopeStatement(sharedEnumDeclaration->get_scope());

                 // printf ("get the symbol for the shared enum = %s \n",sharedEnumDeclaration->get_name().str());
                    ROSE_ASSERT(sharedEnumScope != NULL);
                    ROSE_ASSERT(scopeStatement == enumDeclaration->get_scope());
                 // printf ("Take shared symbol from this scope:           sharedEnumScope = %p = %s \n",sharedEnumScope,sharedEnumScope->class_name().c_str());
                 // printf ("Add new symbol from merged AST to this scope: scopeStatement  = %p = %s \n",scopeStatement,scopeStatement->class_name().c_str());
                 // SgSymbol* sharedSymbol = sharedEnumScope->lookup_enum_symbol(sharedEnumDeclaration->get_name());
                    SgSymbol* sharedSymbol = sharedEnumDeclaration->get_symbol_from_symbol_table();

                 // DQ (5/31/2007): Handle the case of a sharedSymbol == NULL (see mergeTest_28.C)
                    if (sharedSymbol != NULL)
                       {
                         ROSE_ASSERT(sharedSymbol != NULL);
                         SgEnumSymbol* sharedEnumSymbol = isSgEnumSymbol(sharedSymbol);
                         ROSE_ASSERT(sharedEnumSymbol != NULL);

                      // SgSymbolTable* sharedScopeSymbolTable = sharedEnumScope->get_symbol_table();
                      // ROSE_ASSERT(sharedScopeSymbolTable != NULL);
                      // ROSE_ASSERT(sharedScopeSymbolTable->get_table() != NULL);

                      // Insert the shared symbol
                      // DQ (2/11/2007): In the case of "enum{}; enum{};" the symbol will have already been put 
                      // into place when the second symbol is evaluated (because the sharing via name mangling 
                      // will pick one matchingNodeInMergedAST from the merged file not one for each declaration 
                      // with the same mangled name!
                      // printf ("Insert symbol %p with declaration = %p into symbol table %p in scope = %p \n",sharedEnumSymbol,sharedEnumSymbol->get_declaration(),sharedEnumScope->get_symbol_table(),sharedEnumScope);

                      // DQ (2/19/2007): We would like to try to avoid calling such an expensive operator!
                      // if (enumDeclaration->get_scope()->symbol_exists(sharedEnumSymbol) == true)
                         if (enumDeclaration->get_scope()->symbol_exists(sharedEnumDeclaration->get_name(),sharedEnumSymbol) == true)
                            {
                              printf ("************** OUTPUT SYMBOL TABLE (enumDeclaration) *************\n");
                              SageInterface::outputLocalSymbolTables(enumDeclaration->get_scope());
                              printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                           // printf ("********** OUTPUT SYMBOL TABLE (sharedEnumDeclaration) ***********\n");
                           // SageInterface::outputLocalSymbolTables(sharedEnumDeclaration->get_scope());
                           // printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                            }
                           else
                            {
                           // DQ (2/19/2007): We would like to try to avoid calling such an expensive operator!
                           // ROSE_ASSERT(scopeStatement->symbol_exists(sharedEnumSymbol) == false);
                           // ROSE_ASSERT(enumDeclaration->get_scope()->symbol_exists(sharedEnumSymbol) == false);

                           // scopeStatement->insert_symbol(sharedEnumDeclaration->get_name(),sharedEnumSymbol);
                              enumDeclaration->get_scope()->insert_symbol(sharedEnumDeclaration->get_name(),sharedEnumSymbol);

                           // Reset the parent of the symbol (for member functions the versions form either file being merged might be used 
                           // (because the class definitions may be merged ahead of the member functions and thus the symbols can have a 
                           // parent set to the symbol table that is eliminated).
                              ROSE_ASSERT(sharedEnumScope->get_symbol_table() != NULL);
                              sharedEnumSymbol->set_parent(sharedEnumScope->get_symbol_table());
                            }
                       }
                      else
                       {
                         printf ("Note that sharedEnumDeclaration->get_symbol_from_symbol_table() == NULL \n");
                       }
                  }
                 else
                  {
                    if (matchingNodeInMergedAST != NULL)
                       {
                         printf ("##### Case of sharing within a file (not yet handled for enum declarations) ##### \n");
                         SgSymbol* removeSymbol = enumDeclaration->get_symbol_from_symbol_table();
                         printf ("Removing removeSymbol = %p for enumDeclaration = %p from symbol table = %p \n",removeSymbol,enumDeclaration,scopeStatement->get_symbol_table());

                         if (SgProject::get_verbose() > 0)
                              printf ("Skipping scopeSymbolTable->remove(removeSymbol = %p = %s); \n",removeSymbol,removeSymbol->class_name().c_str());

                      // scopeStatement->get_symbol_table()->remove(removeSymbol);
                       }
                  }
               break;
             }

       // DQ (9/1/2012): These cases should likely not be the same (any more). Since SgTemplateClassDeclaration is not derived from SgTemplateDeclaration any more in the New EDG connection).
       // DQ (6/11/2011): Added support for new template IR nodes.
       // case V_SgTemplateClassDeclaration:
          case V_SgTemplateDeclaration:
             {
               SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);

            // DQ (9/1/2012): Added assertion.
               ROSE_ASSERT(templateDeclaration != NULL);

            // Template declarations have no associated type (so what is the SgTemplateType for???)!
            // nodeList.insert(templateDeclaration->get_type());
            // addAssociatedNodes(templateDeclaration->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);

               SgScopeStatement* scopeStatement = templateDeclaration->get_scope();
               ROSE_ASSERT(scopeStatement != NULL);
#if 0
               printf ("\n\naddAssociatedNodes(): Looking for template symbol in scope = %p for %p = %s matchingNodeInMergedAST = %p \n",
                    scopeStatement,templateDeclaration,templateDeclaration->get_name().str(),matchingNodeInMergedAST);
#endif
            // DQ (2/14/2007): Don't get any symbol, get the exact symbol that we require.
            // SgSymbol* symbol = scopeStatement->lookup_template_symbol(templateDeclaration->get_name());
               SgSymbol* symbol = templateDeclaration->get_symbol_from_symbol_table();
            // printf ("results from templateDeclaration->get_symbol_from_symbol_table(): symbol = %p \n",symbol);
#if 0
               if (symbol == NULL)
                  {
                    templateDeclaration->get_startOfConstruct()->display("Error: templateDeclaration located at:");
#if 0
                    printf ("************************ OUTPUT SYMBOL TABLE START *********************\n");
                    SageInterface::outputLocalSymbolTables(templateDeclaration->get_scope());
                    printf ("************************ OUTPUT SYMBOL TABLE END ***********************\n");
#endif
                  }
#endif
            // DQ (2/5/2007): I am unclear where the symbol can be null (unless it is for multiple declarations)!
            // ROSE_ASSERT(symbol != NULL);

            // printf ("template symbol for templateDeclaration = %p symbol = %p \n",templateDeclaration,symbol);
               if (symbol != NULL)
                  {
                    SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                    ROSE_ASSERT(templateSymbol != NULL);

                 // Also add the associated SgTemplateSymbol
                    nodeList.insert(templateSymbol);

                 // if (matchingNodeInMergedAST != NULL)
                    if (matchingNodeInMergedAST != NULL && isSgTemplateDeclaration(matchingNodeInMergedAST)->get_scope() != scopeStatement)
                       {
                         ROSE_ASSERT(isSgTemplateDeclaration(matchingNodeInMergedAST) != NULL);
                         SgTemplateDeclaration* sharedTemplateDeclaration = isSgTemplateDeclaration(matchingNodeInMergedAST);
                         SgScopeStatement* sharedTemplateScope = isSgScopeStatement(sharedTemplateDeclaration->get_scope());

                      // printf ("get the symbol for the shared template declaration = %s \n",sharedTemplateDeclaration->get_name().str());
                         ROSE_ASSERT(sharedTemplateScope != NULL);
                      // printf ("sharedTemplateScope = %p = %s \n",sharedTemplateScope,sharedTemplateScope->class_name().c_str());

                      // DQ (2/16/2007): Get the exact symbol we are looking for not just one with the same name!
                      // SgSymbol* sharedSymbol = sharedTemplateScope->lookup_template_symbol(sharedTemplateDeclaration->get_name());
                         SgSymbol* sharedSymbol = sharedTemplateDeclaration->get_symbol_from_symbol_table();

                      // DQ (2/5/2007): I think this may have already been moved where multiple foward 
                      // declaration exist (or a forward declaration and a definind declaration).
                      // ROSE_ASSERT(sharedSymbol != NULL);
                         if (sharedSymbol != NULL)
                            {
                           // DQ (2/5/2007): Moved this to only be removed if we have a sharedSymbol.
                           // SgSymbolTable* scopeSymbolTable = scopeStatement->get_symbol_table();

                              SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(templateDeclaration->get_symbol_from_symbol_table());
                           // printf ("Removing the associated template symbol = %p from scopeSymbolTable = %p \n",templateSymbol,scopeSymbolTable);
                              ROSE_ASSERT(templateSymbol != NULL);

                              if (SgProject::get_verbose() > 0)
                                   printf ("Skipping scopeSymbolTable->remove(templateSymbol = %p = %s); \n",templateSymbol,templateSymbol->class_name().c_str());

                           // scopeSymbolTable->remove(templateSymbol);

                              SgTemplateSymbol* sharedTemplateSymbol = isSgTemplateSymbol(sharedSymbol);
                              ROSE_ASSERT(sharedTemplateSymbol != NULL);

                              SgSymbolTable* sharedScopeSymbolTable = sharedTemplateScope->get_symbol_table();

                              ROSE_ASSERT(sharedScopeSymbolTable != NULL);
                              ROSE_ASSERT(sharedScopeSymbolTable->get_table() != NULL);

                           // Insert the shared symbol
                           // printf ("Check template symbol %p in symbol table %p in scope = %p \n",sharedTemplateSymbol,sharedTemplateScope->get_symbol_table(),sharedTemplateScope);
                           // if (sharedTemplateDeclaration->get_scope()->symbol_exists(sharedTemplateSymbol) == true)

                           // DQ (2/19/2007): We would like to try to avoid calling such an expensive operator!
                           // if (scopeStatement->symbol_exists(sharedTemplateSymbol) == true)
                              if (scopeStatement->symbol_exists(sharedTemplateDeclaration->get_name(),sharedTemplateSymbol) == true)
                                 {
                                // printf ("sharedTemplateSymbol = %p already exists in scopeStatement = %p \n",sharedTemplateSymbol,scopeStatement);
                                // ROSE_ASSERT(false);
#if 0
                                   printf ("************** OUTPUT SYMBOL TABLE START (sharedTemplateDeclaration) *************\n");
                                   SageInterface::outputLocalSymbolTables(sharedTemplateDeclaration->get_scope());
                                   printf ("************************ OUTPUT SYMBOL TABLE END*********************\n");
#endif
                                 }
                                else
                                 {
                                // printf ("Insert template symbol %p into symbol table %p in scope = %p \n",sharedTemplateSymbol,sharedTemplateScope->get_symbol_table(),sharedTemplateScope);
                                   scopeStatement->insert_symbol(sharedTemplateDeclaration->get_name(),sharedTemplateSymbol);

                                // Reset the parent of the symbol (for member functions the versions form either file being merged might be used 
                                // (because the class definitions may be merged ahead of the member functions and thus the symbols can have a 
                                // parent set to the symbol table that is eliminated).
                                   sharedTemplateSymbol->set_parent(sharedScopeSymbolTable);
                                 }
                            }
                           else
                            {
                              printf ("Note that for sharedTemplateDeclaration = %p = %s sharedTemplateDeclaration->get_symbol_from_symbol_table() == NULL \n",
                                   sharedTemplateDeclaration,sharedTemplateDeclaration->get_name().str());
                            }
                       }
                  }
               break;
             }

          case V_SgPragma:
             {
            // Since the SgPragma will be traversed we don't have to do anything special here!
            // SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(node);
            // nodeList.insert(pragmaDeclaration->get_pragma());
               nodeList.insert(node);
               break;
             }

          case V_SgPragmaDeclaration:
             {
            // Since the SgPragma will be traversed we don't have to do anything special here!
            // SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(node);
            // nodeList.insert(pragmaDeclaration->get_pragma());
               nodeList.insert(node);
               SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(node);
               ROSE_ASSERT(pragmaDeclaration->get_pragma() != NULL);

            // DQ (3/17/2007): Make this shareable
               nodeList.insert(pragmaDeclaration->get_pragma());
               break;
             }

         // Liao 2013/1/9 placeholders for OpenMP specific nodes
         // Somehow ./dotGeneratorWholeASTGraph calls into this function. I need this to generate the dot graph
          case V_SgOmpAtomicStatement: 
          case V_SgOmpBarrierStatement: 
          case V_SgOmpBodyStatement: 
          case V_SgOmpClause: 
          case V_SgOmpClauseBodyStatement     :   
          case V_SgOmpCollapseClause    : 
          case V_SgOmpCopyinClause      : 
          case V_SgOmpCopyprivateClause : 
          case V_SgOmpCriticalStatement : 
          case V_SgOmpDefaultClause     : 
          case V_SgOmpDoStatement       : 
          case V_SgOmpExpressionClause  : 
          case V_SgOmpFirstprivateClause: 
          case V_SgOmpFlushStatement    : 
          case V_SgOmpForStatement      : 
          case V_SgOmpForSimdStatement  : 
          case V_SgOmpIfClause          : 
          case V_SgOmpFinalClause       :  
          case V_SgOmpPriorityClause    :  
          case V_SgOmpLastprivateClause:  
          case V_SgOmpMasterStatement  :  
          case V_SgOmpNowaitClause     :  
          case V_SgOmpNumThreadsClause :  
          case V_SgOmpOrderedClause    :  
          case V_SgOmpOrderedStatement :  
          case V_SgOmpParallelStatement:  
          case V_SgOmpPrivateClause    :  
          case V_SgOmpReductionClause  :  
          case V_SgOmpDependClause     :  
          case V_SgOmpScheduleClause   :  
          case V_SgOmpSectionsStatement:  
          case V_SgOmpSectionStatement :  
          case V_SgOmpSharedClause     :  
          case V_SgOmpSingleStatement  :  
          case V_SgOmpDeclareSimdStatement  :  
          case V_SgOmpSimdStatement  :  
          case V_SgOmpTaskStatement    :  
          case V_SgOmpTaskwaitStatement : 
          case V_SgOmpThreadprivateStatement :    
          case V_SgOmpUntiedClause      : 
          case V_SgOmpMergeableClause      : 
          case V_SgOmpVariablesClause   : 
          case V_SgOmpWorkshareStatement:
            {
              //TODO real code to support AST merge
              nodeList.insert(node);
              break;
            }
          case V_SgPointerType:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               SgPointerType* pointerType = isSgPointerType(node);
               addAssociatedNodes(pointerType->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
               break;
             }

          case V_SgModifierType:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               SgModifierType* modifierType = isSgModifierType(node);
               addAssociatedNodes(modifierType->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
               break;
             }

          case V_SgArrayType:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               SgArrayType* arrayType = isSgArrayType(node);
               addAssociatedNodes(arrayType->get_base_type(),nodeList,markMemberNodesDefinedToBeDeleted);
               break;
             }

          case V_SgBaseClass:
             {
               SgBaseClass* baseClass = isSgBaseClass(node);

               nodeList.insert(baseClass);
               ROSE_ASSERT(baseClass->get_base_class() != NULL);
               nodeList.insert(baseClass->get_base_class());

            // DQ (1/21/2019): I think we don't want the reference to the pointer.
            // The modifer access function returns by reference but this is non-uniform handling of IR nodes within ROSE.
            // nodeList.insert( &(baseClass->get_baseClassModifier()) );
               ROSE_ASSERT(baseClass->get_baseClassModifier() != NULL);
               nodeList.insert(baseClass->get_baseClassModifier());
               break;
             }

          case V_SgStorageModifier:
             {
               nodeList.insert(node);
               break;
             }

          case V_SgTemplateArgument:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());

            // DQ (2/8/2007): Added this!
               nodeList.insert(node);
               SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
               ROSE_ASSERT(templateArgument != NULL);
               if (templateArgument->get_expression() != NULL)
                    nodeList.insert(templateArgument->get_expression());
               if (templateArgument->get_type() != NULL)
                  {
                    nodeList.insert(templateArgument->get_type());

                 // DQ (3/26/2007): Fix for pointer types specified in template arguments.
                 // printf ("In addAssociatedNodes() for SgTemplateArgument adding type = %p = %s \n",
                 //      templateArgument->get_type(),templateArgument->get_type()->class_name().c_str());
                    addAssociatedNodes(templateArgument->get_type(),nodeList,markMemberNodesDefinedToBeDeleted);
                  }
               if (templateArgument->get_templateDeclaration() != NULL)
                    nodeList.insert(templateArgument->get_templateDeclaration());
               break;
             }

           case V_SgIfStmt:
             {
               SgIfStmt* ifStatement = isSgIfStmt(node);
               nodeList.insert(ifStatement->get_true_body());

            // The false body might not be traversed if there is no true body (see if this fixes the problem)
               if (ifStatement->get_false_body() != NULL) {
                 nodeList.insert(ifStatement->get_false_body());
               }
             }

       // Ignore these SgStatement cases since we don't permit them to be shared
          case V_SgGlobal:
          case V_SgReturnStmt:
          case V_SgExprStatement:
          case V_SgBreakStmt:
          case V_SgCaseOptionStmt:
          case V_SgCatchStatementSeq:
          case V_SgClinkageStartStatement:
          case V_SgContinueStmt:
          case V_SgDefaultOptionStmt:
          case V_SgForInitStatement:
          case V_SgGotoStatement:
          case V_SgLabelStatement:
          case V_SgNullStatement:
          case V_SgSpawnStmt:
          case V_SgTryStmt:
          case V_SgVariantStatement:
          case V_SgUpcBarrierStatement:
          case V_SgUpcFenceStatement:
          case V_SgUpcNotifyStatement:
          case V_SgUpcWaitStatement:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               break;
             }

       // Ignore these SgSymbol cases since we don't permit them to be shared
       // case V_SgClassSymbol:
       // case V_SgEnumSymbol:
          case V_SgEnumFieldSymbol:
          case V_SgFunctionSymbol:
          case V_SgMemberFunctionSymbol:
          case V_SgLabelSymbol:
       // case V_SgTypedefSymbol:
          case V_SgVariableSymbol:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               nodeList.insert(node);
               break;
             }

       // DQ (2/10/2007): Added new case (previously didn't save internal declaration)
          case V_SgClassSymbol:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               nodeList.insert(node);
               SgClassSymbol* classSymbol = isSgClassSymbol(node);
               if (classSymbol != NULL)
                  {
                    nodeList.insert(classSymbol->get_declaration());
                  }
               break;
             }

       // DQ (2/10/2007): Added new case (previously didn't save internal declaration)
          case V_SgEnumSymbol:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               nodeList.insert(node);
               SgEnumSymbol* enumSymbol = isSgEnumSymbol(node);
               if (enumSymbol != NULL)
                  {
                    nodeList.insert(enumSymbol->get_declaration());
                  }
               break;
             }

       // DQ (2/10/2007): Added new case (previously didn't save internal declaration)
          case V_SgTypedefSymbol:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               nodeList.insert(node);
               SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(node);
               if (typedefSymbol != NULL)
                  {
                    nodeList.insert(typedefSymbol->get_declaration());
                  }
               break;
             }

       // DQ (2/10/2007): Added new case.
          case V_SgNamespaceSymbol:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               nodeList.insert(node);
               SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(node);
               if (namespaceSymbol != NULL)
                  {
                    nodeList.insert(namespaceSymbol->get_declaration());
                  }
               break;
             }

          case V_SgTemplateSymbol:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               nodeList.insert(node);
               SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(node);
               if (templateSymbol != NULL)
                  {
                    nodeList.insert(templateSymbol->get_declaration());
                  }
               break;
             }

          case V_SgCtorInitializerList:
             {
               SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(node);
               SgInitializedNamePtrList::iterator i = ctorInitializerList->get_ctors().begin();
               while (i != ctorInitializerList->get_ctors().end())
                  {
                    nodeList.insert(*i);
                    i++;
                  }
               break;
             }

       // DQ (1/21/2018): Added C++11 support
          case V_SgStaticAssertionDeclaration:

       // DQ (8/22/2007): Added Fortran support
          case V_SgImplicitStatement:
          case V_SgWhereStatement:
          case V_SgElseWhereStatement:

          case V_SgAsmStmt:
          case V_SgNamespaceAliasDeclarationStatement:
       // case V_SgTemplateInstantiationDecl:

       // DQ (9/1/2012): The template function and member function declaration is derived from the SgFunctionDeclaration.
          case V_SgTemplateVariableDeclaration:

          case V_SgVariableDeclaration:

          case V_SgUsingDeclarationStatement:
          case V_SgUsingDirectiveStatement:
          case V_SgTemplateInstantiationDirectiveStatement:
             {
            // It might be a mistake to ignore handling these cases.
            // printf ("There maybe IR nodes that we should not be ignoring for this case! node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               nodeList.insert(node);
               break;
             }

       // DQ (3/17/2007): Added support to make sure that the bitfield is not orphaned in the AST merge
          case V_SgVariableDefinition:
             {
               SgVariableDefinition* variableDefinition = isSgVariableDefinition(node);
               ROSE_ASSERT(variableDefinition != NULL);
               if (variableDefinition->get_bitfield() != NULL)
                    nodeList.insert(variableDefinition->get_bitfield());
               break;
             }
#if 1
       // DQ (6/11/2011): Added support for new template IR nodes.
          case V_SgTemplateClassDefinition:

       // DQ (3/17/2007): Added support to make sure that the SgBaseClass is not orphaned in the AST merge
          case V_SgClassDefinition:
          case V_SgTemplateInstantiationDefn:
             {
               SgClassDefinition* classDefinition = isSgClassDefinition(node);
               ROSE_ASSERT(classDefinition != NULL);
               SgBaseClassPtrList::iterator i = classDefinition->get_inheritances().begin();
               while (i != classDefinition->get_inheritances().end())
                  {
                    nodeList.insert(*i);
                    i++;
                  }
               break;
             }
#endif

       // DQ (9/3/2014): Added support for C++11 Lambda expressions.
       // case V_SgLambdaExp:
          case V_SgLambdaCapture:
          case V_SgLambdaCaptureList:

       // DQ (4/16/2011): Added support for another IR node.
          case V_SgJavaImportStatement:
          case V_SgJavaPackageStatement:

       // DQ (11/16/2007): Added support for another IR node.
          case V_SgFortranDo:

       // DQ (11/17/2007): Added support for another IR node.
          case V_SgAttributeSpecificationStatement:

       // DQ (11/19/2007): Added support for another IR node.
          case V_SgNamelistStatement:

       // DQ (11/20/2007): Added support for another IR node.
          case V_SgModuleStatement:

       // DQ (11/21/2007): Added support for another IR node.
          case V_SgEquivalenceStatement:
          case V_SgCommonBlock:
          case V_SgCommonBlockObject:
          case V_SgInterfaceStatement:
          case V_SgImportStatement:

       // DQ (11/24/2007): Added support for another IR node.
       // case V_SgIOStatement:
          case V_SgPrintStatement:
          case V_SgReadStatement:
          case V_SgWriteStatement:
          case V_SgOpenStatement:
          case V_SgCloseStatement:
          case V_SgInquireStatement:
          case V_SgFlushStatement:
          case V_SgRewindStatement:
          case V_SgBackspaceStatement:
          case V_SgEndfileStatement:
          case V_SgWaitStatement:
          case V_SgArithmeticIfStatement:
          case V_SgComputedGotoStatement:
          case V_SgFormatStatement:
          case V_SgStopOrPauseStatement:
          case V_SgUseStatement:
          case V_SgForAllStatement:
          case V_SgContainsStatement:
          case V_SgActualArgumentExpression:
          case V_SgEntryStatement:

       // DQ (11/30/2007): Added support for another IR node.
          case V_SgAssociateStatement:

       // DQ (2/18/2008): Added support for another IR node.
          case V_SgFortranIncludeLine:

       // DQ (1/30/2009): Added support for two more IR nodes.
          case V_SgAllocateStatement:
          case V_SgDeallocateStatement:

       // DXN (09/14/2011):
          case V_SgNullifyStatement:

       // Rasmussen (08/02/2019): Added support for Jovial
          case V_SgJovialCompoolStatement:
          case V_SgJovialDirectiveStatement:
          case V_SgJovialDefineDeclaration:

       // Rasmussen (10/23/2018): Added support for Jovial for statement with then construct
          case V_SgJovialForThenStatement:

          case V_SgMatlabForStatement:

       // DQ (7/18/2017): Added support to ignore the new SgDeclarationScope.
          case V_SgDeclarationScope:

       // DQ (3/26/2018): Added support for new C++11 IR node.
          case V_SgRangeBasedForStatement:

       // Ignore these scope statements since they are not yet shared
          case V_SgScopeStatement:
          case V_SgBasicBlock:
          case V_SgNamespaceDefinitionStatement:
          case V_SgForStatement:
          case V_SgUpcForAllStatement:
          case V_SgCatchOptionStmt:
          case V_SgDoWhileStmt:
          case V_SgSwitchStatement:
          case V_SgWhileStmt:
            {
               nodeList.insert(node);
               break;
             }
       // DQ (9/8/2012): Added missing case for SgTemplateFunctionDefinition.
          case V_SgTemplateFunctionDefinition:
          case V_SgFunctionDefinition:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
                SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(node);
                SgBasicBlock* body = functionDefinition->get_body();
                if (body != NULL)
                   {
                     nodeList.insert(body);

                  // Need to add the list of IR nodes in the basic block to the nodeList
                  // set<SgNode*> extraNodes = buildRequiredNodeList(body);
                  // nodeList.insert(extraNodes.begin(),extraNodes.end());
                     set<SgNode*> extraNodes = generateNodeListFromAST (body);
                     nodeList.insert(extraNodes.begin(),extraNodes.end());
                   }
               break;
             }

       // DQ (11/20/2010): We don't share token IR nodes, I think this is not required.
          case V_SgToken:

       // DQ (1/23/2010): These are relatively new IR nodes that are finally being used and tested.
          case V_SgFileList:
          case V_SgDirectory:
          case V_SgDirectoryList:

       // DQ (7/30/2010): This is the new type table support.
          case V_SgTypeTable:

       // Ignore these SgSupport cases since we don't permit them to be shared
          case V_SgRenamePair:
          case V_SgInterfaceBody:
          case V_Sg_File_Info:
          case V_SgProject:
       // case V_SgFile:
          case V_SgSourceFile:
          case V_SgBinaryComposite:
          case V_SgUnknownFile:
          case V_SgSymbolTable:
          case V_SgFunctionTypeTable:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               break;
             }

       // DQ (8/2/2018): Added support for token based unparsing to the unparse header file support.
          case V_SgHeaderFileBody:
            {
              printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
              break;
            }

       // Ignore these SgType cases since we handle types directly, via the addAssociatedNodes() function
          case V_SgJavaWildcardType:
          case V_SgFunctionType:
          case V_SgMemberFunctionType:
          case V_SgTypeUnknown:
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
          case V_SgTypeDefault:
          case V_SgPointerMemberType:
          case V_SgReferenceType:
          case V_SgClassType:
          case V_SgTemplateType:
          case V_SgEnumType:
          case V_SgTypedefType:
          case V_SgPartialFunctionModifierType:
          case V_SgTypeEllipse:
       // case V_SgUnknownMemberFunctionType:
          case V_SgQualifiedNameType:
             {
            // printf ("addAssociatedNodes(): ignoring this case of node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               SgType* type = isSgType(node);
               addAssociatedNodes(type,nodeList,markMemberNodesDefinedToBeDeleted);
               break;
             }

       // DQ (9/25/2018): Specialized the support for the case of header file unparsing.
          case V_SgIncludeDirectiveStatement:
            {
           // DQ (9/25/2018): This is now seen when using the header file unparsing, but can be ignored.
              break;
            }

       // DQ (11/23/2008): Added cases for CPP directives...
       // DQ (9/25/2018): Handling this case above.
       // case V_SgIncludeDirectiveStatement:
          case V_SgDefineDirectiveStatement:
          case V_SgUndefDirectiveStatement:
          case V_SgIfdefDirectiveStatement:
          case V_SgIfndefDirectiveStatement:
          case V_SgDeadIfDirectiveStatement:
          case V_SgIfDirectiveStatement:
          case V_SgElseDirectiveStatement:
          case V_SgElseifDirectiveStatement:
          case V_SgEndifDirectiveStatement:
          case V_SgLineDirectiveStatement:
          case V_SgWarningDirectiveStatement:
          case V_SgErrorDirectiveStatement:
          case V_SgEmptyDirectiveStatement:
          case V_SgIdentDirectiveStatement:
          case V_SgIncludeNextDirectiveStatement:
          case V_SgLinemarkerDirectiveStatement:
             {
               printf ("Handling a CPP directive in AST merge... node = %s \n",node->class_name().c_str());
               break;
             }
#if 1             
      //Liao 3/11/2009: cases for Fortran, not very sure if this is the right way
      //TODO Need Dan to review this code.
      // Copy the way of handling V_SgFunctionParameterList
         case V_SgFormatItemList:
           {
             SgFormatItemPtrList itemlist = isSgFormatItemList(node)->get_format_item_list();
             SgFormatItemPtrList::iterator i = itemlist.begin();
             while (i!=itemlist.end())
             {
               nodeList.insert(*i);
               addAssociatedNodes(*i,nodeList,markMemberNodesDefinedToBeDeleted);
               i++;
             }
             break;
           }
         case V_SgFormatItem:
         {
           nodeList.insert(node);
           break;
           }
#endif

           case V_SgTemplateParameter:
             {
               SgTemplateParameter * tpl_param = isSgTemplateParameter(node);
               ROSE_ASSERT(tpl_param != NULL);
               if (tpl_param->get_type() != NULL) {
                 addAssociatedNodes(tpl_param->get_type(), nodeList,markMemberNodesDefinedToBeDeleted);
               }
               if (tpl_param->get_defaultTypeParameter() != NULL) {
                 addAssociatedNodes(tpl_param->get_defaultTypeParameter(), nodeList, markMemberNodesDefinedToBeDeleted);
               }
               break;
             }
           case V_SgNonrealDecl:
             {
               SgNonrealDecl * nrdecl = isSgNonrealDecl(node);
               ROSE_ASSERT(nrdecl != NULL);
               nodeList.insert(nrdecl);

//             SgDeclarationScope * nrscope = nrdecl->get_nonreal_decl_scope();
//             if (nrscope != NULL)
//               addAssociatedNodes(nrscope,nodeList,markMemberNodesDefinedToBeDeleted);

               SgNonrealType * nrtype = nrdecl->get_type();
               if (nrtype != NULL)
                 addAssociatedNodes(nrtype,nodeList,markMemberNodesDefinedToBeDeleted);

               SgTemplateArgumentPtrList & tpl_args = nrdecl->get_tpl_args();
               for (SgTemplateArgumentPtrList::iterator it = tpl_args.begin(); it != tpl_args.end(); it ++) {
                 addAssociatedNodes(*it,nodeList,markMemberNodesDefinedToBeDeleted);
               }

               SgTemplateParameterPtrList & tpl_params = nrdecl->get_tpl_params();
               for (SgTemplateParameterPtrList::iterator it = tpl_params.begin(); it != tpl_params.end(); it ++) {
                 addAssociatedNodes(*it,nodeList,markMemberNodesDefinedToBeDeleted);
               }

               break;
             }
           case V_SgNonrealType:
             {
               SgType* type = isSgType(node);
               ROSE_ASSERT(type != NULL);
               addAssociatedNodes(type,nodeList,markMemberNodesDefinedToBeDeleted);

               break;
             }
           case V_SgNonrealSymbol:
             {
               SgNonrealSymbol * symbol = isSgNonrealSymbol(node);
               ROSE_ASSERT(symbol != NULL);

               break;
             }

        // DQ (3/27/2019): Added case for new IR node.
           case V_SgEmptyDeclaration:
             {
               SgEmptyDeclaration * emptyDeclaration = isSgEmptyDeclaration(node);
               ROSE_ASSERT(emptyDeclaration != NULL);
               break;
             }

       // Rasmussen 6/14/2017: Ignore SgUntyped nodes for now.  Untyped nodes are currently used in
       // parsing Jovial as a temporary conversion mechanism to store node information before complete
       // type resolution has been done.
          case V_SgUntypedProgramHeaderDeclaration:
          case V_SgUntypedDeclarationStatementList:
          case V_SgUntypedFunctionDeclarationList:
          case V_SgUntypedExprListExpression:
          case V_SgUntypedNullDeclaration:
          case V_SgUntypedNullExpression:
          case V_SgUntypedOtherExpression:
          case V_SgUntypedScope:
          case V_SgUntypedReferenceExpression:
          case V_SgUntypedStatementList:
          case V_SgUntypedStructureDefinition:
          case V_SgUntypedSubscriptExpression:
          case V_SgUntypedValueExpression:
          case V_SgUntypedVariableDeclaration:
             {
               break;
             }

          default:
             {
               printf ("addAssociatedNodes(): default case node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(nodeList.find(NULL) == nodeList.end());

  // printf ("Leaving addAssociatedNodes(SgNode* node): nodeList.size() = %ld \n",nodeList.size());
   }







#endif // COLLECT_ASSOCIATE_NODES_C

