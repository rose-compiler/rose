#ifndef TYPE_SIZE_MAPPING_H
#define TYPE_SIZE_MAPPING_H

#include <vector>
#include <unordered_map>

namespace CodeThorn {

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
    BITYPE_REFERENCE,
    BITYPE_SIZE
  };
  
  class TypeSizeMapping {
  public:
    TypeSizeMapping();
    virtual ~TypeSizeMapping();
    // sets sizes of all types (same as reported by sizeof on respective architecture)
    void setMapping(std::vector<CodeThorn::TypeSize> mapping);
    // sets size of one type (same as reported by sizeof on respective architecture)
    void setTypeSize(BuiltInType bitype, CodeThorn::TypeSize size);
    CodeThorn::TypeSize getTypeSize(CodeThorn::BuiltInType bitype);
    std::size_t sizeOfOp(BuiltInType bitype);
    bool isCpp11StandardCompliant();
    virtual std::string toString()=0;
  protected:
    // default setting LP64 data model
    std::vector<CodeThorn::TypeSize> _mapping={1,
                                           1,1,2,2,4,4,
                                           2,2,4,4,4,4,8,8,
                                           4,8,16,
                                           8,
                                           8,
    };
    std::unordered_map<SgType*,unsigned int> _typeToSizeMapping;
  };
}

#endif
