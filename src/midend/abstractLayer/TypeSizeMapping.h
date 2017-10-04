#ifndef TYPE_SIZE_MAPPING_H
#define TYPE_SIZE_MAPPING_H

#include <vector>

namespace SPRAY {

  // typesize in bytes
  typedef unsigned int TypeSize;
  
  enum BuiltInType {
    BITYPE_BOOL,
    BITYPE_SCHAR, BITYPE_UCHAR,
    BITYPE_SCHAR16, BITYPE_UCHAR16,
    BITYPE_SCHAR32, BITYPE_UCHAR32,
    BITYPE_SSHORT, BITYPE_USHORT,
    BITYPE_SINT, BITYPE_UINT,
    BITYPE_SLONG, BITYPE_ULONG,
    BITYPE_SLONG_LONG, BITYPE_ULONG_LONG,
    BITYPE_FLOAT, BITYPE_DOUBLE, BITYPE_LONG_DOUBLE,
    BITYPE_POINTER,
    BITYPE_REFERENCE
  };
  
  class TypeSizeMapping {
  public:
    TypeSizeMapping();
    // sets sizes of all types (same as reported by sizeof on respective architecture)
    void setMapping(std::vector<SPRAY::TypeSize> mapping);
    // sets size of one type (same as reported by sizeof on respective architecture)
    void setTypeSize(BuiltInType bitype, SPRAY::TypeSize size);
    SPRAY::TypeSize getTypeSize(SPRAY::BuiltInType bitype);
    std::size_t sizeOfOp(BuiltInType bitype);
    bool isCpp11StandardCompliant();
  private:
#if 0
    // C++11 version
    // default setting LP64 data model
    std::vector<SPRAY::TypeSize> _mapping={1,
                                           1,1,2,2,4,4,
                                           2,2,4,4,4,4,8,8,
                                           4,8,16,
                                           8,
                                           8,
    };
#else
    std::vector<SPRAY::TypeSize> _mapping;
#endif
  };
}

#endif
