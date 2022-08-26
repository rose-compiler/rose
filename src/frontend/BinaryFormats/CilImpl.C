// This file CilImpl.C has been generated from a JSON description file using json2rosetta.
//   Do not modify this file directly, but the corresponding JSON file.

/* CIL Implementation for Classes defined in src/ROSETTA/src/BinaryInstruction.C */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <unordered_map>
#include "Rose/BinaryAnalysis/Disassembler/X86.h"
#include "Rose/BinaryAnalysis/DisassemblerCil.h"
#include "frontend/SageIII/sageInterface/SageBuilderAsm.h"

namespace sb = Rose::SageBuilderAsm;


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
     T value = ByteOrder::le_to_host(*reinterpret_cast<T*>(buf.data()+index));

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
    // FIXME: read real Utf8 string
    return readString(buf, index, maxLen);
  }

  uint32_t
  readStringPadding(std::vector<uint8_t>& buf, size_t& index, size_t strLen, size_t reservedLen)
  {
    ROSE_ASSERT(strLen <= reservedLen);

    uint32_t res = 0;

    printf ("skip string padding of %zu bytes\n", (reservedLen - strLen));
    while (strLen < reservedLen)
    {
      res = (res<<8) + buf.at(index);

      ROSE_ASSERT(index + 1 <= buf.size());
      ++index; ++strLen;
    }

    // printf ("padding bytes value = %" PRIu32 "\n", res);
    return res;
  }


  template <class Reader>
  auto
  readExpected(Reader rd, std::vector<uint8_t>& buf, size_t& index, decltype(rd(buf, index)) expected) -> decltype(rd(buf, index))
  {
    using elem_type = decltype(rd(buf, index));

    elem_type res = rd(buf, index);
    ROSE_ASSERT(res == expected);
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

    printf ("Output the number of rows for each table: \n");

    for (uint64_t i = 0; i < num; ++i)
    {
      elem_type tmp_rows_value = rd(buf, index);
      //~ printf ("--- table %2zu: tmp_rows_value = %u \n",i,tmp_rows_value);
      res.push_back(tmp_rows_value);
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
      if (TRACE_CONSTRUCTION)
        std::cerr << "START: stream header " << i << " of " << numberOfStreams << ": index = " << index
                  << std::endl;

      StreamHeader        streamHeader = StreamHeader::parse(buf,index);
      SgAsmCilDataStream* dataStream = nullptr;

      if (  (SgAsmCilDataStream::ID_STRING_HEAP == streamHeader.name())
         || (SgAsmCilDataStream::ID_US_HEAP     == streamHeader.name())
         || (SgAsmCilDataStream::ID_BLOB_HEAP   == streamHeader.name())
         )
        dataStream = new SgAsmCilUint8Heap(streamHeader.offset(), streamHeader.size(), streamHeader.name(), streamHeader.namePadding());
      else if (SgAsmCilDataStream::ID_GUID_HEAP == streamHeader.name())
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

  template <class SageAsmCilNode>
  SageAsmCilNode*
  parseAsmCilNode(SgAsmCilMetadataHeap* parent, std::vector<uint8_t>& buf, size_t& index, uint64_t dataSizeflags)
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

void SgAsmCilCustomAttribute::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
   {
     p_Parent = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_has_custom_attribute);
     p_Type = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_method_def_or_ref);
     p_Value = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

     if (TRACE_CONSTRUCTION)
        {
          std::cerr << "p_Parent = " << p_Parent << std::endl;
          std::cerr << "p_Type = " << p_Type << std::endl;
          std::cerr << "p_Value = " << p_Value << std::endl;
        }
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

void SgAsmCilInterfaceImpl::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
   {
     p_Class = read16bitValue(buf,index);
     p_Interface = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_type_def_or_ref);

     if (TRACE_CONSTRUCTION)
        {
          std::cerr << "p_Class = " << p_Class << std::endl;
          std::cerr << "p_Interface = " << p_Interface << std::endl;
        }
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

void SgAsmCilModuleRef::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
   {
     p_Name = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);

     if (TRACE_CONSTRUCTION)
        {
          std::cerr << "p_Name = " << p_Name << std::endl;
        }
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

void SgAsmCilStandAloneSig::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
   {
     p_Signature = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

     if (TRACE_CONSTRUCTION)
        {
          std::cerr << "p_Signature = " << p_Signature << std::endl;
        }
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

void SgAsmCilTypeRef::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
   {
     p_ResolutionScope = read16bitValue(buf,index);
     p_TypeName = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);
     p_TypeNamespace = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_string_heap);

     if (TRACE_CONSTRUCTION)
        {
          std::cerr << "p_ResolutionScope = " << p_ResolutionScope << std::endl;
          std::cerr << "p_TypeName = " << p_TypeName << std::endl;
          std::cerr << "p_TypeNamespace = " << p_TypeNamespace << std::endl;
        }
   }

