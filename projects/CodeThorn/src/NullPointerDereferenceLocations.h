#ifndef NULL_POINTER_DEREFERENCE_LOCATIONS_H
#include <string>
#include "Labeler.h"
namespace CodeThorn {
  class NullPointerDereferenceLocations {
  public:
    void writeResultFile(std::string fileName, SPRAY::Labeler* labeler);
    SPRAY::LabelSet definitiveDereferenceLocations;
    SPRAY::LabelSet potentialDereferenceLocations;
    void recordDefinitiveDereference(SPRAY::Label lab);
    void recordPotentialDereference(SPRAY::Label lab);
  private:
    std::string programLocation(SPRAY::Labeler* labeler, SPRAY::Label lab);
  };
}
#endif
