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
    std::set<char> getInputCharSet();
    std::set<int> getInputValueSet();
    std::set<char> getOutputCharSet();
    std::set<int> getOutputValueSet();
    int getValue(char c);
    char getChar(int value);
    std::string getIOString(int value); // e.g. "iA" or "oZ"
    bool isInput(char c);
    bool isInput(int value);
    bool isOutput(char c);
    bool isOutput(int value);
  private:
    // data structures
    std::map<char,int> _inputMappingCharInt;
    std::map<int,char> _inputMappingIntChar;
    std::map<char,int> _outputMappingCharInt;
    std::map<int,char> _outputMappingIntChar;
  };
}

#endif
