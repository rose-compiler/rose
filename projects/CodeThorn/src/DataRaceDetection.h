#ifndef DATA_RACE_DETECTION_H
#define DATA_RACE_DETECTION_H

#include <string>
#include "Analyzer.h"
#include "CommandLineOptions.h"

/*! \brief Data race detection based on a computed state transition
    system and read/write sets.

  * \author Markus Schordan
  * \date 2015, 2016.

  \details This class provides functions for handling command line options
  and a function 'run' that invokes all analyzer functions necessary to perform a
  read-write-set based data race detection.
 */

class DataRaceDetection {
 public:
  DataRaceDetection();
  void setCsvFileName(std::string fileName);
  void handleCommandLineOptions(CodeThorn::Analyzer& analyzer, BoolOptions& boolOptions);
  bool run(CodeThorn::Analyzer& analyzer, BoolOptions& boolOptions);
 private:
  struct Options {
    Options();
    bool active;
    // fail if data race is detected (terminate, return, and report data race found)
    bool dataRaceFail;
    // name of csv file for data race analysis results. If string is empty, no information is written to file.
    string dataRaceCsvFileName;
    unsigned int maxFloatingPointOperations;
    bool useConstSubstitutionRule;
    bool visualizeReadWriteSets;
    bool printUpdateInfos;
  };
  Options options;
};
#endif
