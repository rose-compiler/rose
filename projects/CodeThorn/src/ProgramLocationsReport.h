#ifndef PROGRAM_LOCATIONS_REPORT_H
#define PROGRAM_LOCATIONS_REPORT_H

#include <string>
#include "Labeler.h"
#include <iostream>
#include "CFAnalysis.h"

namespace CodeThorn {
  class ProgramLocationsReport {
  public:
    void writeResultFile(std::string fileName, CodeThorn::Labeler* labeler);
    void writeResultToStream(std::ostream& os,CodeThorn::Labeler* labeler);
    CodeThorn::LabelSet definitiveLocations;
    CodeThorn::LabelSet potentialLocations;
    void recordDefinitiveLocation(CodeThorn::Label lab);
    void recordPotentialLocation(CodeThorn::Label lab);
    bool isNonRecordedLocation(Label lab);
    size_t numDefinitiveLocations();
    size_t numPotentialLocations();
    size_t numTotalLocations();
    // returns entry labels of functions for which no location is recorded in locationsOfInterest
    // locationsOfInterest are supposed to be computed previously for the respective analysis
    // e.g. for a null pointer analysis the locationsOfInterest are pointer dereference locations.
    // RFF=LoI-Definitive-Potential
    // verified=RFF=LoI-Definitive-Potential, falsified=Definitive, unknown=Potential
    LabelSet determineRecordFreeFunctions(CFAnalysis& cfAnalyzer, Flow& flow, LabelSet& locationsOfInterest);
    private:
    std::string programLocation(CodeThorn::Labeler* labeler, CodeThorn::Label lab);
    std::string sourceCodeAtProgramLocation(CodeThorn::Labeler* labeler, CodeThorn::Label lab);
    bool hasSourceLocation(SgStatement* stmt);
  };
}
#endif
