// This file CilImpl.C has been generated from a JSON description file using json2rosetta.
//   Do not modify this file directly, but the corresponding JSON file.

/* CIL Implementation for Classes defined in src/ROSETTA/src/BinaryInstruction.C */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <unordered_map>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Cil.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <frontend/SageIII/sageInterface/SageBuilderAsm.h>

namespace sb = Rose::SageBuilderAsm;
using namespace Rose::Diagnostics; // mlog WARN, ...
using namespace Rose::BinaryAnalysis;

namespace // anonymous namespace for auxiliary functions
{
  constexpr bool TRACE_CONSTRUCTION = false;

  /// This function abstracts the details of reading a x-byte value from the disk image,
  /// where x == N / CHAR_BIT;
  template <class T>
  T readNBitValue(std::vector<uint8_t>& buf, size_t& index)
  {
     static constexpr int NUM_BYTES = sizeof(T);

     ROSE_ASSERT(index+NUM_BYTES <= buf.size());
     T value = Rose::BinaryAnalysis::ByteOrder::leToHost(*reinterpret_cast<T*>(buf.data()+index));

     index += NUM_BYTES;
     return value;
  }

  /// This function abstracts the details of reading a 1-byte values from the disk image.
  uint8_t read8bitValue(std::vector<uint8_t>& buf, size_t& index)
  {
     return readNBitValue<uint8_t>(buf, index);
  }

  /// This function abstracts the details of reading a 2-byte values from the disk image.
  uint16_t read16bitValue(std::vector<uint8_t>& buf, size_t& index)
  {
     return readNBitValue<uint16_t>(buf, index);
  }

  /// This function abstracts the details of reading a 4-byte values from the disk image.
  uint32_t read32bitValue(std::vector<uint8_t>& buf, size_t& index)
  {
     return readNBitValue<uint32_t>(buf, index);
  }

  /// This function abstracts the details of reading a 8-byte values from the disk image.
  uint64_t read64bitValue(std::vector<uint8_t>& buf, size_t& index)
  {
     return readNBitValue<uint64_t>(buf, index);
  }

  /// This function abstracts the details of reading either a 2-byte or 4-byte value.
  uint32_t readValue (std::vector<uint8_t>& buf, size_t& index, bool uses4byteIndexing)
  {
     // This function abstracts the details of reading 2 byte or 4 byte values from the disk image.
     return uses4byteIndexing ? read32bitValue(buf,index) : read16bitValue(buf,index);
  }

  /// This function abstracts the details of reading either a 2-byte or 4-byte value.
  std::string
  readString(std::vector<uint8_t>& buf, size_t& index, size_t maxLen = std::numeric_limits<size_t>::max())
  {
    std::string res;
    size_t      i = 0;

    while (char charValue = buf.at(index+i))
    {
      ROSE_ASSERT(i < maxLen);
      res += charValue;
      ++i;
    }

    ROSE_ASSERT(index + i <= buf.size());
    index += i;
    return res;
  }

  std::string
  readUtf8String(std::vector<uint8_t>& buf, size_t& index, size_t maxLen = std::numeric_limits<size_t>::max())
  {
    // \todo FIXME: read real Utf8 string
    return readString(buf, index, maxLen);
  }

  uint32_t
  readStringPadding(std::vector<uint8_t>& buf, size_t& index, size_t strLen, size_t reservedLen)
  {
    ROSE_ASSERT(strLen <= reservedLen);

    uint32_t res = 0;

    if (TRACE_CONSTRUCTION)
      std::cerr << "skip string padding of " << (reservedLen - strLen) << " bytes"
                << std::endl;
      
    while (strLen < reservedLen)
    {
      res = (res<<8) + buf.at(index);

      ROSE_ASSERT(index + 1 <= buf.size());
      ++index; ++strLen;
    }

    return res;
  }

  // convert chars to ints
  std::int32_t asValue(std::uint8_t val) { return val; }
  
  // do not convert other types
  template <class I>
  I asValue(I val) { return val; }

  template <class Reader>
  auto
  readExpected(Reader rd, std::vector<uint8_t>& buf, size_t& index, decltype(rd(buf, index)) expected) -> decltype(rd(buf, index))
  {
    using elem_type = decltype(rd(buf, index));

    elem_type res = rd(buf, index);

    if (res != expected)
    {
      mlog[INFO] << "unexpected read: expected " << asValue(expected) 
                 << ", got " << asValue(res) << "."
                 << std::endl;      
    }

    return res;
  }

  uint8_t read8bitPadding (std::vector<uint8_t>& buf, size_t& index, uint8_t expected)
  {
     return readExpected(read8bitValue, buf, index, expected);
  }

  template <class Reader>
  auto
  readVector(Reader rd, uint64_t num, std::vector<uint8_t>& buf, size_t &index) -> std::vector<decltype(rd(buf, index))>
  {
    using elem_type = decltype(rd(buf, index));

    std::vector<elem_type> res;

    if (TRACE_CONSTRUCTION)
      std::cerr << "Output the number of rows for each table: "
                << std::endl;

    for (uint64_t i = 0; i < num; ++i)
    {
      elem_type tmp_rows_value = rd(buf, index);
      res.push_back(tmp_rows_value);
      
      if (TRACE_CONSTRUCTION)
        std::cerr << "--- table " << i << ": tmp_rows_value = " << tmp_rows_value
                  << std::endl;
    }

    return res;
  }

  struct StreamHeader : std::tuple<uint32_t, uint32_t, std::string, uint32_t>
  {
    using base = std::tuple<uint32_t, uint32_t, std::string, uint32_t>;
    using base::base;

    uint32_t           offset()      const { return std::get<0>(*this); }
    uint32_t           size()        const { return std::get<1>(*this); }
    const std::string& name()        const { return std::get<2>(*this); }
    uint32_t           namePadding() const { return std::get<3>(*this); }

    static
    StreamHeader parse(std::vector<uint8_t>& buf, size_t& index);
  };

  StreamHeader
  StreamHeader::parse(std::vector<uint8_t>& buf, size_t& index)
  {
    uint32_t offset      = read32bitValue(buf, index);
    uint32_t size        = read32bitValue(buf, index);
    std::string name     = readString(buf, index);
    uint32_t namePadding = readStringPadding(buf, index, name.size(), ((name.size() + 4) / 4) * 4);

    return StreamHeader{offset, size, std::move(name), namePadding};
  }


  std::vector<SgAsmCilDataStream*>
  parseStreams(SgAsmCilMetadataRoot* parent, std::vector<uint8_t>& buf, size_t& index, size_t start_of_MetadataRoot, uint16_t numberOfStreams)
  {
    std::vector<SgAsmCilDataStream*> res;

    for (size_t i = 0; i < numberOfStreams; ++i)
    {
      if (TRACE_CONSTRUCTION) {
        std::cerr << "START: stream header " << i << " of " << numberOfStreams << ": index = " << index
                  << std::endl;
      }
      
      StreamHeader        streamHeader = StreamHeader::parse(buf,index);
      SgAsmCilDataStream* dataStream = nullptr;
      
      if (TRACE_CONSTRUCTION) {
        std::cerr << ": name is " << streamHeader.name() << "\n";
      }      

      if (  (SgAsmCilDataStream::ID_STRING_HEAP == streamHeader.name())
         || (SgAsmCilDataStream::ID_BLOB_HEAP   == streamHeader.name())
         || (SgAsmCilDataStream::ID_US_HEAP     == streamHeader.name()) // \todo should be a Utf16 Stream
         )
        dataStream = new SgAsmCilUint8Heap(streamHeader.offset(), streamHeader.size(), streamHeader.name(), streamHeader.namePadding());
      else if (SgAsmCilDataStream::ID_GUID_HEAP == streamHeader.name()) // \todo should be a Uint128? stream
        dataStream = new SgAsmCilUint32Heap(streamHeader.offset(), streamHeader.size(), streamHeader.name(), streamHeader.namePadding());
      else if (SgAsmCilDataStream::ID_METADATA_HEAP == streamHeader.name())
        dataStream = new SgAsmCilMetadataHeap(streamHeader.offset(), streamHeader.size(), streamHeader.name(), streamHeader.namePadding());

      ASSERT_not_null(dataStream);

      dataStream->set_parent(parent);
      dataStream->parse(buf, start_of_MetadataRoot);
      res.push_back(dataStream);

      if (TRACE_CONSTRUCTION)
        std::cerr << "END: stream header " << i << " of " << numberOfStreams << ": index = " << index
                  << std::endl;
    }

    return res;
  }

  template <class SageAsmCilNode, class SageAsmCilMetadataTable>
  SageAsmCilNode*
  parseAsmCilNode(SageAsmCilMetadataTable* parent, std::vector<uint8_t>& buf, size_t& index, uint64_t dataSizeflags)
  {
      SageAsmCilNode* res = new SageAsmCilNode;
      ASSERT_not_null(res);

      res->set_parent(parent);
      // ::mlog[INFO] << "Creating " << res->class_name() << " node."
      //              << std::endl;

      if (TRACE_CONSTRUCTION)
        std::cerr << "Creating " << res->class_name() << " node at " << index
                  << std::endl;

      res->parse(buf, index, dataSizeflags);
      return res;
  }
  
  template <class IntT>
  std::vector<IntT>
  parseIntStream(std::vector<uint8_t>& buf, size_t start, size_t ofs, size_t len)
  {
    const size_t pos = start + ofs;
    const IntT*  beg = reinterpret_cast<IntT*>(buf.data() + pos);
    const IntT*  lim = beg + (len / sizeof(IntT));

    ROSE_ASSERT(len % sizeof(IntT) == 0);
    return std::vector<IntT>(beg, lim);
  }
  
  inline
  const SgAsmCilMetadataHeap&
  getMetadataHeap(const SgAsmCilMetadata* o)
  {
    ASSERT_not_null(o);
    
    const SgNode* metadataTable = o->get_parent();
    ASSERT_not_null(metadataTable);
    
    const SgAsmCilMetadataHeap* res = isSgAsmCilMetadataHeap(metadataTable->get_parent());
    ASSERT_not_null(res);
    
    return *res;
  }
  
  inline
  SgAsmCilMetadataHeap&
  getMetadataHeap(SgAsmCilMetadata* o)
  {
    ASSERT_not_null(o);
    
    SgNode* metadataTable = o->get_parent();
    ASSERT_not_null(metadataTable);
    
    SgAsmCilMetadataHeap* res = isSgAsmCilMetadataHeap(metadataTable->get_parent());
    ASSERT_not_null(res);
    
    return *res;
  }
  
