#ifndef ROSE_NUMBER_QUERY
#define ROSE_NUMBER_QUERY

#include "AstProcessing.h"

typedef Rose_STL_Container<int> NumberQuerySynthesizedAttributeType;

namespace NumberQuery{

  enum TypeOfQueryTypeOneParameter
  {
    UnknownListElementType = 0,
    NumberOfArgsInConstructor = 1,
    NumberOfOperands = 2,
    NumberOfArgsInScalarIndexingOperator = 3,
    END_OF_NODE_TYPE_LIST_ONE_PARAMETER
  };

  enum TypeOfQueryTypeTwoParameters
  {
    UnknownListElementTypeTwoParameters = 0,
    NumberOfArgsInParanthesisOperator = 1,
    END_OF_NODE_TYPE_LIST_TWO_PARAMETERS
  };

  typedef NumberQuerySynthesizedAttributeType (*roseFunctionPointerOneParameter)  (SgNode *);
  typedef NumberQuerySynthesizedAttributeType (*roseFunctionPointerTwoParameters) (SgNode *, std::string);



  NumberQuerySynthesizedAttributeType
    queryNumberOfArgsInParenthesisOperator (SgNode * astNode, std::string typeName);


  NumberQuerySynthesizedAttributeType
    queryNumberOfArgsInConstructor (SgNode * astNode);

  NumberQuerySynthesizedAttributeType
    queryNumberOfOperands (SgNode * astNode);

  NumberQuerySynthesizedAttributeType
    queryNumberOfArgsInScalarIndexingOperator (SgNode * astNode);




  /**************************************************************************************************************
   * The function
   *    std::function<NumberQuerySynthesizedAttributeType(SgNode *)> getFunction(TypeOfQueryTypeOneParameter oneParam);
   * will return a functor wrapping the pre-implemented function for TypeOfQueryTypeOneParameter.
   **************************************************************************************************************/

  std::function<NumberQuerySynthesizedAttributeType(SgNode *)> getFunction(TypeOfQueryTypeOneParameter oneParam);

  /**************************************************************************************************************
   * The function
   *    std::function<NumberQuerySynthesizedAttributeType(SgNode *, std::string)>
   *       getFunction(TypeOfQueryTypeTwoParameters twoParam);
   * will return a functor wrapping the pre-implemented function for TypeOfQueryTypeTwoParameters.
   **************************************************************************************************************/

  std::function<NumberQuerySynthesizedAttributeType(SgNode *, std::string)>
     getFunction(TypeOfQueryTypeTwoParameters twoParam);

