#ifndef PROGRAM_LOCATIONS_REPORT_H
#define PROGRAM_LOCATIONS_REPORT_H

#include <string>
#include "Labeler.h"
#include <iostream>
#include "CFAnalysis.h"

namespace CodeThorn {
  class ProgramLocationsReport {
  public:
    void recordDefinitiveLocation(CodeThorn::Label lab);
    void recordPotentialLocation(CodeThorn::Label lab);
    bool isNonRecordedLocation(Label lab);
    size_t numDefinitiveLocations();
    size_t numPotentialLocations();
    size_t numTotalLocations();

    // provide set of all locations
    void setAllLocationsOfInterest(LabelSet loc);
    LabelSet verifiedLocations();
    LabelSet falsifiedLocations();
    LabelSet unverifiedLocations();
    
    // computes function locations from existing recorded/set locations (as subsets)
    // the labeler is used to determine the function entry labels
    LabelSet verifiedFunctions(Labeler* labeler);
    LabelSet falsifiedFunctions(Labeler* labeler);
    LabelSet unverifiedFunctions(Labeler* labeler);

    bool hasSourceLocation(SgStatement* stmt);
    LabelSet filterFunctionEntryLabels(Labeler* labeler, LabelSet labSet);

    void writeResultFile(std::string fileName, CodeThorn::Labeler* labeler);
    void writeResultToStream(std::ostream& stream, CodeThorn::Labeler* labeler);
    void writeAllDefinitiveLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, bool qualifier, bool programLocation, bool sourceCode);
    void writeAllPotentialLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, bool qualifier, bool programLocation, bool sourceCode);
    void writeLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, LabelSet& set, std::string qualifier, bool programLocation, bool sourceCode);

    void writeLocationsVerificationReport(std::ostream& os, CodeThorn::Labeler* labeler);
    void writeFunctionsVerificationReport(std::ostream& os, CodeThorn::Labeler* labeler);
  private:
    std::string programLocation(CodeThorn::Labeler* labeler, CodeThorn::Label lab);
    std::string sourceCodeAtProgramLocation(CodeThorn::Labeler* labeler, CodeThorn::Label lab);
    // provide set of all locations
    LabelSet determineRecordFreeFunctions(CFAnalysis& cfAnalyzer, Flow& flow);

    CodeThorn::LabelSet definitiveLocations;
    CodeThorn::LabelSet potentialLocations;
    CodeThorn::LabelSet allLocations;
    CodeThorn::LabelSet definitiveFunctionEntryLocations;
    CodeThorn::LabelSet potentialFunctionEntryLocations;
    CodeThorn::LabelSet allFunctionEntryLocations;
  };
}
#endif
