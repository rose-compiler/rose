#include <map>
#include <algorithm>
#include <iostream>
#include <string>
#include <boost/foreach.hpp>

#include "MatlabSimpleTransformer.h"

#include "rose.h"
#include "sageGeneric.h"

#include "typeInference/FastNumericsRoseSupport.h"
#include "utility/utils.h"
#include "utility/FunctionReturnAttribute.h"

// #include "ast/SgColonExpression.h"

namespace sb = SageBuilder;
namespace si = SageInterface;
namespace fn = FastNumericsRoseSupport;
namespace ru = RoseUtils;

namespace
{
  template <class SageNode>
  struct TransformExecutor
  {
    typedef void (*TransformExecutorFn)(SageNode*);

    explicit
    TransformExecutor(TransformExecutorFn fun)
    : fn(fun)
    {}

    void handle(SgNode& n)   { std::cerr << "Unkown type: " << typeid(n).name() << std::endl; ROSE_ASSERT(false); }
    void handle(SageNode& n) { fn(&n); }

    TransformExecutorFn fn;
  };

  template <class SageNode>
  static inline
  TransformExecutor<SageNode>
  createTransformExecutor(void (* fn)(SageNode*))
  {
    return TransformExecutor<SageNode>(fn);
  }

  template <class TFn>
  static
  void forAllNodes(SgProject* proj, TFn fn, VariantT v_sageNode)
  {
    Rose_STL_Container<SgNode*> nodes = NodeQuery::querySubTree(proj, v_sageNode);

    for(Rose_STL_Container<SgNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
      sg::dispatch(createTransformExecutor(fn), *it);
    }
  }
}

namespace MatlabToCpp
{
  //
  // ForloopTransformer

  namespace
  {
    void tfForLoop(SgMatlabForStatement* matlabFor)
    {
      SgVarRefExp*      index = isSgVarRefExp(matlabFor->get_index());
      ROSE_ASSERT(index);

      SgScopeStatement* body = isSgScopeStatement(matlabFor->get_body());
      ROSE_ASSERT(body);

      SgExpression*     range = matlabFor->get_range();
      SgExpression*     init = NULL;
      SgExpression*     incr = NULL;
      SgStatement*      test = NULL;

      //The RHS in i = rangeExp could be a real range or other expressions return a range.
      if (SgRangeExp* rangeExp = isSgRangeExp(range))
      {
        //If the expression is a real range, we can just convert it to a normal for loop

        SgExpression* start  = rangeExp->get_start();
        SgExpression* stride = rangeExp->get_stride();

        if (stride == NULL)
        {
          stride = sb::buildIntVal(1);
        }

        SgExpression* end = rangeExp->get_end();

        init = start;  //sb::buildAssignOp(index, start);
        test = sb::buildExprStatement(sb::buildLessOrEqualOp(si::deepCopy(index), end));
        incr = sb::buildPlusAssignOp(si::deepCopy(index), stride);
      }
      else
      {
        //Create a .begin() method call on range
        SgFunctionCallExp* beginCall =
               ru::createMemberFunctionCall( "Matrix",
                                             range,
                                             "begin",
                                             ru::buildEmptyParams(),
                                             matlabFor
                                           );

        //create index = range.getMatrix().begin()
        init = beginCall;//sb::buildAssignOp(index, beginCall);

        SgFunctionCallExp *endCall =
               ru::createMemberFunctionCall( "Matrix",
                                             range,
                                             "end",
                                             ru::buildEmptyParams(),
                                             matlabFor
                                           );

        //create index != range.getMatrix().end()
        test = sb::buildExprStatement(sb::buildNotEqualOp(index, endCall));

        //++index
        incr = sb::buildPlusPlusOp(index, SgUnaryOp::prefix);

        //create *index from index
        SgExpression *dereferencedIndex = sb::buildPointerDerefExp(index);

        /*Replace each occurrence of index in the loop body with *index
          So that if in a loop:
          for i = 1:10
          y = i
          end

          then y = i will become y = *i since i in c++ is an iterator over the matrix that represents the range 1:10
        */
        ru::replaceVariable(body, isSgVarRefExp(index), dereferencedIndex);
      }

      SgForInitStatement*    cppForInit = sb::buildForInitStatement();
      SgForStatement*        cppForStmt = sb::buildForStatement(cppForInit, test, incr, body);
      SgBasicBlock*          funDef     = &sg::ancestor<SgBasicBlock>(SG_DEREF(matlabFor));

      SgType*                autoType   = sb::buildOpaqueType("auto", funDef);
      SgAssignInitializer*   cppForLow  = sb::buildAssignInitializer(init, autoType);
      SgName                 indexName  = ru::nameOf(isSgVarRefExp(index));

      SgVariableDeclaration* cppForVar =
              sb::buildVariableDeclaration( indexName,
                                            autoType,
                                            cppForLow,
                                            cppForStmt
                                          );

      cppForInit->append_init_stmt(cppForVar);

      // TODO replace all varrefs in the body with real varrefs...
      si::replaceStatement(matlabFor, cppForStmt);
    }
  }

