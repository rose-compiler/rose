#ifndef LTL_RERS_MAPPING_H
#define LTL_RERS_MAPPING_H

#include <string>
#include <map>
#include <set>

namespace CodeThorn {
  class LtlRersMapping {
  public:
    void addInput(char,int);
    void addOutput(char,int);
    // legacy support for old format
    void addInputAsciiValueSetWithOffsetA(std::string);
    // legacy support for old format
    void addOutputAsciiValueSetWithOffsetA(std::string);
    std::set<char> getInputCharSet() const;
    std::set<int> getInputValueSet() const;
    std::set<char> getOutputCharSet() const;
    std::set<int> getOutputValueSet() const;
    std::set<int> getInputOutputValueSet() const;
    int getValue(char c) const;
    char getChar(int value) const;
    std::string getIOString(int value) const; // e.g. "iA" or "oZ"
    bool isInput(char c) const;
    bool isInput(int value) const;
    bool isOutput(char c) const;
    bool isOutput(int value) const;
  private:
    // data structures
    std::map<char,int> _inputMappingCharInt;
    std::map<int,char> _inputMappingIntChar;
    std::map<char,int> _outputMappingCharInt;
    std::map<int,char> _outputMappingIntChar;
  };
}

#endif
