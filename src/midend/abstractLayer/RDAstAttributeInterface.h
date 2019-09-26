// Author: Markus Schordan, 2013.

#ifndef RDASTATTRIBUTEINTERFACE_H
#define RDASTATTRIBUTEINTERFACE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "DFAstAttribute.h"

namespace CodeThorn {

/*!
  * \author Markus Schordan
  * \date 2013.
 */
class RDAstAttributeInterface : public DFAstAttribute {
 public:
  typedef std::pair<CodeThorn::Label,VariableId> RDPair;
  typedef std::set<RDPair> RDPairSet;
  typedef RDPairSet::iterator iterator;
  virtual bool isBottomElement()=0;
  virtual VariableIdSet allVariableIds()=0;
  virtual CodeThorn::LabelSet allLabels()=0;
  virtual CodeThorn::LabelSet definitionsOfVariableId(VariableId varId)=0;
  virtual VariableIdSet variableIdsOfDefinition(CodeThorn::Label def)=0;
  virtual iterator begin()=0;
  virtual iterator end()=0;
  virtual ~RDAstAttributeInterface();
};

}

#endif