void SgAsmCilTypeSpec::parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing)
   {
     p_Signature = readValue(buf,index,uses4byteIndexing & SgAsmCilMetadataHeap::e_ref_blob_heap);

     if (TRACE_CONSTRUCTION)
        {
          std::cerr << "p_Signature = " << p_Signature << std::endl;
        }
   }

namespace
{
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


////// from metadataHeap_C.txt

namespace
{
/// Metadata table identifiers to support .net metadata layout within the .text section.
enum MetaDataHeapTableKinds
{
// Values of enum fields are the bit positions in the valid array of valid tables (for each table).
  e_unknown_table_kind     = 0xFF,
  e_error_table_kind       = 0xFE,

  e_assembly        = 0x20,
  e_assemblyProcessor        = 0x21,
  e_assemblyOS        = 0x22,
  e_assemblyRef        = 0x23,
  e_assemblyRefProcessor        = 0x24,
  e_assemblyRefOS        = 0x25,
  e_classLayout        = 0x0F,
  e_constant        = 0x0B,
  e_customAttribute        = 0x0C,
  e_declSecurity        = 0x0E,
  e_eventMap        = 0x12,
  e_event        = 0x14,
  e_exportedType        = 0x27,
  e_field        = 0x04,
  e_fieldLayout        = 0x10,
  e_fieldMarshal        = 0x0D,
  e_fieldRVA        = 0x1D,
  e_file        = 0x26,
  e_genericParam        = 0x2A,
  e_genericParamConstraint        = 0x2C,
  e_implMap        = 0x1C,
  e_interfaceImpl        = 0x09,
  e_manifestResource        = 0x28,
  e_memberRef        = 0x0A,
  e_methodDef        = 0x06,
  e_methodImpl        = 0x19,
  e_methodSemantics        = 0x18,
  e_methodSpec        = 0x2B,
  e_module        = 0x00,
  e_moduleRef        = 0x1A,
  e_nestedClass        = 0x29,
  e_param        = 0x08,
  e_property        = 0x17,
  e_propertyMap        = 0x15,
  e_standAloneSig        = 0x11,
  e_typeDef        = 0x02,
  e_typeRef        = 0x01,
  e_typeSpec        = 0x1B,
  e_last_table_kind        = 0xFD
};

const char*
table_kind_to_string ( enum MetaDataHeapTableKinds e )
{
   const char* res = nullptr;

   switch (e)
   {
     case e_unknown_table_kind:     res = "e_unknown_table_kind"; break;
     case e_error_table_kind:       res = "e_error_table_kind"; break;

     case e_assembly:        res = "e_assembly"; break;
     case e_assemblyProcessor:        res = "e_assemblyProcessor"; break;
     case e_assemblyOS:        res = "e_assemblyOS"; break;
     case e_assemblyRef:        res = "e_assemblyRef"; break;
     case e_assemblyRefProcessor:        res = "e_assemblyRefProcessor"; break;
     case e_assemblyRefOS:        res = "e_assemblyRefOS"; break;
     case e_classLayout:        res = "e_classLayout"; break;
     case e_constant:        res = "e_constant"; break;
     case e_customAttribute:        res = "e_customAttribute"; break;
     case e_declSecurity:        res = "e_declSecurity"; break;
     case e_eventMap:        res = "e_eventMap"; break;
     case e_event:        res = "e_event"; break;
     case e_exportedType:        res = "e_exportedType"; break;
     case e_field:        res = "e_field"; break;
     case e_fieldLayout:        res = "e_fieldLayout"; break;
     case e_fieldMarshal:        res = "e_fieldMarshal"; break;
     case e_fieldRVA:        res = "e_fieldRVA"; break;
     case e_file:        res = "e_file"; break;
     case e_genericParam:        res = "e_genericParam"; break;
     case e_genericParamConstraint:        res = "e_genericParamConstraint"; break;
     case e_implMap:        res = "e_implMap"; break;
     case e_interfaceImpl:        res = "e_interfaceImpl"; break;
     case e_manifestResource:        res = "e_manifestResource"; break;
     case e_memberRef:        res = "e_memberRef"; break;
     case e_methodDef:        res = "e_methodDef"; break;
     case e_methodImpl:        res = "e_methodImpl"; break;
     case e_methodSemantics:        res = "e_methodSemantics"; break;
     case e_methodSpec:        res = "e_methodSpec"; break;
     case e_module:        res = "e_module"; break;
     case e_moduleRef:        res = "e_moduleRef"; break;
     case e_nestedClass:        res = "e_nestedClass"; break;
     case e_param:        res = "e_param"; break;
     case e_property:        res = "e_property"; break;
     case e_propertyMap:        res = "e_propertyMap"; break;
     case e_standAloneSig:        res = "e_standAloneSig"; break;
     case e_typeDef:        res = "e_typeDef"; break;
     case e_typeRef:        res = "e_typeRef"; break;
     case e_typeSpec:        res = "e_typeSpec"; break;
     case e_last_table_kind:        res = "e_last_table_kind"; break;

     default:
        {
          ROSE_ABORT();
        }
   }

  return res;
}

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
calcTableIdentifierSize(const std::vector<uint8_t>& tables)
{
  const size_t tblsz = tables.size();
  ROSE_ASSERT(tblsz > 0);

  auto hiBit = Rose::BitOps::highestSetBit(tblsz-1);

  return hiBit ? (*hiBit) + 1 : 0;
}

uint32_t
calcMaxTableSize( const std::vector<uint32_t>& numberOfRows,
                  const std::vector<int8_t>& posInRowVector,
                  const std::vector<uint8_t>& tables
                )
{
  uint32_t max = 0;

  for (uint8_t tbl : tables)
  {
    if (tbl == e_unknown_table_kind) continue;

    const int8_t posInRowVec = posInRowVector.at(tbl);
    if (posInRowVec < 0) continue;

    const uint32_t numEntries = numberOfRows.at(posInRowVec);
    if (max < numEntries) max = numEntries;
  }

  return max;
}


const std::unordered_map<std::uint64_t, std::vector<uint8_t> >
REF_TABLES = { // single table
               { SgAsmCilMetadataHeap::e_ref_assembly_ref,  { e_assembly } }
             , { SgAsmCilMetadataHeap::e_ref_type_def,      { e_typeDef } }
             , { SgAsmCilMetadataHeap::e_ref_event,         { e_event } }
             , { SgAsmCilMetadataHeap::e_ref_field,         { e_field } }
             , { SgAsmCilMetadataHeap::e_ref_generic_param, { e_genericParam } }
             , { SgAsmCilMetadataHeap::e_ref_method_def,    { e_methodDef } }
             , { SgAsmCilMetadataHeap::e_ref_module_ref,    { e_moduleRef } }
             , { SgAsmCilMetadataHeap::e_ref_param,         { e_param } }
             , { SgAsmCilMetadataHeap::e_ref_property,      { e_property } }
               // multi-table
             , { SgAsmCilMetadataHeap::e_ref_has_constant,  { e_field, e_param, e_property } }
             , { SgAsmCilMetadataHeap::e_ref_has_custom_attribute,
                     { e_methodDef, e_field, e_typeRef, e_typeDef
                     , e_param, e_interfaceImpl, e_memberRef, e_module
                     , e_unknown_table_kind // FIXME instead of: e_permission
                     , e_property, e_event, e_standAloneSig
                     , e_moduleRef, e_typeSpec, e_assembly, e_assemblyRef
                     , e_file, e_exportedType, e_manifestResource, e_genericParam
                     , e_genericParamConstraint, e_methodSpec
                     }
               }
             , { SgAsmCilMetadataHeap::e_ref_has_decl_security, { e_typeDef, e_methodDef, e_assembly } }
             , { SgAsmCilMetadataHeap::e_ref_has_field_marshall, { e_field, e_param } }
             , { SgAsmCilMetadataHeap::e_ref_has_semantics, { e_event, e_property } }
             , { SgAsmCilMetadataHeap::e_ref_method_def_or_ref, { e_methodDef, e_memberRef } }
             , { SgAsmCilMetadataHeap::e_ref_type_def_or_ref, { e_typeDef, e_typeRef, e_typeSpec } }
             , { SgAsmCilMetadataHeap::e_ref_implementation, { e_file, e_assemblyRef, e_exportedType } }
             , { SgAsmCilMetadataHeap::e_ref_member_forwarded, { e_field, e_methodDef } }
             , { SgAsmCilMetadataHeap::e_ref_member_ref_parent, { e_typeDef, e_typeRef, e_moduleRef, e_methodDef, e_typeSpec } }
             , { SgAsmCilMetadataHeap::e_ref_type_or_method_def, { e_typeDef, e_methodDef } }
               // not used inside the tables
             , { SgAsmCilMetadataHeap::e_ref_custom_attribute_type,
                     { e_unknown_table_kind, e_unknown_table_kind, e_methodDef, e_memberRef
                     , e_unknown_table_kind
                     }
               }
             , { SgAsmCilMetadataHeap::e_ref_resolution_scope, { e_module, e_moduleRef, e_assemblyRef, e_typeRef } }
             };


uint64_t
uses4byteIndex( const std::vector<uint32_t>& numberOfRows,
                const std::vector<int8_t>& posInRowVector,
                const std::vector<uint8_t>& tables,
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

struct AccessTuple : std::tuple<uint8_t, std::uint32_t>
{
  using base = std::tuple<uint8_t, std::uint32_t>;
  using base::base;

  std::uint8_t  table() const { return std::get<0>(*this); }
  std::uint32_t index() const { return std::get<1>(*this); }
};

AccessTuple
computeAccessPair( const std::vector<int8_t>& posInRowVector,
                   const std::vector<uint32_t>& numberOfRows,
                   std::uint32_t refcode,
                   SgAsmCilMetadataHeap::ReferenceKind knd
                 )
{
  const std::vector<uint8_t>& tables = REF_TABLES.at(knd);

  if (tables.size() == 1) return AccessTuple{ tables.front(), refcode };

  const bool                  uses4Bytes = uses4byteIndex(numberOfRows, posInRowVector, tables, knd) != 0;

  ROSE_ASSERT(uses4Bytes || (refcode < (1<<16)));

  const std::uint8_t          idxLen = uses4Bytes ? 32 : 16;
  const std::uint64_t         tableIdSize = calcTableIdentifierSize(tables);

  ROSE_ASSERT(idxLen > tableIdSize);
  const std::uint8_t          numShifts = (idxLen-tableIdSize);
  const std::uint8_t          table = refcode >> numShifts;
  const std::uint32_t         index = refcode ^ (table << numShifts);

  return AccessTuple{ table, index };
}


template <class SageAsmCilMetadata>
std::vector<SageAsmCilMetadata*>
parseMetadataTable( SgAsmCilMetadataHeap* parent,
                    std::vector<uint8_t>& buf,
                    size_t& index,
                    uint64_t sizeFlags,
                    size_t rows,
                    const char* tblName
                  )
{
  std::vector<SageAsmCilMetadata*> res;

  if (TRACE_CONSTRUCTION)
    std::cerr << "Build the e_" << tblName << " table; rows = " << rows << std::endl;

  for (size_t j=0; j < rows; ++j)
  {
    if (TRACE_CONSTRUCTION)
      std::cerr << " --- processing row j = " << j << std::endl;

    SageAsmCilMetadata* obj = parseAsmCilNode<SageAsmCilMetadata>(parent,buf,index,sizeFlags);
    res.push_back(obj);

    if (TRACE_CONSTRUCTION)
      std::cerr << "DONE: processing row j = " << j << std::endl;
  }

  if (TRACE_CONSTRUCTION)
    std::cerr << "DONE: Build the e_" << tblName << " table; rows = " << rows << std::endl;

  return res;
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

      case e_assembly:
        p_assembly = parseMetadataTable<SgAsmCilAssembly>(this, buf, index, get_DataSizeFlags(), rows, "assembly");
        break;
      case e_assemblyProcessor:
        p_assemblyProcessor = parseMetadataTable<SgAsmCilAssemblyProcessor>(this, buf, index, get_DataSizeFlags(), rows, "assemblyProcessor");
        break;
      case e_assemblyOS:
        p_assemblyOS = parseMetadataTable<SgAsmCilAssemblyOS>(this, buf, index, get_DataSizeFlags(), rows, "assemblyOS");
        break;
      case e_assemblyRef:
        p_assemblyRef = parseMetadataTable<SgAsmCilAssemblyRef>(this, buf, index, get_DataSizeFlags(), rows, "assemblyRef");
        break;
      case e_assemblyRefProcessor:
        p_assemblyRefProcessor = parseMetadataTable<SgAsmCilAssemblyRefProcessor>(this, buf, index, get_DataSizeFlags(), rows, "assemblyRefProcessor");
        break;
      case e_assemblyRefOS:
        p_assemblyRefOS = parseMetadataTable<SgAsmCilAssemblyRefOS>(this, buf, index, get_DataSizeFlags(), rows, "assemblyRefOS");
        break;
      case e_classLayout:
        p_classLayout = parseMetadataTable<SgAsmCilClassLayout>(this, buf, index, get_DataSizeFlags(), rows, "classLayout");
        break;
      case e_constant:
        p_constant = parseMetadataTable<SgAsmCilConstant>(this, buf, index, get_DataSizeFlags(), rows, "constant");
        break;
      case e_customAttribute:
        p_customAttribute = parseMetadataTable<SgAsmCilCustomAttribute>(this, buf, index, get_DataSizeFlags(), rows, "customAttribute");
        break;
      case e_declSecurity:
        p_declSecurity = parseMetadataTable<SgAsmCilDeclSecurity>(this, buf, index, get_DataSizeFlags(), rows, "declSecurity");
        break;
      case e_eventMap:
        p_eventMap = parseMetadataTable<SgAsmCilEventMap>(this, buf, index, get_DataSizeFlags(), rows, "eventMap");
        break;
      case e_event:
        p_event = parseMetadataTable<SgAsmCilEvent>(this, buf, index, get_DataSizeFlags(), rows, "event");
        break;
      case e_exportedType:
        p_exportedType = parseMetadataTable<SgAsmCilExportedType>(this, buf, index, get_DataSizeFlags(), rows, "exportedType");
        break;
      case e_field:
        p_field = parseMetadataTable<SgAsmCilField>(this, buf, index, get_DataSizeFlags(), rows, "field");
        break;
      case e_fieldLayout:
        p_fieldLayout = parseMetadataTable<SgAsmCilFieldLayout>(this, buf, index, get_DataSizeFlags(), rows, "fieldLayout");
        break;
      case e_fieldMarshal:
        p_fieldMarshal = parseMetadataTable<SgAsmCilFieldMarshal>(this, buf, index, get_DataSizeFlags(), rows, "fieldMarshal");
        break;
      case e_fieldRVA:
        p_fieldRVA = parseMetadataTable<SgAsmCilFieldRVA>(this, buf, index, get_DataSizeFlags(), rows, "fieldRVA");
        break;
      case e_file:
        p_file = parseMetadataTable<SgAsmCilFile>(this, buf, index, get_DataSizeFlags(), rows, "file");
        break;
      case e_genericParam:
        p_genericParam = parseMetadataTable<SgAsmCilGenericParam>(this, buf, index, get_DataSizeFlags(), rows, "genericParam");
        break;
      case e_genericParamConstraint:
        p_genericParamConstraint = parseMetadataTable<SgAsmCilGenericParamConstraint>(this, buf, index, get_DataSizeFlags(), rows, "genericParamConstraint");
        break;
      case e_implMap:
        p_implMap = parseMetadataTable<SgAsmCilImplMap>(this, buf, index, get_DataSizeFlags(), rows, "implMap");
        break;
      case e_interfaceImpl:
        p_interfaceImpl = parseMetadataTable<SgAsmCilInterfaceImpl>(this, buf, index, get_DataSizeFlags(), rows, "interfaceImpl");
        break;
      case e_manifestResource:
        p_manifestResource = parseMetadataTable<SgAsmCilManifestResource>(this, buf, index, get_DataSizeFlags(), rows, "manifestResource");
        break;
      case e_memberRef:
        p_memberRef = parseMetadataTable<SgAsmCilMemberRef>(this, buf, index, get_DataSizeFlags(), rows, "memberRef");
        break;
      case e_methodDef:
        p_methodDef = parseMetadataTable<SgAsmCilMethodDef>(this, buf, index, get_DataSizeFlags(), rows, "methodDef");
        break;
      case e_methodImpl:
        p_methodImpl = parseMetadataTable<SgAsmCilMethodImpl>(this, buf, index, get_DataSizeFlags(), rows, "methodImpl");
        break;
      case e_methodSemantics:
        p_methodSemantics = parseMetadataTable<SgAsmCilMethodSemantics>(this, buf, index, get_DataSizeFlags(), rows, "methodSemantics");
        break;
      case e_methodSpec:
        p_methodSpec = parseMetadataTable<SgAsmCilMethodSpec>(this, buf, index, get_DataSizeFlags(), rows, "methodSpec");
        break;
      case e_module:
        p_module = parseMetadataTable<SgAsmCilModule>(this, buf, index, get_DataSizeFlags(), rows, "module");
        break;
      case e_moduleRef:
        p_moduleRef = parseMetadataTable<SgAsmCilModuleRef>(this, buf, index, get_DataSizeFlags(), rows, "moduleRef");
        break;
      case e_nestedClass:
        p_nestedClass = parseMetadataTable<SgAsmCilNestedClass>(this, buf, index, get_DataSizeFlags(), rows, "nestedClass");
        break;
      case e_param:
        p_param = parseMetadataTable<SgAsmCilParam>(this, buf, index, get_DataSizeFlags(), rows, "param");
        break;
      case e_property:
        p_property = parseMetadataTable<SgAsmCilProperty>(this, buf, index, get_DataSizeFlags(), rows, "property");
        break;
      case e_propertyMap:
        p_propertyMap = parseMetadataTable<SgAsmCilPropertyMap>(this, buf, index, get_DataSizeFlags(), rows, "propertyMap");
        break;
      case e_standAloneSig:
        p_standAloneSig = parseMetadataTable<SgAsmCilStandAloneSig>(this, buf, index, get_DataSizeFlags(), rows, "standAloneSig");
        break;
      case e_typeDef:
        p_typeDef = parseMetadataTable<SgAsmCilTypeDef>(this, buf, index, get_DataSizeFlags(), rows, "typeDef");
        break;
      case e_typeRef:
        p_typeRef = parseMetadataTable<SgAsmCilTypeRef>(this, buf, index, get_DataSizeFlags(), rows, "typeRef");
        break;
      case e_typeSpec:
        p_typeSpec = parseMetadataTable<SgAsmCilTypeSpec>(this, buf, index, get_DataSizeFlags(), rows, "typeSpec");
        break;
      default:
        std::cerr << "default reached:\n"
                  << "parsing not implemented for kind = " << kind << table_kind_to_string(MetaDataHeapTableKinds(kind))
                  << std::endl;
        ROSE_ABORT();
        break;
    }
  }
}

SgAsmCilMetadata*
SgAsmCilMetadataHeap::get_MetadataNode(std::uint32_t refcode, ReferenceKind knd) const
{
  SgAsmCilMetadata*   res    = nullptr;

  // FIXME: maybe cache result in static or in the object if this turns out to be slow
  std::vector<std::int8_t> posInRow = computePositionInRowVector(get_Valid());
  const AccessTuple        access = computeAccessPair(posInRow, get_NumberOfRows(), refcode, knd);
  const std::uint32_t      index  = access.index();

  switch (access.table())
  {

    case e_assembly:
      res = get_assembly().at(index);
      break;
    case e_assemblyProcessor:
      res = get_assemblyProcessor().at(index);
      break;
    case e_assemblyOS:
      res = get_assemblyOS().at(index);
      break;
    case e_assemblyRef:
      res = get_assemblyRef().at(index);
      break;
    case e_assemblyRefProcessor:
      res = get_assemblyRefProcessor().at(index);
      break;
    case e_assemblyRefOS:
      res = get_assemblyRefOS().at(index);
      break;
    case e_classLayout:
      res = get_classLayout().at(index);
      break;
    case e_constant:
      res = get_constant().at(index);
      break;
    case e_customAttribute:
      res = get_customAttribute().at(index);
      break;
    case e_declSecurity:
      res = get_declSecurity().at(index);
      break;
    case e_eventMap:
      res = get_eventMap().at(index);
      break;
    case e_event:
      res = get_event().at(index);
      break;
    case e_exportedType:
      res = get_exportedType().at(index);
      break;
    case e_field:
      res = get_field().at(index);
      break;
    case e_fieldLayout:
      res = get_fieldLayout().at(index);
      break;
    case e_fieldMarshal:
      res = get_fieldMarshal().at(index);
      break;
    case e_fieldRVA:
      res = get_fieldRVA().at(index);
      break;
    case e_file:
      res = get_file().at(index);
      break;
    case e_genericParam:
      res = get_genericParam().at(index);
      break;
    case e_genericParamConstraint:
      res = get_genericParamConstraint().at(index);
      break;
    case e_implMap:
      res = get_implMap().at(index);
      break;
    case e_interfaceImpl:
      res = get_interfaceImpl().at(index);
      break;
    case e_manifestResource:
      res = get_manifestResource().at(index);
      break;
    case e_memberRef:
      res = get_memberRef().at(index);
      break;
    case e_methodDef:
      res = get_methodDef().at(index);
      break;
    case e_methodImpl:
      res = get_methodImpl().at(index);
      break;
    case e_methodSemantics:
      res = get_methodSemantics().at(index);
      break;
    case e_methodSpec:
      res = get_methodSpec().at(index);
      break;
    case e_module:
      res = get_module().at(index);
      break;
    case e_moduleRef:
      res = get_moduleRef().at(index);
      break;
    case e_nestedClass:
      res = get_nestedClass().at(index);
      break;
    case e_param:
      res = get_param().at(index);
      break;
    case e_property:
      res = get_property().at(index);
      break;
    case e_propertyMap:
      res = get_propertyMap().at(index);
      break;
    case e_standAloneSig:
      res = get_standAloneSig().at(index);
      break;
    case e_typeDef:
      res = get_typeDef().at(index);
      break;
    case e_typeRef:
      res = get_typeRef().at(index);
      break;
    case e_typeSpec:
      res = get_typeSpec().at(index);
      break;

    default: ;
  }

  ASSERT_not_null(res);
  return res;
}

////// end from metadataHeap_C.txt


////// from metadataRoot_C.txt

namespace
{

struct MethodHeader : std::tuple<std::uint16_t, std::uint16_t, std::uint32_t, std::uint32_t>
{
  static constexpr std::uint8_t TINY        = 0x2;
  static constexpr std::uint8_t FAT         = 0x3;
  static constexpr std::uint8_t FORMAT      = TINY | FAT;
  static constexpr std::uint8_t MORE_SECTS  = 0x8;
  static constexpr std::uint8_t INIT_LOCALS = 0x10;
  static constexpr std::uint8_t FLAGS       = MORE_SECTS | INIT_LOCALS;

