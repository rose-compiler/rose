// Author: Markus Schordan, 2013.

#ifndef INTERVALASTATTRIBUTEINTERFACE_H
#define INTERVALASTATTRIBUTEINTERFACE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "DFAstAttribute.h"

using namespace CodeThorn;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class IntervalAstAttributeInterface : public DFAstAttribute {
 public:
  virtual bool isBottomElement()=0;
  virtual VariableIdSet allVariableIds()=0;
  virtual ~IntervalAstAttributeInterface();
};

#endif
