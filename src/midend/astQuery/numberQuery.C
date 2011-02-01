// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// include file for transformation specification support
// include "specification.h"
// include "globalTraverse.h"

// include "query.h"

// string class used if compiler does not contain a C++ string class
// include <roseString.h>

#include "numberQuery.h"

#define DEBUG_NUMBERQUERY 0

// #include "arrayTransformationSupport.h"


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

  NumberQuerySynthesizedAttributeType
NumberQuery::queryNumberOfArgsInParenthesisOperator (SgNode * astNode, string typeName)
{
  ROSE_ASSERT (astNode != 0);
  const char *matchingTypeName = typeName.data ();

  NumberQuerySynthesizedAttributeType returnNumberList;

  SgExprListExp *sageExprListExp = isSgExprListExp (astNode);

  switch (astNode->variantT ())
  {
    case V_SgExprListExp:       // SgExprListExp
      {
        // char *typeName = "XXX";
        int numberOfArgs = sageExprListExp->get_expressions ().size ();

#if DEBUG_NUMBERQUER
        printf ("Found a list of arguments \n");
        printf ("In case: numberOfArgs = %d  matchingTypeName = %s \n",numberOfArgs, matchingTypeName);
#endif

        returnNumberList.push_back (numberOfArgs);
        break;
      }

    case V_SgFunctionCallExp:   //SgExprCallExp
      {
        // const char *matchingFunctionName = "operator()";

        // Looking for different types of overloaded functions (of the correct type)
        SgFunctionCallExp *functionCallExp = isSgFunctionCallExp (astNode);
        ROSE_ASSERT (functionCallExp != NULL);

        const char *functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExp).data();

        ROSE_ASSERT (functionTypeName != NULL);

        if (functionTypeName == matchingTypeName)
        {
          ROSE_ASSERT (functionCallExp->get_args() != NULL);
          // ROSE_ASSERT (functionCallExp->get_args()->get_expressions().size() >= 0);
          int numberOfArgs = functionCallExp->get_args()->get_expressions().size();
          returnNumberList.push_back (numberOfArgs);
        }
        break;
      } /* End case FUNC_CALL */

    default:
      {
        // Nothing to do here (implemented to avoid g++ warnings about unhandled enum values)
        break;
      }

  } /* End switch-case */

  return returnNumberList;
}                               // End function queryNumberOfArgsInParenthesisOperator() 


  NumberQuerySynthesizedAttributeType
NumberQuery::queryNumberOfArgsInConstructor (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NumberQuerySynthesizedAttributeType returnNumberList;

  SgConstructorInitializer *sageConstructorInitializer = isSgConstructorInitializer (astNode);

  if (sageConstructorInitializer != NULL) {
    if (sageConstructorInitializer->get_args() != NULL)
    {
      ROSE_ASSERT (sageConstructorInitializer->get_args() != NULL);
      // ROSE_ASSERT (sageConstructorInitializer->get_args()->get_expressions().size() >= 0);

      int numberOfArgs = sageConstructorInitializer->get_args()->get_expressions().size();

      returnNumberList.push_back (numberOfArgs);
    }
    else
    {
      returnNumberList.push_back (0);
    }
  }

  return returnNumberList;
} // End function queryNumberOfArgsInConstructor() 


  NumberQuerySynthesizedAttributeType
