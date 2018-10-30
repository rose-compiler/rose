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

  bool CallString::isLowerValueSubstring(const CodeThorn::CallString& other) const {
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

  bool operator==(const CodeThorn::CallString& c1, const CodeThorn::CallString& c2) {
    if(&c1==&c2)
      return true;
#if 0
    cout<<"DEBUG: "<<c1.getLength()<<" vs "<<c2.getLength()<<endl;
    if(c1.getLength()==c2.getLength()) {
      //cout<<"DEBUG: Checking callstrings: "<<c1.toString()<<" : "<<c2.toString()<<endl;
      return true;
      auto i1=c1._callString.begin();
      auto i2=c2._callString.begin();
      while(i1!=c1._callString.end()) {
        if(*i2!=*i1) {
          return false;
        }
        ++i1;
        ++i2;
      }
      return true;
    } else {
      return false;
    }
#else
    // reusing overloaded operator == from std::vector
    return c1._callString==c2._callString;
#endif
  }

  bool operator!=(const CodeThorn::CallString& c1, const CodeThorn::CallString& c2) {
    return !(c1==c2);
  }
  bool operator<(const CodeThorn::CallString& c1, const CodeThorn::CallString& c2) {
    return c1.isLowerValueSubstring(c2);
  }
}
