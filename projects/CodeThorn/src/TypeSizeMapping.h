#ifndef TYPE_SIZE_MAPPING_H
#define TYPE_SIZE_MAPPING_H

#include <vector>
#include <cstdint>
namespace CodeThorn {
  
enum BuiltInType {
  BITYPE_BOOL,
  BITYPE_CHAR, BITYPE_UCHAR,
  BITYPE_CHAR16, BITYPE_UCHAR16,
  BITYPE_CHAR32, BITYPE_UCHAR32,
  BITYPE_SHORT, BITYPE_USHORT,
  BITYPE_INT, BITYPE_UINT,
  BITYPE_LONG, BITYPE_ULONG,
  BITYPE_LONGLONG, BITYPE_ULONG_LONG,
  BITYPE_FLOAT, BITYPE_DOUBLE, BITYPE_LONG_DOUBLE
};
 
 class TypeSizeMapping {
 public:
   // sets sizes of all types (same as reported by sizeof on respective architecture)
   void setMapping(std::vector<uint8_t> mapping);
   // sets size of one type (same as reported by sizeof on respective architecture)
   void setTypeSize(BuiltInType bitype, uint8_t size);
   uint8_t getTypeSize(BuiltInType bitype);
 private:
   // default setting LP64 data model
   std::vector<std::uint8_t> _mapping={1,
                                       1,1,2,2,4,4,
                                       2,2,4,4,4,4,8,8,
                                       4,8,16
   };
 };
}

#endif
