#include "sage3basic.h"
#include "CallString.h"

namespace CodeThorn {

  void CallString::addLabel(SPRAY::Label lab) {
    _callString.push_back(lab);
  }

  void CallString::removeLabel() {
    _callString.pop_back();
  }

  size_t CallString::getLength() {
    return _callString.size();
  }

  std::string CallString::toString() const {
    std::string s="[";
    for(auto iter = _callString.begin(); iter!=_callString.end();++iter) {
      if(iter!=_callString.begin()) {
        s+=", ";
      }
      s+=(*iter).toString();
    }
    s+="]";
    return s;
  }

  bool CallString::isLowerValueSubstring(const CodeThorn::CallString& other) const {
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

  bool operator==(const CodeThorn::CallString& c1, const CodeThorn::CallString& c2) {
    // reusing overloaded operator == from std::vector
    return c1._callString==c2._callString;
  }

  bool operator!=(const CodeThorn::CallString& c1, const CodeThorn::CallString& c2) {
    return !(c1==c2);
  }
  bool operator<(const CodeThorn::CallString& c1, const CodeThorn::CallString& c2) {
    return c1.isLowerValueSubstring(c2);
  }
}