  inline
  const SgAsmCilMetadataRoot&
  getMetadataRoot(const SgAsmCilMetadata* o)
  {
    ROSE_ASSERT(o);
    const SgAsmCilMetadataRoot* res = isSgAsmCilMetadataRoot(getMetadataHeap(o).get_parent());
    
    ROSE_ASSERT(res);
    return *res;
  }
  
  inline
  SgAsmCilMetadataRoot&
  getMetadataRoot(SgAsmCilMetadata* o)
  {
    ROSE_ASSERT(o);
    SgAsmCilMetadataRoot* res = isSgAsmCilMetadataRoot(getMetadataHeap(o).get_parent());
    
    ROSE_ASSERT(res);
    return *res;
  }
}



void SgAsmCilAssembly::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_HashAlgId = read32bitValue(buf,index);
  p_MajorVersion = read16bitValue(buf,index);
  p_MinorVersion = read16bitValue(buf,index);
  p_BuildNumber = read16bitValue(buf,index);
  p_RevisionNumber = read16bitValue(buf,index);
  p_Flags = read32bitValue(buf,index);
  p_PublicKey = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Culture = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_HashAlgId = " << p_HashAlgId << std::endl;
    std::cerr << "p_MajorVersion = " << p_MajorVersion << std::endl;
    std::cerr << "p_MinorVersion = " << p_MinorVersion << std::endl;
    std::cerr << "p_BuildNumber = " << p_BuildNumber << std::endl;
    std::cerr << "p_RevisionNumber = " << p_RevisionNumber << std::endl;
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_PublicKey = " << p_PublicKey << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_Culture = " << p_Culture << std::endl;
  }
}
        
const std::uint8_t*
SgAsmCilAssembly::get_PublicKey_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_PublicKey();
} 
      
const std::uint8_t*
SgAsmCilAssembly::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const std::uint8_t*
SgAsmCilAssembly::get_Culture_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Culture();
} 
      

void SgAsmCilAssemblyOS::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_OSPlatformID = read32bitValue(buf,index);
  p_OSMajorVersion = read32bitValue(buf,index);
  p_OSMinorVersion = read32bitValue(buf,index);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_OSPlatformID = " << p_OSPlatformID << std::endl;
    std::cerr << "p_OSMajorVersion = " << p_OSMajorVersion << std::endl;
    std::cerr << "p_OSMinorVersion = " << p_OSMinorVersion << std::endl;
  }
}
     

void SgAsmCilAssemblyProcessor::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Processor = read32bitValue(buf,index);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Processor = " << p_Processor << std::endl;
  }
}
   

void SgAsmCilAssemblyRef::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_MajorVersion = read16bitValue(buf,index);
  p_MinorVersion = read16bitValue(buf,index);
  p_BuildNumber = read16bitValue(buf,index);
  p_RevisionNumber = read16bitValue(buf,index);
  p_Flags = read32bitValue(buf,index);
  p_PublicKeyOrToken = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Culture = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_HashValue = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_MajorVersion = " << p_MajorVersion << std::endl;
    std::cerr << "p_MinorVersion = " << p_MinorVersion << std::endl;
    std::cerr << "p_BuildNumber = " << p_BuildNumber << std::endl;
    std::cerr << "p_RevisionNumber = " << p_RevisionNumber << std::endl;
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_PublicKeyOrToken = " << p_PublicKeyOrToken << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_Culture = " << p_Culture << std::endl;
    std::cerr << "p_HashValue = " << p_HashValue << std::endl;
  }
}
       
const std::uint8_t*
SgAsmCilAssemblyRef::get_PublicKeyOrToken_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_PublicKeyOrToken();
} 
      
const std::uint8_t*
SgAsmCilAssemblyRef::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const std::uint8_t*
SgAsmCilAssemblyRef::get_Culture_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Culture();
} 
      
const std::uint8_t*
SgAsmCilAssemblyRef::get_HashValue_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_HashValue();
} 
      

void SgAsmCilAssemblyRefOS::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_OSPlatformID = read32bitValue(buf,index);
  p_OSMajorVersion = read32bitValue(buf,index);
  p_OSMinorVersion = read32bitValue(buf,index);
  p_AssemblyRefOS = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_assembly_ref);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_OSPlatformID = " << p_OSPlatformID << std::endl;
    std::cerr << "p_OSMajorVersion = " << p_OSMajorVersion << std::endl;
    std::cerr << "p_OSMinorVersion = " << p_OSMinorVersion << std::endl;
    std::cerr << "p_AssemblyRefOS = " << p_AssemblyRefOS << std::endl;
  }
}
     
const SgAsmCilMetadata*
SgAsmCilAssemblyRefOS::get_AssemblyRefOS_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_AssemblyRefOS(), SgAsmCilMetadataHeap::e_ref_assembly_ref);
}     
      

void SgAsmCilAssemblyRefProcessor::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Processor = read32bitValue(buf,index);
  p_AssemblyRef = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_assembly_ref);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Processor = " << p_Processor << std::endl;
    std::cerr << "p_AssemblyRef = " << p_AssemblyRef << std::endl;
  }
}
   
const SgAsmCilMetadata*
SgAsmCilAssemblyRefProcessor::get_AssemblyRef_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_AssemblyRef(), SgAsmCilMetadataHeap::e_ref_assembly_ref);
}     
      

void SgAsmCilClassLayout::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_PackingSize = read16bitValue(buf,index);
  p_ClassSize = read32bitValue(buf,index);
  p_Parent = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_PackingSize = " << p_PackingSize << std::endl;
    std::cerr << "p_ClassSize = " << p_ClassSize << std::endl;
    std::cerr << "p_Parent = " << p_Parent << std::endl;
  }
}
    
const SgAsmCilMetadata*
SgAsmCilClassLayout::get_Parent_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Parent(), SgAsmCilMetadataHeap::e_ref_type_def);
}     
      

void SgAsmCilConstant::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Type = read8bitValue(buf,index);
  p_Padding = read8bitPadding(buf,index,0);
  p_Parent = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_has_constant);
  p_Value = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Type = " << p_Type << std::endl;
    std::cerr << "p_Padding = " << p_Padding << std::endl;
    std::cerr << "p_Parent = " << p_Parent << std::endl;
    std::cerr << "p_Value = " << p_Value << std::endl;
  }
}
    
const SgAsmCilMetadata*
SgAsmCilConstant::get_Parent_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Parent(), SgAsmCilMetadataHeap::e_ref_has_constant);
}     
      
const std::uint8_t*
SgAsmCilConstant::get_Value_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Value();
} 
      

void SgAsmCilCustomAttribute::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Parent = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_has_custom_attribute);
  p_Type = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_custom_attribute_type);
  p_Value = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Parent = " << p_Parent << std::endl;
    std::cerr << "p_Type = " << p_Type << std::endl;
    std::cerr << "p_Value = " << p_Value << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilCustomAttribute::get_Parent_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Parent(), SgAsmCilMetadataHeap::e_ref_has_custom_attribute);
}     
      
const SgAsmCilMetadata*
SgAsmCilCustomAttribute::get_Type_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Type(), SgAsmCilMetadataHeap::e_ref_custom_attribute_type);
}     
      
const std::uint8_t*
SgAsmCilCustomAttribute::get_Value_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Value();
} 
      

void SgAsmCilDeclSecurity::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Action = read16bitValue(buf,index);
  p_Parent = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_has_decl_security);
  p_PermissionSet = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Action = " << p_Action << std::endl;
    std::cerr << "p_Parent = " << p_Parent << std::endl;
    std::cerr << "p_PermissionSet = " << p_PermissionSet << std::endl;
  }
}
   
const SgAsmCilMetadata*
SgAsmCilDeclSecurity::get_Parent_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Parent(), SgAsmCilMetadataHeap::e_ref_has_decl_security);
}     
      
const std::uint8_t*
SgAsmCilDeclSecurity::get_PermissionSet_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_PermissionSet();
} 
      

void SgAsmCilEvent::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_EventFlags = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_EventType = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def_or_ref);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_EventFlags = " << p_EventFlags << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_EventType = " << p_EventType << std::endl;
  }
}
   
const std::uint8_t*
SgAsmCilEvent::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const SgAsmCilMetadata*
SgAsmCilEvent::get_EventType_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_EventType(), SgAsmCilMetadataHeap::e_ref_type_def_or_ref);
}     
      

void SgAsmCilEventMap::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Parent = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def);
  p_EventList = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_event);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Parent = " << p_Parent << std::endl;
    std::cerr << "p_EventList = " << p_EventList << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilEventMap::get_Parent_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Parent(), SgAsmCilMetadataHeap::e_ref_type_def);
}     
      
const SgAsmCilMetadata*
SgAsmCilEventMap::get_EventList_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_EventList(), SgAsmCilMetadataHeap::e_ref_event);
}     
      

void SgAsmCilExportedType::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_EventFlags = read32bitValue(buf,index);
  p_TypeDefIdName = read32bitValue(buf,index);
  p_TypeName = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_TypeNamespace = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Implementation = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_implementation);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_EventFlags = " << p_EventFlags << std::endl;
    std::cerr << "p_TypeDefIdName = " << p_TypeDefIdName << std::endl;
    std::cerr << "p_TypeName = " << p_TypeName << std::endl;
    std::cerr << "p_TypeNamespace = " << p_TypeNamespace << std::endl;
    std::cerr << "p_Implementation = " << p_Implementation << std::endl;
  }
}
    
const std::uint8_t*
SgAsmCilExportedType::get_TypeName_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_TypeName();
} 
      
const std::uint8_t*
SgAsmCilExportedType::get_TypeNamespace_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_TypeNamespace();
} 
      
const SgAsmCilMetadata*
SgAsmCilExportedType::get_Implementation_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Implementation(), SgAsmCilMetadataHeap::e_ref_implementation);
}     
      

void SgAsmCilField::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Flags = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Signature = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_Signature = " << p_Signature << std::endl;
  }
}
   
const std::uint8_t*
SgAsmCilField::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const std::uint8_t*
SgAsmCilField::get_Signature_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Signature();
} 
      

void SgAsmCilFieldLayout::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Offset = read32bitValue(buf,index);
  p_Field = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_field);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Offset = " << p_Offset << std::endl;
    std::cerr << "p_Field = " << p_Field << std::endl;
  }
}
   
const SgAsmCilMetadata*
SgAsmCilFieldLayout::get_Field_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Field(), SgAsmCilMetadataHeap::e_ref_field);
}     
      

void SgAsmCilFieldMarshal::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Parent = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_has_field_marshall);
  p_NativeType = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Parent = " << p_Parent << std::endl;
    std::cerr << "p_NativeType = " << p_NativeType << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilFieldMarshal::get_Parent_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Parent(), SgAsmCilMetadataHeap::e_ref_has_field_marshall);
}     
      
const std::uint8_t*
SgAsmCilFieldMarshal::get_NativeType_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_NativeType();
} 
      

