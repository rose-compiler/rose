#include "sage3basic.h"

#include "MatlabFunctionBuilder.h"
#include "StatementList.h"

MatlabFunctionBuilder::MatlabFunctionBuilder(SgFunctionParameterList* parameterList, SgStatement* statementList, SgScopeStatement* scope)
{
  this->parameterList = parameterList;
  this->statementList = statementList;
  this->scope = scope;
  returnList = NULL;
}

MatlabFunctionBuilder::MatlabFunctionBuilder(SgStatement* statementList, SgScopeStatement* scope) 		
{	
  this->parameterList = SageBuilder::buildFunctionParameterList(); //An empty parameter list
		
  this->statementList = statementList;
  this->scope = scope;
  returnList = NULL;
}

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

  SgFunctionDeclaration *declaration = SageBuilder::buildDefiningFunctionDeclaration(functionName, 
									       SageBuilder::buildVoidType(), parameterList, scope);

  //The body for current function has already been pushed. Its in top of scope stack
  //we'll have to throw the existing function body
   SgBasicBlock *functionBody = isSgBasicBlock(SageBuilder::topScopeStack());
   declaration->get_definition()->set_body(functionBody);
   functionBody->set_parent(declaration->get_definition());

   //Append all the accumulated statements into the currentScope
   ((StatementList*)statementList)->appendAll();
   
  if(returnList != NULL)
    {
      SageInterface::appendStatement(SageBuilder::buildReturnStmt(returnList), functionBody);
    }
  
  return declaration;
}
