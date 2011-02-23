#ifndef ROSE_NODE_QUERY
#define ROSE_NODE_QUERY


#include "AstProcessing.h"
#include "astQuery.h"
#include <functional>

// #include "variantVector.h"

// **********************************
// Prototypes for Variable Node Query
// **********************************
// forward declaration

class NodeQueryInheritedAttributeType;

typedef Rose_STL_Container <SgNode * > NodeQuerySynthesizedAttributeType;

namespace NodeQuery
{
  using namespace AstQueryNamespace;

/*
 * The function
 *    queryNodeAnonymousTypedef()
 * is a NodeQuery which finds all Anonymous Typedefs is the scope.
 */
  NodeQuerySynthesizedAttributeType
  queryNodeAnonymousTypedef (SgNode * node);

/*
 * The function
 *    queryNodeAnonymousTypedefClassDeclaration()
 * is a NodeQuery which finds all Anonymous Typedefs is the scope.
 */
  NodeQuerySynthesizedAttributeType
  queryNodeAnonymousTypedefClassDeclaration (SgNode * node);

/*
 *   The function
 *      queryNodeBottomUpClassDeclarationFromName()
 *   takes as a first parameter a SgNode*. As a second parameter
 *   it takes a SgNode* which is a SgName* which contains the
 *   name the class should have. It returns a class declaration
 *   if it corresponds to the name.
 *    
 */

  NodeQuerySynthesizedAttributeType
  queryNodeClassDeclarationFromName (SgNode * node, SgNode * nameNode);

/*
 *   The method
 *       queryNodeClassDeclarationsFromTypename()
 *   takes as a first parameter a SgNode*. As a second parameter
 *   it takes a SgNode* which is a SgName* which contains the
 *   typename the class should have. A SgNode* is returned if
 *   the base-type of the class corresponds to the typename.
 */

  NodeQuerySynthesizedAttributeType
  queryNodeClassDeclarationsFromTypeName (SgNode * node, SgNode * nameNode);

/*
 * The function
 *     queryNodePragmaDeclarationFromName()
 * takes as a first parameter a SgNode*. As a second parameter it takes
 * a SgNode* who must be of type SgName. The SgName contains a std::string which
 * should be the same as the left side in the pragma or a part of the left
 * side of the pragma. If the std::string is empty,
 * there will be an error message.
 *
 *        #pragma std::stringInSgNode = information
 *         
 */
  Rose_STL_Container <SgNode * > queryNodePragmaDeclarationFromName (SgNode * node, SgNode * nameNode);

/*
 *  The function
 *      queryNodeVariableDeclarationFromName()
 *  takes as a first parameter a SgNode*, and as a second parameter a
 *  SgNode* which is of type SgName* and contains the name of the 
 *  interesting variable.
 * 
 */

  Rose_STL_Container <SgNode * > queryNodeVariableDeclarationFromName (SgNode * astNode, SgNode * nameNode);

