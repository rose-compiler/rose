/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"

#include "Labeler.h"
#include "SgNodeHelper.h"
//#include "AstTerm.h"
#include <sstream>

Label Labeler::NO_LABEL=Label::Label();

ostream& operator<<(ostream& os, const Label& label) {
  os<<label._labelId;
  return os;
}

LabelProperty::LabelProperty():_isValid(false),_node(0),_labelType(LABEL_UNDEF),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false) {
}
LabelProperty::LabelProperty(SgNode* node):_isValid(true),_node(node),_labelType(LABEL_UNDEF),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false) {
}
LabelProperty::LabelProperty(SgNode* node, LabelType labelType):_isValid(true),_node(node),_labelType(labelType),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false) {
}
LabelProperty::LabelProperty(SgNode* node, VariableIdMapping* variableIdMapping):_isValid(false),_node(node),_labelType(LABEL_UNDEF),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false) {
  initializeIO(variableIdMapping);
  assert(_isValid);
}
LabelProperty::LabelProperty(SgNode* node, LabelType labelType, VariableIdMapping* variableIdMapping):_isValid(false),_node(node),_labelType(labelType),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false) {
  initializeIO(variableIdMapping); 
  assert(_isValid);
}
void LabelProperty::initializeIO(VariableIdMapping* variableIdMapping) {
  _isValid=true; // to be able to use access functions in initialization
  SgVarRefExp* varRefExp=0;
  _ioType=LABELIO_NONE;
  if((varRefExp=SgNodeHelper::Pattern::matchSingleVarScanf(_node))) {
    _ioType=LABELIO_STDIN;
  } else if((varRefExp=SgNodeHelper::Pattern::matchSingleVarFPrintf(_node))) {
    _ioType=LABELIO_STDERR;
  } else {
    SgNodeHelper::Pattern::OutputTarget ot=SgNodeHelper::Pattern::matchSingleVarOrValuePrintf(_node);
    switch(ot.outType) {
    case SgNodeHelper::Pattern::OutputTarget::VAR:
      _ioType=LABELIO_STDOUTVAR;
      varRefExp=ot.varRef;
      break;
    case SgNodeHelper::Pattern::OutputTarget::INT:
      _ioType=LABELIO_STDOUTCONST;
      _ioValue=ot.intVal;
      break;
    case SgNodeHelper::Pattern::OutputTarget::UNKNOWNPRINTF:
      cerr<<"WARNING: non-supported output operation:"<<_node->unparseToString()<<endl;
      break;
    case SgNodeHelper::Pattern::OutputTarget::UNKNOWNOPERATION:
      ;//intentionally ignored (filtered)
    } // END SWITCH
  }
  if(varRefExp) {
    SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
    assert(sym);
    _variableId=variableIdMapping->variableId(sym);
  }
  _isTerminationRelevant=SgNodeHelper::isLoopCond(_node);
  _isLTLRelevant=(isIOLabel()||isTerminationRelevant());
  assert(varRefExp==0 ||_ioType!=LABELIO_NONE);
}

string LabelProperty::toString() {
  //assert(_isValid);
  stringstream ss;
  ss<<_node<<":";
  if(_node)
    ss<<SgNodeHelper::nodeToString(_node)<<", ";
  else
    ss<<"null, ";
  ss<<"var:"<<_variableId.toString()<<", ";
  ss<<"labelType:"<<_labelType<<", ";
  ss<<"ioType:"<<_ioType<<", ";
  ss<<"ltl:"<<isLTLRelevant()<<", ";
  ss<<"termination:"<<isTerminationRelevant();
  return ss.str();
}

SgNode* LabelProperty::getNode() { if(!_isValid) cout<<"ERROR:"<<toString()<<endl; assert(_isValid); return _node;}

bool LabelProperty::isFunctionCallLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONCALL; }
bool LabelProperty::isFunctionCallReturnLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONCALLRETURN; }
bool LabelProperty::isFunctionEntryLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONENTRY; }
bool LabelProperty::isFunctionExitLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONEXIT; }
bool LabelProperty::isBlockBeginLabel() { assert(_isValid); return _labelType==LABEL_BLOCKBEGIN; }
bool LabelProperty::isBlockEndLabel() { assert(_isValid); return _labelType==LABEL_BLOCKEND; }

