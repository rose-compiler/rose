#include "ArrayElementAccessData.h"

using namespace std;

ArrayElementAccessData::ArrayElementAccessData() {
}

VariableId ArrayElementAccessData::getVariable() {
  return varId;
}

int ArrayElementAccessData::getSubscript(int dimension) {
  return subscripts.at(dimension);
}

int ArrayElementAccessData::getDimensions() {
  return subscripts.size();
}

string ArrayElementAccessData::toString(VariableIdMapping* variableIdMapping) {
  if(isValid()) {
    stringstream ss;
    ss<< variableIdMapping->uniqueShortVariableName(varId);
    for(vector<int>::iterator i=subscripts.begin();i!=subscripts.end();++i) {
      ss<<"["<<*i<<"]";
    }
    return ss.str();
  } else {
    return "$non-valid-array-access$";
  }
}

bool ArrayElementAccessData::isValid() {
  return varId.isValid() && subscripts.size()>0;
}

ArrayElementAccessData::ArrayElementAccessData(SgPntrArrRefExp* ref, VariableIdMapping* variableIdMapping) {
  // determine data
  SgExpression* arrayNameExp;
  std::vector<SgExpression*> subscriptsvec;
  std::vector<SgExpression*> *subscripts=&subscriptsvec;
  SageInterface::isArrayReference(ref, &arrayNameExp, &subscripts);
  //cout<<"Name:"<<arrayNameExp->unparseToString()<<" arity"<<subscripts->size()<<"subscripts:";
  varId=variableIdMapping->variableId(SageInterface::convertRefToInitializedName(ref));
  //cout<<"NameCheck:"<<variableIdMapping->uniqueShortVariableName(access.varId)<<" ";
  for(size_t i=0;i<(*subscripts).size();++i) {
    //cout<<(*subscripts)[i]<<":"<<(*subscripts)[i]->unparseToString()<<" ";
    if(SgIntVal* subscriptint=isSgIntVal((*subscripts)[i])) {
      //cout<<"VAL:"<<subscriptint->get_value();
      this->subscripts.push_back(subscriptint->get_value());
    }
  }
  ROSE_ASSERT(this->subscripts.size()>0);
}

