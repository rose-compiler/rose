#include "sage3basic.h"
#include "ProgramAbstractionLayer.h"
#include "Normalization.h"

#include <iostream>

using namespace std;

CodeThorn::ProgramAbstractionLayer::ProgramAbstractionLayer()
  :_modeArrayElementVariableId(true),_labeler(0),_variableIdMapping(0) {
}

CodeThorn::ProgramAbstractionLayer::~ProgramAbstractionLayer() {
}

void CodeThorn::ProgramAbstractionLayer::setModeArrayElementVariableId(bool val) {
  _modeArrayElementVariableId=val; 
}

bool CodeThorn::ProgramAbstractionLayer::getModeArrayElementVariableId() {
  return _modeArrayElementVariableId;; 
}

SgProject* CodeThorn::ProgramAbstractionLayer::getRoot() {
  return _root;
}

void CodeThorn::ProgramAbstractionLayer::initialize(SgProject* root) {
  _root=root;
  CodeThorn::Normalization lowering;
  lowering.setInliningOption(getInliningOption());
  lowering.normalizeAst(root,getNormalizationLevel());
  _variableIdMapping=new VariableIdMapping();
  getVariableIdMapping()->setModeVariableIdForEachArrayElement(getModeArrayElementVariableId());
  getVariableIdMapping()->computeVariableSymbolMapping(root);
  _labeler=new Labeler(root);
  _functionIdMapping=new FunctionIdMapping();
  getFunctionIdMapping()->computeFunctionSymbolMapping(root);
}

CodeThorn::Labeler* CodeThorn::ProgramAbstractionLayer::getLabeler(){
  ROSE_ASSERT(_labeler!=0);
  return _labeler;
}

CodeThorn::VariableIdMapping* CodeThorn::ProgramAbstractionLayer::getVariableIdMapping(){
  ROSE_ASSERT(_variableIdMapping!=0);
  return _variableIdMapping;
}

CodeThorn::FunctionIdMapping* CodeThorn::ProgramAbstractionLayer::getFunctionIdMapping(){
  ROSE_ASSERT(_functionIdMapping!=0);
  return _functionIdMapping;
}

void CodeThorn::ProgramAbstractionLayer::setNormalizationLevel(unsigned int level) {
  _normalizationLevel=level;
}

bool CodeThorn::ProgramAbstractionLayer::getNormalizationLevel() {
  return _normalizationLevel;
}

void CodeThorn::ProgramAbstractionLayer::setInliningOption(bool flag) {
  _inliningOption=flag;
}

bool CodeThorn::ProgramAbstractionLayer::getInliningOption() {
  return _inliningOption;
}
