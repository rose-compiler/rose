#include "sage3basic.h"
#include "CallString.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace CodeThorn {

  void CallString::addLabel(SPRAY::Label lab) {
    _callString.push_back((int)lab.getId());
  }

  void CallString::removeLabel() {
    _callString.pop_back();
  }

  size_t CallString::getLength() const {
    return _callString.size();
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
        if(*i1>=*i2) {
          return false;
        }
        ++i1;
        ++i2;
      }
      return true;
    } else {
      return _callString.size()<other._callString.size();
    }
  }
}