  void transformForloop(SgProject *project)
  {
    forAllNodes(project, tfForLoop, V_SgMatlabForStatement);
  }


  //
  // MatrixOnFunctionCallArgumentsTransformer


  // \todo optimize this method to ignore function calls that do not contain any SgMatrix.
  // Currently the arguments are copied/pasted for every function call.
  namespace
  {
    void tfMatrixOnFunctionCallArguments(SgFunctionCallExp* functionCall)
    {
      SgExprListExp *arguments = functionCall->get_args();

      if (arguments == NULL) return;

      SgScopeStatement *scope = si::getEnclosingScope(functionCall);

      BOOST_FOREACH(SgExpression *currentArg, arguments->get_expressions())
      {
        if( SgMatrixExp *matrix = isSgMatrixExp(currentArg)) {
          //If the argument is a matrix, change it to initializer list

          //TODO: We have to think if it is a multidimensional matrix
          //In that case, first create a variable to hold the matrix
          //and then pass the variable to the function

          Rose_STL_Container<SgExprListExp*> rows = ru::getMatrixRows(matrix);

          //I just want to work on a vector now
          ROSE_ASSERT(rows.size() == 1);

          //Convert each list of numbers [..] to a braced list {..} using AggregateInitializer
          BOOST_FOREACH(SgExprListExp *currentRow, rows)
          {
            SgAggregateInitializer *initializerList = sb::buildAggregateInitializer(currentRow);

            si::replaceExpression(currentArg, initializerList, true);
          }
        }
        else if (isSgMagicColonExp(currentArg))
        {
          //replace a SgMagicColonExp by MatlabSymbol::COLON
          SgVarRefExp *colon = sb::buildVarRefExp("MatlabSymbol::COLON", scope);

          si::replaceExpression(currentArg, colon);
        }
      }
    }
  }


  void transformMatrixOnFunctionCallArguments(SgProject *project)
  {
    forAllNodes(project, tfMatrixOnFunctionCallArguments, V_SgFunctionCallExp);
  }


  //
  // RangeExpressionTransformer

  namespace
  {
    void tfRangeExpression(SgRangeExp* rangeExp)
    {
      // Skip the range expression inside a for loop
      if (isSgMatlabForStatement(rangeExp->get_parent()))
      {
        // This is because the for loop will deal with the range in a different way.
        // Actually the range expression M:N in for loop gets transformed to a
        //   i = M; i <= N; ++i
        return;
      }

      SgStatement *enclosingStatement = si::getEnclosingStatement(rangeExp);

      //The scope where the range variable will be created
      SgScopeStatement *destinationScope = si::getEnclosingScope(rangeExp);

      if (enclosingStatement == destinationScope)
      {
        //in for loop, the expressions inside have the same enclosingStatement and scope
        destinationScope = si::getEnclosingScope(enclosingStatement);
      }

      //each variable will have a unique name
      std::string    varName = si::generateUniqueVariableName(destinationScope, "range");
      SgTypeMatrix*  matrixType = isSgTypeMatrix(fn::getInferredType(rangeExp));

      //Range<type> r
      SgVariableDeclaration* rangeVarDeclaration =
              ru::createOpaqueTemplateObject( varName,
                                                     "Range",
                                                     matrixType->get_base_type()->unparseToString(),
                                                     destinationScope
                                                   );

      si::insertStatementBefore(enclosingStatement, rangeVarDeclaration);

      SgExprListExp* functionCallArgs = ru::getExprListExpFromRangeExp(rangeExp);
      SgVarRefExp*   object = sb::buildVarRefExp(varName, destinationScope);

      //r.setBounds(1, 2, 3);
      SgFunctionCallExp* setBoundsCallExp =
              ru::createMemberFunctionCall( "Range",
                                                   object,
                                                   "setBounds",
                                                   functionCallArgs,
                                                   destinationScope
                                                 );

      si::insertStatementAfter(rangeVarDeclaration, sb::buildExprStatement(setBoundsCallExp));

      //r.getMatrix()
      SgFunctionCallExp *getMatrixCallExp =
              ru::createMemberFunctionCall( "Range",
                                                   object,
                                                   "getMatrix",
                                                   sb::buildExprListExp(),
                                                   destinationScope
                                                 );

      // replace 1:2:3 with r.getMatrix()
      si::replaceExpression(rangeExp, getMatrixCallExp, true);
    }
  }

