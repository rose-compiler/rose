#include <utility>

#include "FunctionAnalyzer.h"

#include "sageGeneric.h"

#include "BottomUpTypeAnalysis.h"
#include "FastNumericsRoseSupport.h"

static
SgExprListExp* getReturnVariables(SgFunctionDeclaration* function)
{
  Rose_STL_Container<SgNode*> allReturnStmts = NodeQuery::querySubTree(function, V_SgReturnStmt);

  if (allReturnStmts.size() == 0)
    return NULL;

  // \todo check that they all have the same type
  SgReturnStmt*  returnStmt = isSgReturnStmt(allReturnStmts[0]);

  ROSE_ASSERT(returnStmt);
  SgExpression*  retexpr = returnStmt->get_expression();
  SgExprListExp* res = isSgExprListExp(retexpr);

  ROSE_ASSERT(retexpr == NULL || res);
  return res;
}

namespace MatlabAnalysis
{
  struct TopDownTypeAssigner : AstSimpleProcessing
  {
    explicit
    TopDownTypeAssigner(SgAssignOp* assignOp)
    : inferredTypeAttribute((TypeAttribute*)assignOp->getAttribute("TYPE_INFERRED"))
    {}

    void visit(SgNode *node)
    {
      SgVarRefExp* varref = isSgVarRefExp(node);

      if (varref)
      {
        /*
          We would not set the type for a variable name that represents a function call

          eg multiply(1,2) is represented as
            FunctionCallExp
             |
            SgVarRefExp(multiply)
         */
        if (isSgFunctionCallExp(node->get_parent()))
          return;

        // SgVariableSymbol *variableSymbol = var_ref->get_symbol();
        // SgInitializedName *initializedName = variableSymbol->get_declaration();
        //
        // initializedName->set_type(inferredTypeAttribute->get_inferred_type());
        // FastNumericsRoseSupport::updateVarRefTypeWithDeclaration(var_ref, inferredTypeAttribute->get_inferred_type());

        FastNumericsRoseSupport::updateVarRefTypeWithDominantType(varref, inferredTypeAttribute);
        // inferredTypeAttribute->attach_to(node);
      }
    }

    private:
      TypeAttribute* inferredTypeAttribute;
  };

  struct ExpressionRootAnalyzer : AstSimpleProcessing
  {
      ExpressionRootAnalyzer(FunctionAnalyzer* funanalyzer, SgProject* project)
      : fa(funanalyzer), proj(project)
      {
        ROSE_ASSERT(fa && proj);
      }

      void handle(SgNode& n) {}
      
      void handle(SgAssignOp& n)
      {
        runBottomUpInference(&n, proj, fa);

        ROSE_ASSERT(n.attributeExists("TYPE_INFERRED"));

        // Types for type tuple have already been assigned. Do not reassign them
        if (isSgTypeTuple(FastNumericsRoseSupport::getInferredType(&n)))
        {
          return;
        }

        TopDownTypeAssigner topDownTypeAssigner(&n);

        topDownTypeAssigner.traverse(n.get_lhs_operand(), preorder);
      }

      void handle(SgExprStatement& n)
      {
        runBottomUpInference(n.get_expression(), proj, fa);
      }

      void handle(SgMatlabForStatement& n)
      {
        runBottomUpInference(n.get_index(), proj, fa);

        //~ std::cerr << "FOR 2:" << n.get_range()->unparseToString() << std::endl;
        runBottomUpInference(n.get_range(), proj, fa);
      }

      void visit(SgNode* node)
      {
        sg::dispatch(*this, node);
      }

    private:
      FunctionAnalyzer* fa;
      SgProject*        proj;
  };


