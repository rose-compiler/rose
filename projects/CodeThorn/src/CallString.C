#include "sage3basic.h"
#include "CallString.h"
#include <iostream>
#include <sstream>

using namespace std;
#include <limits>
#include <cstddef>

namespace CodeThorn {

  // use maximum value for default call string length
  size_t CallString::_maxLength=std::numeric_limits<size_t>::max();

  bool CallString::isEmpty() {
    return getLength()==0;
  }

  bool CallString::isMaxLength() {
    return getLength()==getMaxLength();
  }

  bool CallString::exists(CodeThorn::Label lab) {
    return std::find(_callString.begin(), _callString.end(), lab) != _callString.end();
  }

  bool CallString::addLabel(CodeThorn::Label lab) {
    // exists is used to avoid that recursion adds labels up to length
    if(exists(lab))
      return false;
    if(getLength()<getMaxLength()) {
      _callString.push_back(lab);
    }
    return true;
  }

  void CallString::removeLastLabel() {
    if(_callString.size()>0) {
      _callString.pop_back();
    }
  }

  CallString CallString::withoutLastLabel() {
    ROSE_ASSERT(_callString.size()>0);
    CallString csCopy=CallString(*this);
    csCopy.removeLastLabel();
    return csCopy;
  }
  
  bool CallString::removeIfLastLabel(CodeThorn::Label lab) {
    if(isLastLabel(lab)) {
      _callString.pop_back();
      return true;
    }
    return false;
  }

  bool CallString::isLastLabel(CodeThorn::Label lab) {
    if(_callString.size()>0) {
      if(lab==_callString.back()) {
        return  true;
      }
    }
    return false;
  }

  size_t CallString::getLength() const {
    return _callString.size();
  }

  size_t CallString::getMaxLength() {
    return _maxLength;
  }

  void CallString::setMaxLength(size_t maxLength) {
    _maxLength=maxLength;
  }


  std::string CallString::toString() const {
    stringstream ss;
    ss<<"[";
    for(auto iter = _callString.begin(); iter!=_callString.end();++iter) {
      if(iter!=_callString.begin()) {
        ss<<", ";
      }
      ss<<(*iter).toString();
    }
    ss<<"]";
    return ss.str();
  }

  std::string CallString::toString(Labeler* labeler) const {
    stringstream ss;
    ss<<"[";
    for(auto iter = _callString.begin(); iter!=_callString.end();++iter) {
      if(iter!=_callString.begin()) {
        ss<<", ";
      }
      ROSE_ASSERT(labeler->isFunctionCallLabel(*iter));
      SgNode* node=labeler->getNode(*iter);
      string functionName=SgNodeHelper::getFunctionName(node);
      ss<<(*iter).toString()+":"+functionName;
    }
    ss<<"]";
    return ss.str();
  }

  bool CallString::operator==(const CodeThorn::CallString& other) const {
    if(this==&other)
      return true;
    return this->_callString==other._callString;
  }

  bool CallString::operator!=(const CodeThorn::CallString& other) const {
    return !(*this==other);
  }

  bool CallString::operator<(const CodeThorn::CallString& other) const {
    if(this==&other)
      return false;
    if(_callString.size()==other._callString.size()) {
      auto i1=_callString.begin();
      auto i2=other._callString.begin();
      while(i1!=_callString.end()) {
        if(*i1!=*i2) {
          return *i1<*i2;
        }
        ++i1;
        ++i2;
      }
      return false; // all labels are equal
    } else {
      return _callString.size()<other._callString.size();
    }
  }
  size_t CallString::hash() const {
    size_t h=1;
    for(auto lab : _callString) {
      h=h*11*lab.getId();
    }
    return h;
  }
}
