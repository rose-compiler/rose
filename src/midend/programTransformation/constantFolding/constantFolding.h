// This file implements constant folding, but leverages the work done
// by the frontend and adds support for constant folding on any transformations
// introduced after the frontend processing (the transformations are typically
// not seen by the frontend, so we have to handle constant folding directly
// for transformations.  Since many transformations are introduced automatically
// there is a substantial requirement for constant folding to clean the generated 
// code up a bit.  However, since the backend compiler (vendor's compiler) 

#ifndef ROSE_CONSTANT_FOLDING_H
#define ROSE_CONSTANT_FOLDING_H

namespace ConstantFolding {

// Build an inherited attribute for the tree traversal to skip constant folded expressions
class ConstantFoldingInheritedAttribute
   {
     public:
          bool isConstantFoldedValue;
          bool isPartOfFoldedExpression;
          bool internalTestingAgainstFrontend;

      //! Specific constructors are required
          ConstantFoldingInheritedAttribute()
             : isConstantFoldedValue(false), 
               isPartOfFoldedExpression(false),
               internalTestingAgainstFrontend(false)
             {};

       // Need to implement the copy constructor
          ConstantFoldingInheritedAttribute ( const ConstantFoldingInheritedAttribute & X )
             : isConstantFoldedValue(X.isConstantFoldedValue), 
               isPartOfFoldedExpression(X.isPartOfFoldedExpression),
               internalTestingAgainstFrontend(X.internalTestingAgainstFrontend)
             {};
   };

class ConstantFoldingSynthesizedAttribute
   {
     public:
          SgValueExp* newValueExp;

          ConstantFoldingSynthesizedAttribute() : newValueExp(NULL) {};
          ConstantFoldingSynthesizedAttribute ( const ConstantFoldingSynthesizedAttribute & X )
             : newValueExp(X.newValueExp) {};
   };

class ConstantFoldingTraversal
   : public SgTopDownBottomUpProcessing<ConstantFoldingInheritedAttribute,ConstantFoldingSynthesizedAttribute>
   {
     public:
       // Functions required by the rewrite mechanism
          ConstantFoldingInheritedAttribute evaluateInheritedAttribute (
             SgNode* n, 
             ConstantFoldingInheritedAttribute inheritedAttribute );
          ConstantFoldingSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* n,
             ConstantFoldingInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };
//! This is the external interface of constant folding:
//It relies on the EDG frontend to do constant folding by default. 
// The original source code pass trough EDG will have all constant fold already  (not know how)
// Only new constant folding opportunities introduced by custom transformations will be 
// actually handled by this function itself.
// Note: It will fold children nodes of the input node to some constants, 
// not folding the input node itself. 
ROSE_DLL_API void constantFoldingOptimization(SgNode* n, bool internalTestingAgainstFrontend = false);

// DQ (6/13/2015): Added support to return the constant valued expression.
ROSE_DLL_API SgValueExp* returnConstantFoldedValueExpression(SgNode* n, bool internalTestingAgainstFrontend = false);

// ***************************************************************************
// Constant un-folding is implemented here as a test to verify the correctness 
// of the constant folding.
//    As an example: "int x = 3;" is transformed to be "int x = 1 + 2;"
// This transformation permits automated testing of the constant folding.
// ***************************************************************************

// Build a synthesized attribute for the tree traversal to unfold constants so that we can test the constant folding
class ConstantUnFoldingSynthesizedAttribute
   {
     public:
          SgExpression* newExp;

