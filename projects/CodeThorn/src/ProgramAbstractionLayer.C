#include "sage3basic.h"
#include "ProgramAbstractionLayer.h"
#include "ClassHierarchyGraph.h"
#include "Normalization.h"

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

void CodeThorn::ProgramAbstractionLayer::initialize(SgProject* root) {
  _root=root;
  cout << "INIT: Normalization level " << getNormalizationLevel() << endl;
  CodeThorn::Normalization normalization;
  normalization.setInliningOption(getInliningOption());
  normalization.normalizeAst(root,getNormalizationLevel());
  _variableIdMapping=new VariableIdMappingExtended();
  getVariableIdMapping()->computeVariableSymbolMapping(root);
  _labeler=new Labeler(root);
  
  _classHierarchy=new ClassHierarchyWrapper(root);
  _cfanalyzer=new CFAnalysis(_labeler);
  _functionIdMapping=nullptr; 
  _functionCallMapping=nullptr; 
  _functionCallMapping2=nullptr;

  if (!SgNodeHelper::WITH_EXTENDED_NORMALIZED_CALL)
  {
    // a function resolution mode
    _functionIdMapping = new FunctionIdMapping();
    
    getFunctionIdMapping()->computeFunctionSymbolMapping(root);
    _cfanalyzer->setFunctionIdMapping(getFunctionIdMapping());
    
    // another function resolution mode
    _functionCallMapping = new FunctionCallMapping();
  
    getFunctionCallMapping()->setClassHierarchy(getClassHierarchy());
    getFunctionCallMapping()->computeFunctionCallMapping(root);
    _cfanalyzer->setFunctionCallMapping(getFunctionCallMapping());
  }
  else
  {
    // PP (02/17/20) add class hierarchy and call mapping
    _functionCallMapping2=new FunctionCallMapping2();
    getFunctionCallMapping2()->setLabeler(_labeler);
    getFunctionCallMapping2()->setClassHierarchy(getClassHierarchy());
    getFunctionCallMapping2()->computeFunctionCallMapping(root);
    _cfanalyzer->setFunctionCallMapping2(getFunctionCallMapping2());
  }
  
  //cout<< "DEBUG: mappingLabelToLabelProperty: "<<endl<<getLabeler()->toString()<<endl;
  cout << "INIT: Building CFG for each function."<<endl;
  _fwFlow = _cfanalyzer->flow(root);
  cout << "STATUS: Building CFGs finished."<<endl;
  cout << "INIT: Intra-Flow OK. (size: " << _fwFlow.size() << " edges)"<<endl;
  InterFlow interFlow=_cfanalyzer->interFlow(_fwFlow);
  cout << "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
  _cfanalyzer->intraInterFlow(_fwFlow,interFlow);
  cout << "INIT: IntraInter-CFG OK. (size: " << _fwFlow.size() << " edges)"<<endl;

  _bwFlow = _fwFlow.reverseFlow();
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

CodeThorn::FunctionIdMapping* CodeThorn::ProgramAbstractionLayer::getFunctionIdMapping(){
  ROSE_ASSERT(_functionIdMapping!=0);
  return _functionIdMapping;
}

CodeThorn::FunctionCallMapping* CodeThorn::ProgramAbstractionLayer::getFunctionCallMapping(){
  ROSE_ASSERT(_functionCallMapping!=0);
  return _functionCallMapping;
}

CodeThorn::FunctionCallMapping2* CodeThorn::ProgramAbstractionLayer::getFunctionCallMapping2(){
  ROSE_ASSERT(_functionCallMapping2!=0);
  return _functionCallMapping2;
}

ClassHierarchyWrapper* CodeThorn::ProgramAbstractionLayer::getClassHierarchy(){
  ROSE_ASSERT(_classHierarchy!=0);
  return _classHierarchy;
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
