#ifndef CPASTATTRIBUTE_H
#define CPASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "CPAstAttributeInterface.h"
#include "AbstractValue.h"

using namespace CodeThorn;

class VariableConstInfo;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class SgNode;

class CPAstAttribute : public CPAstAttributeInterface {
 public:
  //virtual VariableIdSet allVariableIds();
  virtual bool isConstantInteger(VariableId varId);
  virtual CPAstAttributeInterface::ConstantInteger getConstantInteger(VariableId varId);
  //virtual iterator begin();
  //virtual iterator end();
  //virtual ~CPAstAttribute();
 public:
  CPAstAttribute(VariableConstInfo* elem, SgNode* node, VariableIdMapping* variableIdMapping);
  void toStream(ostream& os, VariableIdMapping* vim);
  string toString();
 private:
  VariableConstInfo* _elem;
  SgNode* _node;
  VariableIdMapping* _variableIdMapping;
};

#endif
