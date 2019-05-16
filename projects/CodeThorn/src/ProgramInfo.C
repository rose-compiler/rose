
#include "sage3basic.h"
#include "ProgramInfo.h"
#include <iostream>
#include <sstream>

using namespace CodeThorn;
using namespace std;

ProgramInfo::ProgramInfo(ProgramAbstractionLayer* pal) {
  root=pal->getRoot();
  _programAbstractionLayer=pal;
}

ProgramInfo::ProgramInfo(SgProject* root) {
  this->root=root;
}

void ProgramInfo::compute() {
  ROSE_ASSERT(root);
  RoseAst ast(root);
  for (auto node : ast) {
    if(isSgFunctionCallExp(node)) {
      numFunCall++;
    } else if(isSgWhileStmt(node)) {
      numWhileLoop++;
    } else if(isSgDoWhileStmt(node)) {
      numDoWhileLoop++;
    } else if(isSgForStatement(node)) {
      numForLoop++;
    } else if(isSgOrOp(node)) {
      numLogicOrOp++;
    } else if(isSgAndOp(node)) {
      numLogicAndOp++;
    } else if(isSgConditionalExp(node)) {
      numConditionalExp++;
    } else if(isSgArrowExp(node)) {
      numArrowOp++;
    } else if(isSgPointerDerefExp(node)) {
      numDerefOp++;
    } else if(isSgDotExp(node)) {
      numStructAccess++;
    }
  }
}

void ProgramInfo::printDetailed() {
  cout<<toStringDetailed();
}

std::string ProgramInfo::toStringDetailed() {
  stringstream ss;
  ss<<"Function calls  : "<<numFunCall<<endl;
  ss<<"While loops     : "<<numWhileLoop<<endl;
  ss<<"Do-While loops  : "<<numDoWhileLoop<<endl;
  ss<<"For loops       : "<<numForLoop<<endl;
  ss<<"Logic-or ops    : "<<numLogicOrOp<<endl;
  ss<<"Logic-and ops   : "<<numLogicAndOp<<endl;
  ss<<"Conditional ops : "<<numConditionalExp<<endl;
  ss<<"Arrow ops       : "<<numArrowOp<<endl;
  ss<<"Dereference ops : "<<numDerefOp<<endl;
  ss<<"Dot ops         : "<<numStructAccess<<endl;
  return ss.str();
}
