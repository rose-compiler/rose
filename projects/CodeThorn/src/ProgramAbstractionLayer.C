#include "sage3basic.h"
#include "ProgramAbstractionLayer.h"
#include "Normalization.h"

#include <iostream>

using namespace std;

CodeThorn::ProgramAbstractionLayer::ProgramAbstractionLayer()
  :_modeArrayElementVariableId(true),_labeler(0),_variableIdMapping(0) {
}

CodeThorn::ProgramAbstractionLayer::~ProgramAbstractionLayer() {
  delete _cfanalyzer;
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
  cout << "INIT: Normalizing " << getNormalizationLevel() << endl;
  CodeThorn::Normalization lowering;
  lowering.setInliningOption(getInliningOption());
  lowering.normalizeAst(root,getNormalizationLevel());
  _variableIdMapping=new VariableIdMapping();
  getVariableIdMapping()->setModeVariableIdForEachArrayElement(getModeArrayElementVariableId());
  getVariableIdMapping()->computeVariableSymbolMapping(root);
  _labeler=new Labeler(root);
  _functionIdMapping=new FunctionIdMapping();
  getFunctionIdMapping()->computeFunctionSymbolMapping(root);

  // PP (07/15/19) moved flow generation from DFAnalysisBase class
  _cfanalyzer = new CFAnalysis(_labeler);

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

unsigned int CodeThorn::ProgramAbstractionLayer::getNormalizationLevel() {
  return _normalizationLevel;
}

void CodeThorn::ProgramAbstractionLayer::setInliningOption(bool flag) {
  _inliningOption=flag;
}

bool CodeThorn::ProgramAbstractionLayer::getInliningOption() {
  return _inliningOption;
}