void SgAsmCilFieldRVA::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_RVA = read16bitValue(buf,index);
  p_Field = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_field);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_RVA = " << p_RVA << std::endl;
    std::cerr << "p_Field = " << p_Field << std::endl;
  }
}
   
const SgAsmCilMetadata*
SgAsmCilFieldRVA::get_Field_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Field(), SgAsmCilMetadataHeap::e_ref_field);
}     
      

void SgAsmCilFile::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Flags = read32bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_HashValue = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_HashValue = " << p_HashValue << std::endl;
  }
}
   
const std::uint8_t*
SgAsmCilFile::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const std::uint8_t*
SgAsmCilFile::get_HashValue_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_HashValue();
} 
      

void SgAsmCilGenericParam::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Number = read16bitValue(buf,index);
  p_Flags = read16bitValue(buf,index);
  p_Owner = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_or_method_def);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Number = " << p_Number << std::endl;
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_Owner = " << p_Owner << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
  }
}
    
const SgAsmCilMetadata*
SgAsmCilGenericParam::get_Owner_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Owner(), SgAsmCilMetadataHeap::e_ref_type_or_method_def);
}     
      
const std::uint8_t*
SgAsmCilGenericParam::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      

void SgAsmCilGenericParamConstraint::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Owner = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_generic_param);
  p_Constraint = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def_or_ref);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Owner = " << p_Owner << std::endl;
    std::cerr << "p_Constraint = " << p_Constraint << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilGenericParamConstraint::get_Owner_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Owner(), SgAsmCilMetadataHeap::e_ref_generic_param);
}     
      
const SgAsmCilMetadata*
SgAsmCilGenericParamConstraint::get_Constraint_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Constraint(), SgAsmCilMetadataHeap::e_ref_type_def_or_ref);
}     
      

void SgAsmCilImplMap::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_MappingFlags = read16bitValue(buf,index);
  p_MemberForwarded = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_member_forwarded);
  p_ImportName = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_ImportScope = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_module_ref);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_MappingFlags = " << p_MappingFlags << std::endl;
    std::cerr << "p_MemberForwarded = " << p_MemberForwarded << std::endl;
    std::cerr << "p_ImportName = " << p_ImportName << std::endl;
    std::cerr << "p_ImportScope = " << p_ImportScope << std::endl;
  }
}
   
const SgAsmCilMetadata*
SgAsmCilImplMap::get_MemberForwarded_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_MemberForwarded(), SgAsmCilMetadataHeap::e_ref_member_forwarded);
}     
      
const std::uint8_t*
SgAsmCilImplMap::get_ImportName_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_ImportName();
} 
      
const SgAsmCilMetadata*
SgAsmCilImplMap::get_ImportScope_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_ImportScope(), SgAsmCilMetadataHeap::e_ref_module_ref);
}     
      

void SgAsmCilInterfaceImpl::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Class = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def);
  p_Interface = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def_or_ref);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Class = " << p_Class << std::endl;
    std::cerr << "p_Interface = " << p_Interface << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilInterfaceImpl::get_Class_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Class(), SgAsmCilMetadataHeap::e_ref_type_def);
}     
      
const SgAsmCilMetadata*
SgAsmCilInterfaceImpl::get_Interface_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Interface(), SgAsmCilMetadataHeap::e_ref_type_def_or_ref);
}     
      

void SgAsmCilManifestResource::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Offset = read32bitValue(buf,index);
  p_Flags = read32bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Implementation = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_implementation);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Offset = " << p_Offset << std::endl;
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_Implementation = " << p_Implementation << std::endl;
  }
}
    
const std::uint8_t*
SgAsmCilManifestResource::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const SgAsmCilMetadata*
SgAsmCilManifestResource::get_Implementation_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Implementation(), SgAsmCilMetadataHeap::e_ref_implementation);
}     
      

void SgAsmCilMemberRef::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Class = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_member_ref_parent);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Signature = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Class = " << p_Class << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_Signature = " << p_Signature << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilMemberRef::get_Class_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Class(), SgAsmCilMetadataHeap::e_ref_member_ref_parent);
}     
      
const std::uint8_t*
SgAsmCilMemberRef::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const std::uint8_t*
SgAsmCilMemberRef::get_Signature_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Signature();
} 
      

void SgAsmCilMethodDef::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_RVA = read32bitValue(buf,index);
  p_ImplFlags = read16bitValue(buf,index);
  p_Flags = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Signature = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);
  p_ParamList = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_param);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_RVA = " << p_RVA << std::endl;
    std::cerr << "p_ImplFlags = " << p_ImplFlags << std::endl;
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_Signature = " << p_Signature << std::endl;
    std::cerr << "p_ParamList = " << p_ParamList << std::endl;
  }
}
     
const std::uint8_t*
SgAsmCilMethodDef::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const std::uint8_t*
SgAsmCilMethodDef::get_Signature_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Signature();
} 
      
const SgAsmCilMetadata*
SgAsmCilMethodDef::get_ParamList_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_ParamList(), SgAsmCilMetadataHeap::e_ref_param);
}     
      

void SgAsmCilMethodImpl::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Class = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def);
  p_MethodBody = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_method_def_or_ref);
  p_MethodDeclaration = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_method_def_or_ref);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Class = " << p_Class << std::endl;
    std::cerr << "p_MethodBody = " << p_MethodBody << std::endl;
    std::cerr << "p_MethodDeclaration = " << p_MethodDeclaration << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilMethodImpl::get_Class_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Class(), SgAsmCilMetadataHeap::e_ref_type_def);
}     
      
const SgAsmCilMetadata*
SgAsmCilMethodImpl::get_MethodBody_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_MethodBody(), SgAsmCilMetadataHeap::e_ref_method_def_or_ref);
}     
      
const SgAsmCilMetadata*
SgAsmCilMethodImpl::get_MethodDeclaration_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_MethodDeclaration(), SgAsmCilMetadataHeap::e_ref_method_def_or_ref);
}     
      

void SgAsmCilMethodSemantics::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Semantics = read16bitValue(buf,index);
  p_Method = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_method_def);
  p_Association = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_has_semantics);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Semantics = " << p_Semantics << std::endl;
    std::cerr << "p_Method = " << p_Method << std::endl;
    std::cerr << "p_Association = " << p_Association << std::endl;
  }
}
   
const SgAsmCilMetadata*
SgAsmCilMethodSemantics::get_Method_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Method(), SgAsmCilMetadataHeap::e_ref_method_def);
}     
      
const SgAsmCilMetadata*
SgAsmCilMethodSemantics::get_Association_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Association(), SgAsmCilMetadataHeap::e_ref_has_semantics);
}     
      

void SgAsmCilMethodSpec::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Method = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_method_def_or_ref);
  p_Instantiation = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Method = " << p_Method << std::endl;
    std::cerr << "p_Instantiation = " << p_Instantiation << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilMethodSpec::get_Method_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Method(), SgAsmCilMetadataHeap::e_ref_method_def_or_ref);
}     
      
const std::uint8_t*
SgAsmCilMethodSpec::get_Instantiation_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Instantiation();
} 
      

void SgAsmCilModule::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Generation = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Mvid = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_guid_heap);
  p_Encld = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_guid_heap);
  p_EncBaseId = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_guid_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Generation = " << p_Generation << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_Mvid = " << p_Mvid << std::endl;
    std::cerr << "p_Encld = " << p_Encld << std::endl;
    std::cerr << "p_EncBaseId = " << p_EncBaseId << std::endl;
  }
}
   
const std::uint8_t*
SgAsmCilModule::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const std::uint32_t*
SgAsmCilModule::get_Mvid_guid() const
{
  return getMetadataRoot(this).get_GuidHeap()->get_Stream().data() + get_Mvid();
} 
      
const std::uint32_t*
SgAsmCilModule::get_Encld_guid() const
{
  return getMetadataRoot(this).get_GuidHeap()->get_Stream().data() + get_Encld();
} 
      
const std::uint32_t*
SgAsmCilModule::get_EncBaseId_guid() const
{
  return getMetadataRoot(this).get_GuidHeap()->get_Stream().data() + get_EncBaseId();
} 
      

void SgAsmCilModuleRef::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Name = " << p_Name << std::endl;
  }
}
  
const std::uint8_t*
SgAsmCilModuleRef::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      

void SgAsmCilNestedClass::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_NestedClass = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def);
  p_EnclosingClass = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_NestedClass = " << p_NestedClass << std::endl;
    std::cerr << "p_EnclosingClass = " << p_EnclosingClass << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilNestedClass::get_NestedClass_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_NestedClass(), SgAsmCilMetadataHeap::e_ref_type_def);
}     
      
const SgAsmCilMetadata*
SgAsmCilNestedClass::get_EnclosingClass_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_EnclosingClass(), SgAsmCilMetadataHeap::e_ref_type_def);
}     
      

void SgAsmCilParam::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Flags = read16bitValue(buf,index);
  p_Sequence = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_Sequence = " << p_Sequence << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
  }
}
    
const std::uint8_t*
SgAsmCilParam::get_Name_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Name();
} 
      

void SgAsmCilProperty::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Flags = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Type = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_Name = " << p_Name << std::endl;
    std::cerr << "p_Type = " << p_Type << std::endl;
  }
}
   
const std::uint8_t*
SgAsmCilProperty::get_Name_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_Name();
} 
      
const std::uint8_t*
SgAsmCilProperty::get_Type_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Type();
} 
      

void SgAsmCilPropertyMap::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Parent = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def);
  p_PropertyList = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_property);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Parent = " << p_Parent << std::endl;
    std::cerr << "p_PropertyList = " << p_PropertyList << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilPropertyMap::get_Parent_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Parent(), SgAsmCilMetadataHeap::e_ref_type_def);
}     
      
const SgAsmCilMetadata*
SgAsmCilPropertyMap::get_PropertyList_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_PropertyList(), SgAsmCilMetadataHeap::e_ref_property);
}     
      

void SgAsmCilStandAloneSig::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Signature = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Signature = " << p_Signature << std::endl;
  }
}
  
const std::uint8_t*
SgAsmCilStandAloneSig::get_Signature_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Signature();
} 
      

void SgAsmCilTypeDef::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Flags = read32bitValue(buf,index);
  p_TypeName = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_TypeNamespace = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_Extends = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def_or_ref);
  p_FieldList = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_field);
  p_MethodList = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_method_def);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Flags = " << p_Flags << std::endl;
    std::cerr << "p_TypeName = " << p_TypeName << std::endl;
    std::cerr << "p_TypeNamespace = " << p_TypeNamespace << std::endl;
    std::cerr << "p_Extends = " << p_Extends << std::endl;
    std::cerr << "p_FieldList = " << p_FieldList << std::endl;
    std::cerr << "p_MethodList = " << p_MethodList << std::endl;
  }
}
   
