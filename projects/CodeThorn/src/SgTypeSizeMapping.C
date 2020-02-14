#include "sage3basic.h"
#include "SgTypeSizeMapping.h"
#include <sstream>

namespace CodeThorn {
  CodeThorn::TypeSize SgTypeSizeMapping::determineTypeSize(SgType* sgType) {
    ROSE_ASSERT(_mapping.size()!=0);
    ROSE_ASSERT(sgType);
    if(_typeToSizeMapping.find(sgType)!=_typeToSizeMapping.end()) {
      return _typeToSizeMapping[sgType];
    }
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
    case V_SgReferenceType:
      return getTypeSize(BITYPE_REFERENCE);
    case V_SgArrayType: {
      CodeThorn::TypeSize elementTypeSize=determineElementTypeSize(isSgArrayType(sgType));
      // TODO determine size of the array and multiply it with element type size.
      CodeThorn::TypeSize numberOfElements=1; // TODO
      unsigned int totalSize=numberOfElements*elementTypeSize;
      _typeToSizeMapping[sgType]=totalSize; // cache result
      return totalSize;
    }
    case V_SgClassType: {
      typedef std::vector< std::pair< SgNode*, std::string > > DataMemberPointers;
      CodeThorn::TypeSize sum=0;
      DataMemberPointers dataMemPtrs=isSgClassType(sgType)->returnDataMemberPointers();
      // returnDataMemberPointers includes all declarations (methods need to be filtered)
      for(DataMemberPointers::iterator i=dataMemPtrs.begin();i!=dataMemPtrs.end();++i) {
        SgNode* node=(*i).first; 
        if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
          sum+=determineTypeSize(varDecl->get_type());
        }
      }
      _typeToSizeMapping[sgType]=sum;
      return sum;
    }
      //case V_SgFunctionType:
      //case V_SgTypeComplex:

    default:
      // to investigate
      return 0;
    }
  }

  CodeThorn::TypeSize SgTypeSizeMapping::determineElementTypeSize(SgArrayType* sgType) {
    ROSE_ASSERT(_mapping.size()!=0);
    SgType* elementType=sgType->get_base_type();
    return determineTypeSize(elementType);
  }

  CodeThorn::TypeSize SgTypeSizeMapping::determineTypeSizePointedTo(SgPointerType* sgType) {
    ROSE_ASSERT(_mapping.size()!=0);
    SgType* typePointedTo=sgType->get_base_type();
    return determineTypeSize(typePointedTo);
  }

  std::string SgTypeSizeMapping::toString() {
    std::ostringstream ss;
    for(auto entry : _typeToSizeMapping ) {
      ss<<entry.first->unparseToString()<<":"<<entry.second<<std::endl;
    }
    return ss.str();
  }

} // end of namespace CodeThorn
