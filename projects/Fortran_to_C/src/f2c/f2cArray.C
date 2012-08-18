#include "f2c.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;


SgExpression* Fortran_to_C::getFortranDimensionSize(SgExpression* inputExpression)
{
  SgExpression* indexExpression;
  SgSubscriptExpression* subscriptExpression = isSgSubscriptExpression(inputExpression);
  /*
    This is for the 1st type of array declaration: a(10,15,20)
  */
  if(subscriptExpression == NULL)
  {
    indexExpression = deepCopy(inputExpression);
  }
  else
  /*
    This is for the 2nd type of array declaration: a(1:10,5:15,10:20)
    Actual dimension size = upperBound - lowerBound + 1
  */
  {
    indexExpression = buildSubtractOp(deepCopy(subscriptExpression->get_upperBound()),
                                      deepCopy(subscriptExpression->get_lowerBound()));
    indexExpression = buildAddOp(indexExpression,buildIntVal(1));
  }
  return indexExpression;
}

/******************************************************************************************************************************/
/* 
  Translate the array declaration without linearize the multi-dimensional array
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateArrayDeclaration(SgArrayType* originalArrayType)
{
  // Get dim_info
  SgExprListExp* dimInfo = originalArrayType->get_dim_info();
  // Get dim list
  SgExpressionPtrList dimExpressionPtrList = dimInfo->get_expressions();
  // Get array base_type
  SgType* baseType = originalArrayType->get_base_type();

  Rose_STL_Container<SgExpression*>::iterator j =  dimExpressionPtrList.begin();
  SgExpression* indexExpression = getFortranDimensionSize(*j);
  //std::cout << "array rank:" << originalArrayType->get_rank() << std::endl;
  if(originalArrayType->get_rank() == 1)
  {
    originalArrayType->set_base_type(baseType);
    originalArrayType->set_index(indexExpression);
    indexExpression->set_parent(originalArrayType);
  }
  else
  {
    SgArrayType* newType = buildArrayType(baseType,indexExpression);
    baseType->set_parent(newType);
    j = j + 1;
    for(; j< (dimExpressionPtrList.end()-1); ++j)
    {
      indexExpression = getFortranDimensionSize(*j);
      baseType = newType;
      newType = buildArrayType(baseType,indexExpression);
      baseType->set_parent(newType);
    }
    j = dimExpressionPtrList.end()-1;
    indexExpression = getFortranDimensionSize(*j);
    originalArrayType->set_base_type(newType);
    originalArrayType->set_index(indexExpression);
    indexExpression->set_parent(originalArrayType);
  }
}


/******************************************************************************************************************************/
/* 
  Replace all array subscripts into single dimension
  This section is for the declaration.  Multi-dimensional arrays are converted to single-dimensional array.
*/
/******************************************************************************************************************************/
void Fortran_to_C::linearizeArrayDeclaration(SgArrayType* originalArrayType)
{
  // Get dim_info
  SgExprListExp* dimInfo = originalArrayType->get_dim_info();
  // Get dim list
  SgExpressionPtrList dimExpressionPtrList = dimInfo->get_expressions();

  SgExpression* newDimExpr; 
  Rose_STL_Container<SgExpression*>::iterator j =  dimExpressionPtrList.begin();
  while(j != dimExpressionPtrList.end())
  {
    SgExpression* indexExpression = getFortranDimensionSize(*j);

    /*
      Total array size is equal to the multiplication of all individual dimension size.
    */
    if(j != dimExpressionPtrList.begin()){
        newDimExpr = buildMultiplyOp(newDimExpr, indexExpression);
    }
    else
    /*
      If it's first dimension, array size is just its first dimension size.
    */
    {
      newDimExpr = indexExpression;
    }
    ++j;
  }
  // calling set_index won't replace the default index expression.  I have to delete the default manually.
  deepDelete(originalArrayType->get_index());
  originalArrayType->set_index(newDimExpr);
  newDimExpr->set_parent(originalArrayType);
  originalArrayType->set_rank(1); 
}


