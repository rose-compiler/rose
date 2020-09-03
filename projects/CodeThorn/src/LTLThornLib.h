#ifndef LTLTHORN_LIB_H
#define LTLTHORN_LIB_H

#include <set>
#include "CodeThornOptions.h"
#include "LTLOptions.h"

namespace CodeThorn {
  class LtlRersMapping {
  public:
    void addInput(char,int);
    void addOutput(char,int);
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

  void initDiagnosticsLTL();
  void runLTLAnalysis(CodeThornOptions& ctOpt, LTLOptions& ltlOpt,IOAnalyzer* analyzer, TimingCollector& tc);
  void optionallyInitializePatternSearchSolver(CodeThornOptions& ctOpt,IOAnalyzer* analyzer,TimingCollector& timingCollector);
  /* reads and parses LTL rers mapping file into the alphabet
     sets. Returns false if reading fails. Exits with error message if
     format is wrong.
  */
  bool readAndParseLTLRersMappingFile(string ltlRersMappingFileName, LtlRersMapping& ltlRersMapping);
}

#endif