NumberQuery::queryNumberOfOperands (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NumberQuerySynthesizedAttributeType returnNumberList;

  int numberOfArgs = 0;

  switch (astNode->variantT ())
  {
    case V_SgAddOp:
    case V_SgAndAssignOp:
    case V_SgAndOp:
    case V_SgArrowExp:
    case V_SgArrowStarOp:
    case V_SgAssignOp:
    case V_SgBitAndOp:
    case V_SgBitOrOp:
    case V_SgBitXorOp:
    case V_SgCommaOpExp:
    case V_SgDivAssignOp:
    case V_SgDivideOp:
    case V_SgDotExp:
    case V_SgDotStarOp:
    case V_SgEqualityOp:
    case V_SgGreaterOrEqualOp:
    case V_SgGreaterThanOp:
    case V_SgIntegerDivideOp:
    case V_SgIorAssignOp:
    case V_SgLessOrEqualOp:
    case V_SgLessThanOp:
    case V_SgLshiftAssignOp:
    case V_SgLshiftOp:
    case V_SgMinusAssignOp:
    case V_SgModAssignOp:
    case V_SgModOp:
    case V_SgMultAssignOp:
    case V_SgMultiplyOp:
    case V_SgNotEqualOp:
    case V_SgOrOp:
    case V_SgPlusAssignOp:
    case V_SgPntrArrRefExp:
    case V_SgRshiftAssignOp:
    case V_SgRshiftOp:
    case V_SgScopeOp:
    case V_SgSubtractOp:
    case V_SgXorAssignOp:
      if (isSgBinaryOp (astNode)->get_lhs_operand () != NULL)
        numberOfArgs += 1;
      if (isSgBinaryOp (astNode)->get_rhs_operand () != NULL)
        numberOfArgs += 1;
      break;

    case V_SgSizeOfOp:
      {
        ROSE_ASSERT (isSgTypeIdOp (astNode)->get_operand_expr () != NULL);
        numberOfArgs += 1;
        break;
      }

    case V_SgAddressOfOp:
    case V_SgBitComplementOp:
    case V_SgCastExp:
    case V_SgExpressionRoot:
    case V_SgMinusMinusOp:
    case V_SgMinusOp:
    case V_SgNotOp:
    case V_SgPlusPlusOp:
    case V_SgPointerDerefExp:
    case V_SgThrowOp:
    case V_SgUnaryAddOp:
      ROSE_ASSERT (isSgUnaryOp (astNode)->get_operand () != NULL);
      numberOfArgs += 1;
      break;

    default:
      {
        // Nothing to do here (implemented to avoid g++ warnings about unhandled enum values)
        break;
      }
  }                             /* End switch-case */

  if (numberOfArgs > 0)
  {
    returnNumberList.push_back (numberOfArgs);
    cout << numberOfArgs << endl;

#if DEBUG_NUMBERQUER
    printf ("\nHere is a declaration:Line = %d Columns = %d \n",
        ROSE::getLineNumber (isSgLocatedNode (astNode)),
        ROSE::getColumnNumber (isSgLocatedNode (astNode)));
#endif
  }

  return returnNumberList;
} // End function queryNumberOperands() 




  NumberQuerySynthesizedAttributeType
NumberQuery::queryNumberOfArgsInScalarIndexingOperator (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);

  const char *matchingTypeName = "int";

  NumberQuerySynthesizedAttributeType returnNumberList;

  SgExprListExp *sageExprListExp = isSgExprListExp (astNode);

  switch (astNode->variantT ())
  {
    case V_SgExprListExp:       // SgExprListExp
      {
        // char *typeName = "XXX";
        int numberOfArgs = sageExprListExp->get_expressions ().size ();

#if DEBUG_NUMBERQUER
        printf ("Found a list of arguments \n");
        printf ("In case: numberOfArgs = %d  matchingTypeName = %s \n",numberOfArgs, matchingTypeName);
#endif

        returnNumberList.push_back (numberOfArgs);
        break;
      }

    case V_SgFunctionCallExp:   //SgExprCallExp
      {
        // const char *matchingFunctionName = "operator()";

        // Looking for different types of overloaded functions (of the correct type)
        SgFunctionCallExp *functionCallExp = isSgFunctionCallExp (astNode);
        ROSE_ASSERT (functionCallExp != NULL);

        const char *functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExp).data();

        ROSE_ASSERT (functionTypeName != NULL);

        if (functionTypeName == matchingTypeName)
        {
          ROSE_ASSERT (functionCallExp->get_args() != NULL);
          // ROSE_ASSERT (functionCallExp->get_args()->get_expressions().size() >= 0);
          int numberOfArgs = functionCallExp->get_args()->get_expressions().size();
          returnNumberList.push_back (numberOfArgs);
        }
        break;
      } /* End case FUNC_CALL */

    default:
      {
        // Nothing to do here (implemented to avoid g++ warnings about unhandled enum values)
        break;
      }
  } /* End switch-case */

  return returnNumberList;
} // End function queryNumberOfArgsInScalarIndexingOperator() 


