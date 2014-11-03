#include "mlmapi.h"
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

extern vector<SgStatement*> DeletepragmasList;
extern vector<SgCallExpression*> callExprList;

using namespace MLMAPIInsertion;

void mlmFrontend::attachAttribute(SgPragmaDeclaration* pragma, AstAttribute* attr)
{
  SgVariableDeclaration* decl = isSgVariableDeclaration(getNextStatement(pragma));  
  ROSE_ASSERT(decl);
  SgInitializedNamePtrList nameList = decl->get_variables();
  if(nameList.size() == 1)
  {
    SgInitializedName* initName = nameList[0];
    SgSymbol* symbol = initName->get_symbol_from_symbol_table();
    symbol->setAttribute("mlmAttribute", attr);
  } 
}

void mlmFrontend::visit(SgNode* node)
{
  if (isSgPragmaDeclaration(node) != NULL)
  {
    SgPragmaDeclaration * pragDecl = isSgPragmaDeclaration(node);
    char* end;
    int type = strtol(pragDecl->get_pragma()->get_pragma().c_str(),&end, 10);
    if(end == pragDecl->get_pragma()->get_pragma().c_str())
    {
      cout << "pragma has no integer value" << endl;
      type = -1;
    } 
    if(type >= 0) 
    {
      //cout << "type = " << type << endl;
      mlmAttribute* newAttr = new mlmAttribute(type);
      mlmFrontend::attachAttribute(pragDecl, newAttr);
      DeletepragmasList.push_back(pragDecl);
    }
    // handle omp parallel, omp for, omp single, ..etc.
    //createOmpAttributeOld(node);
   }

}


void mlmTransform::visit(SgNode* node)
{
  // Searching for malloc, memcpy, and free
  if (isSgCallExpression(node) != NULL)
  {
    SgCallExpression* callExp = isSgCallExpression(node);
    ROSE_ASSERT(callExp);
    callExprList.push_back(callExp);
  }
}