  using base = std::tuple<std::uint16_t, std::uint16_t, std::uint32_t, std::uint32_t>;
  using base::base;

  std::uint16_t flags() const          { return std::get<0>(*this); }
  bool          tiny()  const          { return (flags() & FORMAT) == TINY; }
  bool          moreSections()  const  { return (flags() & FLAGS) == MORE_SECTS; }
  bool          initLocals()  const    { return (flags() & FLAGS) == INIT_LOCALS; }
  std::uint8_t  headerSize() const     { return tiny() ? 1 : (flags() >> 12); }

  std::uint16_t maxStackSize() const   { return std::get<1>(*this); }
  std::uint32_t codeSize() const       { return std::get<2>(*this); }
  std::uint32_t localVarSigTok() const { return std::get<3>(*this); }
};

MethodHeader
parseFatHeader(rose_addr_t base_va, std::uint32_t rva, SgAsmPEFileHeader* fhdr)
{
  std::uint8_t         buf[12];
  const std::size_t    nread = fhdr->get_loader_map()->readQuick(&buf, base_va + rva, sizeof(buf));
  ROSE_ASSERT(nread == 12);

  const std::uint16_t  flags    = ByteOrder::le_to_host(*reinterpret_cast<uint16_t*>(buf+0));
  const std::uint16_t  maxStack = ByteOrder::le_to_host(*reinterpret_cast<uint16_t*>(buf+2));
  const std::uint16_t  codeSize = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(buf+4));
  const std::uint16_t  localIni = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(buf+8));
  const MethodHeader   res{ flags, maxStack, codeSize, localIni };