void LabelProperty::makeTerminationIrrelevant(bool t) {assert(_isTerminationRelevant); _isTerminationRelevant=false;}
bool LabelProperty::isTerminationRelevant() {assert(_isValid); return _isTerminationRelevant;}
bool LabelProperty::isLTLRelevant() {assert(_isValid); return _isLTLRelevant;}
/* deprecated */ bool LabelProperty::isStdOutLabel() { assert(_isValid); return  isStdOutVarLabel()||isStdOutConstLabel();}
bool LabelProperty::isStdOutVarLabel() { assert(_isValid); return _ioType==LABELIO_STDOUTVAR; }
bool LabelProperty::isStdOutConstLabel() { assert(_isValid); return _ioType==LABELIO_STDOUTCONST; }
bool LabelProperty::isStdInLabel() { assert(_isValid); return _ioType==LABELIO_STDIN; }
bool LabelProperty::isStdErrLabel() { assert(_isValid); return _ioType==LABELIO_STDERR; }
bool LabelProperty::isIOLabel() { assert(_isValid); return isStdOutLabel()||isStdInLabel()||isStdErrLabel(); }
VariableId LabelProperty::getIOVarId() { assert(_ioType!=LABELIO_NONE); return _variableId; }
int LabelProperty::getIOConst() { assert(_ioType!=LABELIO_NONE); return _ioValue; }

Labeler::Labeler(){}
Labeler::Labeler(SgNode* start) {
  createLabels(start);
  computeNodeToLabelMapping();
}

// returns number of labels to be associated with node
int Labeler::isLabelRelevantNode(SgNode* node) {
  if(node==0) 
    return 0;

  /* special case: the incExpr in a SgForStatement is a raw SgExpression
   * hence, the root can be any unary or binary node. We therefore perform
   * a lookup in the AST to check whether we are inside a SgForStatement
   */
  if(SgNodeHelper::isForIncExpr(node))
    return 1;

  switch(node->variantT()) {
  case V_SgFunctionCallExp:
  case V_SgBasicBlock:
    return 2;
  case V_SgFunctionDefinition:
    return 2;
  case V_SgExprStatement:
    //special case of FunctionCall inside expression
    if(SgNodeHelper::Pattern::matchFunctionCall(node)) {
      //cout << "DEBUG: Labeler: assigning 2 labels for SgFunctionCallExp"<<endl;
      return 2;
    }
    else
      return 1;
  case V_SgIfStmt:
  case V_SgWhileStmt:
  case V_SgDoWhileStmt:
  case V_SgForStatement:
    //  case V_SgForInitStatement: // TODO: investigate: we might not need this
  case V_SgBreakStmt:
  case V_SgVariableDeclaration:
  case V_SgLabelStatement:
  case V_SgFunctionDeclaration:
  case V_SgNullStatement:
  case V_SgPragmaDeclaration:
  case V_SgReturnStmt:
    if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node)) {
      //cout << "DEBUG: Labeler: assigning 3 labels for SgReturnStmt(SgFunctionCallExp)"<<endl;
      return 3;
    }
    else
      return 1;
  default:
    return 0;
  }
}

void Labeler::registerLabel(LabelProperty lp) {
  mappingLabelToLabelProperty.push_back(lp);
  _isValidMappingNodeToLabel=false;
}

void Labeler::createLabels(SgNode* root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(int num=isLabelRelevantNode(*i)) {
      if(SgNodeHelper::Pattern::matchFunctionCall(*i)) {
        if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(*i)) {
          assert(num==3);
          registerLabel(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALL));
          registerLabel(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALLRETURN));
          registerLabel(LabelProperty(*i)); // return-stmt-label
        } else {
          assert(num==2);
          registerLabel(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALL));
          registerLabel(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALLRETURN));
        }
      } else if(isSgFunctionDefinition(*i)) {
        assert(num==2);
        registerLabel(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONENTRY));
        registerLabel(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONEXIT));
      } else if(isSgBasicBlock(*i)) {
        assert(num==2);
        registerLabel(LabelProperty(*i,LabelProperty::LABEL_BLOCKBEGIN));
        registerLabel(LabelProperty(*i,LabelProperty::LABEL_BLOCKEND));
      } else {
        // all other cases
        for(int j=0;j<num;j++) {
          registerLabel(LabelProperty(*i));
        }
      }
    }
       if(isSgExprStatement(*i)||isSgReturnStmt(*i)||isSgVariableDeclaration(*i))
      i.skipChildrenOnForward();
  }
  std::cout << "STATUS: Assigned "<<mappingLabelToLabelProperty.size()<< " labels."<<std::endl;
  //std::cout << "DEBUG: mappingLabelToLabelProperty:\n"<<this->toString()<<std::endl;
}

