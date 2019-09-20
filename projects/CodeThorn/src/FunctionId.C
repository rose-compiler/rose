#include "sage3basic.h"
#include <sstream>
#include "FunctionId.h"
#include "FunctionIdMapping.h"

using namespace std;
using namespace CodeThorn;

FunctionId::FunctionId():_id(-1){
}

const char * const FunctionId::idKindIndicator = "F";

string FunctionId::toString(const char* prefix) const {
  ostringstream ss;
  ss << prefix << _id;
  return ss.str();
}

string FunctionId::toString(const CodeThorn::FunctionIdMapping& idMapping) const {
  return idMapping.getUniqueShortNameFromFunctionId(*this);
}

void FunctionId::setIdCode(int code) {
  if(code >= 0){
    _id = code;
  }
  else {
    ROSE_ASSERT(false);
  }
}

bool FunctionId::operator==(const FunctionId& other) const {
  return _id==other._id;
}

bool FunctionId::operator!=(const FunctionId& other) const {
  return !(*this==other);
}

bool CodeThorn::operator<(CodeThorn::FunctionId id1, CodeThorn::FunctionId id2)  {
  return id1._id<id2._id;
}

FunctionIdSet& CodeThorn::operator+=(FunctionIdSet& s1, const FunctionIdSet& s2) {
  for(FunctionIdSet::const_iterator i=s2.begin();i!=s2.end();++i) {
    s1.insert(*i);
  }
  return s1;
}

//size_t CodeThorn::hash_value(const FunctionId& id) {
//  return id.getIdCode();
//}

size_t CodeThorn::FunctionIdHashFunction::operator()(const FunctionId& fid) const {
  return fid.getIdCode();;
}
