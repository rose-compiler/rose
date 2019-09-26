// Author: Markus Schordan, 2013.

#ifndef LVANALYSISASTATTRIBUTE_H
#define LVANALYSISASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "LVAstAttributeInterface.h"
#include "LVLattice.h"
#include <string>

namespace CodeThorn {

class LVAstAttribute : public LVAstAttributeInterface {
 public:
  virtual bool isLive(VariableId varId);
  virtual bool isBottomElement();
  virtual VariableIdSet allVariableIds();
  virtual LVLattice::iterator begin();
  virtual LVLattice::iterator end();
  virtual ~LVAstAttribute();
  LVAstAttribute(LVLattice* elem);
  void toStream(std::ostream& os, VariableIdMapping* vim);
  std::string toString();
 private:
  LVLattice* _elem;
};

}

#endif
