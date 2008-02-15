#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include "findMethodCalls.h"


static
bool
checkFunctionExpression(SgExpression *functionExpression,
			SgFunctionDeclaration *target)
{
  switch(functionExpression->variantT()) {
  case V_SgFunctionRefExp:
    {
      SgFunctionRefExp* functionRef = isSgFunctionRefExp(functionExpression);
      return (target->get_mangled_name() == functionRef->get_symbol()->get_declaration()->get_mangled_name());
    }
  case V_SgMemberFunctionRefExp:
    {
      SgMemberFunctionRefExp* memberRef = isSgMemberFunctionRefExp(functionExpression);
      return (target->get_mangled_name() == memberRef->get_symbol()->get_declaration()->get_mangled_name());
    }
  case V_SgArrowExp:
  case V_SgDotExp:
    {
      SgBinaryOp* op = isSgBinaryOp(functionExpression);
      return checkFunctionExpression(op->get_rhs_operand_i(), target);
    }
  default:
    {
   // All other cases are ignored
    }
  }

  return false;
}

static
NodeQuerySynthesizedAttributeType
queryFunc(SgNode *match, SgNode *target)
{
  SgFunctionCallExp *call = isSgFunctionCallExp(match);
  SgFunctionDeclaration *targetDecl = isSgFunctionDeclaration(target);
  NodeQuerySynthesizedAttributeType result;
  if (NULL != call) {
    if (checkFunctionExpression(call->get_function(), targetDecl)) {
      result.push_back(match);
    }
  }
  else {
    SgConstructorInitializer *init = isSgConstructorInitializer(match);
    if (NULL != init) {
      SgMemberFunctionDeclaration *mfd = init->get_declaration();
      if (NULL != mfd) {
	if (targetDecl->get_mangled_name() == mfd->get_mangled_name()) {
	  result.push_back(match);
	}
      }
    }
  }
  return result;
}

void
MethodSearchVisitor::visit(SgNode* n)
   {
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(n);
     if (classDeclaration != NULL)
        {
          Rose_STL_Container<SgNode*> functionDeclarations = NodeQuery::querySubTree(n, V_SgFunctionDeclaration);
          for(Rose_STL_Container<SgNode*>::iterator i = functionDeclarations.begin() ; i != functionDeclarations.end(); ++i)
             {
               NodeQuerySynthesizedAttributeType search = NodeQuery::querySubTree(d_root, *i, queryFunc);
               if (search.size() > 0)
                  {
                 // DQ (1/2/2006): Added support for new attribute interface.
                    printf ("MethodSearchVisitor::visit(): using new attribute interface \n");
#if 0
                    if ((*i)->get_attribute() == NULL)
                       {
                         AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                         ROSE_ASSERT(attributePtr != NULL);
                         (*i)->set_attribute(attributePtr);
                       }
#endif
                 // (*i)->attribute.add("functioncalls", new MethodUseAttribute(search));
                 // (*i)->attribute().add("functioncalls", new MethodUseAttribute(search));
                    (*i)->addNewAttribute("functioncalls", new MethodUseAttribute(search));
                    printf("Successful search\n");
                  }
             }
        }
   }

