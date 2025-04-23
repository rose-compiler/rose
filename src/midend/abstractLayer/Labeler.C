#include "sage3basic.h"
#include "CodeThornException.h"

#include "Labeler.h"
#include "SgNodeHelper.h"
#include "sageInterface.h"

#include <sstream>
#include <numeric>

using namespace std;
using namespace CodeThorn;

namespace si = SageInterface;

Label Labeler::NO_LABEL;

LabelProperty::LabelProperty():_isValid(false),_node(0),_labelType(LABEL_UNDEF),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false),_isExternalFunctionCallLabel(false) {
}
LabelProperty::LabelProperty(SgNode* node):_isValid(true),_node(node),_labelType(LABEL_UNDEF),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false),_isExternalFunctionCallLabel(false) {
}
LabelProperty::LabelProperty(SgNode* node, LabelType labelType):_isValid(true),_node(node),_labelType(labelType),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false),_isExternalFunctionCallLabel(false) {
}
LabelProperty::LabelProperty(SgNode* node, VariableIdMapping* variableIdMapping):_isValid(false),_node(node),_labelType(LABEL_UNDEF),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false),_isExternalFunctionCallLabel(false) {
  initializeIO(variableIdMapping);
  assert(_isValid);
}
LabelProperty::LabelProperty(SgNode* node, LabelType labelType, VariableIdMapping* variableIdMapping):_isValid(false),_node(node),_labelType(labelType),_ioType(LABELIO_NONE),_isTerminationRelevant(false),_isLTLRelevant(false),_isExternalFunctionCallLabel(false) {
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
  stringstream ss;
  ss<<_node<<":";
  if(_node) {
    ss<<_node->class_name()
      <<":"
      <<SgNodeHelper::nodeToString(_node)<<", ";
  } else {
    ss<<"null, ";
  }
  ss<<"var:"<<_variableId.toString()<<", ";
  ss<<"labelType:"<<labelTypeToString(_labelType)<<", ";
  ss<<"ioType:"<<_ioType<<", ";
  ss<<"ltl:"<<isLTLRelevant()<<", ";
  ss<<"termination:"<<isTerminationRelevant();
  return ss.str();
}

string LabelProperty::labelTypeToString(LabelType lt) {
  switch(lt) {
  case LABEL_UNDEF: return "undef";
  case LABEL_OTHER: return "other";
  case LABEL_FUNCTIONCALL: return "functioncall";
  case LABEL_FUNCTIONCALLRETURN: return "functioncallreturn";
  case LABEL_FUNCTIONENTRY: return "functionentry";
  case LABEL_FUNCTIONEXIT: return "functionexit";
  case LABEL_BLOCKBEGIN: return "blockbegin";
  case LABEL_BLOCKEND: return "blockend";
  case LABEL_EMPTY_STMT: return "emptystmt";
  case LABEL_FORK: return "fork";
  case LABEL_JOIN: return "join";
  case LABEL_WORKSHARE: return "workshare";
  case LABEL_BARRIER: return "barrier";
  default:
    throw CodeThorn::Exception("Error: unknown label type.");
  }
}

SgNode* LabelProperty::getNode() {
  if(!_isValid) {
    cout<<"ERROR:"<<toString()<<endl;
  }
  assert(_isValid);
  return _node;
}

bool LabelProperty::isFunctionCallLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONCALL; }
bool LabelProperty::isFunctionCallReturnLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONCALLRETURN; }
bool LabelProperty::isFunctionEntryLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONENTRY; }
bool LabelProperty::isFunctionExitLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONEXIT; }
bool LabelProperty::isBlockBeginLabel() { assert(_isValid); return _labelType==LABEL_BLOCKBEGIN; }
bool LabelProperty::isBlockEndLabel() { assert(_isValid); return _labelType==LABEL_BLOCKEND; }
bool LabelProperty::isEmptyStmtLabel() { assert(_isValid); return _labelType==LABEL_EMPTY_STMT; }
bool LabelProperty::isForkLabel() { assert(_isValid); return _labelType == LABEL_FORK; }
bool LabelProperty::isJoinLabel() { assert(_isValid); return _labelType == LABEL_JOIN; }
bool LabelProperty::isWorkshareLabel() { assert(_isValid); return _labelType == LABEL_WORKSHARE; }
bool LabelProperty::isBarrierLabel() { assert(_isValid); return _labelType == LABEL_BARRIER; }