  void FunctionAnalyzer::analyse_function(SgFunctionDeclaration* function)
  {
#if 0
    // we first look at all assignment statements, because they
    //   propagate type information from the right hand to the left hand side.
    Rose_STL_Container<SgNode*> allAssignOps = NodeQuery::querySubTree(function, V_SgAssignOp);
    for ( Rose_STL_Container<SgNode*>::iterator assignOpIterator = allAssignOps.begin();
          assignOpIterator != allAssignOps.end();
          ++assignOpIterator
        )
    {
      SgAssignOp* assignOp = isSgAssignOp(*assignOpIterator);

      // Go bottom up from the nodes that contain terminal values and
      // traverse their types upwards
      runBottomUpInference(assignOp, project, this);

      ROSE_ASSERT(assignOp->attributeExists("TYPE_INFERRED"));

      // Types for type tuple have already been assigned. Do not reassign them
      if (isSgTypeTuple(FastNumericsRoseSupport::getInferredType(assignOp)))
      {
        // \todo consistency check?
        continue;
      }

      TopDownTypeAssigner topDownTypeAssigner(assignOp);

      topDownTypeAssigner.traverse(assignOp->get_lhs_operand(), preorder);
    }
#endif

    ExpressionRootAnalyzer exprroots(this, project);

    exprroots.traverse(function, preorder);

    // Now get the return type of the function
    SgExprListExp* returnList = getReturnVariables(function);
    SgType*        returnType = NULL;

    if (returnList != NULL)
    {
      SgExpressionPtrList expList = returnList->get_expressions();

      if (expList.size() == 1)
      {
        returnType =  expList[0]->get_type(); //FastNumericsRoseSupport::getInferredType(expList[0]);
      }
      else
      {
        returnType = SageBuilder::buildTupleType();

        for(SgExpressionPtrList::iterator it = expList.begin(); it != expList.end(); ++it)
        {
          SgVarRefExp *returnVar = isSgVarRefExp(*it);
          SgType *type = returnVar->get_type(); //FastNumericsRoseSupport::getInferredType(*it);

          isSgTypeTuple(returnType)->append_type(type);
        }
      }
    }
    else
    {
      returnType = SageBuilder::buildVoidType();
    }

    SgFunctionParameterList* paramList = function->get_parameterList();
    SgFunctionType*          functionType = buildFunctionType_Custom(returnType, paramList);
    SgDeclarationStatement*  firstNondefiningDeclaration = function->get_firstNondefiningDeclaration();
    SgFunctionDeclaration*   firstFunDeclaration = isSgFunctionDeclaration(firstNondefiningDeclaration);

    ROSE_ASSERT(firstFunDeclaration);
    firstFunDeclaration->set_type(functionType);
    function->set_type(functionType);
  }

  /*
   * Had to write custom version of this function which is similar to one found in SageBuilder
   * This is because Matrix<T> type always returns the same mangled name for any value of T
   * So if there is a function that takes returns/takes Matrix<int> and another overloaded
   * function that rreturns/takes Matrix<double> then they will have same mangled name.
   * Thus both functions will have one entry in global function type table. This is an error.
   *
   * This could have been modified in ROSE but modification requires EDG license
   */
  SgFunctionType*
  FunctionAnalyzer::buildFunctionType_Custom(SgType* return_type, SgFunctionParameterList* argList)
  {
    SgFunctionParameterTypeList* typeList = SageBuilder::buildFunctionParameterTypeList(argList);
    SgName                       typeName = SgFunctionType::get_mangled(return_type, typeList);
    SgTypePtrList&               args = typeList->get_arguments();

    for (SgTypePtrList::const_iterator it = args.begin(); it != args.end(); ++it)
    {
      if (SgTypeMatrix* matrixType = isSgTypeMatrix(*it))
      {
        typeName << "_" << matrixType->get_base_type()->get_mangled() << "_";
      }
    }

    if (SgTypeMatrix* matrixType = isSgTypeMatrix(return_type))
    {
      typeName << "_" << matrixType->get_base_type()->get_mangled() << "_";
    }

    SgFunctionTypeTable*         fTable = SgNode::get_globalFunctionTypeTable();
    ROSE_ASSERT(fTable);

    SgFunctionType*              funcType = isSgFunctionType(fTable->lookup_function_type(typeName));

    if (funcType == NULL)
    {
      // Only build the new type if it can't be found in the global type table.
      funcType = new SgFunctionType(return_type, false /* has ellipsis */);
      ROSE_ASSERT(funcType);
      ROSE_ASSERT(funcType->get_argument_list() != NULL);
      ROSE_ASSERT(funcType->get_arguments().size() == 0);

      for (SgTypePtrList::iterator it = args.begin(); it != args.end(); ++it)
      {
        funcType->append_argument(*it);
      }

      fTable->insert_function_type(typeName,funcType);
    }

    delete typeList;

    ROSE_ASSERT(funcType);
    return funcType;
  }
}
