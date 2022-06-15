// This file CilImpl.C has been generated from a JSON description file using json2rosetta.
//   Do not modify this file directly, but the corresponding JSON file.

/* CIL Implementation for Classes defined in src/ROSETTA/src/BinaryInstruction.C */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

namespace
{
  constexpr bool TRACE_CONSTRUCTION = false;

uint8_t read8bitValue (uint8_t* buf, size_t & index)
   {
  // This function abstracts the details of reading 2 byte values from the disk image.
     uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+index)));
     index += 1;

     return value;
   }

uint8_t read8bitPadding (uint8_t* buf, size_t & index, uint8_t expected)
   {
     const uint8_t value = read8bitValue(buf, index);

     ROSE_ASSERT(value == expected);
     return value;
   }


uint16_t read16bitValue (uint8_t* buf, size_t & index)
   {
  // This function abstracts the details of reading 2 byte values from the disk image.
     uint16_t value = ByteOrder::le_to_host(*((uint16_t*)(buf+index)));
     index += 2;

     return value;
   }

uint32_t read32bitValue (uint8_t* buf, size_t & index)
   {
  // This function abstracts the details of reading 4 byte values from the disk image.
     uint32_t value = ByteOrder::le_to_host(*((uint32_t*)(buf+index)));
     index += 4;

     return value;
   }

uint32_t readValue (uint8_t* buf, size_t & index, bool uses4byteIndexing)
   {
  // This function abstracts the details of reading 2 byte or 4 byte values from the disk image.
     uint32_t value = 0;
     if (uses4byteIndexing == true)
        {
          value = read32bitValue(buf,index);
        }
       else
        {
          value = read16bitValue(buf,index);
        }

     return value;
   }
}



void SgAsmCilAssembly::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_HashAlgId = read32bitValue(buf,index);
     p_MajorVersion = read16bitValue(buf,index);
     p_MinorVersion = read16bitValue(buf,index);
     p_BuildNumber = read16bitValue(buf,index);
     p_RevisionNumber = read16bitValue(buf,index);
     p_Flags = read32bitValue(buf,index);
     p_PublicKey = readValue(buf,index,uses4byteIndexing);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_Culture = readValue(buf,index,uses4byteIndexing);

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

void SgAsmCilAssemblyOS::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
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

void SgAsmCilAssemblyProcessor::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Processor = read32bitValue(buf,index);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Processor = " << p_Processor << std::endl;
        }
   }

void SgAsmCilAssemblyRef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_MajorVersion = read16bitValue(buf,index);
     p_MinorVersion = read16bitValue(buf,index);
     p_BuildNumber = read16bitValue(buf,index);
     p_RevisionNumber = read16bitValue(buf,index);
     p_Flags = read32bitValue(buf,index);
     p_PublicKeyOrToken = readValue(buf,index,uses4byteIndexing);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_Culture = readValue(buf,index,uses4byteIndexing);
     p_HashValue = readValue(buf,index,uses4byteIndexing);

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

void SgAsmCilAssemblyRefOS::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_OSPlatformID = read32bitValue(buf,index);
     p_OSMajorVersion = read32bitValue(buf,index);
     p_OSMinorVersion = read32bitValue(buf,index);
     p_AssemblyRefOS = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_OSPlatformID = " << p_OSPlatformID << std::endl;
          std::cerr << "p_OSMajorVersion = " << p_OSMajorVersion << std::endl;
          std::cerr << "p_OSMinorVersion = " << p_OSMinorVersion << std::endl;
          std::cerr << "p_AssemblyRefOS = " << p_AssemblyRefOS << std::endl;
        }
   }

void SgAsmCilAssemblyRefProcessor::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Processor = read32bitValue(buf,index);
     p_AssemblyRef = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Processor = " << p_Processor << std::endl;
          std::cerr << "p_AssemblyRef = " << p_AssemblyRef << std::endl;
        }
   }

void SgAsmCilClassLayout::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_PackingSize = read16bitValue(buf,index);
     p_ClassSize = read32bitValue(buf,index);
     p_Parent = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_PackingSize = " << p_PackingSize << std::endl;
          std::cerr << "p_ClassSize = " << p_ClassSize << std::endl;
          std::cerr << "p_Parent = " << p_Parent << std::endl;
        }
   }

void SgAsmCilConstant::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Type = read8bitValue(buf,index);
     p_Padding = read8bitPadding(buf,index,0);
     p_Parent = readValue(buf,index,uses4byteIndexing);
     p_Value = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Type = " << p_Type << std::endl;
          std::cerr << "p_Padding = " << p_Padding << std::endl;
          std::cerr << "p_Parent = " << p_Parent << std::endl;
          std::cerr << "p_Value = " << p_Value << std::endl;
        }
   }