const std::uint8_t*
SgAsmCilTypeDef::get_TypeName_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_TypeName();
} 
      
const std::uint8_t*
SgAsmCilTypeDef::get_TypeNamespace_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_TypeNamespace();
} 
      
const SgAsmCilMetadata*
SgAsmCilTypeDef::get_Extends_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_Extends(), SgAsmCilMetadataHeap::e_ref_type_def_or_ref);
}     
      
const SgAsmCilMetadata*
SgAsmCilTypeDef::get_FieldList_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_FieldList(), SgAsmCilMetadataHeap::e_ref_field);
}     
      
const SgAsmCilMetadata*
SgAsmCilTypeDef::get_MethodList_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_MethodList(), SgAsmCilMetadataHeap::e_ref_method_def);
}     
      
const SgAsmCilMetadata*
SgAsmCilTypeDef::get_MethodList_object(const SgAsmCilMethodDef* methodDef) const
{
  // Also need to know the index of the next typedef (can I get my index, probably not)?
  uint32_t methodListIndex{get_MethodList()};
  uint32_t nextMethodListIndex{methodListIndex};

  // Need to find current first?
  uint32_t myTypeDefIndex{0};

  auto mdh = getMetadataHeap(this);
  SgAsmCilTypeDefTable* tdt = mdh.get_TypeDefTable();
  auto types = tdt->get_elements();
  for (uint32_t ii = 0; ii < types.size(); ii++) {
    myTypeDefIndex = ii;
    auto type = types[ii];
    if (this == type) break;
  }

  uint32_t nextTypeDefIndex = myTypeDefIndex + 1;
  if (nextTypeDefIndex < types.size()) {
    nextMethodListIndex = types[nextTypeDefIndex]->get_MethodList();
  }

  for (uint32_t ii = methodListIndex; ii < nextMethodListIndex; ii++) {
    auto md = mdh.get_CodedMetadataNode(ii, SgAsmCilMetadataHeap::e_ref_method_def);
    if (md == methodDef) return md;
  }

  // It is not required that an object be found
  return nullptr;
}


void SgAsmCilTypeRef::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_ResolutionScope = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_resolution_scope);
  p_TypeName = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
  p_TypeNamespace = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_ResolutionScope = " << p_ResolutionScope << std::endl;
    std::cerr << "p_TypeName = " << p_TypeName << std::endl;
    std::cerr << "p_TypeNamespace = " << p_TypeNamespace << std::endl;
  }
}
  
const SgAsmCilMetadata*
SgAsmCilTypeRef::get_ResolutionScope_object() const
{
  return getMetadataHeap(this).get_CodedMetadataNode(get_ResolutionScope(), SgAsmCilMetadataHeap::e_ref_resolution_scope);
}     
      
const std::uint8_t*
SgAsmCilTypeRef::get_TypeName_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_TypeName();
} 
      
const std::uint8_t*
SgAsmCilTypeRef::get_TypeNamespace_string() const
{
  return getMetadataRoot(this).get_StringHeap()->get_Stream().data() + get_TypeNamespace();
} 
      

void SgAsmCilTypeSpec::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
{ 
  p_Signature = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

  if (TRACE_CONSTRUCTION)
  { 
    std::cerr << "p_Signature = " << p_Signature << std::endl;
  }
}
  
const std::uint8_t*
SgAsmCilTypeSpec::get_Signature_blob() const
{
  return getMetadataRoot(this).get_BlobHeap()->get_Stream().data() + get_Signature();
} 
       

void SgAsmCilUint8Heap::parse(std::vector<uint8_t>& buf, size_t startOfMetaData)
{
  p_Stream = parseIntStream<uint8_t>(buf, startOfMetaData, get_Offset(), get_Size());
}

void SgAsmCilUint32Heap::parse(std::vector<uint8_t>& buf, size_t startOfMetaData)
{
  p_Stream = parseIntStream<uint32_t>(buf, startOfMetaData, get_Offset(), get_Size());
}

const std::string SgAsmCilDataStream::ID_STRING_HEAP{"#Strings"};
const std::string SgAsmCilDataStream::ID_BLOB_HEAP{"#Blob"};
const std::string SgAsmCilDataStream::ID_US_HEAP{"#US"};
const std::string SgAsmCilDataStream::ID_GUID_HEAP{"#GUID"};
const std::string SgAsmCilDataStream::ID_METADATA_HEAP{"#~"};



//
// MetadataHeap


namespace
{
const char*
table_kind_to_string ( SgAsmCilMetadataHeap::TableKind e )
{
  const char* res = nullptr;

  switch (e)
  {

    case SgAsmCilMetadataHeap::e_Assembly: res = "e_Assembly"; break;
    case SgAsmCilMetadataHeap::e_AssemblyOS: res = "e_AssemblyOS"; break;
    case SgAsmCilMetadataHeap::e_AssemblyProcessor: res = "e_AssemblyProcessor"; break;
    case SgAsmCilMetadataHeap::e_AssemblyRef: res = "e_AssemblyRef"; break;
    case SgAsmCilMetadataHeap::e_AssemblyRefOS: res = "e_AssemblyRefOS"; break;
    case SgAsmCilMetadataHeap::e_AssemblyRefProcessor: res = "e_AssemblyRefProcessor"; break;
    case SgAsmCilMetadataHeap::e_ClassLayout: res = "e_ClassLayout"; break;
    case SgAsmCilMetadataHeap::e_Constant: res = "e_Constant"; break;
    case SgAsmCilMetadataHeap::e_CustomAttribute: res = "e_CustomAttribute"; break;
    case SgAsmCilMetadataHeap::e_DeclSecurity: res = "e_DeclSecurity"; break;
    case SgAsmCilMetadataHeap::e_Event: res = "e_Event"; break;
    case SgAsmCilMetadataHeap::e_EventMap: res = "e_EventMap"; break;
    case SgAsmCilMetadataHeap::e_ExportedType: res = "e_ExportedType"; break;
    case SgAsmCilMetadataHeap::e_Field: res = "e_Field"; break;
    case SgAsmCilMetadataHeap::e_FieldLayout: res = "e_FieldLayout"; break;
    case SgAsmCilMetadataHeap::e_FieldMarshal: res = "e_FieldMarshal"; break;
    case SgAsmCilMetadataHeap::e_FieldRVA: res = "e_FieldRVA"; break;
    case SgAsmCilMetadataHeap::e_File: res = "e_File"; break;
    case SgAsmCilMetadataHeap::e_GenericParam: res = "e_GenericParam"; break;
    case SgAsmCilMetadataHeap::e_GenericParamConstraint: res = "e_GenericParamConstraint"; break;
    case SgAsmCilMetadataHeap::e_ImplMap: res = "e_ImplMap"; break;
    case SgAsmCilMetadataHeap::e_InterfaceImpl: res = "e_InterfaceImpl"; break;
    case SgAsmCilMetadataHeap::e_ManifestResource: res = "e_ManifestResource"; break;
    case SgAsmCilMetadataHeap::e_MemberRef: res = "e_MemberRef"; break;
    case SgAsmCilMetadataHeap::e_MethodDef: res = "e_MethodDef"; break;
    case SgAsmCilMetadataHeap::e_MethodImpl: res = "e_MethodImpl"; break;
    case SgAsmCilMetadataHeap::e_MethodSemantics: res = "e_MethodSemantics"; break;
    case SgAsmCilMetadataHeap::e_MethodSpec: res = "e_MethodSpec"; break;
    case SgAsmCilMetadataHeap::e_Module: res = "e_Module"; break;
    case SgAsmCilMetadataHeap::e_ModuleRef: res = "e_ModuleRef"; break;
    case SgAsmCilMetadataHeap::e_NestedClass: res = "e_NestedClass"; break;
    case SgAsmCilMetadataHeap::e_Param: res = "e_Param"; break;
    case SgAsmCilMetadataHeap::e_Property: res = "e_Property"; break;
    case SgAsmCilMetadataHeap::e_PropertyMap: res = "e_PropertyMap"; break;
    case SgAsmCilMetadataHeap::e_StandAloneSig: res = "e_StandAloneSig"; break;
    case SgAsmCilMetadataHeap::e_TypeDef: res = "e_TypeDef"; break;
    case SgAsmCilMetadataHeap::e_TypeRef: res = "e_TypeRef"; break;
    case SgAsmCilMetadataHeap::e_TypeSpec: res = "e_TypeSpec"; break;

    default:
      ROSE_ABORT();
   }

  ASSERT_not_null(res);
  return res;
}

}



