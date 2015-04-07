// Author: Markus Schordan, 2013.

#ifndef RDASTATTRIBUTE_H
#define RDASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "RDAstAttributeInterface.h"
#include "RDLattice.h"
#include <string>

using std::string;

namespace SPRAY {

class RDAstAttribute : public RDAstAttributeInterface {
 public:
  virtual bool isBottomElement();
  virtual SPRAY::VariableIdSet allVariableIds();
  virtual SPRAY::LabelSet allLabels();
  virtual SPRAY::LabelSet definitionsOfVariableId(SPRAY::VariableId varId);
  virtual SPRAY::VariableIdSet variableIdsOfDefinition(SPRAY::Label def);
  virtual iterator begin();
  virtual iterator end();
  virtual ~RDAstAttribute();
 public:
  RDAstAttribute(SPRAY::RDLattice* elem);
  void toStream(std::ostream& os, SPRAY::VariableIdMapping* vim);
  string toString();
 private:
  SPRAY::RDLattice* _elem;
};

}

#endif
