#ifndef GENERAL_RESULT_ATTRIBUTE_H
#define GENERAL_RESULT_ATTRIBUTE_H

#include "sage3basic.h"
#include "DFAstAttribute.h"

class GeneralResultAttribute : public DFAstAttribute {
public:
  GeneralResultAttribute(string info);
  string toString();
private:
  string _info;
};

#endif