namespace 
{

std::vector<int8_t>
computePositionInRowVector(uint64_t valid)
{
  constexpr int NUM_BITS = sizeof(valid) * CHAR_BIT;

  std::vector<int8_t> res;
  const int           numValidBits = Rose::BitOps::nSet(valid);
  int                 ctr = -1;

  res.reserve(NUM_BITS);
  for (int i = 0; i < NUM_BITS; ++i)
  {
    int8_t idx = (valid & 1) ? ++ctr : -1;

    valid >>= 1;
    res.push_back(idx);
  }

  //~ std::cerr << ctr << " ctr <? " << numValidBits << std::endl;
  ROSE_ASSERT(ctr < numValidBits);
  return res;
}

/// computes number of bits required to store the table identifier
///   ii.24.2.6
uint64_t
calcTableIdentifierSize(const std::vector<SgAsmCilMetadataHeap::TableKind>& tables)
{
  const size_t tblsz = tables.size();
  ROSE_ASSERT(tblsz > 0);

  auto hiBit = Rose::BitOps::highestSetBit(tblsz-1);

  return hiBit ? (*hiBit) + 1 : 0;
}

uint32_t
calcMaxTableSize( const std::vector<uint32_t>& numberOfRows,
                  const std::vector<int8_t>& posInRowVector,
                  const std::vector<SgAsmCilMetadataHeap::TableKind>& tables
                )
{
  uint32_t max = 0;

  for (SgAsmCilMetadataHeap::TableKind tbl : tables)
  {
    if (tbl == SgAsmCilMetadataHeap::e_Unknown_table_kind) continue;

    const int8_t posInRowVec = posInRowVector.at(tbl);
    if (posInRowVec < 0) continue;

    const uint32_t numEntries = numberOfRows.at(posInRowVec);
    if (max < numEntries) max = numEntries;
  }

  return max;
}


const std::unordered_map<std::uint64_t, std::vector<SgAsmCilMetadataHeap::TableKind> >
REF_TABLES = { // single table
               { SgAsmCilMetadataHeap::e_ref_assembly_ref,  { SgAsmCilMetadataHeap::e_Assembly } }
             , { SgAsmCilMetadataHeap::e_ref_type_def,      { SgAsmCilMetadataHeap::e_TypeDef } }
             , { SgAsmCilMetadataHeap::e_ref_event,         { SgAsmCilMetadataHeap::e_Event } }
             , { SgAsmCilMetadataHeap::e_ref_field,         { SgAsmCilMetadataHeap::e_Field } }
             , { SgAsmCilMetadataHeap::e_ref_generic_param, { SgAsmCilMetadataHeap::e_GenericParam } }
             , { SgAsmCilMetadataHeap::e_ref_method_def,    { SgAsmCilMetadataHeap::e_MethodDef } }
             , { SgAsmCilMetadataHeap::e_ref_module_ref,    { SgAsmCilMetadataHeap::e_ModuleRef } }
             , { SgAsmCilMetadataHeap::e_ref_param,         { SgAsmCilMetadataHeap::e_Param } }
             , { SgAsmCilMetadataHeap::e_ref_property,      { SgAsmCilMetadataHeap::e_Property } }
               // multi-table
             , { SgAsmCilMetadataHeap::e_ref_has_constant,  { SgAsmCilMetadataHeap::e_Field, SgAsmCilMetadataHeap::e_Param, SgAsmCilMetadataHeap::e_Property } }
             , { SgAsmCilMetadataHeap::e_ref_has_custom_attribute,
                     { SgAsmCilMetadataHeap::e_MethodDef, SgAsmCilMetadataHeap::e_Field, SgAsmCilMetadataHeap::e_TypeRef, SgAsmCilMetadataHeap::e_TypeDef
                     , SgAsmCilMetadataHeap::e_Param, SgAsmCilMetadataHeap::e_InterfaceImpl, SgAsmCilMetadataHeap::e_MemberRef, SgAsmCilMetadataHeap::e_Module
                     , SgAsmCilMetadataHeap::e_Unknown_table_kind /* FIXME instead of: e_Permission */
                     , SgAsmCilMetadataHeap::e_Property, SgAsmCilMetadataHeap::e_Event, SgAsmCilMetadataHeap::e_StandAloneSig
                     , SgAsmCilMetadataHeap::e_ModuleRef, SgAsmCilMetadataHeap::e_TypeSpec, SgAsmCilMetadataHeap::e_Assembly, SgAsmCilMetadataHeap::e_AssemblyRef
                     , SgAsmCilMetadataHeap::e_File, SgAsmCilMetadataHeap::e_ExportedType, SgAsmCilMetadataHeap::e_ManifestResource, SgAsmCilMetadataHeap::e_GenericParam
                     , SgAsmCilMetadataHeap::e_GenericParamConstraint, SgAsmCilMetadataHeap::e_MethodSpec
                     }
               }
             , { SgAsmCilMetadataHeap::e_ref_has_decl_security, { SgAsmCilMetadataHeap::e_TypeDef, SgAsmCilMetadataHeap::e_MethodDef, SgAsmCilMetadataHeap::e_Assembly } }
             , { SgAsmCilMetadataHeap::e_ref_has_field_marshall, { SgAsmCilMetadataHeap::e_Field, SgAsmCilMetadataHeap::e_Param } }
             , { SgAsmCilMetadataHeap::e_ref_has_semantics, { SgAsmCilMetadataHeap::e_Event, SgAsmCilMetadataHeap::e_Property } }
             , { SgAsmCilMetadataHeap::e_ref_method_def_or_ref, { SgAsmCilMetadataHeap::e_MethodDef, SgAsmCilMetadataHeap::e_MemberRef } }
             , { SgAsmCilMetadataHeap::e_ref_type_def_or_ref, { SgAsmCilMetadataHeap::e_TypeDef, SgAsmCilMetadataHeap::e_TypeRef, SgAsmCilMetadataHeap::e_TypeSpec } }
             , { SgAsmCilMetadataHeap::e_ref_implementation, { SgAsmCilMetadataHeap::e_File, SgAsmCilMetadataHeap::e_AssemblyRef, SgAsmCilMetadataHeap::e_ExportedType } }
             , { SgAsmCilMetadataHeap::e_ref_member_forwarded, { SgAsmCilMetadataHeap::e_Field, SgAsmCilMetadataHeap::e_MethodDef } }
             , { SgAsmCilMetadataHeap::e_ref_member_ref_parent, { SgAsmCilMetadataHeap::e_TypeDef, SgAsmCilMetadataHeap::e_TypeRef, SgAsmCilMetadataHeap::e_ModuleRef, SgAsmCilMetadataHeap::e_MethodDef, SgAsmCilMetadataHeap::e_TypeSpec } }
             , { SgAsmCilMetadataHeap::e_ref_type_or_method_def, { SgAsmCilMetadataHeap::e_TypeDef, SgAsmCilMetadataHeap::e_MethodDef } }
               // not used inside the tables
             , { SgAsmCilMetadataHeap::e_ref_custom_attribute_type,
                     { SgAsmCilMetadataHeap::e_Unknown_table_kind, SgAsmCilMetadataHeap::e_Unknown_table_kind, SgAsmCilMetadataHeap::e_MethodDef, SgAsmCilMetadataHeap::e_MemberRef
                     , SgAsmCilMetadataHeap::e_Unknown_table_kind
                     }
               }
             , { SgAsmCilMetadataHeap::e_ref_resolution_scope, { SgAsmCilMetadataHeap::e_Module, SgAsmCilMetadataHeap::e_ModuleRef, SgAsmCilMetadataHeap::e_AssemblyRef, SgAsmCilMetadataHeap::e_TypeRef } }
             };


uint64_t
uses4byteIndex( const std::vector<uint32_t>& numberOfRows,
                const std::vector<int8_t>& posInRowVector,
                const std::vector<SgAsmCilMetadataHeap::TableKind>& tables,
                uint64_t flag
              )
{
  const size_t   bitsForTableIdentifier = calcTableIdentifierSize(tables);

  if (TRACE_CONSTRUCTION)
    std::cerr << "|numberOfRows, posInRowVector, tables| = "
              << numberOfRows.size() << ", "
              << posInRowVector.size() << ", "
              << tables.size()
              << "  flag = " << flag
              << "  bitsForTableIdentifier = " << bitsForTableIdentifier
              << std::endl;

  const uint32_t maxTableSize = calcMaxTableSize(numberOfRows, posInRowVector, tables);
  const uint32_t TWO_BYTE_MAX = (1 << (16-bitsForTableIdentifier))-1;

  return TWO_BYTE_MAX >= maxTableSize ? 0 : flag;
}


inline
uint64_t
uses4byteIndex(const std::vector<uint32_t>& numberOfRows, const std::vector<int8_t>& posInRowVector, uint64_t flag)
{
  return uses4byteIndex(numberOfRows, posInRowVector, REF_TABLES.at(flag), flag);
}


uint64_t
computeDataSizeFlags( uint8_t heapSizes,
                      const std::vector<int8_t>& posInRowVector,
                      const std::vector<uint32_t>& numberOfRows
                    )
{
  // the lower three flags are taken from the heapSizes field
  uint64_t            res = heapSizes;

  //   - single table references (constants are arbitrary)
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_assembly_ref);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_type_def);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_event);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_field);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_generic_param);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_method_def);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_module_ref);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_param);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_property);

  //   - multi table references (constants are arbitrary)
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_has_constant);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_has_custom_attribute);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_has_decl_security);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_has_field_marshall);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_has_semantics);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_method_def_or_ref);

  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_type_def_or_ref);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_implementation);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_member_forwarded);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_member_ref_parent);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_type_or_method_def);

  //   - not used from inside the metadata tables
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_custom_attribute_type);
  res |= uses4byteIndex(numberOfRows, posInRowVector, SgAsmCilMetadataHeap::e_ref_resolution_scope);

  return res;
}

struct AccessTuple : std::tuple<SgAsmCilMetadataHeap::TableKind, std::uint32_t>
{
  using base = std::tuple<SgAsmCilMetadataHeap::TableKind, std::uint32_t>;
  using base::base;

  SgAsmCilMetadataHeap::TableKind table() const { return std::get<0>(*this); }
  std::uint32_t                   index() const { return std::get<1>(*this); }
};

AccessTuple
computeAccessPair( const std::vector<int8_t>& posInRowVector,
                   const std::vector<uint32_t>& numberOfRows,
                   std::uint32_t refcode,
                   SgAsmCilMetadataHeap::ReferenceKind knd
                 )
{
  const std::vector<SgAsmCilMetadataHeap::TableKind>& tables = REF_TABLES.at(knd);

  if (tables.size() == 1) return AccessTuple{ tables.front(), refcode };

  const bool          uses4Bytes = uses4byteIndex(numberOfRows, posInRowVector, tables, knd) != 0;
  ROSE_ASSERT(uses4Bytes || (refcode < (1<<16)));

  const std::uint64_t tableIdSize = calcTableIdentifierSize(tables);
  const std::uint32_t index = refcode >> tableIdSize; 
  const std::uint8_t  table = refcode ^ (index << tableIdSize);
  
  return AccessTuple{ tables.at(table), index };
}


template <class SageAsmCilMetadataTable>
SageAsmCilMetadataTable*
parseMetadataTable( SgAsmCilMetadataHeap* parent,
                    std::vector<uint8_t>& buf,
                    size_t& index,
                    uint64_t sizeFlags,
                    size_t rows,
                    const char* tblName
                  )
{
  using CilMetadataType = typename SageAsmCilMetadataTable::CilMetadataType;

  SageAsmCilMetadataTable*       sgnode = new SageAsmCilMetadataTable;
  std::vector<CilMetadataType*>& res = sgnode->get_elements();
  
  sgnode->set_parent(parent);

  if (TRACE_CONSTRUCTION)
    std::cerr << "Build the e_" << tblName << " table; rows = " << rows << std::endl;

  res.reserve(rows);
  for (size_t j=0; j < rows; ++j)
  {
    if (TRACE_CONSTRUCTION)
      std::cerr << " --- processing row j = " << j << std::endl;

    res.push_back(parseAsmCilNode<CilMetadataType>(sgnode,buf,index,sizeFlags));

    if (TRACE_CONSTRUCTION)
      std::cerr << "DONE: processing row j = " << j << std::endl;
  }

  if (TRACE_CONSTRUCTION)
    std::cerr << "DONE: Build the e_" << tblName << " table; rows = " << rows << std::endl;

  return sgnode;
}

template <class T>
T traceRep(const T& el) { return el; }

template <class T>
int traceRep(const std::vector<T>& vec) { return vec.size(); }

}