void SgAsmCilCustomAttribute::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Parent = readValue(buf,index,uses4byteIndexing);
     p_Type = readValue(buf,index,uses4byteIndexing);
     p_Value = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Parent = " << p_Parent << std::endl;
          std::cerr << "p_Type = " << p_Type << std::endl;
          std::cerr << "p_Value = " << p_Value << std::endl;
        }
   }

void SgAsmCilDeclSecurity::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Action = read16bitValue(buf,index);
     p_Parent = readValue(buf,index,uses4byteIndexing);
     p_PermissionSet = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Action = " << p_Action << std::endl;
          std::cerr << "p_Parent = " << p_Parent << std::endl;
          std::cerr << "p_PermissionSet = " << p_PermissionSet << std::endl;
        }
   }

void SgAsmCilEvent::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_EventFlags = read16bitValue(buf,index);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_EventType = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_EventFlags = " << p_EventFlags << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
          std::cerr << "p_EventType = " << p_EventType << std::endl;
        }
   }

void SgAsmCilEventMap::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Parent = readValue(buf,index,uses4byteIndexing);
     p_EventList = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Parent = " << p_Parent << std::endl;
          std::cerr << "p_EventList = " << p_EventList << std::endl;
        }
   }

void SgAsmCilExportedType::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_EventFlags = read32bitValue(buf,index);
     p_TypeDefIdName = read32bitValue(buf,index);
     p_TypeName = readValue(buf,index,uses4byteIndexing);
     p_TypeNamespace = readValue(buf,index,uses4byteIndexing);
     p_Implementation = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_EventFlags = " << p_EventFlags << std::endl;
          std::cerr << "p_TypeDefIdName = " << p_TypeDefIdName << std::endl;
          std::cerr << "p_TypeName = " << p_TypeName << std::endl;
          std::cerr << "p_TypeNamespace = " << p_TypeNamespace << std::endl;
          std::cerr << "p_Implementation = " << p_Implementation << std::endl;
        }
   }

void SgAsmCilField::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Flags = read16bitValue(buf,index);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_Signature = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Flags = " << p_Flags << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
          std::cerr << "p_Signature = " << p_Signature << std::endl;
        }
   }

void SgAsmCilFieldLayout::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Offset = read32bitValue(buf,index);
     p_Field = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Offset = " << p_Offset << std::endl;
          std::cerr << "p_Field = " << p_Field << std::endl;
        }
   }

void SgAsmCilFieldMarshal::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Parent = readValue(buf,index,uses4byteIndexing);
     p_NativeType = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Parent = " << p_Parent << std::endl;
          std::cerr << "p_NativeType = " << p_NativeType << std::endl;
        }
   }

void SgAsmCilFieldRVA::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_RVA = (read16bitValue(buf,index));
     p_Field = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_RVA = " << p_RVA << std::endl;
          std::cerr << "p_Field = " << p_Field << std::endl;
        }
   }

void SgAsmCilFile::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Flags = read32bitValue(buf,index);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_HashValue = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Flags = " << p_Flags << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
          std::cerr << "p_HashValue = " << p_HashValue << std::endl;
        }
   }

void SgAsmCilGenericParam::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Number = read16bitValue(buf,index);
     p_Flags = read16bitValue(buf,index);
     p_Owner = readValue(buf,index,uses4byteIndexing);
     p_Name = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Number = " << p_Number << std::endl;
          std::cerr << "p_Flags = " << p_Flags << std::endl;
          std::cerr << "p_Owner = " << p_Owner << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
        }
   }

void SgAsmCilGenericParamConstraint::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Owner = readValue(buf,index,uses4byteIndexing);
     p_Constraint = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Owner = " << p_Owner << std::endl;
          std::cerr << "p_Constraint = " << p_Constraint << std::endl;
        }
   }

void SgAsmCilImplMap::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_MappingFlags = read16bitValue(buf,index);
     p_MemberForwarded = readValue(buf,index,uses4byteIndexing);
     p_ImportName = readValue(buf,index,uses4byteIndexing);
     p_ImportScope = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_MappingFlags = " << p_MappingFlags << std::endl;
          std::cerr << "p_MemberForwarded = " << p_MemberForwarded << std::endl;
          std::cerr << "p_ImportName = " << p_ImportName << std::endl;
          std::cerr << "p_ImportScope = " << p_ImportScope << std::endl;
        }
   }

void SgAsmCilInterfaceImpl::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Class = (read16bitValue(buf,index));
     p_Interface = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Class = " << p_Class << std::endl;
          std::cerr << "p_Interface = " << p_Interface << std::endl;
        }
   }

void SgAsmCilManifestResource::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Offset = read32bitValue(buf,index);
     p_Flags = read32bitValue(buf,index);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_Implementation = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Offset = " << p_Offset << std::endl;
          std::cerr << "p_Flags = " << p_Flags << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
          std::cerr << "p_Implementation = " << p_Implementation << std::endl;
        }
   }