/******************************************************************************************************************************/
/* 
  Translate the array subscript, without linearalize the multi-dimensional array
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateArraySubscript(SgPntrArrRefExp* pntrArrRefExp)
{
  // get lhs operand
  SgVarRefExp*  arrayName = isSgVarRefExp(pntrArrRefExp->get_lhs_operand());
  SgExpression* baseExp = isSgExpression(arrayName);
  // get array symbol
  SgVariableSymbol* arraySymbol = arrayName->get_symbol();
  // get array type and dim_info
  SgArrayType* arrayType = isSgArrayType(arraySymbol->get_type());
  ROSE_ASSERT(arrayType);
  SgExprListExp* dimInfo = arrayType->get_dim_info();

  // get rhs operand
  SgExprListExp*  arraySubscript = isSgExprListExp(pntrArrRefExp->get_rhs_operand());
  /*
    No matter it is single or multi dimensional array,  pntrArrRefExp always has a
    child, SgExprListExp, to store the subscript information.
  */
  if(arraySubscript != NULL)
  {
    // get the list of subscript
    SgExpressionPtrList subscriptExprList = arraySubscript->get_expressions();
    // get the list of dimension inforamtion from array definition.
    SgExpressionPtrList dimExpressionPtrList = dimInfo->get_expressions();

    // Create new SgExpressionPtrList for the linearalized array subscript. 
    SgExpressionPtrList newSubscriptExprList;

    // rank info has to match between subscripts and dim_info
    ROSE_ASSERT(arraySubscript->get_expressions().size() == dimInfo->get_expressions().size());

    Rose_STL_Container<SgExpression*>::iterator j1 =  subscriptExprList.begin();
    Rose_STL_Container<SgExpression*>::iterator j2 =  dimExpressionPtrList.begin();
    SgExpression* newIndexExp = get0basedIndex(*j1, *j2);
    if(subscriptExprList.size() == 1)
    {
      pntrArrRefExp->set_rhs_operand(newIndexExp);
    }
    else
    {
      SgPntrArrRefExp* newPntrArrRefExp = buildPntrArrRefExp(baseExp, newIndexExp);
      baseExp->set_parent(newPntrArrRefExp);
      j1 = j1 + 1;
      j2 = j2 + 1;
      for(; j1< (subscriptExprList.end()-1); ++j1, ++j2)
      {
        SgExpression* newIndexExp = get0basedIndex(*j1, *j2);
        baseExp = isSgExpression(newPntrArrRefExp);
        newPntrArrRefExp = buildPntrArrRefExp(baseExp, newIndexExp);
        baseExp->set_parent(newPntrArrRefExp);
      }
      SgExpression* newIndexExp = get0basedIndex(*j1, *j2);
      pntrArrRefExp->set_lhs_operand(newPntrArrRefExp);
      pntrArrRefExp->set_rhs_operand(newIndexExp);
      newIndexExp->set_parent(pntrArrRefExp);
    }
  }
}

