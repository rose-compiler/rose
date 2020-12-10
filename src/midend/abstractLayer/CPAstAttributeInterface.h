// Author: Markus Schordan, 2013.

#ifndef CPASTATTRIBUTEINTERFACE_H
#define CPASTATTRIBUTEINTERFACE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "DFAstAttribute.h"

namespace CodeThorn {

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class CPAstAttributeInterface : public DFAstAttribute {
 public:
  typedef ssize_t ConstantInteger;
  virtual bool isConstantInteger(VariableId varId)=0;
  virtual ConstantInteger getConstantInteger(VariableId varId)=0;
  virtual ~CPAstAttributeInterface();
};

}

#endif
