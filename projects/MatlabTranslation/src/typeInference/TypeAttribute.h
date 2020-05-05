#ifndef OCTAVE_ANALYSIS_TYPE_ATTRIBUTE_H
#define OCTAVE_ANALYSIS_TYPE_ATTRIBUTE_H

#include "rose.h"

enum TypeFlag
{
  FlagBool    = 1 << 0,
  FlagInteger = 1 << 1,
  FlagDouble  = 1 << 2,
  FlagMatrix  = 1 << 3,
  FlagString  = 1 << 4,
};

class TypeAttribute;

struct MatlabTypeInfo
{
  TypeFlag       flag;
  TypeAttribute* attribute;
};


struct TypeAttribute : AstAttribute
{
    static TypeAttribute* buildAttribute(SgType* type);
    static TypeAttribute* get_typeAttribute(SgNode* node);

    explicit
    TypeAttribute(SgType* inferredType);

    void set_inferred_type(SgType* inferredType);
    SgType* get_inferred_type() const;

    MatlabTypeInfo get_type_info();

    void attach_to(SgNode* node);

    std::string toString() ROSE_OVERRIDE;
    std::string attribute_class_name() const ROSE_OVERRIDE;

    AstAttribute::OwnershipPolicy
    getOwnershipPolicy() const ROSE_OVERRIDE;

  private:
    SgType* inferredType;
};
#endif