void LabelProperty::makeTerminationIrrelevant(bool) {assert(_isTerminationRelevant); _isTerminationRelevant=false;}
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

CLabeler::CLabeler()
: Labeler(), isFunctionCallNode(SgNodeHelper::Pattern::matchFunctionCall)
{}

void CLabeler::initialize(SgNode* start)
{
  createLabels(start);
  computeNodeToLabelMapping();
}

#if OBSOLETE_CODE
// C++ initializer lists will be normalized
namespace
{
  struct InitListLabelCounter
  {
    int operator()(int total, SgInitializedName* el) const
    {
      ROSE_ASSERT(el);

      // count 1 label for AssignInitializer and 2 for SgConstructorInitializer
      if (isSgConstructorInitializer(el->get_initializer())) ++total;

      return ++total;
    }
  };

  int countLabels(SgCtorInitializerList* n)
  {
    SgInitializedNamePtrList& inits = n->get_ctors();

    return std::accumulate(inits.begin(), inits.end(), 0, InitListLabelCounter());
  }
}
#endif /* OBSOLETE_CODE */

LabelSet CLabeler::getLabelSet(set<SgNode*>& nodeSet) {
  LabelSet lset;
  for(set<SgNode*>::iterator i=nodeSet.begin();i!=nodeSet.end();++i) {
    lset.insert(getLabel(*i));
  }
  return lset;
}

// returns number of labels to be associated with node
int CLabeler::numberOfAssociatedLabels(SgNode* node) {
  if(node==0)
    return 0;

  /* special case: the incExpr in a SgForStatement is a raw SgExpression
   * hence, the root can be any unary or binary node. We therefore perform
   * a lookup in the AST to check whether we are inside a SgForStatement
   */
  if(SgNodeHelper::isForIncExpr(node))
    return 1;

  // special case of FunctionCall is matched as : return f(...);
  if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node)) {
    return 3;
  }

  //special case of FunctionCall is matched as : f(), x=f(); T x=f();
  if(isFunctionCallNode(node)) {
    return 2;
  }

  switch(node->variantT()) {
    //  case V_SgFunctionCallExp:
  case V_SgBasicBlock:
  case V_SgTryStmt:
    return 1;
  case V_SgFunctionDefinition:
    return 2;
  case V_SgExprStatement:
    return 1;
  case V_SgIfStmt:
  case V_SgWhileStmt:
  case V_SgDoWhileStmt:
  case V_SgForStatement:
    //  case V_SgForInitStatement: // not necessary
  case V_SgBreakStmt:
  case V_SgContinueStmt:
  case V_SgGotoStatement:
  case V_SgLabelStatement:
  case V_SgNullStatement:
  case V_SgSwitchStatement:
  case V_SgDefaultOptionStmt:
  case V_SgCaseOptionStmt:
    return 1;

    // declarations
  case V_SgPragmaDeclaration:
  case V_SgFunctionDeclaration:
  case V_SgVariableDeclaration:
  case V_SgClassDeclaration:
  case V_SgEnumDeclaration:
  case V_SgTypedefDeclaration:
  case V_SgStaticAssertionDeclaration:
  case V_SgUsingDeclarationStatement:
    return 1;

  // All OpenMP constructs are in the CFG
  // omp parallel results in fork/join nodes.
  // for and sections (and currently also SIMD) results in workshare/barrier nodes
  case V_SgOmpForSimdStatement:
  case V_SgOmpSimdStatement:
  case V_SgOmpForStatement:
  case V_SgOmpSectionsStatement:
  case V_SgOmpParallelStatement:
    return 2;
  // OpenMP barrier results in an actual barrier node
  case V_SgOmpBarrierStatement:
  case V_SgOmpCriticalStatement:
  case V_SgOmpAtomicStatement:
  case V_SgOmpDoStatement:
  case V_SgOmpFlushStatement:
  case V_SgOmpMasterStatement:
  case V_SgOmpOrderedStatement:
  case V_SgOmpSectionStatement:
  case V_SgOmpSingleStatement:
  case V_SgOmpTargetDataStatement:
  case V_SgOmpTargetStatement:
  case V_SgOmpTaskStatement:
  case V_SgOmpTaskwaitStatement:
  case V_SgOmpThreadprivateStatement:
  case V_SgOmpWorkshareStatement:
    return 1;

  case V_SgReturnStmt:
    return 1;

  case V_SgAsmStmt:
    return 1;

#if OBSOLETE_CODE
  case V_SgCtorInitializerList:
      return countLabels(isSgCtorInitializerList(node));
#endif /* OBSOLETE_CODE */

  default:
    return 0;
  }
}

