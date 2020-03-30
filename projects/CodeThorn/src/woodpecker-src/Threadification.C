#include "sage3basic.h"
#include "Threadification.h"
#include "RoseAst.h"

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;
Threadification::Threadification(CodeThorn::VariableIdMapping* vim):
  _variableIdMapping(vim) {
}

Threadification::~Threadification() {
}

void Threadification::transform(SgNode* root) {
  VariableIdSet varIdSet=determineVariablesOfInterest(root);
  transformVariableDeclarations(root,varIdSet);
  transformVariableAccesses(root,varIdSet);
}

VariableIdSet Threadification::determineVariablesOfInterest(SgNode* root) {
  VariableIdSet varIdSet;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      if(varDecl->get_bitfield ()) {
        // skip bitfield
        continue;
      }
      VariableId varId=_variableIdMapping->variableId(varDecl);
      // specifically exclude array variable declarations
      if( (_variableIdMapping->hasPointerType(varId)||_variableIdMapping->hasIntegerType(varId)) 
            && _variableIdMapping->variableName(varId) != "stdout" 
            && _variableIdMapping->variableName(varId) != "__PRETTY_FUNCTION__" ) {
        varIdSet.insert(varId);
      }
    }
  }
  return varIdSet;
}

void Threadification::transformVariableDeclaration(SgVariableDeclaration* varDecl) {
  SgNode* initName0=varDecl->get_traversalSuccessorByIndex(1); // get-InitializedName
  ROSE_ASSERT(initName0);
  SgInitializedName* initName=isSgInitializedName(initName0);
  ROSE_ASSERT(initName);
  SgSymbol* initDeclVarSymbol=initName->search_for_symbol_from_symbol_table();
  //VariableId initDeclVarId=getVariableIdMapping()->variableId(initName);
  SgName initDeclVarName=initDeclVarSymbol->get_name();
  string initDeclVarNameString=initDeclVarName.getString();
  //SgInitializer* initializer=initName->get_initializer(); // initializer not used in this transformation
  SgType* type=initName->get_type();
  string newVarDecl="\n"+type->unparseToString()+"*"+" "+initDeclVarNameString+";\n";
  SgNodeHelper::replaceAstWithString(varDecl,newVarDecl);
}

void Threadification::transformVariableDeclarations(SgNode* root, CodeThorn::VariableIdSet varIdSet) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      VariableId varId=_variableIdMapping->variableId(varDecl);
      if(varIdSet.find(varId)!=varIdSet.end()) {
        transformVariableDeclaration(varDecl);
      }
    }
  }
}

void Threadification::transformVariableAccess(SgVarRefExp* varRefExp) {
  string newVarRefExp=varRefExp->unparseToString()+"[thread_id]";
  SgNodeHelper::replaceAstWithString(varRefExp,newVarRefExp);
}

void Threadification::transformVariableAccess(SgPntrArrRefExp* arrRefExp, CodeThorn::VariableIdSet varIdSet) {
  SgNode* lhs=SgNodeHelper::getLhs(arrRefExp);
  SgNode* rhs=SgNodeHelper::getRhs(arrRefExp);
  transformVariableAccesses(rhs,varIdSet);
  string newArrayRefExp=lhs->unparseToString()+"[thread_id]"+"["+rhs->unparseToString()+"]";
  SgNodeHelper::replaceAstWithString(arrRefExp,newArrayRefExp);
}

void Threadification::transformVariableAccesses(SgNode* root, CodeThorn::VariableIdSet varIdSet) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVarRefExp* varRefExp=isSgVarRefExp(*i)) {
      // access of variable var
      VariableId varId=_variableIdMapping->variableId(varRefExp);
      if(varIdSet.find(varId)!=varIdSet.end()) {
        transformVariableAccess(varRefExp);
      }
    } else if(SgPntrArrRefExp* arrRefExp=isSgPntrArrRefExp(*i)) {
      // access of array/pointer var[Expr] (lhs=var, rhs=Expr)
      SgNode* lhs=SgNodeHelper::getLhs(arrRefExp);
      if(SgVarRefExp* arrayVarRefExp=isSgVarRefExp(lhs)) {
        VariableId varId=_variableIdMapping->variableId(arrayVarRefExp);
        if(varIdSet.find(varId)!=varIdSet.end()) {
          transformVariableAccess(arrRefExp,varIdSet);
          /* transformVariableAccess transforms the subtree of the
           array access recursively, therefore the subtree can be
           skipped. */
          i.skipChildrenOnForward();
        }
      }
    }
  }
}