/******************************************************************************************************************************/
/* 
  Replace all array subscripts into single dimension
  This section is for the array references.  
  Subscripts for multi-dimensional arrays are transformed into single-dimensional array subscript.
*/
/******************************************************************************************************************************/
void Fortran_to_C::linearizeArraySubscript(SgPntrArrRefExp* pntrArrRefExp)
{
  // get lhs operand
  SgVarRefExp*  arrayName = isSgVarRefExp(pntrArrRefExp->get_lhs_operand());
  // get array symbol
  SgVariableSymbol* arraySymbol = arrayName->get_symbol();
  // get array type and dim_info
  SgArrayType* arrayType = isSgArrayType(arraySymbol->get_type());
  ROSE_ASSERT(arrayType);
  SgExprListExp* dimInfo = arrayType->get_dim_info();

  // get rhs operand
  SgExprListExp*  arraySubscript = isSgExprListExp(pntrArrRefExp->get_rhs_operand());
  /*
    No matter it is single or multi dimensional array,  pntrArrRefExp always has a
    child, SgExprListExp, to store the subscript information.
  */
  if(arraySubscript != NULL)
  {
    // get the list of subscript
    SgExpressionPtrList subscriptExprList = arraySubscript->get_expressions();
    // get the list of dimension inforamtion from array definition.
    SgExpressionPtrList dimExpressionPtrList = dimInfo->get_expressions();

    // Create new SgExpressionPtrList for the linearalized array subscript. 
    SgExpressionPtrList newSubscriptExprList;

    // rank info has to match between subscripts and dim_info
    ROSE_ASSERT(arraySubscript->get_expressions().size() == dimInfo->get_expressions().size());
   
    /*
      The subscript conversion is following this example:
      case 1:
      dimension a(d1,d2,d3,d4)    ====>   dimension a(d1*d2*d3*d4)
      a(s1,s2,s3,s4)              ====>   a(s1-1 + d1*(s2-1 + d2*( s3-1 + d3*(s4-1))))

      case 2:
      dimension a(d1L:d1H,d2L:d2H)    ====>   dimension a((d1H-d1L+1)*(d2H-d2L+1))
      a(s1,s2)              ====>   a(s1-d1L + (d1H-d1L+1)*(s2-d2L)) 
    */ 
    Rose_STL_Container<SgExpression*>::reverse_iterator j1 =  subscriptExprList.rbegin();
    Rose_STL_Container<SgExpression*>::reverse_iterator j2 =  dimExpressionPtrList.rbegin();
    // Need to know current size of both current and previous dimension
    SgExpression* newSubscript;
    while((j1 != subscriptExprList.rend()) && (j2 != dimExpressionPtrList.rend()))
    {
      //  get the lowerBound for each dimension
      SgExpression* newDimIndex;
      SgExpression* dimSize;
      /*  
        get the dimension size at each dimension
      */
      SgSubscriptExpression* subscriptExpression = isSgSubscriptExpression(*j2);
      /*
        This is for the 1st type of array declaration: a(10,15,20)
        Fortran is 1-based array.  Lowerbound is 1 by default.
      */
      if(subscriptExpression == NULL)
      {
        dimSize = deepCopy(*j2);
      }
      /*
        This is for the 2nd type of array declaration: a(1:10,5:15,10:20)
        Actual dimension size = upperBound - lowerBound + 1
      */
      else
      {
        dimSize = buildAddOp(buildSubtractOp(deepCopy(subscriptExpression->get_upperBound()),
                                             deepCopy(subscriptExpression->get_lowerBound())),
                                             buildIntVal(1));
      }

      // convert the 1-based subscript to 0-based subscript
      newDimIndex = get0basedIndex(*j1, *j2); 
      if(j1 != subscriptExprList.rbegin())
      {
        newSubscript = buildAddOp(newDimIndex,
                                  buildMultiplyOp(dimSize,newSubscript));
      }
      else
      {
        newSubscript = newDimIndex;
        delete(dimSize);
      }
      ++j1;
      ++j2;
    } // end of while loop

    newSubscriptExprList.push_back(newSubscript);
    SgExprListExp* newSubscriptList = buildExprListExp(newSubscriptExprList);
    // un-link and remove the rhs operand
    pntrArrRefExp->get_rhs_operand()->set_parent(NULL);
    deepDelete(pntrArrRefExp->get_rhs_operand());
    // add the new subscriptExpression into rhs operand
    pntrArrRefExp->set_rhs_operand(newSubscriptList);
    newSubscriptList->set_parent(pntrArrRefExp);
    
  } // end of arraySubscript != NULL
}

SgExpression* Fortran_to_C::get0basedIndex(SgExpression* subscript, SgExpression* dimInfo)
{
  SgExpression* dimLowerBound;
  SgExpression* newDimIndex;
  SgSubscriptExpression* subscriptExpression = isSgSubscriptExpression(dimInfo);
  if(subscriptExpression == NULL)
  {
    dimLowerBound = buildIntVal(1);
  }
  else
  {
    dimLowerBound = deepCopy(subscriptExpression->get_lowerBound());
  }

  // convert the 1-based subscript to 0-based subscript
  newDimIndex = buildSubtractOp(deepCopy(subscript), dimLowerBound);

  return newDimIndex;
}