  // get the SgNode's conforming to the test in querySolverFunction or
  // get the SgNode's conforming to the test in the TypeOfQueryTypeOneParamter the user specify.

  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, TypeOfQueryTypeOneParameter elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all numbers in the sub-tree of the variable 'subTree' found by the 
   * preimplemented function 'elementReturnType'.
   *******************************************************************************************/
  ROSE_DLL_API NumberQuerySynthesizedAttributeType
    querySubTree
    (SgNode * subTree,
     TypeOfQueryTypeOneParameter elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  ROSE_DLL_API NumberQuerySynthesizedAttributeType queryNodeList
    (Rose_STL_Container< SgNode * > nodeList,
     TypeOfQueryTypeOneParameter elementReturnType);

  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, roseFunctionPointerOneParameter elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all numbers in the sub-tree of the variable 'subTree' found by the 
   * function 'elementReturnType'.
   *******************************************************************************************/
  ROSE_DLL_API NumberQuerySynthesizedAttributeType
    querySubTree
    (SgNode * subTree,
     roseFunctionPointerOneParameter elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  ROSE_DLL_API NumberQuerySynthesizedAttributeType queryNodeList
    (Rose_STL_Container< SgNode * >nodeList,
     roseFunctionPointerOneParameter querySolverFunction);


  // get the SgNode's conforming to the test in querySolverFunction or
  // get the SgNode's conforming to the test in the TypeOfQueryTypeTwoParamters the user specify.
  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, SgNode* traversal, roseFunctionPointerTwoParameters elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all numbers in the sub-tree of the variable 'subTree' found by the 
   * function 'querySolverFunction' given a second argument 'traversal'.
   *******************************************************************************************/
  ROSE_DLL_API NumberQuerySynthesizedAttributeType
    querySubTree
    (SgNode * subTree,
     std::string traversal,
     roseFunctionPointerTwoParameters querySolverFunction,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  ROSE_DLL_API NumberQuerySynthesizedAttributeType queryNodeList
    (Rose_STL_Container< SgNode * >nodeList,
     std::string targetNode,
     roseFunctionPointerTwoParameters querySolverFunction);

  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, SgNode* traversal, TypeOfQueryTypeTwoParameters elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all numbers in the sub-tree of the variable 'subTree' found by the 
   * preimplemented function 'elementReturnType' given a second argument 'traversal'.
   *******************************************************************************************/

  ROSE_DLL_API NumberQuerySynthesizedAttributeType
    querySubTree
    (SgNode * subTree,
     std::string traversal,
     TypeOfQueryTypeTwoParameters elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  ROSE_DLL_API NumberQuerySynthesizedAttributeType queryNodeList
    (Rose_STL_Container< SgNode * >nodeList,
     std::string targetNode, TypeOfQueryTypeTwoParameters elementReturnType);


  // perform a query on a list<SgNode>


  /********************************************************************************
   * The function
   *  NumberQuerySynthesizedAttributeType
   * queryMemoryPool(NodeFunctional nodeFunc , VariantVector* targetVariantVector = NULL)
   * will query the memory pool for IR nodes which satisfies the criteria specified in and 
   * returned by the predicate in the second argument in the nodes with a corresponding variant
   * in VariantVector.
   ********************************************************************************/
  template<typename NodeFunctional>
    NumberQuerySynthesizedAttributeType
    queryMemoryPool(NodeFunctional nodeFunc , VariantVector* targetVariantVector = NULL)
    {
      return AstQueryNamespace::queryMemoryPool(nodeFunc,targetVariantVector);
    };


  /********************************************************************************
   * The function
   *      NumberQuerySynthesizedAttributeType queryMemoryPool ( 
   *                   _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
   *                   VariantVector* ){
   * will on every node of the memory pool which has a corresponding variant in VariantVector
   * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
   ********************************************************************************/
  ROSE_DLL_API NumberQuerySynthesizedAttributeType
    queryMemoryPool
    (
     std::string traversal,
     roseFunctionPointerTwoParameters querySolverFunction, VariantVector* targetVariantVector);

  /********************************************************************************
   * The function
   *      _Result queryMemoryPool ( 
   *                   _Result (*__x)(SgNode*),
   *                   VariantVector* ){
   * will on every node of the memory pool which has a corresponding variant in VariantVector
   * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
   ********************************************************************************/
  ROSE_DLL_API NumberQuerySynthesizedAttributeType
    queryMemoryPool
    (
     std::string traversal,
     roseFunctionPointerOneParameter querySolverFunction, VariantVector* targetVariantVector = NULL);

  /********************************************************************************
   * The function
   *      _Result queryMemoryPool ( SgNode*,
   *                   TypeOfQueryTypeTwoParameters,
   *                   VariantVector* ){
   * will on every node of the memory pool which has a corresponding variant in VariantVector
   * performa the predefined action specified by the second argument and return a 
   * NodeQuerySynthesizedAttributeType.
   ********************************************************************************/

  ROSE_DLL_API NumberQuerySynthesizedAttributeType
    queryMemoryPool
    (
     std::string traversal,
     TypeOfQueryTypeTwoParameters elementReturnType,
     VariantVector* targetVariantVector = NULL);

  /********************************************************************************
   * The function
   *      _Result queryMemoryPool ( 
   *                   TypeOfQueryTypeOneParameter,
   *                   VariantVector* ){
   * will on every node of the memory pool which has a corresponding variant in VariantVector
   * performa the predefined action specified by the second argument and return a 
   * NodeQuerySynthesizedAttributeType.
   ********************************************************************************/

  ROSE_DLL_API NumberQuerySynthesizedAttributeType
    queryMemoryPool
    (
     TypeOfQueryTypeOneParameter elementReturnType,
     VariantVector* targetVariantVector = NULL);




  // DQ (4/8/2004): Added query based on vector of variants

}


// endif for ROSE_NAME_QUERY
#endif
