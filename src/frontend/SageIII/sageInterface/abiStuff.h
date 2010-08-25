#ifndef ROSE_ABISTUFF_H
#define ROSE_ABISTUFF_H


#include <vector>
#include <string>
#include <iosfwd>

//! Support for cross compilation or extended UPC support
/*! UPC data type sizes depend on a specified runtime implementation,
 *  So we allow users to optionally provide customized sizes and alignments. 
 */
struct StructCustomizedSizes
 {
    // optional values like x86, x86-64, ia64, sparcv9, sparcv8 etc.
    std::string str_abi; 
    //Primitive types: redundant if ABI is specified.  
    size_t sz_bool;
    size_t sz_alignof_bool;
    size_t sz_char;
    size_t sz_alignof_char;
    size_t sz_int;
    size_t sz_alignof_int;
    size_t sz_short;
    size_t sz_alignof_short;
    size_t sz_long;
    size_t sz_alignof_long;
    size_t sz_longlong;
    size_t sz_alignof_longlong;
    size_t sz_float;
    size_t sz_alignof_float;
    size_t sz_double;
    size_t sz_alignof_double;
    size_t sz_longdouble;
    size_t sz_alignof_longdouble;
    size_t sz_pointer; // memory handle
    size_t sz_alignof_pointer;
    size_t sz_reference; 
    size_t sz_alignof_reference;

   //Extended types beyond ABI's scope
    size_t sz_void_ptr;
    size_t sz_alignof_void_ptr;
    size_t sz_ptrdiff_t;
    size_t sz_alignof_ptrdiff_t;
    size_t sz_size_t;
    size_t sz_alignof_size_t;
    size_t sz_wchar;
    size_t sz_alignof_wchar;

    //UPC specified sizes
    size_t sz_shared_ptr; 
    size_t sz_alignof_shared_ptr;
    size_t sz_pshared_ptr; 
    size_t sz_alignof_pshared_ptr;
    size_t sz_mem_handle;
    size_t sz_alignof_mem_handle;
    size_t sz_reg_handle;
    size_t sz_alignof_reg_handle;

    size_t sz_alignof_dbl_1st;
    size_t sz_alignof_int64_1st;
    size_t sz_alignof_sharedptr_1st ;
    size_t sz_alignof_psharedptr_1st ;
    size_t sz_alignof_dbl_innerstruct;
    size_t sz_alignof_int64_innerstruct;
    size_t sz_alignof_sharedptr_innerstruct ;
    size_t sz_alignof_psharedptr_innerstruct;
    size_t sz_maxblocksz;
 };

struct StructLayoutEntry {
  //! If a SgInitializedName, the field represented by this entry
  //! If a SgClassDeclaration, the anonymous union represented by this entry
  //! If a SgBaseClass, the base class represented by this entry
  //! If NULL, this entry is padding
  SgNode* decl;
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

  StructLayoutEntry(SgNode* decl, size_t byteOffset, size_t fieldSize, size_t bitFieldContainerSize = 0, size_t bitOffset = 0):
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

//! Basic type layout engine -- handles bookkeeping, plus handing typedefs and
// modifiers
class ChainableTypeLayoutGenerator {
  public:
    virtual ~ChainableTypeLayoutGenerator() {}
    ChainableTypeLayoutGenerator* next;
    ChainableTypeLayoutGenerator* beginning;
    StructCustomizedSizes* custom_sizes;

    ChainableTypeLayoutGenerator(ChainableTypeLayoutGenerator* nx, StructCustomizedSizes* sizes=NULL)
#ifdef _MSC_VER
      : next(NULL), beginning(NULL), custom_sizes(sizes)
      {
     // DQ (11/27/2009): MSVC reports a warning when "this" is used in the preinitialization list.
                beginning = this;
                this->setNext(nx);
          }
#else
      : next(NULL), beginning(this), custom_sizes(sizes)
      {
		this->setNext(nx);
	  }
#endif

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

//! Layout generator for i386 ABI-like struct layout
// Handles structs and unions only
// Does not handle C++ stuff (inheritance, virtual functions) for now
class NonpackedTypeLayoutGenerator: public ChainableTypeLayoutGenerator {
  public:
  NonpackedTypeLayoutGenerator(ChainableTypeLayoutGenerator* next)
      : ChainableTypeLayoutGenerator(next)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;

  private:
  void layoutOneField(SgType* fieldType, SgNode* decl, bool isUnion /* Is type being laid out a union? */, size_t& currentOffset, StructLayoutInfo& layout) const;
};

//! Layout generator for i386 primitive types
class I386PrimitiveTypeLayoutGenerator: public ChainableTypeLayoutGenerator {
  public:
  I386PrimitiveTypeLayoutGenerator(ChainableTypeLayoutGenerator* next)
      : ChainableTypeLayoutGenerator(next)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;
};

//! Slight modification for Visual Studio -- doubles are 8-byte aligned
class I386_VSPrimitiveTypeLayoutGenerator: public I386PrimitiveTypeLayoutGenerator {
  public:
  I386_VSPrimitiveTypeLayoutGenerator(ChainableTypeLayoutGenerator* next)
      : I386PrimitiveTypeLayoutGenerator(next)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;
};

//! Layout generator for x86-64 primitive types
class X86_64PrimitiveTypeLayoutGenerator: public ChainableTypeLayoutGenerator {
  public:
  X86_64PrimitiveTypeLayoutGenerator(ChainableTypeLayoutGenerator* next)
      : ChainableTypeLayoutGenerator(next)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;
};

//! Slight modification for Visual Studio -- long is 4 bytes, not 8
class X86_64_VSPrimitiveTypeLayoutGenerator: public X86_64PrimitiveTypeLayoutGenerator {
  public:
  X86_64_VSPrimitiveTypeLayoutGenerator(ChainableTypeLayoutGenerator* next)
      : X86_64PrimitiveTypeLayoutGenerator(next)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;
};

//! Layout generator for the native system (uses sizeof)
class SystemPrimitiveTypeLayoutGenerator: public ChainableTypeLayoutGenerator {
  public:
  SystemPrimitiveTypeLayoutGenerator(ChainableTypeLayoutGenerator* next)
      : ChainableTypeLayoutGenerator(next)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;
};

//! Layout generator for customized primitive types, mostly for UPC relying on Berkeley runtime library now
class CustomizedPrimitiveTypeLayoutGenerator: public ChainableTypeLayoutGenerator {
  public:
  CustomizedPrimitiveTypeLayoutGenerator(ChainableTypeLayoutGenerator* next,StructCustomizedSizes* custom_sizes)
      : ChainableTypeLayoutGenerator(next,custom_sizes)
    {}
  virtual StructLayoutInfo layoutType(SgType* t) const;
};


#endif // ROSE_ABISTUFF_H
