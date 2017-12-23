#include "rose.h"

#include "MatlabFunctionBuilder.h"
#include "StatementList.h"
// #include "rose-utils/FunctionReturnAttribute.h"


MatlabFunctionBuilder::MatlabFunctionBuilder(SgFunctionParameterList* paramList, StatementList* stmtList, SgScopeStatement* sc)
: parameterList(paramList),
  statementList(stmtList),
  scope(sc),
  functionName(),
  returnList(NULL)
{}
		
MatlabFunctionBuilder::MatlabFunctionBuilder(StatementList* stmtList, SgScopeStatement* sc)
: parameterList(SageBuilder::buildFunctionParameterList()),
  statementList(stmtList),
  scope(sc),
  functionName(),
  returnList(NULL)
{}

void MatlabFunctionBuilder::set_function_name(std::string name)
{
  this->functionName = name;
}

void MatlabFunctionBuilder::set_return_list(SgExprListExp *returnList)
{
  this->returnList = returnList;
}

SgFunctionDeclaration* MatlabFunctionBuilder::build_function()
{
  ROSE_ASSERT(scope != NULL);

  SgFunctionDeclaration* declaration =
        SageBuilder::buildDefiningFunctionDeclaration(
               functionName,
               SageBuilder::buildVoidType(),
               parameterList,
               scope
          );

  //The body for current function has already been pushed. Its in top of scope stack
  //we'll have to throw the existing function body
   SgBasicBlock *functionBody = isSgBasicBlock(SageBuilder::topScopeStack());
  ROSE_ASSERT(functionBody);

  SgFunctionDefinition* def = declaration->get_definition();
  ROSE_ASSERT(def);

  def->set_body(functionBody);
  functionBody->set_parent(def);

   //Append all the accumulated statements into the currentScope
  statementList->appendAll();
   
  if(returnList != NULL)
    {
      SageInterface::appendStatement(SageBuilder::buildReturnStmt(returnList), functionBody);
/*
      FunctionReturnAttribute *returnAttribute = new FunctionReturnAttribute(returnList);

      returnAttribute->attachTo(declaration);
*/
    }
  
  return declaration;
}
