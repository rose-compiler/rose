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
  transformationSequenceList.clear();
}

void Backstroke::TransformationSequence::preOrderVisit(SgNode *astNode) {
}

void Backstroke::TransformationSequence::postOrderVisit(SgNode *astNode) {
  //cout<<"SgNode:"<<astNode->class_name()<<endl;
  if(isSgAssignOp(astNode)) {
    SgNode* lhs;
    lhs=SgNodeHelper::getLhs(astNode);
    transformationSequenceList.push_back(make_pair(TRANSOP_ASSIGNMENT,lhs));
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
    transformationSequenceList.push_back(make_pair(TRANSOP_ASSIGNMENT,lhs));
  }
  if(isSgPlusPlusOp(astNode) || isSgMinusMinusOp(astNode)) {
    SgNode* operand=SgNodeHelper::getUnaryOpChild(astNode);
    transformationSequenceList.push_back(make_pair(TRANSOP_ASSIGNMENT,operand));
  }

  if(SgNewExp* newExp=isSgNewExp(astNode)) {
    if(isSgArrayType(newExp->get_specified_type())) {
      transformationSequenceList.push_back(make_pair(TRANSOP_MEM_ALLOC_ARRAY,newExp));
    } else {
      transformationSequenceList.push_back(make_pair(TRANSOP_MEM_ALLOC,newExp));
    }
  }

  if(SgDeleteExp* deleteExp=isSgDeleteExp(astNode)) {
    if(deleteExp->get_is_array()) {
      transformationSequenceList.push_back(make_pair(TRANSOP_MEM_DELETE_ARRAY,deleteExp));
    } else {
      transformationSequenceList.push_back(make_pair(TRANSOP_MEM_DELETE,astNode));
    }
  }
}

// computes the list of nodes for which the bs-memory-mod transformation must be applied
void Backstroke::TransformationSequence::create(SgNode* node) {
  traverse(node);
}

void Backstroke::TransformationSequence::apply() {
  for(TransformationSequenceList::iterator i=transformationSequenceList.begin();i!=transformationSequenceList.end();++i) {
    if(_showTransformationTrace) {
      cout<<"TRACE: "<<SgNodeHelper::sourceLineColumnToString((*i).second)<<": ";
    }
    TransformationOperation op=(*i).first;
    SgNode* ast=(*i).second;
    ROSE_ASSERT(ast);
    
    switch(op) {
    case TRANSOP_ASSIGNMENT: {
      if(isSgAssignOp(ast)) {
        SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(ast));
        SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(ast));
        ROSE_ASSERT(lhs && rhs);
        string s=string("rts.assign(&(")+lhs->unparseToString()+")"
          +", "
          +rhs->unparseToString()
          +")";
        if(_showTransformationTrace) {
          cout<<"transformation: assignop: "<<(ast)->unparseToString()<<" ==> "<<s<<endl;
        }
        SgNodeHelper::replaceAstWithString(ast,s);
      } else {
        SgExpression* exp=isSgExpression(ast);
        ROSE_ASSERT(exp);
        if(!isLocalVariable(exp)) {
          string s;
          if(isPointerType(exp->get_type())) {
            s=string("(*rts.avpushptr((void**)&(")+exp->unparseToString()+")))";
          } else {
            s=string("(*rts.avpush(&(")+exp->unparseToString()+")))";
          }
          if(_showTransformationTrace)
            cout<<"transformation: "<<(ast)->unparseToString()<<" ==> "<<s<<endl;
          SgNodeHelper::replaceAstWithString(ast,s);
        } else {
          if(_showTransformationTrace)
            cout<<"optimization: detected local variable "<<exp->unparseToString()<<endl;
        }
      }
      break;
    }
    case TRANSOP_MEM_ALLOC: {
      SgNewExp* newExp=isSgNewExp(ast);
      ROSE_ASSERT(newExp);
      SgPointerType* pointerType=isSgPointerType(newExp->get_type());
      ROSE_ASSERT(pointerType);
      string newExpString=newExp->unparseToString();
      string typenameForCast;
      SgType* type=pointerType->get_base_type();
      if(SgClassType* classType=isSgClassType(type)) {
        typenameForCast=classType->get_name();
      } else {
        typenameForCast=type->unparseToString();
      }
      string registerAndCast="static_cast<"+typenameForCast+"*"+">("+"rts.registerForCommit((void*)"+newExpString+"))";
      string newCode=registerAndCast;
      if(_showTransformationTrace) {
        cout<<"transformation: "<<newExp->unparseToString()<<" ==> "<<newCode<<endl;
      }
      SgNodeHelper::replaceAstWithString(newExp,newCode);
      break;
    }
      
    case TRANSOP_MEM_ALLOC_ARRAY: {
      SgNewExp* newExp=isSgNewExp(ast);
      ROSE_ASSERT(newExp);
      SgArrayType* arrayType=isSgArrayType(newExp->get_specified_type());
      ROSE_ASSERT(arrayType);
      SgType* arrayElementType=arrayType->get_base_type();
      stringstream ss;
      size_t arraySize=SageInterface::getArrayElementCount(arrayType);;
      ss<<arraySize;
      string arraySizeString=ss.str();
      string newCode="Backstroke::new_array<"+arrayElementType->unparseToString()+">"+"("+arraySizeString+")";
      if(_showTransformationTrace) {
        cout<<"transformation: "<<newExp->unparseToString()<<" ==> "<<newCode<<endl;
      }
      SgNodeHelper::replaceAstWithString(newExp,newCode);
      break;
    }

    case TRANSOP_MEM_DELETE: {
      // split delete operation in: 1) destructor call, 2) register for commit
      // TODO: implement as proper AST manipulation
      SgNode* operand=SgNodeHelper::getFirstChild(ast);
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
          //cout<<"INFO: delete on non-class type "<<typeName<<endl;
          destructorCall="";
        } else {
          cerr<<"Error: unknown operand or type in delete operation."<<endl;
          exit(1);
        }
      }
      string registerCall=string("rts.registerForCommit((void*)")+operand->unparseToString()+")";
      string newCode=destructorCall+registerCall;
      if(_showTransformationTrace) {
        cout<<"transformation: "<<ast->unparseToString()<<" ==> "<<newCode<<endl;
      }
      SgNodeHelper::replaceAstWithString(ast,newCode);
      break;
    }
    case TRANSOP_MEM_DELETE_ARRAY: {
      SgDeleteExp* deleteExp=dynamic_cast<SgDeleteExp*>(ast);
      ROSE_ASSERT(deleteExp);
      SgType* varType=deleteExp->get_variable()->get_type();
      if(SgPointerType* pointerType=isSgPointerType(varType)) {
        SgType* pointedToType=pointerType->get_base_type();
        string elementTypeName="unknown";
        if(SgClassType* classType=isSgClassType(pointedToType)) {
          elementTypeName=classType->get_name();
        }
        string newCode="Backstroke::delete_array<"+elementTypeName+">("+deleteExp->get_variable()->unparseToString()+")";
        if(_showTransformationTrace) {
          cout<<"transformation: "<<deleteExp->unparseToString()<<" ==> "<<newCode<<endl;
        }    
        SgNodeHelper::replaceAstWithString(deleteExp,newCode);
      } else {
        cerr<<"Error: operand of delete[] has non-pointer type. ROSE AST consistency error. Bailing out."<<"("<<(deleteExp->get_type()->unparseToString())<<")";
        exit(1);
      }
      break;
    }
    default:
      cerr<<"Error: unsupported transformation operation."<<endl;
      exit(1);
    }
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