  NodeQuerySynthesizedAttributeType querySolverArguments (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverFunctionDeclarations (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverMemberFunctionDeclarations (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverVariableTypes (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverVariableDeclarations (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverClassDeclarations (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverStructDeclarations (SgNode * astNode);


  NodeQuerySynthesizedAttributeType querySolverUnionDeclarations (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverTypedefDeclarations (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverClassFields (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverStructFields (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverUnionFields (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverStructDefinitions (SgNode * astNode);

  NodeQuerySynthesizedAttributeType querySolverFunctionDeclarationFromDefinition (SgNode * astNode, SgNode * functionDefinition);

  NodeQuerySynthesizedAttributeType querySolverGrammarElementFromVariant (SgNode * astNode, VariantT targetVariant);

// DQ (7/20/2004): Temporary output function to use in tests (to debug type traversals)
  Rose_STL_Container<SgNode*> generateListOfTypes ( SgNode* astNode );

  void printNodeList ( const Rose_STL_Container<SgNode*> & localList );

  typedef AstQueryNamespace::AstQuery <AstSimpleProcessing, helpFunctionalOneParamater <SgNode*> > NodeAstQueryType;
  typedef AstQueryNamespace::AstQuery <AstSimpleProcessing, helpFunctionalTwoParamaters <SgNode*,SgNode * > > NodeAstTwoParam;
  typedef Rose_STL_Container<SgNode*>(*roseFunctionPointerOneParameter) (SgNode *);
  typedef Rose_STL_Container<SgNode*>(*roseFunctionPointerTwoParameters) (SgNode *, SgNode *);
  typedef AstQueryNamespace::QueryDepth QueryDepth;

  enum TypeOfQueryTypeOneParameter
  {
    UnknownListElementType = 0,
    VariableDeclarations = 1,
    VariableTypes = 2,
    FunctionDeclarations = 3,
    MemberFunctionDeclarations = 4,
    ClassDeclarations = 5,
    StructDeclarations = 6,
    UnionDeclarations = 7,
    Arguments = 8,
    ClassFields = 9,
    StructFields = 10,
    UnionFields = 11,
    StructDefinitions = 12,
    TypedefDeclarations = 13,
    AnonymousTypedefs = 14,
    AnonymousTypedefClassDeclarations = 15,
    END_OF_NODE_TYPE_LIST_ONE_PARAMETER
  };

  enum TypeOfQueryTypeTwoParameters
  {
    UnknownListElementTypeTwoParameters = 0,
    FunctionDeclarationFromDefinition = 1,
    ClassDeclarationFromName = 2,
    ClassDeclarationsFromTypeName = 3,
    PragmaDeclarationFromName = 4,
    VariableDeclarationFromName = 5,

    // DQ (3/24/2004): Added support for templates
    // GrammarElementFromVariant           = 6,
    END_OF_NODE_TYPE_LIST_TWO_PARAMETERS
  };

       /**************************************************************************************************************
        * The function
        *    std::pointer_to_unary_function<SgNode*, std::list<SgNode*> > getFunction(TypeOfQueryTypeOneParameter oneParam);
        * will return a functor wrapping the pre-implemented function for TypeOfQueryTypeOneParameter.
        **************************************************************************************************************/
  std::pointer_to_unary_function < SgNode *,
  Rose_STL_Container<SgNode*> >getFunction (TypeOfQueryTypeOneParameter oneParam);

       /**************************************************************************************************************
        * The function
        * std::pointer_to_binary_function<SgNode*, SgNode*, std::list<SgNode*> > getFunction(TypeOfQueryTypeTwoParameters twoParam);
        * will return a functor wrapping the pre-implemented function for TypeOfQueryTypeTwoParameters.
        **************************************************************************************************************/
  std::pointer_to_binary_function < SgNode *, SgNode *,
    Rose_STL_Container<SgNode*> > getFunction (TypeOfQueryTypeTwoParameters twoParam);



  // Functions supporting the query of variants
  void pushNewNode ( NodeQuerySynthesizedAttributeType* nodeList, const VariantVector & targetVariantVector, SgNode * astNode);
  void* querySolverGrammarElementFromVariantVector ( SgNode * astNode, VariantVector targetVariantVector,  NodeQuerySynthesizedAttributeType* returnNodeList );
  NodeQuerySynthesizedAttributeType querySolverGrammarElementFromVariantVector ( SgNode * astNode, VariantVector targetVariantVector );


   /********************************************************************************************
   *
   * The function template
   *   querySubTree (SgNode * subTree, NodeFunctional ,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a NodeFunctional::result_type of all nodes returned from NodeFunctional when applied to
   * either all the nodes in a subTree or the children of the node 'subTree'.
   *******************************************************************************************/
  template<typename NodeFunctional>
  typename NodeFunctional::result_type 
   querySubTree ( SgNode * subTree, 
             NodeFunctional pred, AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes)
     {
               return AstQueryNamespace::querySubTree(subTree,pred,defineQueryType);

     };
  // get the SgNode's conforming to the test in querySolverFunction or
  // get the SgNode's conforming to the test in the TypeOfQueryTypeOneParamter the user specify.

  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, TypeOfQueryTypeOneParameter elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all SgNodes in the sub-tree of the variable 'subTree' found by the 
   * preimplemented function 'elementReturnType'.
   *******************************************************************************************/
  ROSE_DLL_API NodeQuerySynthesizedAttributeType
  querySubTree 
    (SgNode * subTree,
     TypeOfQueryTypeOneParameter elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  NodeQuerySynthesizedAttributeType queryNodeList
    (NodeQuerySynthesizedAttributeType nodeList,
     TypeOfQueryTypeOneParameter elementReturnType);

   /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, roseFunctionPointerOneParameter elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all SgNodes in the sub-tree of the variable 'subTree' found by the 
   * function 'elementReturnType'.
   *******************************************************************************************/
   ROSE_DLL_API NodeQuerySynthesizedAttributeType
   querySubTree 
    (SgNode * subTree,
     roseFunctionPointerOneParameter elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  Rose_STL_Container< SgNode * > queryNodeList (Rose_STL_Container< SgNode * >nodeList, Rose_STL_Container< SgNode * >(*querySolverFunction) (SgNode *) );

  // get the SgNode's conforming to the test in querySolverFunction or
  // get the SgNode's conforming to the test in the TypeOfQueryTypeTwoParamters the user specify.

   /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, SgNode* traversal, roseFunctionPointerTwoParameters elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all SgNodes in the sub-tree of the variable 'subTree' found by the 
   * function 'querySolverFunction' given a second argument 'traversal'.
   *******************************************************************************************/
  ROSE_DLL_API NodeQuerySynthesizedAttributeType
  querySubTree
    (SgNode * subTree,
     SgNode * traversal,
     roseFunctionPointerTwoParameters querySolverFunction,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  NodeQuerySynthesizedAttributeType queryNodeList
    (NodeQuerySynthesizedAttributeType nodeList,
     SgNode * targetNode,
     roseFunctionPointerTwoParameters querySolverFunction);

   /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, SgNode* traversal, TypeOfQueryTypeTwoParameters elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all SgNodes in the sub-tree of the variable 'subTree' found by the 
   * preimplemented function 'elementReturnType' given a second argument 'traversal'.
   *******************************************************************************************/
  ROSE_DLL_API NodeQuerySynthesizedAttributeType
  querySubTree
    (SgNode * subTree,
     SgNode * traversal,
     TypeOfQueryTypeTwoParameters elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  NodeQuerySynthesizedAttributeType queryNodeList
    (NodeQuerySynthesizedAttributeType nodeList,
     SgNode * targetNode, TypeOfQueryTypeTwoParameters elementReturnType);

  
  // DQ (3/26/2004): Added query based on variant
   /********************************************************************************************
   *
   * The function
   *    querySubTree (SgNode * subTree,VariantT targetVariant,
   *       AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all SgNodes in the sub-tree of the variable 'subTree' which is
   * of SgNode VariantT 'targetVariant'.
   *******************************************************************************************/
  ROSE_DLL_API NodeQuerySynthesizedAttributeType
  querySubTree
    (SgNode * subTree,
     VariantT targetVariant,
     AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);

  NodeQuerySynthesizedAttributeType
  queryNodeList
    (NodeQuerySynthesizedAttributeType,
     VariantT targetVariant);

  // Liao, 8/27/2009, a more generic nodeList query, not just for nodes of type SgNode*
  template <typename T> 
  Rose_STL_Container <T* >
  queryNodeList ( Rose_STL_Container <T* > queryList, VariantVector targetVector)
  {
    Rose_STL_Container <T* > result;
    //Rose_STL_Container <T*>::iterator iter; // not recognized ??
    size_t i=0;
    for (i= 0; i<queryList.size(); i++)
    {
      SgNode* node = dynamic_cast<SgNode*> (queryList[i]);
      ROSE_ASSERT(node != NULL);
      for (VariantVector::const_iterator iter = targetVector.begin();
            iter != targetVector.end(); iter++)
      {
        if (node->variantT() == *iter)
        {
          result.push_back(queryList[i]);
          break;
        }
      }
    }
    return result;
  }
  // Single VariantT 
  template <typename T> 
  Rose_STL_Container <T* >
  queryNodeList ( Rose_STL_Container <T* > queryList, VariantT targetVariant)
  {
    return queryNodeList<T>( queryList, VariantVector(targetVariant));
#if 0    
    Rose_STL_Container <T* > result;
    //Rose_STL_Container <T*>::iterator iter; // not recognized ??
    size_t i=0;
    for (i= 0; i<queryList.size(); i++)
    {
      SgNode* node = dynamic_cast<SgNode*> (queryList[i]);
      ROSE_ASSERT(node != NULL);
      if (node->variantT() == targetVariant)
        result.push_back(queryList[i]);
    }
    return result;
#endif    
  }


  // DQ (4/8/2004): Added query based on vector of variants

  /**********************************************************************************************
   * The function
   *    querySubTree (SgNode * subTree, VariantVector targetVariantVector,
   *      AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * reutnrs a list of all SgNodes us the AST sub-tree of the variable 'subTree' conforming to the
   * variants in VariantVector.
   *********************************************************************************************/
  ROSE_DLL_API NodeQuerySynthesizedAttributeType  querySubTree
    (SgNode * subTree,
     VariantVector targetVariantVector,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  // DQ (3/25/2004): Added to support more general form of query based on variant value
  NodeQuerySynthesizedAttributeType queryNodeList
    ( NodeQuerySynthesizedAttributeType,
      VariantVector);

  void mergeList (Rose_STL_Container< SgNode * >&nodeList, const Rose_STL_Container< SgNode * >&localList);


 /********************************************************************************
   * The function
   *  NodeQuerySynthesizedAttributeType
   * queryMemoryPool(SgNode* node, NodeFunctional nodeFunc , VariantVector* targetVariantVector = NULL)
   * will query the memory pool for IR nodes which satisfies the criteria specified in and 
   * returned by the predicate in the second argument in the nodes with a corresponding variant
   * in VariantVector.
   ********************************************************************************/
  template<typename NodeFunctional>
  NodeQuerySynthesizedAttributeType
  queryMemoryPool(NodeFunctional nodeFunc , VariantVector* targetVariantVector = NULL)
                {
        return AstQueryNamespace::queryMemoryPool(nodeFunc,targetVariantVector);
                };


  /********************************************************************************
   * The function
   *      NodeQuerySynthesizedAttributeType queryMemoryPool ( SgNode * subTree,
   *                   _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
   *                   VariantVector* ){
   * will on every node of the memory pool which has a corresponding variant in VariantVector
   * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
   ********************************************************************************/
  NodeQuerySynthesizedAttributeType
  queryMemoryPool
    (
     SgNode * traversal,
     roseFunctionPointerTwoParameters querySolverFunction, VariantVector* targetVariantVector);

/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree,
 *                   _Result (*__x)(SgNode*),
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
 ********************************************************************************/
  NodeQuerySynthesizedAttributeType
  queryMemoryPool
    (
     SgNode * traversal,
     roseFunctionPointerOneParameter querySolverFunction, VariantVector* targetVariantVector = NULL);

/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree, SgNode*,
 *                   TypeOfQueryTypeTwoParameters,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the predefined action specified by the second argument and return a 
 * NodeQuerySynthesizedAttributeType.
 ********************************************************************************/

  NodeQuerySynthesizedAttributeType
  queryMemoryPool
    (
     SgNode * traversal,
     TypeOfQueryTypeTwoParameters elementReturnType,
     VariantVector* targetVariantVector = NULL);

/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree,
 *                   TypeOfQueryTypeOneParameter,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the predefined action specified by the second argument and return a 
 * NodeQuerySynthesizedAttributeType.
 ********************************************************************************/

  NodeQuerySynthesizedAttributeType
  queryMemoryPool
    (
     TypeOfQueryTypeOneParameter elementReturnType,
     VariantVector* targetVariantVector = NULL);

/********************************************************************************
 * The function
 *  DefaultNodeFunctional::result_type
 *             queryMemoryPool(VariantVector& targetVariantVector);
 * will return every node in the AST with a corresponding variant in the VariantVector.
 ********************************************************************************/
   DefaultNodeFunctional::result_type 
             queryMemoryPool(VariantVector& targetVariantVector);


// END NAMESPACE NodeQuery2
}

// endif for ROSE_NODE_QUERY
#endif
