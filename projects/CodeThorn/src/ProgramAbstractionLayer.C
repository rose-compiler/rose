#include "sage3basic.h"
#include "ProgramAbstractionLayer.h"
#include "ClassHierarchyGraph.h"
#include "Normalization.h"
#include "CTIOLabeler.h"
#include "CodeThornLib.h"

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
  _root=root;
  if(ctOpt.status) cout<<"Phase: normalization"<<endl;
  normalizationPass(ctOpt,root);

  if(ctOpt.status) cout<<"Phase: variable-id mapping"<<endl;
  _variableIdMapping=CodeThorn::createVariableIdMapping(ctOpt,root);
  if(ctOpt.status) cout<<"Phase: program location labeling"<<endl;
  _labeler=createLabeler(root,_variableIdMapping);
  
  if(ctOpt.status) cout<<"Phase: class hierarchy analysis"<<endl;
  _classHierarchy=new ClassHierarchyWrapper(root);
  _cfanalyzer=new CFAnalysis(_labeler);
  _functionCallMapping=nullptr; 
  _functionCallMapping2=nullptr;

  if(ctOpt.status) cout<<"Phase: function call mapping"<<endl;
  if (!SgNodeHelper::WITH_EXTENDED_NORMALIZED_CALL)
  {
    // another function resolution mode
    _functionCallMapping = new FunctionCallMapping();
  
    getFunctionCallMapping()->setClassHierarchy(_classHierarchy);
    getFunctionCallMapping()->computeFunctionCallMapping(root);
    _cfanalyzer->setFunctionCallMapping(getFunctionCallMapping());
  }
  else
  {
    // PP (02/17/20) add class hierarchy and call mapping
    _functionCallMapping2=new FunctionCallMapping2();
    getFunctionCallMapping2()->setLabeler(_labeler);
    getFunctionCallMapping2()->setClassHierarchy(_classHierarchy);
    getFunctionCallMapping2()->computeFunctionCallMapping(root);
    _cfanalyzer->setFunctionCallMapping2(getFunctionCallMapping2());
  }
  
  //cout<< "DEBUG: mappingLabelToLabelProperty: "<<endl<<getLabeler()->toString()<<endl;
  if(ctOpt.status) cout<<"Phase: CFG construction"<<endl;
  if(ctOpt.status) cout<<"  - creating inter-procedural CFGs"<<endl;
  _fwFlow = _cfanalyzer->flow(root);
  if(ctOpt.status) cout<<"    CFG edges: " << _fwFlow.size()<<endl;
  if(ctOpt.status) cout<<"  - creating inter-procedural control flow"<<endl;
  _interFlow=_cfanalyzer->interFlow(_fwFlow);
  if(ctOpt.status) cout<<"    Call edges: " << _interFlow.size() <<endl;
  if(ctOpt.status) cout<<"  - Creating ICFG"<<endl;
  _cfanalyzer->intraInterFlow(_fwFlow,_interFlow);
  if(ctOpt.status) cout<<"  - ICFG total size: " << _fwFlow.size() << " edges"<<endl;
  if(ctOpt.status) cout<<"Phase: generating reverse ICFG"<<endl;
  _bwFlow = _fwFlow.reverseFlow();
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
  ROSE_ASSERT(_functionCallMapping!=0);
  return _functionCallMapping;
}

CodeThorn::FunctionCallMapping2* CodeThorn::ProgramAbstractionLayer::getFunctionCallMapping2(){
  ROSE_ASSERT(_functionCallMapping2!=0);
  return _functionCallMapping2;
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
