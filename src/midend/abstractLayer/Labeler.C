/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"
#include "CodeThornException.h"

#include "Labeler.h"
#include "SgNodeHelper.h"
#include "sageInterface.h"

//#include "AstTerm.h"
#include <sstream>
#include <numeric>

using namespace std;
using namespace CodeThorn;

namespace si = SageInterface;

Label Labeler::NO_LABEL;

Label::Label() {
    _labelId=NO_LABEL_ID;
  }

Label::Label(size_t labelId) {
  ROSE_ASSERT(labelId!=NO_LABEL_ID);
  _labelId=labelId;
}

//Copy constructor
Label::Label(const Label& other) {
  _labelId=other._labelId;
}

bool Label::isValid() const {
  return _labelId!=NO_LABEL_ID;
}

//Copy assignemnt operator
Label& Label::operator=(const Label& other) {
  // prevent self-assignment
  if (this != &other) {
    _labelId = other._labelId;
  }
  return *this;
}

bool Label::operator<(const Label& other) const {
  return _labelId<other._labelId;
}
bool Label::operator==(const Label& other) const {
  return _labelId==other._labelId;
}
bool Label::operator!=(const Label& other) const {
  return !(*this==other);
}
bool Label::operator>(const Label& other) const {
  return !(*this<other||*this==other);
}
bool Label::operator>=(const Label& other) const {
  return !(*this<other);
}
Label& Label::operator+(int num) {
  _labelId+=num;
  return *this;
}
// prefix inc operator
Label& Label::operator++() {
  ++_labelId;
  return *this;
}
  // postfix inc operator
Label Label::operator++(int) {
  Label tmp(*this);
  ++(*this);
  return tmp;
}

size_t Label::getId() const {
  return _labelId;
}

std::string Label::toString() const {
  if(_labelId==NO_LABEL_ID) {
    return "NO_LABEL_ID";
  } else {
    stringstream ss;
    ss<<_labelId;
    return ss.str();
  }
}

// friend function
ostream& CodeThorn::operator<<(ostream& os, const Label& label) {
  os<<label.toString();
  return os;
}

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
      //cerr<<"WARNING: non-supported output operation:"<<_node->unparseToString()<<endl;
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

Labeler::~Labeler(){}

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

// returns number of labels to be associated with node
int Labeler::numberOfAssociatedLabels(SgNode* node) {
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
    //cout << "DEBUG: Labeler: assigning 3 labels for SgReturnStmt(SgFunctionCallExp)"<<endl;
    return 3;
  }

  //special case of FunctionCall is matched as : f(), x=f(); T x=f();
  if(SgNodeHelper::matchExtendedNormalizedCall(node) || SgNodeHelper::Pattern::matchFunctionCall(node)) {
    //cout << "DEBUG: Labeler: assigning 2 labels for SgFunctionCallExp"<<endl;
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
  case V_SgPragmaDeclaration:
  case V_SgSwitchStatement:
  case V_SgDefaultOptionStmt:
  case V_SgCaseOptionStmt:
    return 1;

    // declarations
  case V_SgVariableDeclaration:
  case V_SgClassDeclaration:
  case V_SgEnumDeclaration:
  case V_SgTypedefDeclaration:
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
#if OBSOLETE_CODE      
  case V_SgCtorInitializerList:
      return countLabels(isSgCtorInitializerList(node));
#endif /* OBSOLETE_CODE */
          
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
    if(int num=numberOfAssociatedLabels(*i)) {
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
        assert(num==1);
        registerLabel(LabelProperty(*i,LabelProperty::LABEL_BLOCKBEGIN));
        // registerLabel(LabelProperty(*i,LabelProperty::LABEL_BLOCKEND));
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
      } else if(SgNodeHelper::matchExtendedNormalizedCall(*i)) {
        assert(num==2);
        registerLabel(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALL));
        registerLabel(LabelProperty(*i,LabelProperty::LABEL_FUNCTIONCALLRETURN));
#if OBSOLETE_CODE
      } else if (SgCtorInitializerList* inilst = isSgCtorInitializerList(*i)) {
        SgInitializedNamePtrList& lst = inilst->get_ctors();
        
        for (size_t i = 0; i < lst.size(); ++i) {
          SgInitializer* ini = lst[i]->get_initializer();  
          ROSE_ASSERT(ini);
       
          if (isSgConstructorInitializer(ini)) {
            registerLabel(LabelProperty(ini,LabelProperty::LABEL_FUNCTIONCALL));
            registerLabel(LabelProperty(ini,LabelProperty::LABEL_FUNCTIONCALLRETURN));        
          } else {
            ROSE_ASSERT(isSgAssignInitializer(ini));
            
            registerLabel(LabelProperty(ini));
          }
        }
        // std::cerr << "- " << typeid(**i).name() << ": " << num << std::endl;
#endif /* OBSOLETE_CODE */
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
  //std::cout << "STATUS: Assigned "<<mappingLabelToLabelProperty.size()<< " labels."<<std::endl;
  //std::cout << "DEBUG: mappingLabelToLabelProperty:\n"<<this->toString()<<std::endl;
}

string Labeler::labelToString(Label lab) {
  stringstream ss;
  ss<<lab;
  return ss.str();
}