void SgAsmCilMetadataHeap::parse(std::vector<uint8_t>& buf, size_t startOfMetaData)
{
  const uint32_t ofs = get_Offset();

  if (TRACE_CONSTRUCTION)
    printf ("In MetadataTable constructor: buf = 0x%" PRIxPTR ", index = %zu, offset = %" PRIu32 "\n",(uintptr_t)buf.data(),startOfMetaData,ofs);

  size_t index = startOfMetaData + ofs;


  p_ReservedAlwaysZero = readExpected(read32bitValue, buf, index, 0);

  if (TRACE_CONSTRUCTION)
    std::cerr << "ReservedAlwaysZero = " << traceRep(p_ReservedAlwaysZero) << std::endl;
  p_MajorVersion = read8bitValue(buf,index);

  if (TRACE_CONSTRUCTION)
    std::cerr << "MajorVersion = " << traceRep(p_MajorVersion) << std::endl;
  p_MinorVersion = read8bitValue(buf,index);

  if (TRACE_CONSTRUCTION)
    std::cerr << "MinorVersion = " << traceRep(p_MinorVersion) << std::endl;
  p_HeapSizes = read8bitValue(buf,index);

  if (TRACE_CONSTRUCTION)
    std::cerr << "HeapSizes = " << traceRep(p_HeapSizes) << std::endl;
  p_ReservedAlwaysOne = readExpected(read8bitValue,buf,index,1);

  if (TRACE_CONSTRUCTION)
    std::cerr << "ReservedAlwaysOne = " << traceRep(p_ReservedAlwaysOne) << std::endl;
  p_Valid = read64bitValue(buf,index);  std::vector<int8_t> posInRowVector = computePositionInRowVector(get_Valid());

  if (TRACE_CONSTRUCTION)
    std::cerr << "Valid = " << traceRep(p_Valid) << std::endl;
  p_Sorted = read64bitValue(buf,index);

  if (TRACE_CONSTRUCTION)
    std::cerr << "Sorted = " << traceRep(p_Sorted) << std::endl;
  p_NumberOfRows = readVector(read32bitValue, Rose::BitOps::nSet(get_Valid()), buf, index);

  if (TRACE_CONSTRUCTION)
    std::cerr << "NumberOfRows = " << traceRep(p_NumberOfRows) << std::endl;
  p_DataSizeFlags = computeDataSizeFlags(get_HeapSizes(), posInRowVector, get_NumberOfRows());

  if (TRACE_CONSTRUCTION)
    std::cerr << "DataSizeFlags = " << traceRep(p_DataSizeFlags) << std::endl;

  if (TRACE_CONSTRUCTION)
    std::cerr << "Build the tables: " << std::endl;

  for (size_t kind = 0; kind < posInRowVector.size(); ++kind)
  {
    const int    tblpos = posInRowVector[kind];
    if (tblpos < 0) continue;

    const size_t rows = p_NumberOfRows.at(tblpos);

    // Build the associated table.
    switch (kind)
    { 
      case e_Assembly:
        p_AssemblyTable = parseMetadataTable<SgAsmCilAssemblyTable>(this, buf, index, get_DataSizeFlags(), rows, "Assembly");
        break;
      case e_AssemblyOS:
        p_AssemblyOSTable = parseMetadataTable<SgAsmCilAssemblyOSTable>(this, buf, index, get_DataSizeFlags(), rows, "AssemblyOS");
        break;
      case e_AssemblyProcessor:
        p_AssemblyProcessorTable = parseMetadataTable<SgAsmCilAssemblyProcessorTable>(this, buf, index, get_DataSizeFlags(), rows, "AssemblyProcessor");
        break;
      case e_AssemblyRef:
        p_AssemblyRefTable = parseMetadataTable<SgAsmCilAssemblyRefTable>(this, buf, index, get_DataSizeFlags(), rows, "AssemblyRef");
        break;
      case e_AssemblyRefOS:
        p_AssemblyRefOSTable = parseMetadataTable<SgAsmCilAssemblyRefOSTable>(this, buf, index, get_DataSizeFlags(), rows, "AssemblyRefOS");
        break;
      case e_AssemblyRefProcessor:
        p_AssemblyRefProcessorTable = parseMetadataTable<SgAsmCilAssemblyRefProcessorTable>(this, buf, index, get_DataSizeFlags(), rows, "AssemblyRefProcessor");
        break;
      case e_ClassLayout:
        p_ClassLayoutTable = parseMetadataTable<SgAsmCilClassLayoutTable>(this, buf, index, get_DataSizeFlags(), rows, "ClassLayout");
        break;
      case e_Constant:
        p_ConstantTable = parseMetadataTable<SgAsmCilConstantTable>(this, buf, index, get_DataSizeFlags(), rows, "Constant");
        break;
      case e_CustomAttribute:
        p_CustomAttributeTable = parseMetadataTable<SgAsmCilCustomAttributeTable>(this, buf, index, get_DataSizeFlags(), rows, "CustomAttribute");
        break;
      case e_DeclSecurity:
        p_DeclSecurityTable = parseMetadataTable<SgAsmCilDeclSecurityTable>(this, buf, index, get_DataSizeFlags(), rows, "DeclSecurity");
        break;
      case e_Event:
        p_EventTable = parseMetadataTable<SgAsmCilEventTable>(this, buf, index, get_DataSizeFlags(), rows, "Event");
        break;
      case e_EventMap:
        p_EventMapTable = parseMetadataTable<SgAsmCilEventMapTable>(this, buf, index, get_DataSizeFlags(), rows, "EventMap");
        break;
      case e_ExportedType:
        p_ExportedTypeTable = parseMetadataTable<SgAsmCilExportedTypeTable>(this, buf, index, get_DataSizeFlags(), rows, "ExportedType");
        break;
      case e_Field:
        p_FieldTable = parseMetadataTable<SgAsmCilFieldTable>(this, buf, index, get_DataSizeFlags(), rows, "Field");
        break;
      case e_FieldLayout:
        p_FieldLayoutTable = parseMetadataTable<SgAsmCilFieldLayoutTable>(this, buf, index, get_DataSizeFlags(), rows, "FieldLayout");
        break;
      case e_FieldMarshal:
        p_FieldMarshalTable = parseMetadataTable<SgAsmCilFieldMarshalTable>(this, buf, index, get_DataSizeFlags(), rows, "FieldMarshal");
        break;
      case e_FieldRVA:
        p_FieldRVATable = parseMetadataTable<SgAsmCilFieldRVATable>(this, buf, index, get_DataSizeFlags(), rows, "FieldRVA");
        break;
      case e_File:
        p_FileTable = parseMetadataTable<SgAsmCilFileTable>(this, buf, index, get_DataSizeFlags(), rows, "File");
        break;
      case e_GenericParam:
        p_GenericParamTable = parseMetadataTable<SgAsmCilGenericParamTable>(this, buf, index, get_DataSizeFlags(), rows, "GenericParam");
        break;
      case e_GenericParamConstraint:
        p_GenericParamConstraintTable = parseMetadataTable<SgAsmCilGenericParamConstraintTable>(this, buf, index, get_DataSizeFlags(), rows, "GenericParamConstraint");
        break;
      case e_ImplMap:
        p_ImplMapTable = parseMetadataTable<SgAsmCilImplMapTable>(this, buf, index, get_DataSizeFlags(), rows, "ImplMap");
        break;
      case e_InterfaceImpl:
        p_InterfaceImplTable = parseMetadataTable<SgAsmCilInterfaceImplTable>(this, buf, index, get_DataSizeFlags(), rows, "InterfaceImpl");
        break;
      case e_ManifestResource:
        p_ManifestResourceTable = parseMetadataTable<SgAsmCilManifestResourceTable>(this, buf, index, get_DataSizeFlags(), rows, "ManifestResource");
        break;
      case e_MemberRef:
        p_MemberRefTable = parseMetadataTable<SgAsmCilMemberRefTable>(this, buf, index, get_DataSizeFlags(), rows, "MemberRef");
        break;
      case e_MethodDef:
        p_MethodDefTable = parseMetadataTable<SgAsmCilMethodDefTable>(this, buf, index, get_DataSizeFlags(), rows, "MethodDef");
        break;
      case e_MethodImpl:
        p_MethodImplTable = parseMetadataTable<SgAsmCilMethodImplTable>(this, buf, index, get_DataSizeFlags(), rows, "MethodImpl");
        break;
      case e_MethodSemantics:
        p_MethodSemanticsTable = parseMetadataTable<SgAsmCilMethodSemanticsTable>(this, buf, index, get_DataSizeFlags(), rows, "MethodSemantics");
        break;
      case e_MethodSpec:
        p_MethodSpecTable = parseMetadataTable<SgAsmCilMethodSpecTable>(this, buf, index, get_DataSizeFlags(), rows, "MethodSpec");
        break;
      case e_Module:
        p_ModuleTable = parseMetadataTable<SgAsmCilModuleTable>(this, buf, index, get_DataSizeFlags(), rows, "Module");
        break;
      case e_ModuleRef:
        p_ModuleRefTable = parseMetadataTable<SgAsmCilModuleRefTable>(this, buf, index, get_DataSizeFlags(), rows, "ModuleRef");
        break;
      case e_NestedClass:
        p_NestedClassTable = parseMetadataTable<SgAsmCilNestedClassTable>(this, buf, index, get_DataSizeFlags(), rows, "NestedClass");
        break;
      case e_Param:
        p_ParamTable = parseMetadataTable<SgAsmCilParamTable>(this, buf, index, get_DataSizeFlags(), rows, "Param");
        break;
      case e_Property:
        p_PropertyTable = parseMetadataTable<SgAsmCilPropertyTable>(this, buf, index, get_DataSizeFlags(), rows, "Property");
        break;
      case e_PropertyMap:
        p_PropertyMapTable = parseMetadataTable<SgAsmCilPropertyMapTable>(this, buf, index, get_DataSizeFlags(), rows, "PropertyMap");
        break;
      case e_StandAloneSig:
        p_StandAloneSigTable = parseMetadataTable<SgAsmCilStandAloneSigTable>(this, buf, index, get_DataSizeFlags(), rows, "StandAloneSig");
        break;
      case e_TypeDef:
        p_TypeDefTable = parseMetadataTable<SgAsmCilTypeDefTable>(this, buf, index, get_DataSizeFlags(), rows, "TypeDef");
        break;
      case e_TypeRef:
        p_TypeRefTable = parseMetadataTable<SgAsmCilTypeRefTable>(this, buf, index, get_DataSizeFlags(), rows, "TypeRef");
        break;
      case e_TypeSpec:
        p_TypeSpecTable = parseMetadataTable<SgAsmCilTypeSpecTable>(this, buf, index, get_DataSizeFlags(), rows, "TypeSpec");
        break;
      default:
        std::cerr << "default reached:\n"
                  << "parsing not implemented for kind = " << kind << table_kind_to_string(SgAsmCilMetadataHeap::TableKind(kind))
                  << std::endl;
        ROSE_ABORT();
        break;
    }
  }
}


