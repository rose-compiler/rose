#include "sage3basic.h"
#include "SgTypeSizeMapping.h"

namespace SPRAY {

  SPRAY::TypeSize SgTypeSizeMapping::determineTypeSize(SgType* sgType) {
    switch (sgType->variantT()) {

    case V_SgPointerType:
    //case V_SgTypeNullPtr:
      return getTypeSize(BITYPE_POINTER);

    case V_SgTypeBool:
      return getTypeSize(BITYPE_BOOL);

    case V_SgTypeChar:
    case V_SgTypeSignedChar:
      return getTypeSize(BITYPE_SCHAR);
    case V_SgTypeUnsignedChar:
      return getTypeSize(BITYPE_UCHAR);

    case V_SgTypeShort:
    case V_SgTypeSignedShort:
      return getTypeSize(BITYPE_SSHORT);
    case V_SgTypeUnsignedShort:
      return getTypeSize(BITYPE_USHORT);

    case V_SgTypeUnsignedInt:
      return getTypeSize(BITYPE_UINT);
    case V_SgTypeInt:
    case V_SgTypeSignedInt:
      return getTypeSize(BITYPE_SINT);

    case V_SgTypeUnsignedLong:
      return getTypeSize(BITYPE_ULONG);
    case V_SgTypeLong:
    case V_SgTypeSignedLong:
      return getTypeSize(BITYPE_SLONG);

    case V_SgTypeUnsignedLongLong:
      return getTypeSize(BITYPE_ULONG_LONG);
    case V_SgTypeLongLong:
    case V_SgTypeSignedLongLong:
      return getTypeSize(BITYPE_SLONG_LONG);

    case V_SgTypeFloat:
      return getTypeSize(BITYPE_FLOAT);
    case V_SgTypeDouble:
      return getTypeSize(BITYPE_DOUBLE);
    case V_SgTypeLongDouble:
      return getTypeSize(BITYPE_LONG_DOUBLE);

      // TODO: SgArrayType,SgFunctionType,SgReferenceType,SgTypeComplex
      // TODO: structs/class/union

    default:
      // return size 0 for all other cases
      return 0;
    }
  }

  SPRAY::TypeSize SgTypeSizeMapping::determineTypeSizePointedTo(SgPointerType* sgType) {
    SgType* typePointedTo=sgType->get_base_type();
    return determineTypeSize(typePointedTo);
  }


} // end of namespace SPRAY
