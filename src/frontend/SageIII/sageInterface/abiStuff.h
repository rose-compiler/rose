#ifndef ROSE_ABISTUFF_H
#define ROSE_ABISTUFF_H

#include "rose.h"
#include <vector>
#include <iosfwd>

struct StructLayoutEntry {
  //! The field being laid out, or NULL for padding
  SgInitializedName* decl;
  //! The byte offset of this field (or its containing word for bit fields) in
  //! the structure
  size_t byteOffset;
  //! The size of the field or padding
  size_t fieldSize;
  //! The size of the containing element for this bit field (in bytes)
  size_t bitFieldContainerSize;
  //! Offset of LSB of bit field within element of size bitFieldContainerSize
  //! starting at position byteOffset in the struct
  size_t bitOffset;

  StructLayoutEntry(SgInitializedName* decl, size_t byteOffset, size_t fieldSize, size_t bitFieldContainerSize = 0, size_t bitOffset = 0):
    decl(decl), byteOffset(byteOffset), fieldSize(fieldSize), bitFieldContainerSize(bitFieldContainerSize), bitOffset(bitOffset) {}
};

struct StructLayoutInfo {
  //! Size of this struct or union in bytes
  size_t size;
  //! Alignment of this struct or union in bytes
  size_t alignment;
  //! Fields, empty for non-compound types
  std::vector<StructLayoutEntry> fields;

  StructLayoutInfo(): size(0), alignment(0), fields() {}
};

std::ostream& operator<<(std::ostream& o, const StructLayoutEntry& e);
std::ostream& operator<<(std::ostream& o, const StructLayoutInfo& i);

// Basic type layout engine -- handles bookkeeping, plus handing typedefs and
// modifiers
class ChainableTypeLayoutGenerator {
  public:
  virtual ~ChainableTypeLayoutGenerator() {}
  ChainableTypeLayoutGenerator* next;
  ChainableTypeLayoutGenerator* beginning;
  ChainableTypeLayoutGenerator(ChainableTypeLayoutGenerator* nx)
      : next(NULL), beginning(this)
    {this->setNext(nx);}
  protected:
  void setNext(ChainableTypeLayoutGenerator* nx) {
    this->next = nx;
    if (nx) nx->setBeginningRecursively(this->beginning);
  }
  void setBeginningRecursively(ChainableTypeLayoutGenerator* bg) {
    this->beginning = bg;
    if (this->next) this->next->setBeginningRecursively(bg);
  }
  public:
  virtual StructLayoutInfo layoutType(SgType* t) const;
};

// Layout generator for i386 ABI-like struct layout
// Handles structs and unions only
// Does not handle C++ stuff (inheritance, virtual functions) for now
// No handling of arrays either
class NonpackedTypeLayoutGenerator: public ChainableTypeLayoutGenerator {
  public:
  NonpackedTypeLayoutGenerator(ChainableTypeLayoutGenerator* next)
      : ChainableTypeLayoutGenerator(next)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;
};

// Layout generator for i386 primitive types
class I386PrimitiveTypeLayoutGenerator: public ChainableTypeLayoutGenerator {
  public:
  I386PrimitiveTypeLayoutGenerator(ChainableTypeLayoutGenerator* next)
      : ChainableTypeLayoutGenerator(next)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;
};

#endif // ROSE_ABISTUFF_H