void CLabeler::registerLabel(LabelProperty lp) {
  mappingLabelToLabelProperty.push_back(lp);
  _isValidMappingNodeToLabel=false;
}


void CLabeler::createLabels(SgNode* root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(int num=numberOfAssociatedLabels(*i)) {
      if(isFunctionCallNode(*i)) {
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
        assert(num==1);
        registerLabel(LabelProperty(*i,LabelProperty::LABEL_BLOCKBEGIN));
      } else if(isSgOmpParallelStatement(*i)){
        assert(num == 2);
        registerLabel(LabelProperty(*i, LabelProperty::LABEL_FORK));
        registerLabel(LabelProperty(*i, LabelProperty::LABEL_JOIN));
      } else if(isSgOmpForStatement(*i) || isSgOmpSectionsStatement(*i)
                || isSgOmpSimdStatement(*i) || isSgOmpForSimdStatement(*i)) {
        assert(num == 2);
        registerLabel(LabelProperty(*i, LabelProperty::LABEL_WORKSHARE));
        registerLabel(LabelProperty(*i, LabelProperty::LABEL_BARRIER));
      } else if(isSgOmpBarrierStatement(*i)) {
        assert(num == 1);
        registerLabel(LabelProperty(*i, LabelProperty::LABEL_BARRIER));
      } else {
        // all other cases
        for(int j=0;j<num;j++) {
          registerLabel(LabelProperty(*i));
        }
      }
    }
    // schroder3 (2016-07-12): We can not skip the children of a variable declaration
    //  because there might be a member function definition inside the variable declaration.
    //  Example:
    //   int main() {
    //     class A {
    //      public:
    //       void mf() {
    //         int i = 2;
    //       }
    //     } a; // Var decl
    //   }
    if(isSgExprStatement(*i)||isSgReturnStmt(*i)/*||isSgVariableDeclaration(*i)*/)
      i.skipChildrenOnForward();
    // MS 2018: skip templates (only label template instantiations)
    if(isSgTemplateClassDeclaration(*i)||isSgTemplateClassDefinition(*i)) {
      i.skipChildrenOnForward();
    }
  }
}

string Labeler::labelToString(Label lab) {
  stringstream ss;
  ss<<lab;
  return ss.str();
}

bool Labeler::isValidLabelIdRange(Label lab) {
  return lab.getId()<numberOfLabels();
}

std::string CLabeler::sourceLocationToString(Label lab, size_t maxFileNameLength, size_t maxSourceLength) {
  SgNode* node=getNode(lab);
  return SgNodeHelper::locationAndSourceCodeToString(node,maxFileNameLength, maxSourceLength);
}

SgNode* CLabeler::getNode(Label label) {
  if(!isValidLabelIdRange(label)) {
    stringstream ss;
    ss <<"[ "
       << label.getId()
       << " >= "
       << mappingLabelToLabelProperty.size()
       << " ]";
    string errorInfo=ss.str();
    throw CodeThorn::Exception("Labeler: getNode: label id out of bounds "+errorInfo);
  } else if(label==Label()) {
    throw CodeThorn::Exception("Labeler: getNode: invalid label id");
  }
  return mappingLabelToLabelProperty[label.getId()].getNode();
}

