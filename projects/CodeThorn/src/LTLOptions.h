#ifndef LTL_OPTIONS_H
#define LTL_OPTIONS_H

#include "Options.h"
#include <string>

struct LTLOptions : public CodeThorn::Options {
  // general LTL options
  std::string spotVerificationResultsCSVFileName;
  std::string ltlStatisticsCSVFileName;
  std::string ltlFormulaeFile;
  int propertyNrToCheck=-1;
  bool counterExamplesWithOutput=false;
  bool inifinitePathsOnly=false;
  int ioReduction=0; // experimental (threshold value)
  bool keepErrorStates=false;
  std::string ltlInAlphabet;  // format: "{NUM,NUM,...}"
  std::string ltlOutAlphabet; // format: "{NUM,NUM,...}"
  bool ltlDriven=false;
  bool resetAnalyzer=false;
  bool noInputInputTransitions=false; // deprecated
  bool stdIOOnly=false;
  bool withCounterExamples=false;
  bool withAssertCounterExamples=false;
  bool withLTLCounterExamples=false;

  // cegpra LTL options
  struct CEGPra {
    std::string csvStatsFileName;
    int ltlPropertyNr=-1; // between 0..99
    bool checkAllProperties=false;
    int maxIterations=-1;
    std::string visualizationDotFile;
    bool ltlPropertyNrIsSet();
  } cegpra;

  bool activeOptionsRequireSPOTLibrary();
};

#endif
