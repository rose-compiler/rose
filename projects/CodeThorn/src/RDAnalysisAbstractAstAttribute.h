// Author: Markus Schordan, 2013.

#ifndef RDANALYSISABSTRACTASTATTRIBUTE_H
#define RDANALYSISABSTRACTASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"

using namespace CodeThorn;

class RDAnalysisAbstractAstAttribute : public AstAttribute {
 public:
  typedef pair<Label,VariableId> RDPair;
  typedef set<RDPair> RDPairSet;
  typedef RDPairSet::iterator iterator;
  virtual bool isBottomElement()=0;
  virtual VariableIdSet allVariableIds()=0;
  virtual LabelSet allLabels()=0;
  virtual LabelSet definitionsOfVariableId(VariableId varId)=0;
  virtual VariableIdSet variableIdsOfDefinition(Label def)=0;
  virtual iterator begin()=0;
  virtual iterator end()=0;
  virtual ~RDAnalysisAbstractAstAttribute();
};

#endif
