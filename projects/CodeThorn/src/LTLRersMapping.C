#include "LTLRersMapping.h"
#include "CodeThornException.h"
#include "Miscellaneous2.h"
#include <cassert>

using namespace std;

namespace CodeThorn {
  void LtlRersMapping::addInput(char c,int v) {
    assert(_inputMappingCharInt.find(c)==_inputMappingCharInt.end());
    _inputMappingCharInt[c]=v;
    _inputMappingIntChar[v]=c;
  }
  void LtlRersMapping::addOutput(char c,int v) {
    assert(_outputMappingCharInt.find(c)==_outputMappingCharInt.end());
    _outputMappingCharInt[c]=v;
    _outputMappingIntChar[v]=c;
  }

  void LtlRersMapping::addInputAsciiValueSetWithOffsetA(std::string setString) {
    set<int> valueSet=CodeThorn::Parse::integerSet(setString);
    for(auto val : valueSet) {
      addInput('A'+val-1,val);
    }
  }
  void LtlRersMapping::addOutputAsciiValueSetWithOffsetA(std::string setString) {
    set<int> valueSet=CodeThorn::Parse::integerSet(setString);
    for(auto val : valueSet) {
      addOutput('A'+val-1,val);
    }
  }

  std::set<char> LtlRersMapping::getInputCharSet() const {
    std::set<char> set;
    for(auto entry : _inputMappingCharInt) {
      set.insert(entry.first);
    }
    return set;
  }
  std::set<int> LtlRersMapping::getInputValueSet() const {
    std::set<int> set;
    for(auto entry : _inputMappingCharInt) {
      set.insert(entry.second);
    }
    return set;
  }
  std::set<char> LtlRersMapping::getOutputCharSet() const {
    std::set<char> set;
    for(auto entry : _outputMappingCharInt) {
      set.insert(entry.first);
    }
    return set;
  }
  std::set<int> LtlRersMapping::getOutputValueSet() const {
    std::set<int> set;
    for(auto entry : _outputMappingCharInt) {
      set.insert(entry.second);
    }
    return set;
  }
  std::set<int> LtlRersMapping::getInputOutputValueSet() const {
    std::set<int> set=getInputValueSet();
    std::set<int> oset=getOutputValueSet();
    set.insert(oset.begin(),oset.end());
    return set;
  }
  int LtlRersMapping::getValue(char c) const {
    auto iterI=_inputMappingCharInt.find(c);
    if(iterI!=_inputMappingCharInt.end())
      return (*iterI).second;
    auto iterO=_outputMappingCharInt.find(c);
    if(iterO!=_outputMappingCharInt.end())
      return (*iterO).second;
    throw CodeThorn::Exception(string("LtlRersMapping::getValue unknown char: ")+c);
  }
  char LtlRersMapping::getChar(int value) const {
    auto iterI=_inputMappingIntChar.find(value);
    if(iterI!=_inputMappingIntChar.end())
      return (*iterI).second;
    auto iterO=_outputMappingIntChar.find(value);
    if(iterO!=_outputMappingIntChar.end())
      return (*iterO).second;
    stringstream ss;
    ss<<value;
    throw CodeThorn::Exception(string("LtlRersMapping::getChar unknown value: ")+ss.str());
  }
  std::string LtlRersMapping::getIOString(int value) const {
    auto iterI=_inputMappingIntChar.find(value);
    if(iterI!=_inputMappingIntChar.end())
      return string("i")+(*iterI).second;
    auto iterO=_outputMappingIntChar.find(value);
    if(iterO!=_outputMappingIntChar.end())
      return string("o")+(*iterO).second;
    stringstream ss;
    ss<<value;
    throw CodeThorn::Exception(string("LtlRersMapping::getIOString unknown value: ")+ss.str());
  }
  bool LtlRersMapping::isInput(char c) const {
    return _inputMappingCharInt.find(c)!=_inputMappingCharInt.end();
  }
  bool LtlRersMapping::isInput(int value) const {
    return _inputMappingIntChar.find(value)!=_inputMappingIntChar.end();
  }
  bool LtlRersMapping::isOutput(char c) const {
    return _outputMappingCharInt.find(c)!=_inputMappingCharInt.end();
  }
  bool LtlRersMapping::isOutput(int value) const {
    return _outputMappingIntChar.find(value)!=_inputMappingIntChar.end();
  }
  bool LtlRersMapping::isActive() const {
    return _inputMappingCharInt.size()>0||_outputMappingCharInt.size()>0;
  }
} // end of namespace