SgAsmCilMetadata*
SgAsmCilMetadataHeap::get_MetadataNode(std::uint32_t index, TableKind knd) const
{
  SgAsmCilMetadata* res = nullptr;

  switch (knd)
  { 
    case e_Assembly:
      ASSERT_not_null(get_AssemblyTable());
      res = get_AssemblyTable()->get_elements().at(index-1);
      break;
    case e_AssemblyOS:
      ASSERT_not_null(get_AssemblyOSTable());
      res = get_AssemblyOSTable()->get_elements().at(index-1);
      break;
    case e_AssemblyProcessor:
      ASSERT_not_null(get_AssemblyProcessorTable());
      res = get_AssemblyProcessorTable()->get_elements().at(index-1);
      break;
    case e_AssemblyRef:
      ASSERT_not_null(get_AssemblyRefTable());
      res = get_AssemblyRefTable()->get_elements().at(index-1);
      break;
    case e_AssemblyRefOS:
      ASSERT_not_null(get_AssemblyRefOSTable());
      res = get_AssemblyRefOSTable()->get_elements().at(index-1);
      break;
    case e_AssemblyRefProcessor:
      ASSERT_not_null(get_AssemblyRefProcessorTable());
      res = get_AssemblyRefProcessorTable()->get_elements().at(index-1);
      break;
    case e_ClassLayout:
      ASSERT_not_null(get_ClassLayoutTable());
      res = get_ClassLayoutTable()->get_elements().at(index-1);
      break;
    case e_Constant:
      ASSERT_not_null(get_ConstantTable());
      res = get_ConstantTable()->get_elements().at(index-1);
      break;
    case e_CustomAttribute:
      ASSERT_not_null(get_CustomAttributeTable());
      res = get_CustomAttributeTable()->get_elements().at(index-1);
      break;
    case e_DeclSecurity:
      ASSERT_not_null(get_DeclSecurityTable());
      res = get_DeclSecurityTable()->get_elements().at(index-1);
      break;
    case e_Event:
      ASSERT_not_null(get_EventTable());
      res = get_EventTable()->get_elements().at(index-1);
      break;
    case e_EventMap:
      ASSERT_not_null(get_EventMapTable());
      res = get_EventMapTable()->get_elements().at(index-1);
      break;
    case e_ExportedType:
      ASSERT_not_null(get_ExportedTypeTable());
      res = get_ExportedTypeTable()->get_elements().at(index-1);
      break;
    case e_Field:
      ASSERT_not_null(get_FieldTable());
      res = get_FieldTable()->get_elements().at(index-1);
      break;
    case e_FieldLayout:
      ASSERT_not_null(get_FieldLayoutTable());
      res = get_FieldLayoutTable()->get_elements().at(index-1);
      break;
    case e_FieldMarshal:
      ASSERT_not_null(get_FieldMarshalTable());
      res = get_FieldMarshalTable()->get_elements().at(index-1);
      break;
    case e_FieldRVA:
      ASSERT_not_null(get_FieldRVATable());
      res = get_FieldRVATable()->get_elements().at(index-1);
      break;
    case e_File:
      ASSERT_not_null(get_FileTable());
      res = get_FileTable()->get_elements().at(index-1);
      break;
    case e_GenericParam:
      ASSERT_not_null(get_GenericParamTable());
      res = get_GenericParamTable()->get_elements().at(index-1);
      break;
    case e_GenericParamConstraint:
      ASSERT_not_null(get_GenericParamConstraintTable());
      res = get_GenericParamConstraintTable()->get_elements().at(index-1);
      break;
    case e_ImplMap:
      ASSERT_not_null(get_ImplMapTable());
      res = get_ImplMapTable()->get_elements().at(index-1);
      break;
    case e_InterfaceImpl:
      ASSERT_not_null(get_InterfaceImplTable());
      res = get_InterfaceImplTable()->get_elements().at(index-1);
      break;
    case e_ManifestResource:
      ASSERT_not_null(get_ManifestResourceTable());
      res = get_ManifestResourceTable()->get_elements().at(index-1);
      break;
    case e_MemberRef:
      ASSERT_not_null(get_MemberRefTable());
      res = get_MemberRefTable()->get_elements().at(index-1);
      break;
    case e_MethodDef:
      ASSERT_not_null(get_MethodDefTable());
      res = get_MethodDefTable()->get_elements().at(index-1);
      break;
    case e_MethodImpl:
      ASSERT_not_null(get_MethodImplTable());
      res = get_MethodImplTable()->get_elements().at(index-1);
      break;
    case e_MethodSemantics:
      ASSERT_not_null(get_MethodSemanticsTable());
      res = get_MethodSemanticsTable()->get_elements().at(index-1);
      break;
    case e_MethodSpec:
      ASSERT_not_null(get_MethodSpecTable());
      res = get_MethodSpecTable()->get_elements().at(index-1);
      break;
    case e_Module:
      ASSERT_not_null(get_ModuleTable());
      res = get_ModuleTable()->get_elements().at(index-1);
      break;
    case e_ModuleRef:
      ASSERT_not_null(get_ModuleRefTable());
      res = get_ModuleRefTable()->get_elements().at(index-1);
      break;
    case e_NestedClass:
      ASSERT_not_null(get_NestedClassTable());
      res = get_NestedClassTable()->get_elements().at(index-1);
      break;
    case e_Param:
      ASSERT_not_null(get_ParamTable());
      res = get_ParamTable()->get_elements().at(index-1);
      break;
    case e_Property:
      ASSERT_not_null(get_PropertyTable());
      res = get_PropertyTable()->get_elements().at(index-1);
      break;
    case e_PropertyMap:
      ASSERT_not_null(get_PropertyMapTable());
      res = get_PropertyMapTable()->get_elements().at(index-1);
      break;
    case e_StandAloneSig:
      ASSERT_not_null(get_StandAloneSigTable());
      res = get_StandAloneSigTable()->get_elements().at(index-1);
      break;
    case e_TypeDef:
      ASSERT_not_null(get_TypeDefTable());
      res = get_TypeDefTable()->get_elements().at(index-1);
      break;
    case e_TypeRef:
      ASSERT_not_null(get_TypeRefTable());
      res = get_TypeRefTable()->get_elements().at(index-1);
      break;
    case e_TypeSpec:
      ASSERT_not_null(get_TypeSpecTable());
      res = get_TypeSpecTable()->get_elements().at(index-1);
      break;

    default: ;
  }

  ASSERT_not_null(res);
  return res;
}


SgAsmCilMetadata*
SgAsmCilMetadataHeap::get_CodedMetadataNode(std::uint32_t refcode, ReferenceKind knd) const
{
  // FIXME: maybe cache result in static or in the object if this turns out to be slow
  std::vector<std::int8_t> posInRow = computePositionInRowVector(get_Valid());
  const AccessTuple        access = computeAccessPair(posInRow, get_NumberOfRows(), refcode, knd);

  return get_MetadataNode(access.index(), access.table());
}


//
// MetadataRoot 

namespace
{

struct MethodHeader : std::tuple<std::uint16_t, std::uint16_t, std::uint32_t, std::uint32_t>
{
  static constexpr std::uint8_t TINY           = 0x2;
  static constexpr std::uint8_t FAT            = 0x3;
  static constexpr std::uint8_t FORMAT         = TINY | FAT;
  static constexpr std::uint8_t MORE_SECTS     = 0x8;
  static constexpr std::uint8_t INIT_LOCALS    = 0x10;
  static constexpr std::uint8_t FLAGS          = MORE_SECTS | INIT_LOCALS;
  static constexpr std::uint8_t FAT_HEADER_LEN = 12;

  using base = std::tuple<std::uint16_t, std::uint16_t, std::uint32_t, std::uint32_t>;
  using base::base;

  std::uint16_t flags() const          { return std::get<0>(*this); }
  bool          tiny()  const          { return (flags() & FORMAT) == TINY; }
  bool          moreSections()  const  { return (flags() & FLAGS) == MORE_SECTS; }
  bool          initLocals()  const    { return (flags() & FLAGS) == INIT_LOCALS; }
  std::uint8_t  headerSize() const     { return tiny() ? 1 : FAT_HEADER_LEN; }