void SgAsmCilMemberRef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Class = readValue(buf,index,uses4byteIndexing);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_Signature = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Class = " << p_Class << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
          std::cerr << "p_Signature = " << p_Signature << std::endl;
        }
   }

void SgAsmCilMethodDef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_RVA = read32bitValue(buf,index);
     p_ImplFlags = (read16bitValue(buf,index));
     p_Flags = read16bitValue(buf,index);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_Signature = readValue(buf,index,uses4byteIndexing);
     p_ParamList = readValue(buf,index,uses4byteIndexing);

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

void SgAsmCilMethodImpl::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Class = readValue(buf,index,uses4byteIndexing);
     p_MethodBody = readValue(buf,index,uses4byteIndexing);
     p_MethodDeclaration = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Class = " << p_Class << std::endl;
          std::cerr << "p_MethodBody = " << p_MethodBody << std::endl;
          std::cerr << "p_MethodDeclaration = " << p_MethodDeclaration << std::endl;
        }
   }

void SgAsmCilMethodSemantics::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Semantics = read16bitValue(buf,index);
     p_Method = readValue(buf,index,uses4byteIndexing);
     p_Association = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Semantics = " << p_Semantics << std::endl;
          std::cerr << "p_Method = " << p_Method << std::endl;
          std::cerr << "p_Association = " << p_Association << std::endl;
        }
   }

void SgAsmCilMethodSpec::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Method = readValue(buf,index,uses4byteIndexing);
     p_Instantiation = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Method = " << p_Method << std::endl;
          std::cerr << "p_Instantiation = " << p_Instantiation << std::endl;
        }
   }

void SgAsmCilModule::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Generation = read16bitValue(buf,index);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_Mvid = readValue(buf,index,uses4byteIndexing);
     p_Encld = readValue(buf,index,uses4byteIndexing);
     p_EncBaseId = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Generation = " << p_Generation << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
          std::cerr << "p_Mvid = " << p_Mvid << std::endl;
          std::cerr << "p_Encld = " << p_Encld << std::endl;
          std::cerr << "p_EncBaseId = " << p_EncBaseId << std::endl;
        }
   }

void SgAsmCilModuleRef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Name = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Name = " << p_Name << std::endl;
        }
   }

void SgAsmCilNestedClass::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_NestedClass = readValue(buf,index,uses4byteIndexing);
     p_EnclosingClass = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_NestedClass = " << p_NestedClass << std::endl;
          std::cerr << "p_EnclosingClass = " << p_EnclosingClass << std::endl;
        }
   }

void SgAsmCilParam::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Flags = read16bitValue(buf,index);
     p_Sequence = read16bitValue(buf,index);
     p_Name = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Flags = " << p_Flags << std::endl;
          std::cerr << "p_Sequence = " << p_Sequence << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
        }
   }

void SgAsmCilProperty::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Flags = read16bitValue(buf,index);
     p_Name = readValue(buf,index,uses4byteIndexing);
     p_Type = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Flags = " << p_Flags << std::endl;
          std::cerr << "p_Name = " << p_Name << std::endl;
          std::cerr << "p_Type = " << p_Type << std::endl;
        }
   }

void SgAsmCilPropertyMap::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Parent = readValue(buf,index,uses4byteIndexing);
     p_PropertyList = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Parent = " << p_Parent << std::endl;
          std::cerr << "p_PropertyList = " << p_PropertyList << std::endl;
        }
   }

void SgAsmCilStandAloneSig::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Signature = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Signature = " << p_Signature << std::endl;
        }
   }

void SgAsmCilTypeDef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Flags = read32bitValue(buf,index);
     p_TypeName = readValue(buf,index,uses4byteIndexing);
     p_TypeNamespace = readValue(buf,index,uses4byteIndexing);
     p_Extends = readValue(buf,index,uses4byteIndexing);
     p_FieldList = readValue(buf,index,uses4byteIndexing);
     p_MethodList = readValue(buf,index,uses4byteIndexing);

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

void SgAsmCilTypeRef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_ResolutionScope = read16bitValue(buf,index);
     p_TypeName = readValue(buf,index,uses4byteIndexing);
     p_TypeNamespace = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_ResolutionScope = " << p_ResolutionScope << std::endl;
          std::cerr << "p_TypeName = " << p_TypeName << std::endl;
          std::cerr << "p_TypeNamespace = " << p_TypeNamespace << std::endl;
        }
   }

void SgAsmCilTypeSpec::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
   { 
     p_Signature = readValue(buf,index,uses4byteIndexing);

     if (TRACE_CONSTRUCTION)
        { 
          std::cerr << "p_Signature = " << p_Signature << std::endl;
        }
   }

#endif /* ROSE_ENABLE_BINARY_ANALYSIS */
