// Author: Markus Schordan, 2013.

#ifndef CPASTATTRIBUTEINTERFACE_H
#define CPASTATTRIBUTEINTERFACE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "DFAstAttribute.h"

using namespace CodeThorn;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class CPAstAttributeInterface : public DFAstAttribute {
 public:
  typedef ssize_t ConstantNumber;
  typedef pair<VariableId,ConstantNumber> CPPair;
  typedef set<CPPair> CPPairSet;
  typedef CPPairSet::iterator iterator;
  virtual VariableIdSet allVariableIds()=0;
  virtual bool isConstantNumber(VariableId varId)=0;
  virtual ConstantNumber getConstantNumber(VariableId varId)=0;
  virtual iterator begin()=0;
  virtual iterator end()=0;
  virtual ~CPAstAttributeInterface();
};

#endif
