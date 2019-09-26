// Author: Markus Schordan, 2013.

#ifndef RDASTATTRIBUTE_H
#define RDASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "RDAstAttributeInterface.h"
#include "RDLattice.h"
#include <string>

using std::string;

namespace CodeThorn {

class RDAstAttribute : public RDAstAttributeInterface {
 public:
  virtual bool isBottomElement();
  virtual CodeThorn::VariableIdSet allVariableIds();
  virtual CodeThorn::LabelSet allLabels();
  virtual CodeThorn::LabelSet definitionsOfVariableId(CodeThorn::VariableId varId);
  virtual CodeThorn::VariableIdSet variableIdsOfDefinition(CodeThorn::Label def);
  virtual iterator begin();
  virtual iterator end();
  virtual ~RDAstAttribute();
 public:
  RDAstAttribute(CodeThorn::RDLattice* elem);
  void toStream(std::ostream& os, CodeThorn::VariableIdMapping* vim);
  string toString();
 private:
  CodeThorn::RDLattice* _elem;
};

}

#endif
