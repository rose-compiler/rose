#ifndef LTL_OPTIONS_H
#define LTL_OPTIONS_H

#include "Options.h"
#include <string>

struct LTLOptions : public Options {
  // general LTL options
  std::string spotVerificationResultsCSVFileName;
  std::string ltlStatisticsCSVFileName;
  std::string ltlFormulaeFile;
  int propertyNrToCheck;
  bool counterExamplesWithOutput;
  bool inifinitePathsOnly;
  int ioReduction; // experimental
  bool keepErrorStates;
  std::string ltlInAlphabet;  // format: "{NUM,NUM,...}"
  std::string ltlOutAlphabet; // format: "{NUM,NUM,...}"
  bool ltlDriven;
  bool resetAnalyzer;
  bool noInputInputTransitions; // deprecated
  bool stdIOOnly;
  bool withCounterExamples;
  bool withAssertCounterExamples;
  bool withLTLCounterExamples;

  // cegpra LTL options
  struct CEGPra {
    std::string csvStats;
    int ltlPropertyNr; // between 0..99
    bool checkAllProperties;
    int maxIterations;
    std::string visualizationDotFile;
  } cegpra;
  
};

#endif
