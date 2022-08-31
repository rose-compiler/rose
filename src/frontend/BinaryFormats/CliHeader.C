/* CLI Runtime Header */
#include <featureTests.h>

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

// Please delete this capability any time after FY22 (October 2022)
#define PRINT_DEBUG 0

using namespace Rose::BinaryAnalysis;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmCliHeader::SgAsmCliHeader(SgAsmPEFileHeader *fhdr)
  : SgAsmGenericSection(fhdr->get_file(), fhdr), p_cb{0}, p_majorRuntimeVersion{0}, p_minorRuntimeVersion{0}, 
    p_metaData{0}, p_flags{0}, p_entryPointToken{0}, p_resources{0}, p_strongNameSignature{0},
    p_codeManagerTable{0}, p_vTableFixups{0}, p_exportAddressTableJumps{0}, p_managedNativeHeader{0}

{
  get_name()->set_string("CLI Runtime Section");
}

SgAsmCliHeader* SgAsmCliHeader::parse()
{
  SgAsmGenericSection::parse();

  ROSE_ASSERT("CLR Runtime Header" == get_name()->get_string());

  /* Size of the header in bytes */
  auto data = p_data.pool();
  ASSERT_not_null(data);

  p_cb = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(data));
  data += 4;

  /* The major portion of the minimum version of the runtime required to run this program */
  p_majorRuntimeVersion = ByteOrder::le_to_host(*reinterpret_cast<uint16_t*>(data));
  data += 2;

  /* The minor portion of the version */
  p_minorRuntimeVersion = ByteOrder::le_to_host(*reinterpret_cast<uint16_t*>(data));
  data += 2;

  /* RVA and size of implementation-specific resources */
  p_metaData = ByteOrder::le_to_host(*reinterpret_cast<uint64_t*>(data));
  rose_addr_t rva = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(data));
  size_t size = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(data+4));
  data += 8;

  /* Flags describing this runtime image */
  p_flags = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(data));
  data += 4;

  /* Token for the MethodDef or File of the entry point for the image */
  p_entryPointToken = ByteOrder::le_to_host(*reinterpret_cast<uint32_t*>(data));
  data += 4;
  
  /* RVA and size of implementation-specific resources */
  p_resources = ByteOrder::le_to_host(*reinterpret_cast<uint64_t*>(data));
  data += 8;

  /* RVA of the hash data for this PE file used by the CLI loader for binding and versioning */
  p_strongNameSignature = ByteOrder::le_to_host(*reinterpret_cast<uint64_t*>(data));
  data += 8;

  /* Always 0 */
  p_codeManagerTable = ByteOrder::le_to_host(*reinterpret_cast<uint64_t*>(data));
  ROSE_ASSERT(0 == p_codeManagerTable && "Always 0");
  data += 8;

  /* RVA of an array of locations in the file that contain an array of function pointers (e.g., vtable slots) */
  p_vTableFixups = ByteOrder::le_to_host(*reinterpret_cast<uint64_t*>(data));
  data += 8;

  /* Always 0 */
  p_exportAddressTableJumps = ByteOrder::le_to_host(*reinterpret_cast<uint64_t*>(data));
  ROSE_ASSERT(0 == p_exportAddressTableJumps && "Always 0");
  data += 8;

  /* Always 0 */
  p_managedNativeHeader = ByteOrder::le_to_host(*reinterpret_cast<uint64_t*>(data));
  ROSE_ASSERT(0 == p_managedNativeHeader && "Always 0");
  data += 8;
  
#if PRINT_DEBUG
  std::cout << "------------------------SgAsmCliHeader::parse-----------------------------\n";
  std::cout << "... SgAsmCliHeader: name: " << get_name()->get_string()
            << ": offset: " << get_offset() << ": p_data: "
            << &(p_data[0])
            << std::endl;
  std::cout << "... SgAsmCliHeader: cb: " << p_cb << std::endl;
  std::cout << "... SgAsmCliHeader: majorRuntimeVersion: " << p_majorRuntimeVersion << std::endl;
  std::cout << "... SgAsmCliHeader: minorRuntimeVersion: " << p_minorRuntimeVersion << std::endl;
  std::cout << "... SgAsmCliHeader: metaData: " << p_metaData << std::endl;
  std::cout << "... SgAsmCliHeader: flags: " << p_flags << std::endl;
  std::cout << "... SgAsmCliHeader: entryPointToken: " << p_entryPointToken << std::endl;
  std::cout << "... SgAsmCliHeader: resources: " << p_resources << std::endl;
  std::cout << "... SgAsmCliHeader: strongNameSignature: " << p_strongNameSignature << std::endl;
  std::cout << "... SgAsmCliHeader: codeManagerTable: " << p_codeManagerTable << std::endl;
  std::cout << "... SgAsmCliHeader: vTableFixups: " << p_vTableFixups << std::endl;
  std::cout << "... SgAsmCliHeader: exportAddressTableJumps: " << p_exportAddressTableJumps << std::endl;
  std::cout << "... SgAsmCliHeader: managedNativeHeader: " << p_managedNativeHeader << std::endl;
  std::cout << "\n";
#endif

  /* Construct and parse the CIL metatdata root */
  SgAsmCilMetadataRoot* metadata_root = new SgAsmCilMetadataRoot;
  ASSERT_not_null(metadata_root);
  metadata_root->set_parent(this);
  metadata_root->parse();

  return this;
}

void SgAsmCliHeader::dump(FILE*f, const char* prefix, ssize_t idx) const
{
#if PRINT_DEBUG
  std::cout << "SgAsmCliHeader::dump\n";
#endif
  //TODO
  //fprintf(f, "%s:%d:%d:%d\n", prefix, p_access_flags, p_name_index, p_descriptor_index);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
