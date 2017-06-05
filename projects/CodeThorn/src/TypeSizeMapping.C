#include "TypeSizeMapping.h"
#include "CodeThornException.h"

namespace CodeThorn {
  
  void TypeSizeMapping::setMapping(std::vector<CodeThorn::TypeSize> mapping) {
    if(mapping.size()==_mapping.size()) {
      _mapping=mapping;
    } else {
      throw CodeThorn::Exception("TypeSizeMapping: wrong number of type-size mapping values.");
    }
  }
  
  void TypeSizeMapping::setTypeSize(BuiltInType bitype, CodeThorn::TypeSize size) {
    _mapping[bitype]=size;
  }
  
  CodeThorn::TypeSize TypeSizeMapping::getTypeSize(BuiltInType bitype) {
    return _mapping[bitype];
  }
  
  std::size_t TypeSizeMapping::sizeOfOp(BuiltInType bitype) {
    return getTypeSize(bitype);
  }
  
  bool TypeSizeMapping::isCpp11StandardCompliant() {
    size_t byteSize=1;
    return byteSize==sizeOfOp(BITYPE_SCHAR)
      && sizeOfOp(BITYPE_SCHAR)<=sizeOfOp(BITYPE_SSHORT)
      && sizeOfOp(BITYPE_SSHORT)<=sizeOfOp(BITYPE_SINT)
      && sizeOfOp(BITYPE_SINT)<=sizeOfOp(BITYPE_SLONG)
      && sizeOfOp(BITYPE_SLONG)<=sizeOfOp(BITYPE_SLONG_LONG);
  }
  
} // end of namespace CodeThorn
