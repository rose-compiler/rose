
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
  _validData=true;
  for (auto node : ast) {
    if(SgFunctionCallExp* fc=isSgFunctionCallExp(node)) {
      numFunCall++;
      _functionCallNodes.push_back(fc);
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
  ROSE_ASSERT(_validData);
  cout<<toStringDetailed();
}

std::string ProgramInfo::toStringDetailed() {
  ROSE_ASSERT(_validData);
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

void ProgramInfo::writeFunctionCallNodesToFile(string fileName, Labeler* labeler) {
  ROSE_ASSERT(_validData);
  std::ofstream outFile;
  outFile.open(fileName.c_str(),std::ios::out);
  if (outFile.fail()) {
    cerr << "Error: couldn't open file "<<fileName<<" for generating program function call info."<<endl;
    exit(1);
  }
  for(auto node : _functionCallNodes) {
    if(node) {
      outFile<<SgNodeHelper::sourceLineColumnToString(node,";");
      outFile<<";";
      outFile<<SgNodeHelper::nodeToString(node);
      outFile<<";";
      outFile<<SgNodeHelper::sourceFilenameToString(node);
      outFile<<endl;
    }
  }
  outFile.close();
}
