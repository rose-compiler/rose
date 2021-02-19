
#include "sage3basic.h"
#include "ProgramInfo.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace CodeThorn;
using namespace std;

ProgramInfo::ProgramInfo(ProgramAbstractionLayer* pal) {
  root=pal->getRoot();
  _programAbstractionLayer=pal;
  initCount();
}

ProgramInfo::ProgramInfo(SgProject* root) {
  this->root=root;
  initCount();
}

void ProgramInfo::initCount() {
  for(int i=0;i<Element::NUM;i++) {
    count[i]=0;
  }
  countNameMap[Element::numGlobalVars]      ="Global var decls";
  countNameMap[Element::numLocalVars]       ="Local var decls ";
  countNameMap[Element::numFunDefs]         ="Function defs   ";
  countNameMap[Element::numFunCall]         ="Function calls  ";
  countNameMap[Element::numFunCall]         ="Function calls  ";
  countNameMap[Element::numForLoop]         ="For loops       ";
  countNameMap[Element::numWhileLoop]       ="While loops     ";
  countNameMap[Element::numDoWhileLoop]     ="Do-While loops  ";
  countNameMap[Element::numConditionalExp]  ="Conditional ops ";
  countNameMap[Element::numLogicOrOp]       ="Logic-or ops    ";
  countNameMap[Element::numLogicAndOp]      ="Logic-and ops   ";
  countNameMap[Element::numArrayAccess]     ="Array access ops";
  countNameMap[Element::numArrowOp]         ="Arrow ops       ";
  countNameMap[Element::numDerefOp]         ="Deref ops       ";
  countNameMap[Element::numStructAccess]    ="Dot ops         ";
}

void ProgramInfo::compute() {
  ROSE_ASSERT(root);
  RoseAst ast(root);
  _validData=true;
  for (auto node : ast) {
    if(auto funDef=isSgFunctionDefinition(node)) {
      count[numFunDefs]++;
      std::set<SgVariableDeclaration*> localVarDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      count[numLocalVars]+=localVarDecls.size();
    } else if(SgFunctionCallExp* fc=isSgFunctionCallExp(node)) {
      count[numFunCall]++;
      _functionCallNodes.push_back(fc);
    } else if(isSgWhileStmt(node)) {
      count[numWhileLoop]++;
    } else if(isSgDoWhileStmt(node)) {
      count[numDoWhileLoop]++;
    } else if(isSgForStatement(node)) {
      count[numForLoop]++;
    } else if(isSgOrOp(node)) {
      count[numLogicOrOp]++;
    } else if(isSgAndOp(node)) {
      count[numLogicAndOp]++;
    } else if(isSgConditionalExp(node)) {
      count[numConditionalExp]++;
    } else if(isSgArrowExp(node)) {
      count[numArrowOp]++;
    } else if(isSgPointerDerefExp(node)) {
      count[numDerefOp]++;
    } else if(isSgDotExp(node)) {
      count[numStructAccess]++;
    } else if(SgNodeHelper::isArrayAccess(node)) {
      count[numArrayAccess]++;
    }
  }
  if(SgProject* proj=isSgProject(root)) {
    std::list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(proj);
    count[numGlobalVars]=globalVars.size();
  }
}

void ProgramInfo::printDetailed() {
  ROSE_ASSERT(_validData);
  cout<<toStringDetailed();
}

void ProgramInfo::printCompared(ProgramInfo* other) {
  ROSE_ASSERT(_validData);
  cout<<toStringCompared(other);
}

std::string ProgramInfo::toStringDetailed() {
  return toStringCompared(0);
}

std::string ProgramInfo::toStringCompared(ProgramInfo* other) {
  ROSE_ASSERT(_validData);
  stringstream ss;
  for(int i=0;i<Element::NUM;i++) {
    ss<<countNameMap[static_cast<Element>(i)]<<": "<<std::right<<std::setw(8)<<count[i];
    if(other) {
      ss<<" : "<<std::right<<std::setw(8)<<other->count[i];
    }
    ss<<endl;
  }
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
