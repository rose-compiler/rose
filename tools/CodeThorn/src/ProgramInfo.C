
#include "sage3basic.h"
#include "ProgramInfo.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "FunctionCallMapping.h"
#include "CppStdUtilities.h"

using namespace CodeThorn;
using namespace std;

ProgramInfo::ProgramInfo(ProgramAbstractionLayer* pal) {
  _root=pal->getRoot();
  _programAbstractionLayer=pal;
  _variableIdMapping=pal->getVariableIdMapping();
  initCount();
}

/*
ProgramInfo::ProgramInfo(SgProject* root) {
  this->_root=root;
  initCount();
}
*/

ProgramInfo::ProgramInfo(SgProject* root, VariableIdMappingExtended* vim) {
  this->_root=root;
  this->_variableIdMapping=vim;
  initCount();
}

void ProgramInfo::initCount() {
  for(int i=0;i<Element::NUM;i++) {
    count[i]=0;
  }
  countNameMap[Element::numFunDefs]         ="Function defs     ";
  countNameMap[Element::numFunCall]         ="Function calls    ";
  countNameMap[Element::numFunPtrCall]      ="Function ptr calls";
  countNameMap[Element::numForLoop]         ="For loops         ";
  countNameMap[Element::numWhileLoop]       ="While loops       ";
  countNameMap[Element::numDoWhileLoop]     ="Do-While loops    ";
  countNameMap[Element::numConditionalExp]  ="Conditional ops   ";
  countNameMap[Element::numLogicOrOp]       ="Logic-or ops      ";
  countNameMap[Element::numLogicAndOp]      ="Logic-and ops     ";
  countNameMap[Element::numArrayAccess]     ="Array access ops  ";
  countNameMap[Element::numArrowOp]         ="Arrow ops         ";
  countNameMap[Element::numDerefOp]         ="Deref ops         ";
  countNameMap[Element::numStructAccess]    ="Dot ops           ";
}

void ProgramInfo::compute() {
  ROSE_ASSERT(_root);
  RoseAst ast(_root);
  ROSE_ASSERT(_variableIdMapping);
  _validData=true;
  for (auto node : ast) {
    if(isSgFunctionDefinition(node)) {
      count[numFunDefs]++;
    } else if(SgFunctionCallExp* fc=isSgFunctionCallExp(node)) {
      if(FunctionCallMapping::isAstFunctionPointerCall(fc)) {
	count[numFunPtrCall]++;
	_functionPtrCallNodes.push_back(fc);
      } else {
	count[numFunCall]++;
	_functionCallNodes.push_back(fc);
      }
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

std::string ProgramInfo::toCsvStringDetailed() {
  return toCsvStringDetailed(0);
}

std::string ProgramInfo::toCsvStringDetailed(VariableIdMappingExtended* vid) {
  ROSE_ASSERT(_validData);
  return toCsvStringCodeStats()+toCsvStringTypeStats(vid)+'\n';
}

std::string ProgramInfo::toCsvStringCodeStats() {
  ROSE_ASSERT(_validData);
  stringstream ss;
  for(int i=0;i<Element::NUM;i++) {
    if(i!=0)
      ss<<",";
    ss<<std::right<<std::setw(5)<<count[i];
  }
  return ss.str();
}

std::string ProgramInfo::toCsvStringTypeStats(VariableIdMappingExtended* vid) {
  if(vid) {
    // global + local:
    // * max total variables + struct members + array elements
    // * max array size (elements*element size)
    // * max struct size (struct elements)
    return "";
  } else {
    return "";
  }
}

bool ProgramInfo::toCsvFileDetailed(std::string fileName, std::string mode) {
  return CppStdUtilities::writeFile(mode, fileName, toCsvStringDetailed());
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

void ProgramInfo::writeFunctionCallNodesToFile(string fileName, Labeler*) {
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