  void transformRangeExpression(SgProject *project)
  {
    forAllNodes(project, tfRangeExpression, V_SgRangeExp);
  }

  //
  // disp transformer

  /**
   * flattens out argument lists in the form of
   * disp({1, 2, 3}) ->  disp(1,2,3)
   */
  namespace
  {
    void flattenAggregateInitializer(SgExprListExp* args)
    {
      ROSE_ASSERT(args);
      SgExpressionPtrList& exprs = args->get_expressions();

      // nothing to flatten
      if (exprs.size() != 1) return;

      SgExpression*           fst = exprs.front();
      SgAggregateInitializer* lst = isSgAggregateInitializer(fst);

      if (lst == NULL) return;

      SgExprListExp*          inner_args = lst->get_initializers();
      SgCallExpression*       callexp = sg::assert_sage_type<SgCallExpression>(args->get_parent());

      // elevate inner_args
      callexp->set_args(inner_args);
      inner_args->set_parent(callexp);

      // remove aggregate initializer + children
      lst->set_initializers(args);
      args->set_parent(lst);

      // remove backlink
      exprs.pop_back();
      delete lst; // \todo is this sufficient?
    }

    void changeTargetName(SgVarRefExp& n, SgName newname)
    {
      SgVarRefExp* newref = sb::buildVarRefExp(newname, n.get_symbol()->get_scope());

      si::replaceExpression(&n, newref);
    }

    struct CallTransformer
    {
      explicit
      CallTransformer(SgExprListExp* params)
      : args(params)
      {}

      void handle(SgNode&) {}

      void handle(SgVarRefExp& n)
      {
        SgName target = n.get_symbol()->get_name();

        if (SgName("disp") == target)
          flattenAggregateInitializer(args);
        else if (SgName("rand") == target)
          changeTargetName(n, "rand0");
        else if (SgName("size") == target)
          changeTargetName(n, "sizeM");
      }

      SgExprListExp* args;
    };

    void tfCalls(SgFunctionCallExp* callExp)
    {
      sg::dispatch(CallTransformer(callExp->get_args()), callExp->get_function());
    }
  }

  void transformSelectedCalls(SgProject* project)
  {
    forAllNodes(project, tfCalls, V_SgFunctionCallExp);
  }

  //
  // ReturnStatementTransformer

  namespace
  {
    void tfReturnStmt(SgReturnStmt* returnStatement)
    {
      SgExprListExp* returnArgs = isSgExprListExp(returnStatement->get_expression());

      if (returnArgs->get_expressions().size() > 1)
      {
        //create a std::make_tuple statement
        SgScopeStatement* scope = si::getEnclosingScope(returnStatement);
        SgVarRefExp*      varref = sb::buildVarRefExp("std::make_tuple", scope);
        SgExpression*     makeTupleExp = sb::buildFunctionCallExp(varref, returnArgs);

        returnStatement->replace_expression(returnArgs, makeTupleExp);
      }
    }
  }

  void transformReturnStatement(SgProject *project)
  {
    forAllNodes(project, tfReturnStmt, V_SgReturnStmt);
  }

