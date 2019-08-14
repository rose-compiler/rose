#include "sage3basic.h"
#include "TransformationOperators.h"

using namespace std;
using namespace CodeThorn;

// creates a copy of a function definition, but also the corresponding declaration, and creates a new name.
SgFunctionDefinition* CodeThorn::TransformationOperators::createAdaptedFunctionDefinitionCopy(SgFunctionDefinition* originalFunDef, string prefix, string suffix) {
    SgFunctionDeclaration* originalFunDecl=originalFunDef->get_declaration();
    SgFunctionDeclaration* clonedFunDecl=isSgFunctionDeclaration(SageInterface::deepCopyNode(originalFunDecl));
    clonedFunDecl->set_name(SgName(prefix+clonedFunDecl->get_name().getString()+suffix));
    SgScopeStatement* originalFunctionParentScope=originalFunDecl->get_scope();
    SageInterface::appendStatement(clonedFunDecl, originalFunctionParentScope);
    SgFunctionDefinition* clonedFunDef=clonedFunDecl->get_definition();
    return clonedFunDef;
}

list<SgExpression*> CodeThorn::TransformationOperators::varRefExpOfVar(SgNode* root, VariableId varId, VariableIdMapping* variableIdMapping ) {
  list<SgExpression*> varRefList;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVarRefExp* varRefExp=isSgVarRefExp(*i)) {
      if(varId==variableIdMapping->variableId(varRefExp)) {
        varRefList.push_back(varRefExp);
      }
    }
  }
  return varRefList;
}

// requires available expressions analysis
void CodeThorn::TransformationOperators::substituteUsesWithAvailableExpRhsOfDef(string udAttributeName, SgNode* root, Labeler* labeler, VariableIdMapping* variableIdMapping) {
  RoseAst ast(root);
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
    Label lab=i;
    SgNode* node=labeler->getNode(lab);
    UDAstAttribute* udAttr=dynamic_cast<UDAstAttribute*>(node->getAttribute(udAttributeName));
    if(udAttr) {
      VariableIdSet usedVars=udAttr->useVariables(*variableIdMapping);
      if(usedVars.size()>0) {
        //cout<<"Found used vars."<<endl;
        // foreach uvar in usedVars do if(def(uvar)==1) replace(ast(uvar),rhs(def(uvar))) od
        for(VariableIdSet::iterator i=usedVars.begin();i!=usedVars.end();++i) {
          LabelSet varDefs=udAttr->definitionsOfVariable(*i);
          if(varDefs.size()==1) {
            // 1) determine definition rhs
            Label def=*varDefs.begin(); // guaranteed to be one
            SgNode* defRootNode=labeler->getNode(def);
            // only substitute variables
            if(variableIdMapping->hasIntegerType(*i)||variableIdMapping->hasFloatingPointType(*i)) {
              //cout<<"Found UD Attribute with one def. variable:"<<variableIdMapping->uniqueVariableName(*i)<<" ";
              //cout<<"DEF:"<<defRootNode->unparseToString()<<endl;
              // somewhat combersome to determime the rhs of the def
              // 1) declaration initializer
              // 2) assignment
              SgExpression* rhsExp=0;
              if(isSgExprStatement(defRootNode)) {
                defRootNode=SgNodeHelper::getExprStmtChild(defRootNode);
              }
              if(SgVariableDeclaration* decl=isSgVariableDeclaration(defRootNode)) {
                SgExpression* exp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(decl);
                if(exp) {
                  rhsExp=exp;
                }
              }
              if(SgAssignOp* assignOp=isSgAssignOp(defRootNode)) {
                rhsExp=isSgExpression(SgNodeHelper::getRhs(assignOp));
              }
              if(rhsExp) {
                list<SgExpression*> uses=varRefExpOfVar(node, *i, variableIdMapping);
                for(list<SgExpression*>::iterator i=uses.begin();i!=uses.end();++i) {
                  cout<<"Substituting:"<<(*i)->unparseToString()<<" by "<<rhsExp->unparseToString()<<endl;
                  SgNodeHelper::replaceExpression(*i,SageInterface::copyExpression(rhsExp),true); // must be true (otherwise internal error)
                }
              }
              // determine rhs of assignment
            }
          }
        }
      }
    }
  }
}