std::pointer_to_unary_function<SgNode*, NumberQuerySynthesizedAttributeType> NumberQuery::getFunction(NumberQuery::TypeOfQueryTypeOneParameter oneParam){
  NumberQuery::roseFunctionPointerOneParameter __x; 
  switch (oneParam)
  {
    case UnknownListElementType:
      {
        printf ("This is element number 0 in the list. It is not used to anything predefined.\n");
        ROSE_ASSERT (false);
      }
    case NumberOfArgsInConstructor:
      {
        __x = queryNumberOfArgsInConstructor;
        break;
      }
    case NumberOfOperands:
      {
        __x = queryNumberOfOperands;
        break;
      }

    default:
      {
        printf ("This is an invalid member of the enum  TypeOfQueryTypeOneParameter.\n");
        ROSE_ASSERT (false);
      }
  } /* End switch-case */
  return std::ptr_fun(__x);

}

std::pointer_to_binary_function<SgNode*, std::string, NumberQuerySynthesizedAttributeType > NumberQuery::getFunction(NumberQuery::TypeOfQueryTypeTwoParameters twoParam){
  NumberQuery::roseFunctionPointerTwoParameters __x;
  switch (twoParam)
  {
    case UnknownListElementTypeTwoParameters:
      {
        printf ("This is element number 0 in the list. It is not used to anything predefined.\n");
        ROSE_ASSERT (false);
      }
    case NumberOfArgsInParanthesisOperator:
      {
        __x =
          queryNumberOfArgsInParenthesisOperator;
        break;
      }

    default:
      {
        printf ("This is an invalid member of the enum  TypeOfQueryTypeOneParameter.\n");
        ROSE_ASSERT (false);
      }
  }
  return std::ptr_fun(__x);
}


NumberQuerySynthesizedAttributeType NumberQuery::querySubTree 
( SgNode * subTree,
  NumberQuery::TypeOfQueryTypeOneParameter elementReturnType,
  AstQueryNamespace::QueryDepth defineQueryType){
  return AstQueryNamespace::querySubTree(subTree, getFunction(elementReturnType), defineQueryType);
}

// get the SgNode's conforming to the test in querySolverFunction or
// get the SgNode's conforming to the test in the TypeOfQueryTypeTwoParamters the user specify.
NumberQuerySynthesizedAttributeType NumberQuery::querySubTree 
( SgNode * subTree,
  std::string traversal,
  NumberQuery::roseFunctionPointerTwoParameters querySolverFunction,
  AstQueryNamespace::QueryDepth defineQueryType){
  return AstQueryNamespace::querySubTree(subTree, 
      std::bind2nd(std::ptr_fun(querySolverFunction),traversal), defineQueryType);


};
NumberQuerySynthesizedAttributeType NumberQuery::querySubTree
( SgNode * subTree,
  std::string traversal,
  NumberQuery::TypeOfQueryTypeTwoParameters elementReturnType,
  AstQueryNamespace::QueryDepth defineQueryType ){
  return AstQueryNamespace::querySubTree(subTree, 
      std::bind2nd(getFunction(elementReturnType),traversal), defineQueryType);
};



// perform a query on a list<SgNode>
NumberQuerySynthesizedAttributeType NumberQuery::queryNodeList 
( Rose_STL_Container< SgNode * >nodeList,
  NumberQuery::roseFunctionPointerOneParameter querySolverFunction){
  return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),
      std::ptr_fun(querySolverFunction));
};
NumberQuerySynthesizedAttributeType NumberQuery::queryNodeList 
( Rose_STL_Container<SgNode*> nodeList,
  NumberQuery::TypeOfQueryTypeOneParameter elementReturnType ){
  return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),getFunction(elementReturnType));

};