  std::uint16_t maxStackSize() const   { return std::get<1>(*this); }
  std::uint32_t codeSize() const       { return std::get<2>(*this); }
  std::uint32_t localVarSigTok() const { return std::get<3>(*this); }
};

MethodHeader
parseFatHeader(rose_addr_t base_va, std::uint32_t rva, SgAsmPEFileHeader* fhdr)
{
  std::uint8_t         buf[12];
  const std::size_t    nread = fhdr->get_loaderMap()->readQuick(&buf, base_va + rva, sizeof(buf)); // blame ae42dc1f
  ROSE_ASSERT(nread == 12);

  const std::uint16_t  flags    = Rose::BinaryAnalysis::ByteOrder::leToHost(*reinterpret_cast<uint16_t*>(buf+0)); // blame ae42dc1f
  const std::uint16_t  maxStack = Rose::BinaryAnalysis::ByteOrder::leToHost(*reinterpret_cast<uint16_t*>(buf+2));
  const std::uint16_t  codeSize = Rose::BinaryAnalysis::ByteOrder::leToHost(*reinterpret_cast<uint32_t*>(buf+4));
  const std::uint16_t  localIni = Rose::BinaryAnalysis::ByteOrder::leToHost(*reinterpret_cast<uint32_t*>(buf+8));
  const MethodHeader   res{ flags, maxStack, codeSize, localIni };

  ROSE_ASSERT(!res.tiny());
  ROSE_ASSERT(res.headerSize() == MethodHeader::FAT_HEADER_LEN);
  return res;
}

MethodHeader
parseTinyHeader(std::uint8_t header)
{
  return { header & MethodHeader::FORMAT, 8, header >> 2, 0 };
}

SgAsmBlock*
disassemble(rose_addr_t base_va, SgAsmCilMethodDef* m, MethodHeader mh,
            std::vector<std::uint8_t>& buf, const Rose::BinaryAnalysis::Disassembler::Base::Ptr& disasm)
{
  rose_addr_t        addr = 0;
  const std::size_t  sz = buf.size();
  std::vector<SgAsmInstruction*> lst;

  using Rose::BinaryAnalysis::MemoryMap;

  MemoryMap::Ptr map = MemoryMap::instance();
  map->insert(AddressInterval::baseSize(base_va, sz),
              MemoryMap::Segment::staticInstance(buf.data(), sz,
                                                 MemoryMap::READABLE|MemoryMap::EXECUTABLE, "CIL code segment"));

  while (addr < sz) {
    SgAsmInstruction* instr = disasm->disassembleOne(map, base_va + addr);
    ASSERT_not_null(instr);

    lst.push_back(instr);
    addr += instr->get_size();

    if (disasm->architecture()->isUnknown(instr)) {
      // Pad block with noops because something went wrong
      // TODO: don't pad with noops, pad by expanding current unknown instruction
      SgUnsignedCharList rawBytes(1,'\0');
      while (addr < sz) {
        auto insn = new SgAsmCilInstruction(base_va+addr, disasm->architecture()->name(), "nop",
                                            Rose::BinaryAnalysis::CilInstructionKind::Cil_nop);
        insn->set_rawBytes(rawBytes);
        ASSERT_require(insn->get_rawBytes().size() == 1);
        lst.push_back(insn);
        addr += insn->get_size();
      }
    }

    // Just checking for when last instruction is not return from function (or similar terminating instruction)
    ASSERT_require(addr <= sz);
    if (addr == sz) {
      switch (instr->get_anyKind())
      {
        namespace rb = Rose::BinaryAnalysis;
        case 0:
        case rb::CilInstructionKind::Cil_ret:
        case rb::CilInstructionKind::Cil_throw:
        case rb::CilInstructionKind::Cil_br:
        case rb::CilInstructionKind::Cil_br_s:
        case rb::CilInstructionKind::Cil_endfinally:
          break;
        default:
          mlog[INFO] << "last instruction in block is not Cil_ret (or like), is 0x" << std::hex << (int) instr->get_anyKind() << std::dec << "\n";
      }
    }
  }

  if (addr > sz) {
    mlog[FATAL] << "instruction address exceeds size of instruction block\n";
    ROSE_ABORT();
  }

  return sb::buildBasicBlock(lst);
}

void decodeMetadata(rose_addr_t base_va, SgAsmCilMetadataHeap* mdh, SgAsmCilMetadataRoot* root)
{
  constexpr std::uint8_t CIL_CODE       = 0;
  constexpr std::uint8_t NATIVE_CODE    = 1;
  constexpr std::uint8_t OPTIL_RESERVED = 2;
  constexpr std::uint8_t RUNTIME_CODE   = 3;
  constexpr std::uint8_t CODE_TYPE_MASK = CIL_CODE | NATIVE_CODE | OPTIL_RESERVED | RUNTIME_CODE;

  ASSERT_not_null(mdh); ASSERT_not_null(root);

  SgAsmCilUint8Heap* stringHeap = root->get_StringHeap();
  ASSERT_not_null(stringHeap);

  SgAsmPEFileHeader* fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(root);
  ASSERT_not_null(fhdr);
  
  SgAsmCilMethodDefTable* mtbl = mdh->get_MethodDefTable();
  ASSERT_not_null(mtbl);
  
  // decode methods
  for (SgAsmCilMethodDef* m : mtbl->get_elements())
  {
    ASSERT_not_null(m);

    rose_addr_t rva = static_cast<std::uint32_t>(m->get_RVA());
    
    if (rva == 0) continue;
      
    // parse header
    std::uint8_t   mh0;
    std::size_t    nread = fhdr->get_loaderMap()->readQuick(&mh0, base_va + rva, 1); // blame ae42dc1f
    ROSE_ASSERT(nread == 1);

    const bool     isTiny = (mh0 & MethodHeader::FORMAT) == MethodHeader::TINY;
    ROSE_ASSERT(isTiny || (((base_va+rva)%4) == 0));
    MethodHeader   mh = isTiny ? parseTinyHeader(mh0) : parseFatHeader(base_va, rva, fhdr);

    m->set_stackSize(mh.maxStackSize());
    m->set_hasMoreSections(mh.moreSections());
    m->set_initLocals(mh.initLocals());
    
    // parse code
    rose_addr_t    codeRVA = rva + mh.headerSize();
    std::uint32_t  codeLen = mh.codeSize();

    std::vector<std::uint8_t> code(codeLen, 0);
    std::size_t nreadCode = fhdr->get_loaderMap()->readQuick(code.data(), base_va + codeRVA, codeLen); // blame ae42dc1f
    ROSE_ASSERT(nreadCode == codeLen);

    SgAsmBlock* blk = nullptr;
    
    switch (m->get_ImplFlags() & CODE_TYPE_MASK)
    {
      namespace rb = Rose::BinaryAnalysis;

      case CIL_CODE: {
        auto arch = rb::Architecture::findByName("cil").orThrow();
        blk = disassemble(base_va + codeRVA, m, mh, code, rb::Disassembler::Cil::instance(arch));
        break;
      }

      case NATIVE_CODE: {
        auto arch = rb::Architecture::findByName("intel-pentium4").orThrow();
        blk = disassemble(base_va + codeRVA, m, mh, code, rb::Disassembler::X86::instance(arch));
        break;
      }

      case RUNTIME_CODE:
        std::cerr << "  - runtime provided: " << code.size()
                  << std::endl;
        // looking for sample code
        ROSE_ABORT();
        break;

      default:
        ROSE_ABORT();
    }

    ASSERT_not_null(blk);
    m->set_body(blk);
  }
}

template <class SageAsmCilHeap>
SageAsmCilHeap*
getHeapInternal(const std::vector<SgAsmCilDataStream*>& streams, int8_t& idx, const std::string& streamID)
{
  const int numHeaps = streams.size();

  if (idx < 0)
  {
    while ((++idx < numHeaps) && (streams[idx]->get_Name() != streamID));
  }

  if (idx < numHeaps)
    return dynamic_cast<SageAsmCilHeap*>(streams[idx]);

  return nullptr;
}

}


void SgAsmCilMetadataRoot::parse()
{
  SgAsmCliHeader* clih = isSgAsmCliHeader(get_parent());
  ASSERT_not_null(clih);

  SgAsmPEFileHeader* fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(this);
  ASSERT_not_null(fhdr);

  uint64_t    metaData = clih->get_metaData();
  uint8_t*    data = reinterpret_cast<uint8_t*>(&metaData);
  rose_addr_t rva = Rose::BinaryAnalysis::ByteOrder::leToHost(*reinterpret_cast<uint32_t*>(data));
  size_t      size = Rose::BinaryAnalysis::ByteOrder::leToHost(*reinterpret_cast<uint32_t*>(data+4));
  rose_addr_t base_va = clih->get_baseVa();
  rose_addr_t rva_offset = clih->get_rvaOffset(rva);

  if (TRACE_CONSTRUCTION)
  {
    std::cerr << "------------------------SgAsmCilMetadataRoot::parse-----------------------------\n";
    std::cerr << "    rva: " << rva << " size: " << size << std::endl;
    std::cerr << "    base_va: " << base_va << " rva_offset: " << rva_offset << std::endl;
  }

  /* Read the Signature via loader map. */
  // Note: probably want to allocate a larger buffer
  std::vector<uint8_t> buf(size, 0);

  size_t nread = fhdr->get_loaderMap()->readQuick(buf.data(), base_va + rva, size); // blame ae42dc1f
  ASSERT_require(nread == size);

  this->parse(buf, 0);
  decodeMetadata(base_va, get_MetadataHeap(), this);
}


void SgAsmCilMetadataRoot::parse(std::vector<uint8_t>& buf, size_t index)
{
  size_t start_of_MetadataRoot = index;

  if (TRACE_CONSTRUCTION)
    std::cerr << "Initialize the elements of the data structure" << std::endl;

  
  p_Signature = readExpected(read32bitValue, buf, index, MAGIC_SIGNATURE);
  if (TRACE_CONSTRUCTION)
    std::cerr << "Signature = " << p_Signature << std::endl;
  p_MajorVersion = read16bitValue(buf, index);
  if (TRACE_CONSTRUCTION)
    std::cerr << "MajorVersion = " << p_MajorVersion << std::endl;
  p_MinorVersion = read16bitValue(buf, index);
  if (TRACE_CONSTRUCTION)
    std::cerr << "MinorVersion = " << p_MinorVersion << std::endl;
  p_Reserved0 = readExpected(read32bitValue, buf, index, 0);
  if (TRACE_CONSTRUCTION)
    std::cerr << "Reserved0 = " << p_Reserved0 << std::endl;
  p_Length = read32bitValue(buf, index);
  if (TRACE_CONSTRUCTION)
    std::cerr << "Length = " << p_Length << std::endl;
  p_Version = readUtf8String(buf, index, get_Length());
  if (TRACE_CONSTRUCTION)
    std::cerr << "Version = " << p_Version << std::endl;
  p_VersionPadding = readStringPadding(buf, index, p_Version.size(), get_Length());
  if (TRACE_CONSTRUCTION)
    std::cerr << "VersionPadding = " << p_VersionPadding << std::endl;
  p_Flags = readExpected(read16bitValue, buf, index, 0);
  if (TRACE_CONSTRUCTION)
    std::cerr << "Flags = " << p_Flags << std::endl;
  p_NumberOfStreams = read16bitValue(buf, index);
  if (TRACE_CONSTRUCTION)
    std::cerr << "NumberOfStreams = " << p_NumberOfStreams << std::endl;

  
  p_Streams = parseStreams(this, buf, index, start_of_MetadataRoot, get_NumberOfStreams());
  if (TRACE_CONSTRUCTION)
    std::cerr << "Streams has " << p_Streams.size() << " elements." << std::endl;
}

SgAsmCilUint8Heap*
SgAsmCilMetadataRoot::get_StringHeap() const
{
  return getHeapInternal<SgAsmCilUint8Heap>(get_Streams(), idxStringHeap, SgAsmCilDataStream::ID_STRING_HEAP);
}

SgAsmCilUint8Heap*
SgAsmCilMetadataRoot::get_BlobHeap() const
{
  return getHeapInternal<SgAsmCilUint8Heap>(get_Streams(), idxBlobHeap, SgAsmCilDataStream::ID_BLOB_HEAP);
}

SgAsmCilUint8Heap*
SgAsmCilMetadataRoot::get_UsHeap() const
{
  return getHeapInternal<SgAsmCilUint8Heap>(get_Streams(), idxUsHeap, SgAsmCilDataStream::ID_US_HEAP);
}

SgAsmCilUint32Heap*
SgAsmCilMetadataRoot::get_GuidHeap() const
{
  return getHeapInternal<SgAsmCilUint32Heap>(get_Streams(), idxGuidHeap, SgAsmCilDataStream::ID_GUID_HEAP);
}

SgAsmCilMetadataHeap*
SgAsmCilMetadataRoot::get_MetadataHeap() const
{
  return getHeapInternal<SgAsmCilMetadataHeap>(get_Streams(), idxMetadataHeap, SgAsmCilDataStream::ID_METADATA_HEAP);
}

#endif /* ROSE_ENABLE_BINARY_ANALYSIS */
