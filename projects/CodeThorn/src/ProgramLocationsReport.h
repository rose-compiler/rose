#ifndef PROGRAM_LOCATIONS_REPORT_H
#define PROGRAM_LOCATIONS_REPORT_H

#include <string>
#include "Labeler.h"
#include <iostream>
#include "CFAnalysis.h"
#include "CodeThornOptions.h"

namespace CodeThorn {
  class ProgramLocationsReport {
  public:
    void recordDefinitiveLocation(CodeThorn::Label lab);
    void recordPotentialLocation(CodeThorn::Label lab);
    bool isRecordedLocation(Label lab);
    size_t numDefinitiveLocations();
    size_t numPotentialLocations();
    size_t numTotalRecordedLocations();

    // provide set of all locations
    void setReachableLocations(LabelSet loc);
    void setUnreachableLocations(LabelSet loc);

    LabelSet verifiedLocations();
    LabelSet falsifiedLocations();
    LabelSet unverifiedLocations();
    
    // computes function locations from existing recorded/set locations (as subsets)
    // the labeler is used to determine the function entry labels
    LabelSet verifiedFunctions(Labeler* labeler);
    LabelSet falsifiedFunctions(Labeler* labeler);
    LabelSet unverifiedFunctions(Labeler* labeler);

    static bool hasSourceLocation(SgNode* stmt);
    LabelSet filterFunctionEntryLabels(Labeler* labeler, LabelSet labSet);

    void writeLocationsVerificationOverview(CodeThornOptions& ctOpt, std::ostream& os, CodeThorn::Labeler* labeler);
    void writeResultFile(CodeThornOptions& ctOpt, std::string fileName, CodeThorn::Labeler* labeler);
    void writeResultToStream(CodeThornOptions& ctOpt, std::ostream& stream, CodeThorn::Labeler* labeler);
    void writeAllDefinitiveLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, bool qualifier, bool programLocation);
    void writeAllPotentialLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, bool qualifier, bool programLocation);
    void writeLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, LabelSet& set, std::string qualifier, bool programLocation);

    static std::string findOriginalProgramLocationOfLabel(CodeThorn::Labeler* labeler, CodeThorn::Label lab);
    static std::string findOriginalProgramLocationOfNode(SgNode* node);
    static std::string sourceCodeAtLabel(CodeThorn::Labeler* labeler, CodeThorn::Label lab); // not used anymore, obsolete
    static std::string fileInfoAsFormattedProgramLocation(SgNode* node);

    typedef std::string LineColString;
    typedef std::set<std::string> SortedProgramLocationStringSet;
    SortedProgramLocationStringSet createOriginalProgramLocationSet(LabelSet labSet, Labeler* labeler);
  private:

    // provide set of all locations (unused function)
    LabelSet determineRecordFreeFunctions(CFAnalysis& cfAnalysis, Flow& flow);

    CodeThorn::LabelSet definitiveLocations;
    CodeThorn::LabelSet potentialLocations;
    CodeThorn::LabelSet reachableLocations;
    CodeThorn::LabelSet unreachableLocations;
    CodeThorn::LabelSet definitiveFunctionEntryLocations;
    CodeThorn::LabelSet potentialFunctionEntryLocations;
    CodeThorn::LabelSet allFunctionEntryLocations;
  };
}
#endif