NumberQuerySynthesizedAttributeType
NumberQuery::querySubTree
(SgNode * subTree,
 NumberQuery::roseFunctionPointerOneParameter elementReturnType,
 AstQueryNamespace::QueryDepth defineQueryType 
 ){

  return  AstQueryNamespace::querySubTree(subTree,
      std::ptr_fun(elementReturnType),defineQueryType);

};



NumberQuerySynthesizedAttributeType NumberQuery::queryNodeList 
( Rose_STL_Container<SgNode*> nodeList,
  std::string targetNode,
  NumberQuery::roseFunctionPointerTwoParameters querySolverFunction ){
  return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),
      std::bind2nd(std::ptr_fun(querySolverFunction), targetNode));
  //                                  std::bind2nd(getFunction(elementReturnType),traversal), defineQueryType);

};
NumberQuerySynthesizedAttributeType NumberQuery::queryNodeList 
( Rose_STL_Container<SgNode*> nodeList,
  std::string targetNode,
  NumberQuery::TypeOfQueryTypeTwoParameters elementReturnType ){
  return AstQueryNamespace::queryRange(nodeList.begin(), nodeList.end(),
      std::bind2nd(getFunction(elementReturnType), targetNode));

};



/********************************************************************************
 * The function
 *      NumberQuerySynthesizedAttributeType queryMemoryPool ( SgNode * subTree,
 *                   _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
 ********************************************************************************/
NumberQuerySynthesizedAttributeType
NumberQuery::queryMemoryPool
(
 std::string traversal,
 NumberQuery::roseFunctionPointerTwoParameters querySolverFunction, VariantVector* targetVariantVector)
{
  return AstQueryNamespace::queryMemoryPool(
      std::bind2nd(std::ptr_fun(querySolverFunction),traversal), targetVariantVector);

};


/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree,
 *                   _Result (*__x)(SgNode*),
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the action specified by the second argument and return a NodeQuerySynthesizedAttributeType.
 ********************************************************************************/
NumberQuerySynthesizedAttributeType
NumberQuery::queryMemoryPool
(
 std::string traversal,
 NumberQuery::roseFunctionPointerOneParameter querySolverFunction, VariantVector* targetVariantVector)
{
  return  AstQueryNamespace::queryMemoryPool(
      std::ptr_fun(querySolverFunction),targetVariantVector);


};

/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree, SgNode*,
 *                   TypeOfQueryTypeTwoParameters,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the predefined action specified by the second argument and return a 
 * NodeQuerySynthesizedAttributeType.
 ********************************************************************************/

NumberQuerySynthesizedAttributeType
NumberQuery::queryMemoryPool
(
 std::string traversal,
 NumberQuery::TypeOfQueryTypeTwoParameters elementReturnType,
 VariantVector* targetVariantVector)
{
  return AstQueryNamespace::queryMemoryPool( 
      std::bind2nd(getFunction(elementReturnType),traversal), targetVariantVector);


};

/********************************************************************************
 * The function
 *      _Result queryMemoryPool ( SgNode * subTree,
 *                   TypeOfQueryTypeOneParameter,
 *                   VariantVector* ){
 * will on every node of the memory pool which has a corresponding variant in VariantVector
 * performa the predefined action specified by the second argument and return a 
 * NodeQuerySynthesizedAttributeType.
 ********************************************************************************/

NumberQuerySynthesizedAttributeType
NumberQuery::queryMemoryPool
(
 NumberQuery::TypeOfQueryTypeOneParameter elementReturnType,
 VariantVector* targetVariantVector)
{

  return AstQueryNamespace::queryMemoryPool(getFunction(elementReturnType), targetVariantVector);

};