  //
  // ReturnListTransformer

  namespace
  {
    void tfReturnListAttribute(SgFunctionDeclaration* decl)
    {
      if (decl->getAttribute("RETURN_VARS")) return;

      SgFunctionDefinition*    def = isSgFunctionDefinition(decl->get_definition());
      if (def == NULL) return;

      SgBasicBlock*            body = def->get_body();
      ROSE_ASSERT(body);        // def has a body
      if (body->get_statements().size() == 0) return; // empty body

      SgStatement*             last = body->get_statements().back();
      SgReturnStmt*            ret = isSgReturnStmt(last);
      if (ret == NULL) return;  // not a return

      SgExprListExp*           exp = isSgExprListExp(ret->get_expression());
      if (exp == NULL) return;  // empty return

      FunctionReturnAttribute* returnAttribute = new FunctionReturnAttribute(exp);

      returnAttribute->attachTo(decl);
    }
  }

  void transformReturnListAttribute(SgProject* project)
  {
    // alternatively we could check all last statement in a function definition
    forAllNodes(project, tfReturnListAttribute, V_SgFunctionDeclaration);
  }


  //
  // TransposeTransformer

  namespace
  {
    void tfTranspose(SgMatrixTransposeOp* transposeOp)
    {
      SgExpression*      obj   = ru::deepCopy(transposeOp->get_operand());
      SgExprListExp*     args  = sb::buildExprListExp(obj);
      SgScopeStatement*  scope = sg::ancestor<SgScopeStatement>(transposeOp);
      SgFunctionCallExp* call  =
             sb::buildFunctionCallExp( sb::buildVarRefExp("transpose", scope),
                                       args
                                     );

  /*
      SgFunctionCallExp* call =
            ru::createMemberFunctionCall( "Matrix",
                                          obj,
                                          "t",
                                          sb::buildExprListExp(),
                                          scope
                                        );
  */

      si::replaceExpression(transposeOp, call);
    }
  }

  void transformTranspose(SgProject* project)
  {
    forAllNodes(project, tfTranspose, V_SgMatrixTransposeOp);
  }


  //
  // MatrixOnAssignOpTransformer

  namespace
  {
    void tfMatrixOnAssignOp(SgAssignOp *assignOp)
    {
      //If LHS on an assign operator is a list of expression tie it to a tuple
      //So [a, b, c] = fcnCall() changes to
      //std::tie(a, b, c) = fcnCall()

      if(SgExprListExp* assignList = isSgExprListExp(assignOp->get_lhs_operand()))
      {
        SgScopeStatement*  scope    = &sg::ancestor<SgScopeStatement>(*assignList);
        SgFunctionCallExp* tieTuple = sb::buildFunctionCallExp(sb::buildVarRefExp("std::tie", scope), assignList);

        tieTuple->set_lvalue(true);
        assignOp->set_lhs_operand(tieTuple);
      }

      /*A matrix expression on the right hand side of an equality operator
        should change to a valid c++ construct
        For now we change the matrix expression to a shift operator expresssion

        so that:
        x = [1 2; 3 4]
        becomes:
        x << 1 << 2 << endr
          << 3 << 4;
       */
      if (SgMatrixExp *matrix = isSgMatrixExp(assignOp->get_rhs_operand()))
      {
        Rose_STL_Container<SgExprListExp*> rows = ru::getMatrixRows(matrix);

        SgExpression *var = isSgVarRefExp(assignOp->get_lhs_operand());

        Rose_STL_Container<SgExprListExp*>::iterator rowsIterator = rows.begin();

        SgExprListExp* firstRow = *rowsIterator;

        SgExpressionPtrList::iterator firstRowIterator = firstRow->get_expressions().begin();
        SgExpression* firstElement = *firstRowIterator;

        SgExpression* shift = sb::buildLshiftOp(var, firstElement);

        for(firstRowIterator = firstRowIterator + 1; firstRowIterator != firstRow->get_expressions().end(); firstRowIterator++)
        {
          shift = sb::buildLshiftOp(shift, *firstRowIterator);
        }

        SgScopeStatement *scope = SageInterface::getEnclosingScope(matrix);
        SgExpression *endr = sb::buildVarRefExp("arma::endr", scope);
        shift = sb::buildLshiftOp(shift, endr);

        //Now loop through remaining rows
        for(rowsIterator = rowsIterator + 1; rowsIterator != rows.end(); rowsIterator++)
        {
          SgExpressionPtrList currentRow = (*rowsIterator)->get_expressions();

          BOOST_FOREACH(SgExpression *expr, currentRow)
          {
            shift = sb::buildLshiftOp(shift, expr);
          }

          shift = sb::buildLshiftOp(shift, endr);
        }

        //Update the parent to ignore the current assignop and replace with the shift expression
        SgStatement *expressionStatement = SageInterface::getEnclosingStatement(assignOp);
        expressionStatement->replace_expression(assignOp, shift);
      }
    }
  }