void mlmTransform::transformCallExp(SgCallExpression* callExp)
{
    ROSE_ASSERT(callExp);
    SgFunctionRefExp* funcName = isSgFunctionRefExp(callExp->get_function());
    SgExprListExp* funcArgList = callExp->get_args();
    SgExpressionPtrList argList = funcArgList->get_expressions();
    SgScopeStatement* scope = getScope(callExp);         
    //cout << funcName->get_symbol()->get_name() << endl;

    /** if it is malloc, search for the mlm attribute and append the memory level **/
    if(strncmp("malloc",funcName->get_symbol()->get_name().str(),6) == 0)
    {
      if(argList.size() != 1)  return;
      SgExprListExp* funcArgList = callExp->get_args();

      // check if LHS of malloc has an attribute assigned
      SgNode* parentNode = callExp->get_parent();
      // parent node can be a casting expression
      if(isSgCastExp(parentNode))
      {
        parentNode = parentNode->get_parent();
      }
      // the mlm attribute
      AstAttribute* attr = NULL;
      // So far we spot two candidates for parentNode that we need to transform
      if(isSgAssignOp(parentNode))
      {
        SgAssignOp* assignOp = isSgAssignOp(parentNode);
        SgExpression* lhs = isSgExpression(assignOp->get_lhs_operand());
        if(!isSgVarRefExp(lhs))
        {
          //cout << "lhs:" << assignOp->get_lhs_operand()->class_name() << endl;

          // if pointer is packaged inside a struct, then we need to look down in lhs.
          if(isSgDotExp(lhs))
          {
            lhs = isSgDotExp(lhs)->get_rhs_operand();
          }
        }
        SgVarRefExp* lhsVarRef = isSgVarRefExp(lhs);
        ROSE_ASSERT(lhsVarRef);
        SgSymbol* symbol = lhsVarRef->get_symbol();
        ROSE_ASSERT(symbol);
        //retrieve the attribute from symbol
        attr = symbol->getAttribute("mlmAttribute");
        //cout << "LHS symbol name: " << symbol->get_name() << endl;
      }
      else if(isSgAssignInitializer(parentNode))
      {
        SgInitializedName* initName = isSgInitializedName(parentNode->get_parent());
        ROSE_ASSERT(initName);
        SgSymbol* symbol = initName->get_symbol_from_symbol_table();
        if(!symbol) return;
        ROSE_ASSERT(symbol);
        //retrieve the attribute from symbol
        attr = symbol->getAttribute("mlmAttribute");
        //cout << "Initialized symbol name: " << symbol->get_name() << endl;
      }
      else
      {
        // do nothing because no attribute assigned or we always set to default
      }
      // if there is a mlm attribute attached to the symbol, then create new malloc
      if(attr)
      {
        mlmAttribute* mlmAttr = dynamic_cast<mlmAttribute*> (attr);
        SgExprListExp* funcArgList = callExp->get_args();
        funcArgList->append_expression(buildIntVal(mlmAttr->getMemType()));
        replaceExpression(callExp, buildFunctionCallExp("mlm_malloc",deepCopy(callExp->get_type()),deepCopy(funcArgList),getScope(callExp)));
      }
    }
    else if(strncmp("memcpy",funcName->get_symbol()->get_name().str(),6) == 0)
    {
//      cout << "replacing memcpy" << endl;
      if(argList.size() != 3)  return;
      Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(funcArgList, V_SgVarRefExp);
      SgVarRefExp* dst = isSgVarRefExp(varList[0]);
      SgVarRefExp* src = isSgVarRefExp(varList[1]);
      AstAttribute* attrDst = dst->get_symbol()->getAttribute("mlmAttribute");
      AstAttribute* attrSrc = src->get_symbol()->getAttribute("mlmAttribute");
      mlmAttribute* mlmAttrDst =  dynamic_cast<mlmAttribute*>(attrDst);
      mlmAttribute* mlmAttrSrc =  dynamic_cast<mlmAttribute*>(attrSrc);
//      if((mlmAttrDst && !mlmAttrSrc) || (mlmAttrDst && mlmAttrSrc && (mlmAttrDst->getMemType() < mlmAttrDst->getMemType())))
//      {
//        replaceExpression(callExp, buildFunctionCallExp("mlm_memcpy",deepCopy(callExp->get_type()),deepCopy(funcArgList),scope),true);
//        DeletepragmasList2.push_back(callExp);
//      }
//
//      else if((!mlmAttrDst && mlmAttrSrc) || (mlmAttrDst && mlmAttrSrc && (mlmAttrDst->getMemType() > mlmAttrDst->getMemType())))

// 09/30/14 Following Simon's suggestion, we always insert wait for the mlm_memcpy
//      {
         string tagName = generateUniqueVariableName(scope,"copy_tag");
         SgVariableDeclaration* newDecl = buildVariableDeclaration(tagName,
                                          buildOpaqueType("mlm_Tag",getGlobalScope(callExp)), 
                                          buildAssignInitializer(buildFunctionCallExp("mlm_memcpy",deepCopy(callExp->get_type()),deepCopy(funcArgList),scope)));
         SgExprStatement* waitStmt = buildFunctionCallStmt("mlm_waitComplete",
                                                           buildVoidType(),
                                                           buildExprListExp(buildVarRefExp(tagName,scope)),
                                                           scope); 
         insertStatement(getEnclosingStatement(callExp),newDecl,true);
         insertStatement(getEnclosingStatement(callExp),waitStmt,true);
         removeStatement(getEnclosingStatement(callExp));
//      }
    }
    else if(strncmp("free",funcName->get_symbol()->get_name().str(),4) == 0)
    {
//      cout << "replacing free" << endl;
      if(argList.size() != 1)  return;
      SgExpression* varExp = isSgExpression(argList[0]);
cout << "exp:" << varExp->class_name() << endl;
      if(!isSgVarRefExp(varExp))
      {
        if(isSgCastExp(varExp))
        {
          varExp = isSgCastExp(varExp)->get_operand_i();
        }
        // if pointer is packaged inside a struct, then we need to look down in lhs.
        if(isSgDotExp(varExp))
        {
          varExp = isSgDotExp(varExp)->get_rhs_operand();
        }
      }
      SgVarRefExp* varRef = isSgVarRefExp(varExp);
      ROSE_ASSERT(varRef);
      AstAttribute* attr = varRef->get_symbol()->getAttribute("mlmAttribute");
      if(attr)
      {
        replaceExpression(callExp, buildFunctionCallExp("mlm_free",deepCopy(callExp->get_type()),deepCopy(funcArgList),scope),false);
      }
    }

}

void mlmTransform::insertHeaders(SgProject* project)
{
  Rose_STL_Container<SgNode*> globalList = NodeQuery::querySubTree (project,V_SgGlobal);
  for(Rose_STL_Container<SgNode*>::iterator i = globalList.begin(); i != globalList.end(); i++)
  {
    SgGlobal* global = isSgGlobal(*i);
    ROSE_ASSERT(global);

    PreprocessingInfo* headerInfo = insertHeader("mlm.h",PreprocessingInfo::after,false,global);
    headerInfo->set_file_info(global->get_file_info());
  }
  return; //assume always successful currently
}
