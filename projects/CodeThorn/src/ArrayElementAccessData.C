#include "sage3basic.h"
#include "ArrayElementAccessData.h"

using namespace std;
using namespace CodeThorn;

ArrayElementAccessData::ArrayElementAccessData() {
}

VariableId ArrayElementAccessData::getVariable() const {
  return varId;
}

int ArrayElementAccessData::getSubscript(int dimension) const {
  return subscripts.at(dimension);
}

int ArrayElementAccessData::getDimensions() const {
  return subscripts.size();
}

string ArrayElementAccessData::toString(VariableIdMapping* variableIdMapping) const {
  if(isValid()) {
    stringstream ss;
    ss<< variableIdMapping->uniqueVariableName(varId);
    for(vector<int>::const_iterator i=subscripts.begin();i!=subscripts.end();++i) {
      ss<<"["<<*i<<"]";
    }
    return ss.str();
  } else {
    return "$non-valid-array-access$";
  }
}

string ArrayElementAccessData::toStringNoSsaSubscript(VariableIdMapping* variableIdMapping) const {
  if(isValid()) {
    stringstream ss;
    ss<< variableIdMapping->variableName(varId);
    for(vector<int>::const_iterator i=subscripts.begin();i!=subscripts.end();++i) {
      ss<<"["<<*i<<"]";
    }
    return ss.str();
  } else {
    return "$non-valid-array-access$";
  }
}

bool ArrayElementAccessData::isValid() const {
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
  //cout<<"NameCheck:"<<variableIdMapping->uniqueVariableName(access.varId)<<" ";
  for(size_t i=0;i<(*subscripts).size();++i) {
    //cout<<(*subscripts)[i]<<":"<<(*subscripts)[i]->unparseToString()<<" ";
    if(SgIntVal* subscriptint=isSgIntVal((*subscripts)[i])) {
      //cout<<"VAL:"<<subscriptint->get_value();
      this->subscripts.push_back(subscriptint->get_value());
    }
  }
  if(this->subscripts.size()==0) {
    cerr<<"Error: could not determine constant array index. Array-element-access expression: "<<ref->unparseToString()<<endl;
    exit(1);
  }
}

bool operator==(const ArrayElementAccessData& a,const ArrayElementAccessData& other) {
    for(size_t i=0;i<a.subscripts.size();++i)
      if(a.subscripts[i]!=other.subscripts[i])
        return false;
    return a.varId==other.varId;
}
bool operator!=(const ArrayElementAccessData& a, const ArrayElementAccessData& other) {
  return !(a==other);
}
bool operator<(const ArrayElementAccessData& a, const ArrayElementAccessData& other) {
  if(a.varId!=other.varId)
    return a.varId<other.varId;
  if(a.subscripts.size()!=other.subscripts.size())
    return a.subscripts.size()<other.subscripts.size();
  vector<int>::const_iterator i=a.subscripts.begin();
  vector<int>::const_iterator j=other.subscripts.begin();
  while(i!=a.subscripts.end() && j!=other.subscripts.end()) {
    if(*i!=*j) {
      return *i<*j;
    } else {
      ++i;++j;
    }
  }
  ROSE_ASSERT(i==a.subscripts.end() && j==other.subscripts.end());
  return false; // both are equal
}

bool ArrayElementAccessData::hasNegativeIndex() const {
  for (auto index : subscripts ) {
    if(index<0)
      return true;
  }
  return false;
}