string Labeler::labelToString(Label lab) {
  stringstream ss;
  ss<<lab;
  return ss.str();
}

SgNode* Labeler::getNode(Label label) {
  if(label.getId()>=mappingLabelToLabelProperty.size() || label==Label()) {
    cerr << "Error: mapping size: "<<mappingLabelToLabelProperty.size();
    cerr << " getNode: label"<<label<<" => 0."<<endl;
    exit(1);
  }
  return mappingLabelToLabelProperty[label.getId()].getNode();
}

/* this access function has O(n). This is OK as this function should only be used rarely, whereas
   the function getNode is used frequently (and has O(1)).
*/
void Labeler::ensureValidNodeToLabelMapping() {
  if(_isValidMappingNodeToLabel)
    return;
  else
    computeNodeToLabelMapping();
}

void Labeler::computeNodeToLabelMapping() {
  mappingNodeToLabel.clear();
  std::cout << "INFO: computing node<->label with map size: "<<mappingLabelToLabelProperty.size()<<std::endl;
  for(Label i=0;i<mappingLabelToLabelProperty.size();++i) {
    SgNode* node=mappingLabelToLabelProperty[i.getId()].getNode();
    assert(node);
    // There exist nodes with multiple associated labels (1-3 labels). The labels are guaranteed
    // to be in consecutive increasing order. We only store the very first associated label for each
    // node and the access functions adjust the label as necessary.
    // This allows to provide an API where the user does not need to operate on sets but on
    // distinct labels only.
    if(mappingNodeToLabel.find(node)!=mappingNodeToLabel.end())
      continue;
    else
      mappingNodeToLabel[node]=i;
    //cout << "Mapping: "<<i<<" : "<<node<<SgNodeHelper::nodeToString(node)<<endl;
  }
  _isValidMappingNodeToLabel=true;
}

Label Labeler::getLabel(SgNode* node) {
  assert(node);
  if(!node) 
    return Label();
  if(_isValidMappingNodeToLabel) {
    if(mappingNodeToLabel.count(node)==0) {
      //cerr<<"WARNING: getLabel: no label associated with node: "<<node<<endl;
      return Label();
    }
    return mappingNodeToLabel[node];
  } else {
    computeNodeToLabelMapping(); // sets _isValidMappingNodeToLabel to true.
    return mappingNodeToLabel[node];
  }
  throw "Error: internal error getLabel.";
}

long Labeler::numberOfLabels() {
  return mappingLabelToLabelProperty.size();
}

Label Labeler::functionCallLabel(SgNode* node) {
  assert(SgNodeHelper::Pattern::matchFunctionCall(node));
  return getLabel(node);
}

Label Labeler::functionCallReturnLabel(SgNode* node) {
  assert(SgNodeHelper::Pattern::matchFunctionCall(node));
  // in its current implementation it is guaranteed that labels associated with the same node
  // are associated as an increasing sequence of labels
  Label lab=getLabel(node);
  if(lab==NO_LABEL) 
    return NO_LABEL;
  else
    return lab+1; 
}
Label Labeler::blockBeginLabel(SgNode* node) {
  assert(isSgBasicBlock(node));
  return getLabel(node);
}
Label Labeler::blockEndLabel(SgNode* node) {
  assert(isSgBasicBlock(node));
  Label lab=getLabel(node);
  if(lab==NO_LABEL) 
    return NO_LABEL;
  else
    return lab+1; 
}
Label Labeler::functionEntryLabel(SgNode* node) {
  assert(isSgFunctionDefinition(node));
  return getLabel(node);
}
Label Labeler::functionExitLabel(SgNode* node) {
  assert(isSgFunctionDefinition(node));
  Label lab=getLabel(node);
  if(lab==NO_LABEL) 
    return NO_LABEL;
  else
    return lab+1; 
}

bool Labeler::isConditionLabel(Label lab) {
  return SgNodeHelper::isCond(getNode(lab));
}

bool Labeler::isFirstLabelOfMultiLabeledNode(Label lab) {
  return isFunctionCallLabel(lab)||isFunctionEntryLabel(lab)||isBlockBeginLabel(lab);
}

