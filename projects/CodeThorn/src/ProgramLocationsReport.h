#ifndef PROGRAM_LOCATIONS_REPORT_H
#define PROGRAM_LOCATIONS_REPORT_H

#include <string>
#include "Labeler.h"

namespace CodeThorn {
  class ProgramLocationsReport {
  public:
    void writeResultFile(std::string fileName, SPRAY::Labeler* labeler);
    SPRAY::LabelSet definitiveLocations;
    SPRAY::LabelSet potentialLocations;
    void recordDefinitiveLocation(SPRAY::Label lab);
    void recordPotentialLocation(SPRAY::Label lab);
  private:
    std::string programLocation(SPRAY::Labeler* labeler, SPRAY::Label lab);
    std::string sourceCodeAtProgramLocation(SPRAY::Labeler* labeler, SPRAY::Label lab);
  };
}
#endif
