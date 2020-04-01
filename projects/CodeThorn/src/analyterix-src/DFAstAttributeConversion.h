#ifndef DFAstAttributeConversion_H
#define DFAstAttributeConversion_H

#include "Labeler.h"

namespace DFAstAttributeConversion {
  void createUDAstAttributeFromRDAttribute(CodeThorn::Labeler* labeler, std::string rdAttributeName, std::string udAttributeName);
}

#endif
