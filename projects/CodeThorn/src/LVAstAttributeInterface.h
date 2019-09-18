// Author: Markus Schordan, 2013.

#ifndef LVASTATTRIBUTEINTERFACE_H
#define LVASTATTRIBUTEINTERFACE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "DFAstAttribute.h"

namespace CodeThorn {

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class LVAstAttributeInterface : public DFAstAttribute {
 public:
  virtual bool isLive(VariableId varId)=0;
  virtual ~LVAstAttributeInterface();
};

}

#endif
