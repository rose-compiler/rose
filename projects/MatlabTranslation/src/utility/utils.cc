#include "utils.h"

#include <string>

#include "rose.h"
#include "sageGeneric.h"
#include "sageBuilder.h"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace RoseUtils
{
  SgName nameOf(const SgSymbol& varsy)
  {
    return varsy.get_name();
  }

  SgName nameOf(const SgVarRefExp& var_ref)
  {
    return nameOf(sg::deref(var_ref.get_symbol()));
  }

  SgName nameOf(const SgVarRefExp* var_ref)
  {
    return nameOf(sg::deref(var_ref));
  }

  SgExpressionPtrList& arglist(SgCallExpression* call)
  {
    ROSE_ASSERT(call);
    SgExprListExp& args = sg::deref(call->get_args());

    return args.get_expressions();
  }

  SgExpression* argN(SgCallExpression* call, size_t n)
  {
    return arglist(call).at(n);
  }


  /*
    Build Empty parameters
   */
  SgExprListExp* buildEmptyParams()
  {
    return SageBuilder::buildExprListExp();
  }



  /*
Returns a string of types separated by commas from a list of types
*/
  std::string getFunctionCallTypeString(Rose_STL_Container<SgType*> typeList)
    {
      std::string typeString = "";

      for(Rose_STL_Container<SgType*>::iterator it = typeList.begin(); it != typeList.end(); ++it)
        {
          //typeString += (*it)->class_name() + ",";
          typeString += (*it)->unparseToString() + ",";
        }

      return typeString;
    }

  /*
    Replace all variables in the given SgStatement that have the same name as given variable by the new expression
   */
  void replaceVariable(SgStatement *statement, SgVarRefExp *variable, SgExpression *expression)
  {
    class ReplacementTraversal : public AstSimpleProcessing
    {

    public:
      SgName toReplace;
      SgExpression *newExpression;

      virtual void visit(SgNode *node)
      {
        /*
          Replace each variable having the same name as the input variable with the given expression
         */

        if(SgVarRefExp *var = isSgVarRefExp(node))
          {
            if(var->get_symbol()->get_name() == toReplace)
              {
                SageInterface::replaceExpression(var, newExpression);
              }
          }
      }
    };

    ReplacementTraversal traversal;
    traversal.toReplace = variable->get_symbol()->get_name();
    traversal.newExpression = expression;

    traversal.traverse(statement, postorder);
  }

  /**
   * Get the rows of a SgMatrixExp as a vector of ExprListExp
   */
  Rose_STL_Container<SgExprListExp*>
  getMatrixRows(SgMatrixExp* matrix)
  {
    Rose_STL_Container<SgExprListExp*> rows;
    SgExpressionPtrList                rowList = matrix->get_expressions();

    for (SgExpressionPtrList::iterator it = rowList.begin(); it != rowList.end(); ++it)
    {
      rows.push_back(isSgExprListExp(*it));
    }

    return rows;
  }

  /**
   * Create a template variable declaration with your provided types
   *
   * e.g., You may want to create Matrix<int> x;
   *       and you may not have an existing symbol for Matrix<int>
   */
  SgVariableDeclaration*
  createOpaqueTemplateObject(std::string varName, std::string className, std::string type, SgScopeStatement *scope)
  {
    std::string            templatedTypeString = className + "<" + type + ">";
    SgType*                templatedType = sb::buildOpaqueType(templatedTypeString, scope);
    SgVariableDeclaration* varDeclaration = sb::buildVariableDeclaration(varName, templatedType, NULL, scope);

    return varDeclaration;
  }

  /**
   * Create a member function call
   * This function looks for the function symbol in the given className
   * The function should exist in the class
   * The class should be #included or present in the source file parsed by frontend
   */
  SgFunctionCallExp*
  createMemberFunctionCall( std::string       className,
                            SgExpression*     objectExpression,
                            std::string       functionName,
                            SgExprListExp*    params,
                            SgScopeStatement* scope
                          )
  {
    //SgProject *project = SageInterface::getProject();

    //~ std::cout << "looking for " << functionName << " in " << className << std::endl;

    SgClassSymbol* classSymbol = SageInterface::lookupClassSymbolInParentScopes(className, scope);
    ROSE_ASSERT(classSymbol);

    SgDeclarationStatement* classDecl = classSymbol->get_declaration()->get_definingDeclaration();
    SgClassDeclaration*     classDeclaration = isSgClassDeclaration(classDecl);
    ROSE_ASSERT(classDeclaration != NULL);

    SgClassDefinition*      classDefinition = classDeclaration->get_definition();
    ROSE_ASSERT(classDefinition);

    SgSymbol*               funsy = si::lookupFunctionSymbolInParentScopes(functionName, classDefinition);
    SgMemberFunctionSymbol* functionSymbol = isSgMemberFunctionSymbol(funsy);
    ROSE_ASSERT(functionSymbol);

    SgMemberFunctionRefExp* memref = sb::buildMemberFunctionRefExp(functionSymbol, false, false);

    return sb::buildFunctionCallExp(sb::buildDotExp(objectExpression, memref), params);
  }


  /*
   * Returns start:stride:end as a SgExprListExp
   */
  SgExprListExp *getExprListExpFromRangeExp(SgRangeExp *rangeExp)
  {
    SgExprListExp *exprList = SageBuilder::buildExprListExp(rangeExp->get_start());

    if(SgExpression *stride = rangeExp->get_stride())
      SageInterface::appendExpression(exprList, stride);

    SageInterface::appendExpression(exprList, rangeExp->get_end());

    return exprList;
  }

  SgFunctionCallExp* createFunctionCall(std::string functionName, SgScopeStatement *scope, SgExprListExp *parameters)
  {
    return sb::buildFunctionCallExp(SageBuilder::buildVarRefExp(functionName, scope), parameters);
  }
}