  void transformMatrixOnAssignOp(SgProject* project)
  {
    forAllNodes(project, tfMatrixOnAssignOp, V_SgAssignOp);
  }


  //
  // ForLoopColonTransformer

#if 0
  currently we cannot typetest colon expressions

  static
  void tfForLoopColon(SgForStatement* forLoop)
  {
    //~ std::cerr << "Found a for loop ... \n";

    //colonExp is stored in increment field
    SgColonExpression* colonExp = isSgColonExpression(forLoop->get_increment());
    if (!colonExp) return;

    //~ std::cerr << colonExp->sage_class_name() << std::endl;

    SgExpression*    start = colonExp->getStart();
    SgExpression*    increment = colonExp->getIncrement();
    SgExpression*    end = colonExp->getEnd();
    SgExprStatement* exprstmt = isSgExprStatement(forLoop->get_for_init_stmt()->get_traversalSuccessorByIndex(0));
    ROSE_ASSERT(exprstmt);

    SgExpression* initVar = exprstmt->get_expression();
    SgStatement * initAssign = sb::buildAssignStatement(initVar, start);
    SgExpression* incrementAssign = sb::buildPlusAssignOp(initVar, increment);
    SgExpression* testExpression = sb::buildNotEqualOp(initVar, end);

    forLoop->set_for_init_stmt(sb::buildForInitStatement(initAssign));
    forLoop->set_increment(incrementAssign);
    forLoop->set_test(sb::buildExprStatement(testExpression));
  }
#endif

  void transformForLoopColon(SgProject* project)
  {
    // forAllNodes(project, tfForLoopColon, V_SgForStatement);
  }

  //
  // checks if a variable is defined in a Matlab for-loop

  namespace
  {
    struct LoopVariableChecker : sg::DispatchHandler<bool>
    {
      typedef sg::DispatchHandler<bool> base;

      explicit
      LoopVariableChecker(std::string ident)
      : base(false), varname(ident)
      {}

      void recurse(SgScopeStatement& n);

      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

      void handle(SgScopeStatement& n)   { recurse(n); }

      void handle(SgFunctionDefinition&) { /* res = false; */ }
      void handle(SgGlobal&)             { /* res = false; */ }

      void handle(SgMatlabForStatement& n)
      {
        SgVarRefExp* idx = isSgVarRefExp(n.get_index());

        res = (idx && (ru::nameOf(idx) == varname));

        if (!res) recurse(n);
      }

      const std::string varname;
    };

    void LoopVariableChecker::recurse(SgScopeStatement& n)
    {
      res = sg::dispatch(*this, sg::ancestor<SgScopeStatement>(n.get_parent()));
    }
  }

  bool
  forLoopIterationVariable(SgVarRefExp* varref, std::string ident)
  {
    ROSE_ASSERT(varref);

    SgScopeStatement* enclosing = sg::ancestor<SgScopeStatement>(varref);

    return sg::dispatch(LoopVariableChecker(ident), enclosing);
  }

  bool
  forLoopIterationVariable(SgVarRefExp* varref)
  {
    ROSE_ASSERT(varref);

    return forLoopIterationVariable(varref, ru::nameOf(varref));
  }

  /**
   * tests if varref is defined in a MatlabForLoop
   *
   * \detail convenience function
   */
  bool forLoopIterationVariable(SgVarRefExp* varref);
} /* namespace MatlabTransformation */