/* this access function has O(n). This is OK as this function should only be used rarely, whereas
   the function getNode is used frequently (and has O(1)).
*/
void CLabeler::ensureValidNodeToLabelMapping() {
  if(_isValidMappingNodeToLabel)
    return;
  else
    computeNodeToLabelMapping();
}

void CLabeler::computeNodeToLabelMapping() {
  mappingNodeToLabel.clear();
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
  }
  _isValidMappingNodeToLabel=true;
}

Label CLabeler::getLabel(SgNode* node) {
  assert(node);
  if(!node)
    return Label();
  if(_isValidMappingNodeToLabel) {
    if(mappingNodeToLabel.count(node)==0) {
      return Label();
    }
    return mappingNodeToLabel[node];
  } else {
    computeNodeToLabelMapping(); // sets _isValidMappingNodeToLabel to true.
    return mappingNodeToLabel[node];
  }
  throw CodeThorn::Exception("Error: internal error getLabel.");
}

size_t CLabeler::numberOfLabels() {
  return mappingLabelToLabelProperty.size();
}

void CLabeler::setIsFunctionCallFn(std::function<bool(SgNode*)> fn)
{
  isFunctionCallNode = std::move(fn);
}

Label CLabeler::functionCallLabel(SgNode* node) {
  return getLabel(node);
}

Label CLabeler::functionCallReturnLabel(SgNode* node) {
  // in its current implementation it is guaranteed that labels associated with the same node
  // are associated as an increasing sequence of labels
  Label lab=getLabel(node);
  if(lab==NO_LABEL)
    return NO_LABEL;
  else
    return lab+1;
}

Label CLabeler::getFunctionCallReturnLabelFromCallLabel(Label callLabel) {
  ROSE_ASSERT(isFunctionCallLabel(callLabel));
  return functionCallReturnLabel(getNode(callLabel));
}

Label CLabeler::blockBeginLabel(SgNode* node) {
  ROSE_ASSERT(isSgBasicBlock(node));
  return getLabel(node);
}
Label CLabeler::blockEndLabel(SgNode* node) {
  ROSE_ASSERT(isSgBasicBlock(node));
  Label lab=getLabel(node);
  if(lab==NO_LABEL)
    return NO_LABEL;
  else
    return lab+1;
}
Label CLabeler::functionEntryLabel(SgNode* node) {
  ROSE_ASSERT(isSgFunctionDefinition(node));
  return getLabel(node);
}
Label CLabeler::functionExitLabel(SgNode* node) {
  ROSE_ASSERT(isSgFunctionDefinition(node));
  Label lab=getLabel(node);
  if(lab==NO_LABEL)
    return NO_LABEL;
  else
    return lab+1;
}

Label CLabeler::forkLabel(SgNode *node) {
  ROSE_ASSERT(isSgOmpParallelStatement(node));
  Label lab = getLabel(node);
  return lab;
}

Label CLabeler::joinLabel(SgNode *node) {
  ROSE_ASSERT(isSgOmpParallelStatement(node));
  Label lab = getLabel(node);
  return lab+1;
}

Label CLabeler::workshareLabel(SgNode *node) {
  ROSE_ASSERT(isSgOmpForStatement(node) || isSgOmpSectionsStatement(node)
                || isSgOmpSimdStatement(node) || isSgOmpForSimdStatement(node));
  Label lab = getLabel(node);
  return lab;
}

Label CLabeler::barrierLabel(SgNode *node) {
  if (isSgOmpBarrierStatement(node)) {
    Label lab = getLabel(node);
    return lab;
  }
  ROSE_ASSERT(isSgOmpForStatement(node) || isSgOmpSectionsStatement(node)
                || isSgOmpSimdStatement(node) || isSgOmpForSimdStatement(node));
  Label lab = getLabel(node);
  return lab+1;
}

bool CLabeler::isConditionLabel(Label lab) {
  return SgNodeHelper::isCond(getNode(lab));
}

bool CLabeler::isLoopConditionLabel(Label lab) {
  return SgNodeHelper::isLoopCond(getNode(lab));
}

