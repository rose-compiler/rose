
#include <map>
#include "TypeAttribute.h"

//static TypeAttribute* TypeAttribute::buildAttribute(SgType *type);

TypeAttribute::TypeAttribute(SgType *inferredType)
{
  set_inferred_type(inferredType);
}

void TypeAttribute::set_inferred_type(SgType *inferredType)
{
  this->inferredType = inferredType;
}

SgType* TypeAttribute::get_inferred_type() const
{
  return this->inferredType;
}

MatlabTypeInfo TypeAttribute::get_type_info()
{
  MatlabTypeInfo typeInfo;

  switch (inferredType->variantT())
  {
    case V_SgTypeBool:
      typeInfo.flag = FlagBool;
      break;

    case V_SgTypeInt:
      typeInfo.flag = FlagInteger;
      break;

    case V_SgTypeDouble:
      typeInfo.flag = FlagDouble;
      break;

    case V_SgTypeMatrix:
      typeInfo.flag = FlagMatrix;
      break;

    case V_SgTypeTuple: // \pp \todo this does not seem to be correct!!
      typeInfo.flag = FlagMatrix;
      break;

    case V_SgTypeString:
      typeInfo.flag = FlagString;
      break;

    default:
      std::cerr << "Oops: " << typeid(*inferredType).name() << std::endl;
      std::cerr << "Oops: " << inferredType->unparseToString() << std::endl;
      ROSE_ASSERT(false);
  }

  typeInfo.attribute = this;
  return typeInfo;
}

void TypeAttribute::attach_to(SgNode *node)
{
  node->setAttribute("TYPE_INFERRED", this);
}

TypeAttribute* TypeAttribute::get_typeAttribute(SgNode *node)
{
  return (TypeAttribute*)(node->getAttribute("TYPE_INFERRED"));
}

std::string TypeAttribute::toString()
{
  return SageInterface::get_name(this->inferredType);
}

TypeAttribute* TypeAttribute::buildAttribute(SgType *type)
{
  static std::map<SgType*, TypeAttribute*> prebuiltAttributes;

  TypeAttribute *typeAttribute;

  if(prebuiltAttributes.find(type) != prebuiltAttributes.end())
    {
      typeAttribute = prebuiltAttributes[type];
    }
  else
    {
      typeAttribute = new TypeAttribute(type);
      prebuiltAttributes[type] = typeAttribute;
    }

  return typeAttribute;
}

AstAttribute::OwnershipPolicy
TypeAttribute::getOwnershipPolicy() const
{
  return NO_OWNERSHIP;
}

std::string TypeAttribute::attribute_class_name() const
{
  return "MatlabTranslation/TypeAttribute";
}