          ConstantUnFoldingSynthesizedAttribute() : newExp(NULL) {};
          ConstantUnFoldingSynthesizedAttribute ( const ConstantUnFoldingSynthesizedAttribute & X )
             : newExp(X.newExp) {};
   };

class ConstantUnFoldingTraversal
   : public SgBottomUpProcessing<ConstantUnFoldingSynthesizedAttribute>
   {
     public:
       // Functions required by the traversal mechanism
          ConstantUnFoldingSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* n,
          // ConstantUnFoldingInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// This test transforms the AST to turn constant values into expressions that have the 
// same value. This is a test for the constant folding optimization implemented above.
ROSE_DLL_API void constantUnFoldingTest(SgNode* n);

// We move many template functions into this header since they are generally useful for others to use.
// Liao 8/1/2018

// A helper function: get value if it fits into int type
// T1: the internal value's type
// T2: the concrete value exp type
template <typename T1, typename T2> 
 T1 cf_get_value_t(T2* sg_value_exp)
{
  return sg_value_exp->get_value();
}

//! Calculate the  result of a binary operation on two constant float-kind values, 
//   Integer-type-only binary operations are excluded : %, <<, >>, &, ^, |
// We ignore pointer and complex types for constant folding
//   none_integer types: floating point types
// The operators compatible with floating-point types are
//  + - * / <, > <=, >=, ==, !=  &&, ||
//
template<typename T>
T calculate_float_t (SgBinaryOp* binaryOperator, T lhsValue, T rhsValue)
{
// DQ (3/27/2017): Initialize this to avoid Clang warning: from default case.
// T foldedValue; // to be converted to result type   
  T foldedValue = 0; // to be converted to result type   

  switch (binaryOperator->variantT())
  {
    // Basic arithmetic 
    case V_SgAddOp:
      {
        foldedValue = lhsValue + rhsValue;
        break;
      }
    case V_SgSubtractOp:
      {
        foldedValue = lhsValue - rhsValue;
        break;
      }
    case V_SgMultiplyOp:
      {
        foldedValue = lhsValue * rhsValue;
        break;
      }
    case V_SgDivideOp:
      {
        foldedValue = lhsValue / rhsValue;
        break;
      }
     // Fortran only? a**b
      //    case V_SgExponentiationOp: 
      //      {
      //        foldedValue = lhsValue ** rhsValue;
      //        break;
      //      }

    case V_SgIntegerDivideOp://TODO what is this ??
      {
        foldedValue = lhsValue / rhsValue;
        break;
      }
    // logic operations
    case V_SgAndOp:
      {
        foldedValue = lhsValue && rhsValue;
        break;
      }

    case V_SgOrOp:
      {
        foldedValue = lhsValue || rhsValue;
        break;
      }

      // relational operations
    case V_SgEqualityOp:
      {
        foldedValue = (lhsValue == rhsValue);
        break;
      }
    case V_SgNotEqualOp:
      {
        foldedValue = (lhsValue != rhsValue);
        break;
      }
    case V_SgGreaterOrEqualOp:
      {
        foldedValue = (lhsValue >= rhsValue);
        break;
      }
    case V_SgGreaterThanOp:
      {
        foldedValue = (lhsValue > rhsValue);
        break;
      }
    case V_SgLessOrEqualOp:
      {
        foldedValue = (lhsValue <= rhsValue);
        break;
      }
    case V_SgLessThanOp:
      {
        foldedValue = (lhsValue < rhsValue);
        break;
      }
   default:
      {
        std::cerr<<"warning: calculuate - unhandled operator type:"<<binaryOperator->class_name()<<std::endl;
        //ROSE_ASSERT(false); // not every binary operation type can be evaluated
      }

  } // end switch

  return foldedValue;
}
//! string type and binary operator: the allowed operations on string values
// + , <, >, <=, >=, ==, !=  
template<typename T>
T calculate_string_t (SgBinaryOp* binaryOperator, T lhsValue, T rhsValue)
{
  T foldedValue; // to be converted to result type   

  switch (binaryOperator->variantT())
  {
    // Basic arithmetic 
    case V_SgAddOp:
      {
        foldedValue = lhsValue + rhsValue;
        break;
      }
      // relational operations
    case V_SgEqualityOp:
      {
        foldedValue = (lhsValue == rhsValue);
        break;
      }
    case V_SgNotEqualOp:
      {
        foldedValue = (lhsValue != rhsValue);
        break;
      }
    case V_SgGreaterOrEqualOp:
      {
        foldedValue = (lhsValue >= rhsValue);
        break;
      }
    case V_SgGreaterThanOp:
      {
        foldedValue = (lhsValue > rhsValue);
        break;
      }
    case V_SgLessOrEqualOp:
      {
        foldedValue = (lhsValue <= rhsValue);
        break;
      }
    case V_SgLessThanOp:
      {
        foldedValue = (lhsValue < rhsValue);
        break;
      }
    default:
      {
        std::cerr<<"warning: calculate_string_t - unacceptable operator type:"<<binaryOperator->class_name()<<std::endl;
        ROSE_ABORT();
      }
  } // end switch
  return foldedValue;
}
// For T type which is compatible for all binary operators we are interested in.
template<typename T>
T calculate_t (SgBinaryOp* binaryOperator, T lhsValue, T rhsValue)
{
  // DQ (3/27/2017): Initialize this value to avoid Clang warning from default case.
  // T foldedValue; // to be converted to result type   
  T foldedValue = 0; // to be converted to result type   

  switch (binaryOperator->variantT())
  {
    // integer-exclusive oprations
    case V_SgModOp:
      {
        foldedValue = lhsValue % rhsValue;
        break;
      }
    case V_SgLshiftOp:
      {
        foldedValue = (lhsValue << rhsValue);
        break;
      }
    case V_SgRshiftOp:
      {
        foldedValue = (lhsValue >> rhsValue);
        break;
      }
      // bitwise operations
    case V_SgBitAndOp:
      {
        foldedValue = lhsValue & rhsValue;
        break;
      }
    case V_SgBitOrOp:
      {
        foldedValue = lhsValue | rhsValue;
        break;
      }
    case V_SgBitXorOp:
      {
        foldedValue = lhsValue ^ rhsValue;
        break;
      }
   // non-integer-exclusive operations   
     case V_SgAddOp:
      {
        foldedValue = lhsValue + rhsValue;
        break;
      }
    case V_SgSubtractOp:
      {
        foldedValue = lhsValue - rhsValue;
        break;
      }
    case V_SgMultiplyOp:
      {
        foldedValue = lhsValue * rhsValue;
        break;
      }
    case V_SgDivideOp:
      {
        foldedValue = lhsValue / rhsValue;
        break;
      }
     // Fortran only? a**b
      //    case V_SgExponentiationOp: 
      //      {
      //        foldedValue = lhsValue ** rhsValue;
      //        break;
      //      }

    case V_SgIntegerDivideOp://TODO what is this ??
      {
        foldedValue = lhsValue / rhsValue;
        break;
      }
    // logic operations
    case V_SgAndOp:
      {
        foldedValue = lhsValue && rhsValue;
        break;
      }

    case V_SgOrOp:
      {
        foldedValue = lhsValue || rhsValue;
        break;
      }

      // relational operations
    case V_SgEqualityOp:
      {
        foldedValue = (lhsValue == rhsValue);
        break;
      }
    case V_SgNotEqualOp:
      {
        foldedValue = (lhsValue != rhsValue);
        break;
      }
    case V_SgGreaterOrEqualOp:
      {
        foldedValue = (lhsValue >= rhsValue);
        break;
      }
    case V_SgGreaterThanOp:
      {
        foldedValue = (lhsValue > rhsValue);
        break;
      }
    case V_SgLessOrEqualOp:
      {
        foldedValue = (lhsValue <= rhsValue);
        break;
      }
    case V_SgLessThanOp:
      {
        foldedValue = (lhsValue < rhsValue);
        break;
      }
   default:
      {
        std::cerr<<"warning: calculuate - unhandled operator type:"<<binaryOperator->class_name()<<std::endl;
        //ROSE_ASSERT(false); // not every binary operation type can be evaluated
      }
  }
  return foldedValue;
}

// For T type which is compatible for all unary operators we are interested in.
template<typename T>
T calculate_u_t (SgUnaryOp* unaryOperator, T theValue)
{
  T foldedValue = 0; // to be converted to result type
  switch (unaryOperator->variantT())
  {
    // integer-exclusive oprations
    case V_SgMinusOp:
      {
        foldedValue = -theValue;
        break;
      }
   default:
      {
        std::cerr<<"warning: calculuate - unhandled operator type:"<<unaryOperator->class_name()<<std::endl;
      }
  }
  return foldedValue;
}



// T1: a SgValExp's child SAGE class type, 
// T2: its internal storage C type for its value
template <typename T1, typename T2> 
static SgValueExp* buildResultValueExp_t (SgBinaryOp* binaryOperator, SgValueExp* lhsValue, SgValueExp* rhsValue)
{
  SgValueExp* result = NULL;
  T1* lhs_v = dynamic_cast<T1*>(lhsValue);
  T1* rhs_v = dynamic_cast<T1*>(rhsValue);
  ROSE_ASSERT(lhs_v);
  ROSE_ASSERT(rhs_v);

  T2 lhs = cf_get_value_t<T2>(lhs_v);
  T2 rhs = cf_get_value_t<T2>(rhs_v);
  T2 foldedValue ;
  foldedValue = calculate_t(binaryOperator,lhs,rhs);
  result = new T1(foldedValue,"");
  ROSE_ASSERT(result != NULL);
  SageInterface::setOneSourcePositionForTransformation(result);
  return result;
}

// T1: a SgValExp's child SAGE class type, 
// T2: its internal storage C type for its value
template <typename T1, typename T2>
static SgValueExp* buildResultValueExp_u_t (SgUnaryOp* unaryOperator, SgValueExp* theValue)
{
  SgValueExp* result = NULL;
  T1* t1_v = dynamic_cast<T1*>(theValue);
  ROSE_ASSERT(t1_v);
  T2 t2_v = cf_get_value_t<T2>(t1_v);
  T2 foldedValue ;
  foldedValue = calculate_u_t(unaryOperator,t2_v);
  result = new T1(foldedValue,"");
  ROSE_ASSERT(result != NULL);
  SageInterface::setOneSourcePositionForTransformation(result);
  return result;
}

// T1: a SgValExp's child SAGE class type,
// T2: its internal storage C type for its value
// for T2 types (floating point types) which are incompatible with integer-only operations: e.g. if T2 is double, then % is now allowed
template <typename T1, typename T2> 
static SgValueExp* buildResultValueExp_float_t (SgBinaryOp* binaryOperator, SgValueExp* lhsValue, SgValueExp* rhsValue)
{
  SgValueExp* result = NULL;
  T1* lhs_v = dynamic_cast<T1*>(lhsValue);
  T1* rhs_v = dynamic_cast<T1*>(rhsValue);
  ROSE_ASSERT(lhs_v);
  ROSE_ASSERT(rhs_v);

  T2 lhs = cf_get_value_t<T2>(lhs_v);
  T2 rhs = cf_get_value_t<T2>(rhs_v);
  T2 foldedValue ;
  foldedValue = calculate_float_t(binaryOperator,lhs,rhs);
  result = new T1(foldedValue,"");
  ROSE_ASSERT(result != NULL);
  SageInterface::setOneSourcePositionForTransformation(result);
  return result;
}

// end of ConstantFolding namespace
}


#endif
