// This file CilImpl.C has been generated from a JSON description file.
//   Do not modify this file directly, but the corresponding JSON file.

/* CIL Implementation for Classes defined in src/ROSETTA/src/BinaryInstruction.C */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

namespace
{

uint8_t read8bitValue (uint8_t* buf, size_t & index)
   {
  // This function abstracts the details of reading 2 byte values from the disk image.
     uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+index)));
     index += 1;

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
}

void SgAsmCilAssemblyOS::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_OSPlatformID = read32bitValue(buf,index);
  p_OSMajorVersion = read32bitValue(buf,index);
  p_OSMinorVersion = read32bitValue(buf,index);
}

void SgAsmCilAssemblyProcessor::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Processor = read32bitValue(buf,index);
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
}

void SgAsmCilAssemblyRefOS::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_OSPlatformID = read32bitValue(buf,index);
  p_OSMajorVersion = read32bitValue(buf,index);
  p_OSMinorVersion = read32bitValue(buf,index);
  p_AssemblyRefOS = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilAssemblyRefProcessor::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Processor = read32bitValue(buf,index);
  p_AssemblyRef = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilClassLayout::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_PackingSize = read16bitValue(buf,index);
  p_ClassSize = read32bitValue(buf,index);
  p_Parent = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilConstant::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Type = read8bitValue(buf,index);
  p_Parent = readValue(buf,index,uses4byteIndexing);
  p_Value = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilCustomAttribute::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Parent = readValue(buf,index,uses4byteIndexing);
  p_Type = readValue(buf,index,uses4byteIndexing);
  p_Value = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilDeclSecurity::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Action = read16bitValue(buf,index);
  p_Parent = readValue(buf,index,uses4byteIndexing);
  p_PermissionSet = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilEvent::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_EventFlags = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
  p_EventType = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilEventMap::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Parent = readValue(buf,index,uses4byteIndexing);
  p_EventList = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilExportedType::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_EventFlags = read32bitValue(buf,index);
  p_TypeDefIdName = read32bitValue(buf,index);
  p_TypeName = readValue(buf,index,uses4byteIndexing);
  p_TypeNamespace = readValue(buf,index,uses4byteIndexing);
  p_Implementation = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilField::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Flags = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
  p_Signature = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilFieldLayout::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Offset = read32bitValue(buf,index);
  p_Field = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilFieldMarshal::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Parent = readValue(buf,index,uses4byteIndexing);
  p_NativeType = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilFieldRVA::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_RVA = (read16bitValue(buf,index));
  p_Field = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilFile::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Flags = read32bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
  p_HashValue = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilGenericParam::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Number = read16bitValue(buf,index);
  p_Flags = read16bitValue(buf,index);
  p_Owner = readValue(buf,index,uses4byteIndexing);
  p_Name = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilGenericParamConstraint::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Owner = readValue(buf,index,uses4byteIndexing);
  p_Constraint = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilImplMap::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_MappingFlags = read16bitValue(buf,index);
  p_MemberForwarded = readValue(buf,index,uses4byteIndexing);
  p_ImportName = readValue(buf,index,uses4byteIndexing);
  p_ImportScope = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilInterfaceImpl::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Class = (read16bitValue(buf,index));
  p_Interface = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilManifestResource::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Offset = read32bitValue(buf,index);
  p_Flags = read32bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
  p_Implementation = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilMemberRef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Offset = read32bitValue(buf,index);
  p_Flags = read32bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
  p_Implementation = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilMethodDef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_RVA = read32bitValue(buf,index);
  p_ImplFlags = (read16bitValue(buf,index));
  p_Flags = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
  p_Signature = readValue(buf,index,uses4byteIndexing);
  p_ParamList = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilMethodImpl::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Class = readValue(buf,index,uses4byteIndexing);
  p_MethodBody = readValue(buf,index,uses4byteIndexing);
  p_MethodDeclaration = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilMethodSemantics::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Semantics = read16bitValue(buf,index);
  p_Method = readValue(buf,index,uses4byteIndexing);
  p_Association = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilMethodSpec::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Method = readValue(buf,index,uses4byteIndexing);
  p_Instantiation = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilModule::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Generation = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
  p_Mvid = readValue(buf,index,uses4byteIndexing);
  p_Encld = readValue(buf,index,uses4byteIndexing);
  p_EncBaseId = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilModuleRef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Name = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilNestedClass::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_NestedClass = readValue(buf,index,uses4byteIndexing);
  p_EnclosingClass = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilParam::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Flags = read16bitValue(buf,index);
  p_Sequence = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilProperty::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Flags = read16bitValue(buf,index);
  p_Name = readValue(buf,index,uses4byteIndexing);
  p_Type = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilPropertyMap::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Parent = readValue(buf,index,uses4byteIndexing);
  p_PropertyList = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilStandAloneSig::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Signature = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilTypeDef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Flags = read32bitValue(buf,index);
  p_TypeName = readValue(buf,index,uses4byteIndexing);
  p_TypeNamespace = readValue(buf,index,uses4byteIndexing);
  p_Extends = readValue(buf,index,uses4byteIndexing);
  p_FieldList = readValue(buf,index,uses4byteIndexing);
  p_MethodList = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilTypeRef::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_ResolutionScope = read16bitValue(buf,index);
  p_TypeName = readValue(buf,index,uses4byteIndexing);
  p_TypeNamespace = readValue(buf,index,uses4byteIndexing);
}

void SgAsmCilTypeSpec::parse(uint8_t* buf, size_t& index, bool uses4byteIndexing)
{
  p_Signature = readValue(buf,index,uses4byteIndexing);
}

#endif /* ROSE_ENABLE_BINARY_ANALYSIS */