bool Labeler::isSecondLabelOfMultiLabeledNode(Label lab) {
  return isFunctionCallReturnLabel(lab)||isFunctionExitLabel(lab)||isBlockEndLabel(lab);
}

bool Labeler::isFunctionEntryLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionEntryLabel();
}

bool Labeler::isFunctionExitLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionExitLabel();
}
bool Labeler::isBlockBeginLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isBlockBeginLabel();
}

bool Labeler::isBlockEndLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isBlockEndLabel();
}

bool Labeler::isFunctionCallLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionCallLabel();
}

bool Labeler::isFunctionCallReturnLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionCallReturnLabel();
}

LabelSet Labeler::getLabelSet(set<SgNode*>& nodeSet) {
  LabelSet lset;
  for(set<SgNode*>::iterator i=nodeSet.begin();i!=nodeSet.end();++i) {
    lset.insert(getLabel(*i));
  }
  return lset;
}

std::string Labeler::toString() {
  std::stringstream ss;
  for(Label i=0;i<mappingLabelToLabelProperty.size();++i) {
    LabelProperty lp=mappingLabelToLabelProperty[i.getId()];
    ss << i<< ":"<<lp.toString()<<endl;
  }
  return ss.str();
}

Labeler::iterator::iterator():_currentLabel(0),_numLabels(0) {
  ROSE_ASSERT(is_past_the_end());
}
Labeler::iterator::iterator(Label start, size_t numLabels):_currentLabel(start),_numLabels(numLabels) {
}

bool Labeler::iterator::operator==(const iterator& x) const { 
  return (is_past_the_end() && x.is_past_the_end())
    || (_numLabels==x._numLabels && _currentLabel==x._currentLabel)
    ;
}

bool Labeler::iterator::operator!=(const iterator& x) const { 
  return !(*this==x);
}

Label Labeler::iterator::operator*() const { 
  return _currentLabel;
}

Labeler::iterator&
Labeler::iterator::operator++() {
  if(is_past_the_end())
    return *this;
  else
    ++_currentLabel;
  return *this;
}

Labeler::iterator Labeler::iterator::operator++(int) {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

bool Labeler::iterator::is_past_the_end() const {
  return _currentLabel>=_numLabels;
}

Labeler::iterator Labeler::begin() {
  ensureValidNodeToLabelMapping();
  return iterator(0,numberOfLabels());
}

Labeler::iterator Labeler::end() {
  return iterator(0,0);
}


/*
  IO Labeler Implementation
*/

// note: calling Labeler(start) would be wrong because it would call createLabels twice.
IOLabeler::IOLabeler(SgNode* start, VariableIdMapping* variableIdMapping):Labeler() {
  _variableIdMapping=variableIdMapping;
  createLabels(start);
  // initialize all labels' property with additional IO info
  for(LabelToLabelPropertyMapping::iterator i=mappingLabelToLabelProperty.begin();i!=mappingLabelToLabelProperty.end();++i) {
    (*i).initializeIO(variableIdMapping);
  }
  computeNodeToLabelMapping();
}

bool IOLabeler::isStdOutLabel(Label label) {
  return isStdOutVarLabel(label)||isStdOutConstLabel(label);
}

bool IOLabeler::isStdOutVarLabel(Label label, VariableId* id) {
  bool res=false;
  res=mappingLabelToLabelProperty[label.getId()].isStdOutVarLabel();
  if(res&&id)
    *id=mappingLabelToLabelProperty[label.getId()].getIOVarId();
  return res;
}

bool IOLabeler::isStdOutConstLabel(Label label, int* value) {
  bool res=false;
  res=mappingLabelToLabelProperty[label.getId()].isStdOutConstLabel();
  if(res&&value)
    *value=mappingLabelToLabelProperty[label.getId()].getIOConst();
  return res;
}

bool IOLabeler::isStdInLabel(Label label, VariableId* id) {
  bool res=false;
  res=mappingLabelToLabelProperty[label.getId()].isStdInLabel();
  if(res&&id)
    *id=mappingLabelToLabelProperty[label.getId()].getIOVarId();
  return res;
}

bool IOLabeler::isStdErrLabel(Label label, VariableId* id) {
  bool res=false;
  res=mappingLabelToLabelProperty[label.getId()].isStdErrLabel();
  if(res&&id)
    *id=mappingLabelToLabelProperty[label.getId()].getIOVarId();
  return res;
}

