#include "LTLRersMapping.h"
#include "CodeThornException.h"
#include "Miscellaneous2.h"

using namespace std;

namespace CodeThorn {
  void LtlRersMapping::addInput(char c,int v) {
    _inputMappingCharInt[c]=v;
    _inputMappingIntChar[v]=c;
  }
  void LtlRersMapping::addOutput(char c,int v) {
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

  std::set<char> LtlRersMapping::getInputCharSet() {
    std::set<char> set;
    for(auto entry : _inputMappingCharInt) {
      set.insert(entry.first);
    }
    return set;
  }
  std::set<int> LtlRersMapping::getInputValueSet() {
    std::set<int> set;
    for(auto entry : _inputMappingCharInt) {
      set.insert(entry.second);
    }
    return set;
  }
  std::set<char> LtlRersMapping::getOutputCharSet() {
    std::set<char> set;
    for(auto entry : _outputMappingCharInt) {
      set.insert(entry.first);
    }
    return set;
  }
  std::set<int> LtlRersMapping::getOutputValueSet() {
    std::set<int> set;
    for(auto entry : _outputMappingCharInt) {
      set.insert(entry.second);
    }
    return set;
  }
  int LtlRersMapping::getValue(char c) {
    auto iterI=_inputMappingCharInt.find(c);
    if(iterI!=_inputMappingCharInt.end())
      return (*iterI).second;
    auto iterO=_outputMappingCharInt.find(c);
    if(iterO!=_outputMappingCharInt.end())
      return (*iterO).second;
    throw CodeThorn::Exception(string("LtlRersMapping::getValue unknown char: ")+c);
  }
  char LtlRersMapping::getChar(int value) {
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
  std::string LtlRersMapping::getIOString(int value) {
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
  bool LtlRersMapping::isInput(char c) {
    return _inputMappingCharInt.find(c)!=_inputMappingCharInt.end();
  }
  bool LtlRersMapping::isInput(int value) {
    return _inputMappingIntChar.find(value)!=_inputMappingIntChar.end();
  }
  bool LtlRersMapping::isOutput(char c) {
    return _outputMappingCharInt.find(c)!=_inputMappingCharInt.end();
  }
  bool LtlRersMapping::isOutput(int value) {
    return _outputMappingIntChar.find(value)!=_inputMappingIntChar.end();
  }
} // end of namespace
