#include "FunctionReturnAttribute.h"

std::string
FunctionReturnAttribute::attribute_class_name() const
{
  return "MatlabTranslation/FunctionReturnAttribute";
}

AstAttribute::OwnershipPolicy
FunctionReturnAttribute::getOwnershipPolicy() const
{
  return NO_OWNERSHIP;
}

