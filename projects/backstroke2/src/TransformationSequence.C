#include "sage3basic.h"
#include "TransformationSequence.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "CommandLineOptions.h"

using namespace std;

Backstroke::TransformationSequence::TransformationSequence():_showTransformationTrace(false) {
}

Backstroke::TransformationSequence::~TransformationSequence() {
}

void Backstroke::TransformationSequence::reset() {
  transformationSequence.clear();
}

void Backstroke::TransformationSequence::preOrderVisit(SgNode *astNode) {
}

void Backstroke::TransformationSequence::postOrderVisit(SgNode *astNode) {
  //cout<<"SgNode:"<<astNode->class_name()<<endl;
  if(isSgAssignOp(astNode)) {
    SgNode* lhs;
    lhs=SgNodeHelper::getLhs(astNode);
    transformationSequence.push_back(lhs);
  }
  if(isSgPlusAssignOp(astNode)
     || isSgMinusAssignOp(astNode)
     || isSgMultAssignOp(astNode)
     || isSgDivAssignOp(astNode)
     || isSgModAssignOp(astNode) // %=
     || isSgAndAssignOp(astNode) // &=
     || isSgIorAssignOp(astNode) // |=
     || isSgXorAssignOp(astNode) // ^=
     || isSgLshiftAssignOp(astNode) // <<=
     || isSgRshiftAssignOp(astNode) // >>=
     ) {
    SgNode* lhs=SgNodeHelper::getLhs(astNode);
    transformationSequence.push_back(lhs);
  }
  if(isSgPlusPlusOp(astNode) || isSgMinusMinusOp(astNode)) {
    SgNode* operand=SgNodeHelper::getUnaryOpChild(astNode);
    transformationSequence.push_back(operand);
  }
  if(isSgDeleteExp(astNode)) {
    transformationSequenceCommit.push_back(astNode);
  }
}

// computes the list of nodes for which the bs-memory-mod transformation must be applied
void Backstroke::TransformationSequence::create(SgNode* node) {
  traverse(node);
}

void Backstroke::TransformationSequence::apply() {
  for(list<SgNode*>::iterator i=transformationSequence.begin();i!=transformationSequence.end();++i) {
    if(_showTransformationTrace) {
      cout<<"TRACE: "<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<": ";
    }
    if(isSgAssignOp(*i)) {
      SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(*i));
      SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(*i));
      ROSE_ASSERT(lhs && rhs);
      string s=string("rts.assign(&(")+lhs->unparseToString()+")"
        +", "
        +rhs->unparseToString()
        +")";
      if(_showTransformationTrace) {
        cout<<"applying transformation assignop: "<<(*i)->unparseToString()<<" ==> "<<s<<endl;
      }
      SgNodeHelper::replaceAstWithString(*i,s);
    } else {
      SgExpression* exp=isSgExpression(*i);
      ROSE_ASSERT(exp);
      if(!isLocalVariable(exp)) {
        string s;
        if(isPointerType(exp->get_type())) {
          s=string("(*rts.avpushptr((void**)&(")+exp->unparseToString()+")))";
        } else {
          s=string("(*rts.avpush(&(")+exp->unparseToString()+")))";
        }
        if(_showTransformationTrace)
          cout<<"applying transformation on operand: "<<(*i)->unparseToString()<<" ==> "<<s<<endl;
        SgNodeHelper::replaceAstWithString(*i,s);
      } else {
        if(_showTransformationTrace)
          cout<<"optimization: no transformation necessary (detected local variable: "<<exp->unparseToString()<<")"<<endl;
      }
    }
  }
  for(list<SgNode*>::iterator i=transformationSequenceCommit.begin();i!=transformationSequenceCommit.end();++i) {
    // split delete operation in: 1) destructor call, 2) register for commit
    // TODO: implement as proper AST manipulation
    SgNode* operand=SgNodeHelper::getFirstChild(*i);
    SgExpression* deleteOperand=isSgExpression(operand);
    SgType* deleteOperandType=deleteOperand->get_type();
    //SgType* deleteOperandType2=deleteOperandType->findBaseType();
    SgDeclarationStatement* decl=deleteOperandType->getAssociatedDeclaration();
    string typeName;
    string destructorCall;
    if(SgClassDeclaration* classDecl=isSgClassDeclaration(decl)) {
      SgSymbol* symbol=classDecl->get_symbol_from_symbol_table();
      SgName name=symbol->get_name();
      typeName=name;
      destructorCall=string(operand->unparseToString())+"->"+"~"+typeName+"();";
    } else {
      if(SgExpression* exp=isSgExpression(operand)) {
        typeName=exp->get_type()->unparseToString();
        typeName.erase(typeName.size()-1); // remove trailing "*" (must be pointer)
        cout<<"INFO: delete on non-class type "<<typeName<<endl;
        destructorCall="";
      } else {
        cerr<<"Error: unknown operand or type in delete operation."<<endl;
        exit(1);
      }
    }

    string registerCall=string("rts.registerForCommit((void*)")+operand->unparseToString()+")";
    string code=destructorCall+registerCall;
    SgNodeHelper::replaceAstWithString(*i,code);
  }
}

bool Backstroke::TransformationSequence::isPointerType(SgNode* node) {
  return isSgPointerType(node);
}


// TODO: handle global variables and member variables in member functions
bool Backstroke::TransformationSequence::isLocalVariable(SgExpression* exp) {
  return isSgVarRefExp(exp);
}

void Backstroke::TransformationSequence::setShowTransformationTrace(bool trace) {
  _showTransformationTrace=trace;
}
