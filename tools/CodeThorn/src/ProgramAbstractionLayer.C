#include "sage3basic.h"
#include "ProgramAbstractionLayer.h"
#include "ClassHierarchyGraph.h"
#include "Normalization.h"
#include "CTIOLabeler.h"
#include "CodeThornLib.h"
#include "CodeThornPasses.h"

#include <iostream>

using namespace std;

CodeThorn::ProgramAbstractionLayer::ProgramAbstractionLayer()
  :_labeler(0),_variableIdMapping(0) {
}

CodeThorn::ProgramAbstractionLayer::~ProgramAbstractionLayer() {
  delete _cfanalyzer;
}

SgProject* CodeThorn::ProgramAbstractionLayer::getRoot() {
  return _root;
}

void CodeThorn::ProgramAbstractionLayer::initialize(CodeThornOptions& ctOpt, SgProject* root) {
  TimingCollector tc; // dummy object if no timing info is requested
  initialize(ctOpt,root,tc);
}


void CodeThorn::ProgramAbstractionLayer::initialize(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc) {
  _root=root;

  Pass::normalization(ctOpt,root,tc);
  setNormalizationLevel(ctOpt.normalizeLevel); // only for get function
  _variableIdMapping=Pass::createVariableIdMapping(ctOpt, root, tc);
  _labeler=Pass::createLabeler(ctOpt, root, tc, _variableIdMapping);
  _classAnalysis=Pass::createClassAnalysis(ctOpt, root, tc);
  _virtualFunctions=Pass::createVirtualFunctionAnalysis(ctOpt, _classAnalysis, tc);
  _cfanalyzer=Pass::createForwardIcfg(ctOpt,root,tc,_labeler,_classAnalysis,_virtualFunctions);
  _fwFlow=*_cfanalyzer->getIcfgFlow();
  _bwFlow=_fwFlow.reverseFlow();
  _interFlow=*_cfanalyzer->getInterFlow();

  if(ctOpt.status) cout<<"STATUS: Abstraction layer established."<<endl;

}

CodeThorn::InterFlow* CodeThorn::ProgramAbstractionLayer::getInterFlow() {
  return &_interFlow;
}

void CodeThorn::ProgramAbstractionLayer::setForwardFlow(const Flow& fwdflow)
{
  _fwFlow = fwdflow;
  _bwFlow = _fwFlow.reverseFlow();
}

void CodeThorn::ProgramAbstractionLayer::setLabeler(Labeler* labeler)
{
  ROSE_ASSERT(labeler != NULL);
  _labeler = labeler;
}


CodeThorn::Flow* CodeThorn::ProgramAbstractionLayer::getFlow(bool backwardflow)
{
  if (backwardflow) return &_bwFlow;

  return &_fwFlow;
}

CodeThorn::CFAnalysis* CodeThorn::ProgramAbstractionLayer::getCFAnalyzer()
{
  ROSE_ASSERT(_cfanalyzer);
  return _cfanalyzer;
}

CodeThorn::Labeler* CodeThorn::ProgramAbstractionLayer::getLabeler(){
  ROSE_ASSERT(_labeler!=0);
  return _labeler;
}

CodeThorn::VariableIdMappingExtended* CodeThorn::ProgramAbstractionLayer::getVariableIdMapping(){
  ROSE_ASSERT(_variableIdMapping!=0);
  return _variableIdMapping;
}

CodeThorn::FunctionCallMapping* CodeThorn::ProgramAbstractionLayer::getFunctionCallMapping(){
  return _cfanalyzer->getFunctionCallMapping();
}

CodeThorn::FunctionCallMapping2* CodeThorn::ProgramAbstractionLayer::getFunctionCallMapping2(){
  return _cfanalyzer->getFunctionCallMapping2();
}

void CodeThorn::ProgramAbstractionLayer::setNormalizationLevel(unsigned int level) {
  _normalizationLevel=level;
}

unsigned int CodeThorn::ProgramAbstractionLayer::getNormalizationLevel() {
  return _normalizationLevel;
}

void CodeThorn::ProgramAbstractionLayer::setInliningOption(bool flag) {
  _inliningOption=flag;
}

bool CodeThorn::ProgramAbstractionLayer::getInliningOption() {
  return _inliningOption;
}
