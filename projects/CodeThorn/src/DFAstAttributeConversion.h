#ifndef DFAstAttributeConversion_H
#define DFAstAttributeConversion_H

#include "Labeler.h"

namespace DFAstAttributeConversion {
  void createUDAstAttributeFromRDAttribute(SPRAY::Labeler* labeler, std::string rdAttributeName, std::string udAttributeName);
}

#endif
