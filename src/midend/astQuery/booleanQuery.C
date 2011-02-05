// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "booleanQuery.h"
#include "booleanQueryInheritedAttribute.h"

using namespace std;

// *****************************************************************
//                 Interface function for Name Query
// *****************************************************************

BooleanQuery::BooleanQuery()
   {
   }

BooleanQuery::~BooleanQuery()
   {
   }


bool
BooleanQuery::isUnion ( SgNode* astNode )
   {
     return internalBooleanQuery (astNode,UnionedField);
   }

BooleanQuerySynthesizedAttributeType
BooleanQuery::internalBooleanQuery (
   SgNode* astNode,
   TypeOfQueryType elementReturnType )
   {
  // This function just returns the variant value of the astNode (a trivial example)
     ROSE_ASSERT (astNode != NULL);

     BooleanQueryInheritedAttributeType BooleanQueryInheritedData;

     BooleanQueryInheritedData.setQuery(elementReturnType);

     printf ("######################### START OF VARIABLE BOOLEAN QUERY ######################## \n");

#if 0
     printf ("In BooleanQuery(%s): astNode->sage_class_name() = %s \n",matchingName.c_str(),astNode->sage_class_name());
#endif

     BooleanQuery treeTraversal;

  // Do the traversal to that will generate the correct data
     BooleanQuerySynthesizedAttributeType booleanValue = treeTraversal.traverse ( astNode, BooleanQueryInheritedData );

#if 1
     printf ("Boolean value = %s \n",(booleanValue == true) ? "TRUE" : "FALSE");
#endif

     printf ("######################### END OF VARIABLE BOOLEAN QUERY ######################## \n");

#if 0
     printf ("Exiting within the name query! \n");
     ROSE_ABORT();
#endif

     return booleanValue;
   }

BooleanQueryInheritedAttributeType
BooleanQuery::evaluateInheritedAttribute (
   SgNode* astNode,
   BooleanQueryInheritedAttributeType inheritedValue )
   {
  // This function does not do anything.  For this query everything can be done within the assembly function.
     ROSE_ASSERT (astNode != NULL);

#if 0
     printf ("In globalQueryGetListOperandStringFunction(): astNode->sage_class_name() = %s \n",astNode->sage_class_name());
#endif

     return inheritedValue;
   }


BooleanQuerySynthesizedAttributeType
BooleanQuery::evaluateSynthesizedAttribute (
   SgNode* astNode,
   BooleanQueryInheritedAttributeType inheritedValue,
   SubTreeSynthesizedAttributes  synthesizedAttributeList )
   {
  // This function assemble the elements of the input list (a list of lists) to form the output (a single list)

  // Build up a return value
     BooleanQuerySynthesizedAttributeType returnValue;

#if 0
     printf ("TOP of BooleanQueryAssemblyFunction (astNode->sage_class_name() = %s,synthesizedAttributeList.size() = %d) \n",
          astNode->sage_class_name(),synthesizedAttributeList.size());
#endif

  // Build an attribute to represent the local information at this node and add it to the list so that
  // the final synthesized attribute will represent all the information from this node and it children.
  // BooleanQuerySynthesizedAttributeType localAttribute;
  // synthesizedAttributeList.push_back(localAttribute);

  // Note that the typedef defined in the Query class will not work here
  // Query<BooleanQueryInheritedAttributeType,list<string>,int>::SynthesizedAttributeListType::iterator i;
  // list< SynthesizedAttributeListElementType< list<string> > >::iterator i;
     vector<BooleanQuerySynthesizedAttributeType>::iterator i;

  // computing the expected size allows use to check for some sorts of errors
     for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)
        {
       // printf ("In BooleanQueryAssemblyFunction: Looping through the list of SynthesizedAttributeListElements: size of attributeList = %zu \n",synthesizedAttributeList.size());

#if 1
          printf ("boolean value = %s \n",(*i == true) ? "TRUE" : "FALSE");
#endif

#if 0
       // The merge function will sort the lists and assumes that the lists are already sorted
       // (*i).getSynthesizedAttribute().sort();
       // returnList.merge((*i).getSynthesizedAttribute());
          (*i).sort();
          returnList.merge(*i);

       // printf ("Inside of loop: returnList size() = %zu \n",returnList.size());
#endif
        }

  // Each type of query returns a slightly different organization of its list (e.g. type names are unique)
     switch (inheritedValue.getQuery())
        {
       // Build the test for if a variable declaration is defined by a union (e.g. union { int x; int y; } xy; )
          case BooleanQuery::UnionedField:
             {
            // if this is a variable declaration, then look at the parent and see if it is a union structure
               returnValue = false;
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNode);
               if (variableDeclaration != NULL)
                  {
                    SgStatement* parentStatementNode = isSgStatement(variableDeclaration->get_parent());
                    if (parentStatementNode != NULL)
                       {
                         printf ("Implementation of union test is incomplete! \n");
                         ROSE_ABORT();

                         returnValue = true;
                       }
                  }
             
               break;
             }

          case BooleanQuery::UnknownListElementType:
          case BooleanQuery::Type:
          case BooleanQuery::FunctionDeclaration:
          case BooleanQuery::MemberFunctionDeclaration:
          case BooleanQuery::ClassDeclaration:
          case BooleanQuery::Argument:
          case BooleanQuery::Field:
          case BooleanQuery::Struct:
               printf ("Cases not implemented for non BooleanQuery::TypeNames! \n");
               ROSE_ABORT();
               break;

       // Cases where we don't have to do anything
          case BooleanQuery::VariableDeclaration:
          default:
               break;
        }

#if 0
     printf ("BOTTOM of BooleanQueryAssemblyFunction returnValue = %s \n",
          (returnValue == true) ? "TRUE" : "FALSE" );
#endif

     return returnValue;
   }

// *****************************************************
// *****************************************************
// *****************************************************
// *****************************************************

#if 0
BooleanQueryReverseTraversalInheritedAttribute
BooleanQueryReverseTraversal::evaluateInheritedAttribute
   ( SgNode* node,
     BooleanQueryReverseTraversalInheritedAttribute inputInheritedAttribute )
   {
     return inputInheritedAttribute;
   }

BooleanQueryReverseTraversalSynthesizedAttribute
BooleanQueryReverseTraversal::evaluateSynthesizedAttribute
   ( SgNode* node,
     BooleanQueryReverseTraversalInheritedAttribute inputInheritedAttribute,
     BooleanQueryReverseTraversalSynthesizedAttribute inputSynthesizedAttribute )
   {
     BooleanQueryReverseTraversalSynthesizedAttribute localSynthesizedAttribute(inputSynthesizedAttribute);

     printf ("Inside of BooleanQueryReverseTraversal::evaluateSynthesizedAttribute(%s) \n",node->sage_class_name());

     if (node->variantT() == inputInheritedAttribute.targetVariant)
        localSynthesizedAttribute.value = true;

     return localSynthesizedAttribute;
   }

bool
BooleanQuery::isContainedInSubtreeOfType ( SgNode* astNode, VariantT targetNodeVariant )
   {
     BooleanQueryReverseTraversal traversal;
     BooleanQueryReverseTraversalInheritedAttribute inheritedAttribute;

     inheritedAttribute.targetVariant = targetNodeVariant;

     BooleanQueryReverseTraversalSynthesizedAttribute finalSynthesizedAttribute;

     printf ("Calling BooleanQueryReverseTraversal::traversal(%s) \n",astNode->sage_class_name());

     finalSynthesizedAttribute = traversal.traverse(astNode,inheritedAttribute);

     return finalSynthesizedAttribute.value;   
   }
#endif











