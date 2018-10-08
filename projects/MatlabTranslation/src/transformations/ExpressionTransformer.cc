
#include <string>

#include "rose.h"

#include "ExpressionTransformer.h"

#include "utility/utils.h"
#include "typeInference/FastNumericsRoseSupport.h"

namespace si = SageInterface;
namespace sb = SageBuilder;
namespace fn = FastNumericsRoseSupport;

namespace MatlabToCpp
{
  /**
   * This abstract class represents a general algorithm to transform an expression
   * to a new expression. You have to inherit the class and provide the
   * implementation to the abstract functions.
   */
  struct ExpressionTransformerBase
  {
      explicit
      ExpressionTransformerBase(VariantT sagekind)
      : nodeVariant(sagekind)
      {}

      void transform(SgProject *project)
      {
        Rose_STL_Container<SgNode*> nodesToTransform  = NodeQuery::querySubTree(project, nodeVariant);

        for(Rose_STL_Container<SgNode*>::iterator it = nodesToTransform.begin(); it != nodesToTransform.end(); ++it)
        {
          SgExpression *currentExpression = isSgExpression(*it);

          //Check if this node should be skipped. By default it is not skipped
          if(skipExpression(currentExpression))
          {
            continue;
          }

          SgExpression *transformedExpression = getTransformedExpression(currentExpression);

          // \todo set to false, but clone lhs and rhs before sticking them into the argument list
          si::replaceExpression(currentExpression, transformedExpression, true);
          std::cerr << "power out" << typeid(*currentExpression).name() << std::endl;
        }
      }

    protected:
      /// Transform the original expression to a new expression, the algorithm
      /// will take care of properly inserting it to the AST.
      virtual SgExpression* getTransformedExpression(SgExpression* originalExpression) = 0;

      /// Ask if a certain node should be skipped. Some nodes may need to be
      /// skipped.
      virtual bool skipExpression(SgExpression *expr)
      {
        return false;
      }

    private:
      VariantT nodeVariant;
  };


  /// An abstract class to transform a binary expression to a function call
  ///   with lhs and rhs as parameters.
  struct BinaryExpressionToFunctionCallTransformer : ExpressionTransformerBase
  {
    BinaryExpressionToFunctionCallTransformer(VariantT kind, std::string callee)
    : ExpressionTransformerBase(kind), calleename(callee)
    {}

    SgExpression* getTransformedExpression(SgExpression* originalExpression) /* override */
    {
      SgBinaryOp*        binaryOp = isSgBinaryOp(originalExpression);
      SgExpression*      lhs = binaryOp->get_lhs_operand();
      SgExpression*      rhs = binaryOp->get_rhs_operand();
      SgExprListExp*     parameters = sb::buildExprListExp(lhs, rhs);
      SgFunctionCallExp* functionCall =
            RoseUtils::createFunctionCall( calleename,
                                           si::getEnclosingScope(originalExpression),
                                           parameters
                                         );

      return functionCall;
    }

    std::string calleename;
  };


  // \todo move to util
  static inline
  bool isScalarType(SgType* t)
  {
    return isSgTypeMatrix(t) == NULL;
        // && isSgTupleType(t)  == NULL;
  }

  struct RightDivideToFunctionCallTransformer : BinaryExpressionToFunctionCallTransformer
  {
    RightDivideToFunctionCallTransformer()
    : BinaryExpressionToFunctionCallTransformer(V_SgDivideOp, "mrdivide")
    {}

    bool skipExpression(SgExpression* expr) ROSE_OVERRIDE
    {
      return isScalarType(fn::getInferredType(expr));
    }
  };

  //
  // factory functions

  /// Transforms A \ B to leftDivide(A, B)
  static inline
  BinaryExpressionToFunctionCallTransformer
  leftDivideTransformer()
  {
    return BinaryExpressionToFunctionCallTransformer(V_SgLeftDivideOp, "mldivide");
  }

  /// Transforms A ^ n to power(A, n)
  static inline
  BinaryExpressionToFunctionCallTransformer
  powerOpTransformer()
  {
    return BinaryExpressionToFunctionCallTransformer(V_SgPowerOp, "power");
  }

  /// Transforms A \ B -> rightDivide(A, B)
  static inline
  BinaryExpressionToFunctionCallTransformer
  elementwiseRightDivideTransformer()
  {
    return BinaryExpressionToFunctionCallTransformer(V_SgElementwiseDivideOp, "rdivide");
  }

  /// Transforms A .* B -> times(A, B)
  static
  BinaryExpressionToFunctionCallTransformer
  elementwiseMultiplyTransformer()
  {
    return BinaryExpressionToFunctionCallTransformer(V_SgElementwiseMultiplyOp, "times");
  }

  /// for A,B matrixes: Transforms A \ B to rightDivide(A, B)
  static
  RightDivideToFunctionCallTransformer
  rightDivideTransformer()
  {
    return RightDivideToFunctionCallTransformer();
  }


  //
  // functions calling transformers

  template <class Transformer>
  static inline
  void do_transform(Transformer transformer, SgProject* p)
  {
    transformer.transform(p);
  }

  template <class TransformerGen>
  static inline
  void _transform(TransformerGen gen, SgProject* p)
  {
    do_transform(gen(), p);
  }

  //
  // external function implementation

  void transformLeftDivide(SgProject* project)
  {
    _transform(leftDivideTransformer, project);
  }

  void transformElementwiseRightDivide(SgProject* project)
  {
    _transform(elementwiseRightDivideTransformer, project);
  }

  void transformElementwiseMultiply(SgProject* project)
  {
    _transform(elementwiseMultiplyTransformer, project);
  }

  void transformPowerOp(SgProject* project)
  {
    _transform(powerOpTransformer, project);
  }

  void transformRightDivide(SgProject* project)
  {
    _transform(rightDivideTransformer, project);
  }
}