SgNode* Labeler::getNode(Label label) {
  if(label.getId()>=mappingLabelToLabelProperty.size()) {
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
void Labeler::ensureValidNodeToLabelMapping() {
  if(_isValidMappingNodeToLabel)
    return;
  else
    computeNodeToLabelMapping();
}

void Labeler::computeNodeToLabelMapping() {
  mappingNodeToLabel.clear();
  //std::cout << "INFO: computing node<->label with map size: "<<mappingLabelToLabelProperty.size()<<std::endl;
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
  throw CodeThorn::Exception("Error: internal error getLabel.");
}

long Labeler::numberOfLabels() {
  return mappingLabelToLabelProperty.size();
}

Label Labeler::functionCallLabel(SgNode* node) {
  ROSE_ASSERT(SgNodeHelper::Pattern::matchFunctionCall(node) || SgNodeHelper::matchExtendedNormalizedCall(node));
  return getLabel(node);
}

Label Labeler::functionCallReturnLabel(SgNode* node) {
  // PP disable assert, since it does not hold for constructors
  // ROSE_ASSERT(SgNodeHelper::Pattern::matchFunctionCall(node));
  // in its current implementation it is guaranteed that labels associated with the same node
  // are associated as an increasing sequence of labels
  Label lab=getLabel(node);
  if(lab==NO_LABEL)
    return NO_LABEL;
  else
    return lab+1;
}
Label Labeler::blockBeginLabel(SgNode* node) {
  ROSE_ASSERT(isSgBasicBlock(node));
  return getLabel(node);
}
Label Labeler::blockEndLabel(SgNode* node) {
  ROSE_ASSERT(isSgBasicBlock(node));
  Label lab=getLabel(node);
  if(lab==NO_LABEL)
    return NO_LABEL;
  else
    return lab+1;
}
Label Labeler::functionEntryLabel(SgNode* node) {
  ROSE_ASSERT(isSgFunctionDefinition(node));
  return getLabel(node);
}
Label Labeler::functionExitLabel(SgNode* node) {
  ROSE_ASSERT(isSgFunctionDefinition(node));
  Label lab=getLabel(node);
  if(lab==NO_LABEL)
    return NO_LABEL;
  else
    return lab+1;
}

Label Labeler::forkLabel(SgNode *node) {
  ROSE_ASSERT(isSgOmpParallelStatement(node));
  Label lab = getLabel(node);
  return lab;
}

Label Labeler::joinLabel(SgNode *node) {
  ROSE_ASSERT(isSgOmpParallelStatement(node));
  Label lab = getLabel(node);
  return lab+1;
}

Label Labeler::workshareLabel(SgNode *node) {
  ROSE_ASSERT(isSgOmpForStatement(node) || isSgOmpSectionsStatement(node)
                || isSgOmpSimdStatement(node) || isSgOmpForSimdStatement(node));
  Label lab = getLabel(node);
  return lab;
}

Label Labeler::barrierLabel(SgNode *node) {
  if (isSgOmpBarrierStatement(node)) {
    Label lab = getLabel(node);
    return lab;
  }
  ROSE_ASSERT(isSgOmpForStatement(node) || isSgOmpSectionsStatement(node)
                || isSgOmpSimdStatement(node) || isSgOmpForSimdStatement(node));
  Label lab = getLabel(node);
  return lab+1;
}

bool Labeler::isConditionLabel(Label lab) {
  return SgNodeHelper::isCond(getNode(lab));
}

bool Labeler::isLoopConditionLabel(Label lab) {
  return SgNodeHelper::isLoopCond(getNode(lab));
}

bool Labeler::isSwitchExprLabel(Label lab) {
  SgNode* node=getNode(lab);
  if(SgNodeHelper::isCond(node)) {
    SgLocatedNode* loc=isSgLocatedNode(node);
    if(loc) {
      return isSgSwitchStatement(loc->get_parent());
    }
  }
  return false;
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

bool Labeler::isEmptyStmtLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isEmptyStmtLabel();
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

bool Labeler::isExternalFunctionCallLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isExternalFunctionCallLabel();
}

void Labeler::setExternalFunctionCallLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].setExternalFunctionCallLabel();
}

bool Labeler::isFunctionCallReturnLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isFunctionCallReturnLabel();
}

bool Labeler::isForkLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isForkLabel();
}

bool Labeler::isJoinLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isJoinLabel();
}

bool Labeler::isWorkshareLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isWorkshareLabel();
}

bool Labeler::isBarrierLabel(Label lab) {
  return mappingLabelToLabelProperty[lab.getId()].isBarrierLabel();
}

LabelSet Labeler::getLabelSet(set<SgNode*>& nodeSet) {
  LabelSet lset;
  for(set<SgNode*>::iterator i=nodeSet.begin();i!=nodeSet.end();++i) {
    lset.insert(getLabel(*i));
  }
  return lset;
}

LabelSet LabelSet::operator+(LabelSet& s2) {
  LabelSet result;
  result=*this;
  result+=s2;
  return result;
}

LabelSet& LabelSet::operator+=(LabelSet& s2) {
  for(LabelSet::iterator i2=s2.begin();i2!=s2.end();++i2)
    insert(*i2);
  return *this;
}

LabelSet LabelSet::operator-(LabelSet& s2) {
  LabelSet result;
  result=*this;
  result-=s2;
  return result;
}

LabelSet& LabelSet::operator-=(LabelSet& s2) {
  for(LabelSet::iterator i2=s2.begin();i2!=s2.end();++i2)
    erase(*i2);
  return *this;
}

std::string LabelSet::toString() {
  std::stringstream ss;
   ss<<"{";
   for(LabelSet::iterator i=begin();i!=end();++i) {
     if(i!=begin())
       ss<<",";
     ss<<*i;
   }
   ss<<"}";
   return ss.str();
}

bool LabelSet::isElement(Label lab) {
  return find(lab)!=end();
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

bool LabelProperty::isExternalFunctionCallLabel() {
  return _isExternalFunctionCallLabel;
}

void LabelProperty::setExternalFunctionCallLabel() {
  ROSE_ASSERT(isFunctionCallLabel());
  _isExternalFunctionCallLabel=true;
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

IOLabeler::~IOLabeler() {
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