  ROSE_ASSERT(!res.tiny());
  ROSE_ASSERT(res.headerSize() == 3);
  return res;
}

MethodHeader
parseTinyHeader(std::uint8_t header)
{
  return { header & MethodHeader::FORMAT, 8, header >> 2, 0 };
}

SgAsmBlock*
disassemble(SgAsmCilMethodDef* m, MethodHeader mh, std::vector<std::uint8_t>& buf, const Rose::BinaryAnalysis::Disassembler::Base::Ptr& disasm)
{
  const std::size_t              sz = buf.size();
  rose_addr_t                    addr = 0;
  std::uint8_t*                  b = buf.data();
  std::vector<SgAsmInstruction*> lst;

  while (addr < sz)
  {
    SgAsmInstruction* instr = disasm->disassembleOne(b, 0, sz, addr);
    ASSERT_not_null(instr);

    lst.push_back(instr);

    addr += instr->get_size();
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

  // decode methods
  for (SgAsmCilMethodDef* m : mdh->get_methodDef())
  {
    ASSERT_not_null(m);

    std::size_t nameidx = m->get_Name();
    std::cerr << "proc " << readUtf8String(stringHeap->get_Stream(), nameidx)
              << std::endl;

    // parse header
    std::uint32_t  rva = m->get_RVA();
    std::uint8_t   mh0;
    std::size_t    nread = fhdr->get_loader_map()->readQuick(&mh0, base_va + rva, 1);
    ROSE_ASSERT(nread == 1);

    bool           isTiny = (mh0 & MethodHeader::FORMAT) == MethodHeader::TINY;
    ROSE_ASSERT((!isTiny) || (((base_va+rva)%4) == 0));
    MethodHeader   mh = isTiny ? parseTinyHeader(mh0) : parseFatHeader(base_va, rva, fhdr);

    m->set_stackSize(mh.maxStackSize());
    m->set_hasMoreSections(mh.moreSections());
    m->set_initLocals(mh.initLocals());

    // parse code
    std::uint32_t  codeRVA = rva + mh.headerSize();
    std::uint32_t  codeLen = mh.codeSize();

    std::vector<std::uint8_t> code(codeLen, 0);
    std::size_t    nreadCode = fhdr->get_loader_map()->readQuick(code.data(), base_va + codeRVA, codeLen);
    ROSE_ASSERT(nreadCode == codeLen);

    SgAsmBlock* blk = nullptr;

    switch (m->get_ImplFlags() & CODE_TYPE_MASK)
    {
      namespace rb = Rose::BinaryAnalysis;

      case CIL_CODE:
        //~ std::cerr << "  - disassembling CIL code: " << code.size() << " bytes."
        //~           << std::endl;
        blk = disassemble(m, mh, code, rb::DisassemblerCil::instance());
        break;

      case NATIVE_CODE:
        //~ std::cerr << "  - disassembling x86 code: " << code.size() << " bytes."
        //~           << std::endl;
          blk = disassemble(m, mh, code, rb::Disassembler::X86::instance(4 /* word size */));
        break;

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

}


void SgAsmCilMetadataRoot::parse()
{
  SgAsmCliHeader* clih = isSgAsmCliHeader(get_parent());
  ASSERT_not_null(clih);

  SgAsmPEFileHeader* fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(this);
  ASSERT_not_null(fhdr);

  uint64_t    metaData = clih->get_metaData();
  uint8_t*    data = reinterpret_cast<uint8_t*>(&metaData);
  rose_addr_t rva = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(data));
  size_t      size = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(data+4));
  rose_addr_t base_va = clih->get_base_va();
  rose_addr_t rva_offset = clih->get_rva_offset(rva);

  if (TRACE_CONSTRUCTION)
  {
    std::cerr << "------------------------SgAsmCilMetadataRoot::parse-----------------------------\n";
    std::cerr << "    rva: " << rva << " size: " << size << std::endl;
    std::cerr << "    base_va: " << base_va << " rva_offset: " << rva_offset << std::endl;
  }

  /* Read the Signature via loader map. */
  // Note: probably want to allocate a larger buffer
  std::vector<uint8_t> buf(size, 0);

  size_t nread = fhdr->get_loader_map()->readQuick(buf.data(), base_va + rva, size);
  ROSE_ASSERT(nread == size);

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
    std::cerr << "Streams has " << p_Streams.size() << "elements." << std::endl;
}


const std::vector<SgAsmCilDataStream*>&
SgAsmCilMetadataRoot::get_Streams() const
{
  return p_Streams;
}

std::vector<SgAsmCilDataStream*>&
SgAsmCilMetadataRoot::get_Streams()
{
  return p_Streams;
}

namespace
{
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

////// end from metadataRoot_C.txt



#endif /* ROSE_ENABLE_BINARY_ANALYSIS */
