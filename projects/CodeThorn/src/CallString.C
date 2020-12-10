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

  bool CallString::addLabel(CodeThorn::Label lab) {
    if(getLength()<=getMaxLength()) {
      _callString.push_back(lab);
      return true;
    } else {
      cout<<"DEBUG: cutting off callstring!"<<endl;
      return false;
    }
  }

  void CallString::removeLastLabel() {
    if(_callString.size()>0) {
      _callString.pop_back();
    }
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
  size_t CallString::hash() const {
    size_t h=1;
    for(auto lab : _callString) {
      h=h*11*lab.getId();
    }
    return h;
  }
}