bool CLabeler::isSwitchExprLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(SgNodeHelper::isCond(node)) {
    SgLocatedNode* loc=isSgLocatedNode(node);
    if(loc) {
      return isSgSwitchStatement(loc->get_parent());
    }
  }
  return false;
}

bool CLabeler::isExprLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(SgLocatedNode* loc=isSgLocatedNode(node)) {
    return isSgExprStatement(loc)||isSgExpression(loc);
  }
  return false;
}

bool CLabeler::isExprOrDeclLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(SgLocatedNode* loc=isSgLocatedNode(node)) {
    return isExprLabel(lab)||isSgDeclarationStatement(loc);
  }
  return false;
}

bool CLabeler::isFirstLabelOfMultiLabeledNode(Label lab) {
  return isFunctionCallLabel(lab)||isFunctionEntryLabel(lab)||isBlockBeginLabel(lab);
}

bool CLabeler::isSecondLabelOfMultiLabeledNode(Label lab) {
  return isFunctionCallReturnLabel(lab)||isFunctionExitLabel(lab)||isBlockEndLabel(lab);
}

bool CLabeler::isFunctionEntryLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionEntryLabel();
}

bool CLabeler::isFunctionExitLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionExitLabel();
}

bool CLabeler::isEmptyStmtLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isEmptyStmtLabel();
}

bool CLabeler::isBlockBeginLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isBlockBeginLabel();
}

bool CLabeler::isBlockEndLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isBlockEndLabel();
}

bool CLabeler::isFunctionCallLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionCallLabel();
}

bool CLabeler::isExternalFunctionCallLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isExternalFunctionCallLabel();
}

void CLabeler::setExternalFunctionCallLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].setExternalFunctionCallLabel();
}

bool CLabeler::isFunctionCallReturnLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionCallReturnLabel();
}

bool CLabeler::isForkLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isForkLabel();
}

bool CLabeler::isJoinLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isJoinLabel();
}

bool CLabeler::isWorkshareLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isWorkshareLabel();
}

bool CLabeler::isBarrierLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isBarrierLabel();
}

bool CLabeler::areCallAndReturnLabels(Label call, Label ret)
{
  return (  isFunctionCallLabel(call)
         && isFunctionCallReturnLabel(ret)
         && call.getId()+1 == ret.getId()   // alternatively, the underlying node could be compared
         );
}

Label CLabeler::getFunctionCallLabelFromReturnLabel(Label ret)
{
  ROSE_ASSERT(isFunctionCallReturnLabel(ret));

  Label call(ret.getId() - 1);
  ROSE_ASSERT(isFunctionCallLabel(call));

  return call;
}

LabelProperty CLabeler::getProperty(Label lab)
{
  return mappingLabelToLabelProperty.at(lab.getId());
}

std::string CLabeler::toString() {
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

Labeler::iterator CLabeler::begin() {
  ensureValidNodeToLabelMapping();
  return iterator(0,numberOfLabels());
}

Labeler::iterator CLabeler::end() {
  return iterator(0,0);
}

bool LabelProperty::isExternalFunctionCallLabel() {
  return _isExternalFunctionCallLabel;
}

void LabelProperty::setExternalFunctionCallLabel() {
  ROSE_ASSERT(isFunctionCallLabel());
  _isExternalFunctionCallLabel=true;
}

/*
 * IO Labeler Implementation
 */
IOLabeler::IOLabeler(VariableIdMapping* variableIdMapping)
: CLabeler(), _variableIdMapping(variableIdMapping)
{
}

IOLabeler::~IOLabeler() {
}

void IOLabeler::initialize(SgNode* n)
{
  ROSE_ASSERT(_variableIdMapping);

  CLabeler::initialize(n);
  for(LabelToLabelPropertyMapping::iterator i=mappingLabelToLabelProperty.begin();i!=mappingLabelToLabelProperty.end();++i) {
    (*i).initializeIO(_variableIdMapping);
  }
}

bool IOLabeler::isStdIOLabel(Label label) {
  return mappingLabelToLabelProperty[label.getId()].isIOLabel();
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

