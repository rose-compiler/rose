#ifndef ROSE_NAME_QUERY
#define ROSE_NAME_QUERY

#include "AstProcessing.h"
#include <functional>



typedef Rose_STL_Container< std::string > NameQuerySynthesizedAttributeType;

namespace NameQuery{


  typedef AstQueryNamespace::QueryDepth QueryDepth;
  typedef
    NameQuerySynthesizedAttributeType (*roseFunctionPointerOneParameter)
    (SgNode *);
  typedef
    NameQuerySynthesizedAttributeType (*roseFunctionPointerTwoParameters)
    (SgNode *, std::string);





  NameQuerySynthesizedAttributeType
    queryNameArgumentNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameClassDeclarationNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameStructNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameUnionNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameMemberFunctionDeclarationNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameFunctionDeclarationNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameVariableTypeNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameVariableNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameClassFieldNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameUnionFieldNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameStructFieldNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryNameFunctionReferenceNames (SgNode * astNode);

  NameQuerySynthesizedAttributeType
    queryVariableNamesWithTypeName (SgNode * astNode, std::string matchingName);


  NameQuerySynthesizedAttributeType
    queryNameTypedefDeclarationNames (SgNode * astNode);


  NameQuerySynthesizedAttributeType
    queryNameTypeName (SgNode * astNode);


  enum TypeOfQueryTypeOneParameter
  {
    UnknownListElementType = 0,
    VariableNames = 1,
    VariableTypeNames = 2,
    FunctionDeclarationNames = 3,
    MemberFunctionDeclarationNames = 4,
    ClassDeclarationNames = 5,
    ArgumentNames = 6,
    ClassFieldNames = 7,
    UnionFieldNames = 8,
    StructFieldNames = 9,
    FunctionReferenceNames = 10,
    StructNames = 11,
    UnionNames = 12,
    TypedefDeclarationNames = 13,
    TypeNames = 14,
    END_OF_NAME_TYPE_LIST
  };

  enum TypeOfQueryTypeTwoParameters
  {
    UnknownListElementTypeTwoParameters = 0,
    VariableNamesWithTypeName = 1,
    END_OF_NODE_TYPE_LIST_TWO_PARAMETERS
  };


  /**************************************************************************************************************
   * The function
   *    std::function<Rose_STL_Container<std::string>(SgNode *)> getFunction(TypeOfQueryTypeOneParameter oneParam);
   * will return a functor wrapping the pre-implemented function for TypeOfQueryTypeOneParameter.
   **************************************************************************************************************/

  std::function<Rose_STL_Container<std::string>(SgNode *)> getFunction(TypeOfQueryTypeOneParameter oneParam);

  /**************************************************************************************************************
   * The function
   *    std::function<Rose_STL_Container<std::string>(SgNode *, std::string)>
   *       getFunction(TypeOfQueryTypeTwoParameters twoParam);
   * will return a functor wrapping the pre-implemented function for TypeOfQueryTypeTwoParameters.
   **************************************************************************************************************/

  std::function<Rose_STL_Container<std::string>(SgNode *, std::string)>
     getFunction(TypeOfQueryTypeTwoParameters twoParam);

  // get the SgNode's conforming to the test in querySolverFunction or
  // get the SgNode's conforming to the test in the TypeOfQueryTypeOneParamter the user specify.

  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, TypeOfQueryTypeOneParameter elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all strings in the sub-tree of the variable 'subTree' found by the 
   * preimplemented function 'elementReturnType'.
   *******************************************************************************************/
  ROSE_DLL_API NameQuerySynthesizedAttributeType
    querySubTree
    (SgNode * subTree,
     TypeOfQueryTypeOneParameter elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  ROSE_DLL_API NameQuerySynthesizedAttributeType queryNodeList
    (Rose_STL_Container< SgNode * > nodeList,
     TypeOfQueryTypeOneParameter elementReturnType);

  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, roseFunctionPointerOneParameter elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all stings in the sub-tree of the variable 'subTree' found by the 
   * function 'elementReturnType'.
   *******************************************************************************************/
  ROSE_DLL_API NameQuerySynthesizedAttributeType
    querySubTree
    (SgNode * subTree,
     roseFunctionPointerOneParameter elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  ROSE_DLL_API NameQuerySynthesizedAttributeType queryNodeList
    (Rose_STL_Container< SgNode * >nodeList,
     roseFunctionPointerOneParameter querySolverFunction);

  // get the SgNode's conforming to the test in querySolverFunction or
  // get the SgNode's conforming to the test in the TypeOfQueryTypeTwoParamters the user specify.
  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, SgNode* traversal, roseFunctionPointerTwoParameters elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all strings in the sub-tree of the variable 'subTree' found by the 
   * function 'querySolverFunction' given a second argument 'traversal'.
   *******************************************************************************************/
  ROSE_DLL_API NameQuerySynthesizedAttributeType
    querySubTree
    (SgNode * subTree,
     std::string traversal,
     roseFunctionPointerTwoParameters querySolverFunction,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  ROSE_DLL_API NameQuerySynthesizedAttributeType queryNodeList
    (Rose_STL_Container< SgNode * >nodeList,
     std::string targetNode,
     roseFunctionPointerTwoParameters querySolverFunction);


  /********************************************************************************************
   *
   * The function
   *   querySubTree (SgNode * subTree, SgNode* traversal, TypeOfQueryTypeTwoParameters elementReturnType,
   *        AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes);
   * returns a list of all strings in the sub-tree of the variable 'subTree' found by the 
   * preimplemented function 'elementReturnType' given a second argument 'traversal'.
   *******************************************************************************************/
  ROSE_DLL_API NameQuerySynthesizedAttributeType
    querySubTree
    (SgNode * subTree,
     std::string traversal,
     TypeOfQueryTypeTwoParameters elementReturnType,
     AstQueryNamespace::QueryDepth defineQueryType =
     AstQueryNamespace::AllNodes);

  ROSE_DLL_API NameQuerySynthesizedAttributeType queryNodeList
    (Rose_STL_Container< SgNode * >nodeList,
     std::string targetNode, TypeOfQueryTypeTwoParameters elementReturnType);

  // perform a query on a list<SgNode>


  // DQ (4/8/2004): Added query based on vector of variants



  /********************************************************************************
   * The function
   *  NameQuerySynthesizedAttributeType
   * queryMemoryPool(NodeFunctional nodeFunc , VariantVector* targetVariantVector = NULL)
   * will query the memory pool for IR nodes which satisfies the criteria specified in and 
   * returned by the predicate in the second argument in the nodes with a corresponding variant
   * in VariantVector.
   ********************************************************************************/
  template<typename NodeFunctional>
    NameQuerySynthesizedAttributeType
    queryMemoryPool(NodeFunctional nodeFunc , VariantVector* targetVariantVector = NULL)
    {
      return AstQueryNamespace::queryMemoryPool(nodeFunc,targetVariantVector);
    };


  /********************************************************************************
   * The function
   *      NameQuerySynthesizedAttributeType queryMemoryPool ( 
   *                   _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
   *                   VariantVector* ){
   * will on every node of the memory pool which has a corresponding variant in VariantVector
   * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
   ********************************************************************************/
  NameQuerySynthesizedAttributeType
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
  NameQuerySynthesizedAttributeType
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

  NameQuerySynthesizedAttributeType
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

  NameQuerySynthesizedAttributeType
    queryMemoryPool
    (
     TypeOfQueryTypeOneParameter elementReturnType,
     VariantVector* targetVariantVector = NULL);





}



// endif for ROSE_NAME_QUERY
#endif
