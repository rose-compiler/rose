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
  return byteSize==sizeOfOp(BITYPE_CHAR)
    && sizeOfOp(BITYPE_CHAR)<=sizeOfOp(BITYPE_SHORT)
    && sizeOfOp(BITYPE_SHORT)<=sizeOfOp(BITYPE_INT)
    && sizeOfOp(BITYPE_INT)<=sizeOfOp(BITYPE_LONG)
    && sizeOfOp(BITYPE_LONG)<=sizeOfOp(BITYPE_LONG_LONG);
}

} // end of namespace CodeThorn
