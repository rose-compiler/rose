/* CLI Runtime Header */
#include <featureTests.h>

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

// Please delete this capability any time after FY22 (October 2022)
#define PRINT_DEBUG 0

using namespace Rose::BinaryAnalysis;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmCliHeader::SgAsmCliHeader(SgAsmPEFileHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();
    get_name()->set_string("CLI Runtime Section");
}

SgAsmCliHeader* SgAsmCliHeader::parse()
{
  SgAsmGenericSection::parse();

  ROSE_ASSERT("CLR Runtime Header" == get_name()->get_string());

  /* Size of the header in bytes */
  auto data = p_data.pool();
  ASSERT_not_null(data);

  p_cb = ByteOrder::leToHost(*reinterpret_cast<uint32_t*>(data));
  data += 4;

  /* The major portion of the minimum version of the runtime required to run this program */
  p_majorRuntimeVersion = ByteOrder::leToHost(*reinterpret_cast<uint16_t*>(data));
  data += 2;

  /* The minor portion of the version */
  p_minorRuntimeVersion = ByteOrder::leToHost(*reinterpret_cast<uint16_t*>(data));
  data += 2;

  /* RVA and size of implementation-specific resources */
  p_metaData = ByteOrder::leToHost(*reinterpret_cast<uint64_t*>(data));
  data += 8;

  /* Flags describing this runtime image */
  p_flags = ByteOrder::leToHost(*reinterpret_cast<uint32_t*>(data));
  data += 4;

  /* Token for the MethodDef or File of the entry point for the image */
  p_entryPointToken = ByteOrder::leToHost(*reinterpret_cast<uint32_t*>(data));
  data += 4;
  
  /* RVA and size of implementation-specific resources */
  p_resources = ByteOrder::leToHost(*reinterpret_cast<uint64_t*>(data));
  data += 8;

  /* RVA of the hash data for this PE file used by the CLI loader for binding and versioning */
  p_strongNameSignature = ByteOrder::leToHost(*reinterpret_cast<uint64_t*>(data));
  data += 8;

  /* Always 0 */
  p_codeManagerTable = ByteOrder::leToHost(*reinterpret_cast<uint64_t*>(data));
  ROSE_ASSERT(0 == p_codeManagerTable && "Always 0");
  data += 8;

  /* RVA of an array of locations in the file that contain an array of function pointers (e.g., vtable slots) */
  p_vTableFixups = ByteOrder::leToHost(*reinterpret_cast<uint64_t*>(data));
  data += 8;

  /* Always 0 */
  p_exportAddressTableJumps = ByteOrder::leToHost(*reinterpret_cast<uint64_t*>(data));
  ROSE_ASSERT(0 == p_exportAddressTableJumps && "Always 0");
  data += 8;

  /* Always 0 */
  p_managedNativeHeader = ByteOrder::leToHost(*reinterpret_cast<uint64_t*>(data));
  ROSE_ASSERT(0 == p_managedNativeHeader && "Always 0");
  data += 8;
  
  /* Construct and parse the CIL metatdata root */
  SgAsmCilMetadataRoot* metadata_root = new SgAsmCilMetadataRoot;
  ASSERT_not_null(metadata_root);
  
  /* metadataRoot must be set before parsing */
  this->set_metadataRoot(metadata_root);
  metadata_root->set_parent(this);

  metadata_root->parse();
  
  return this;
}

void SgAsmCliHeader::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:\n", prefix);
  fprintf(f, "   name: %s\n", get_name()->get_string().c_str());
  fprintf(f, "   offset: %llu\n", get_offset());
  fprintf(f, "   data: %s\n", &(p_data[0]));
  fprintf(f, "   cb: %u\n", p_cb);
  fprintf(f, "   majorRuntimeVersion: %u\n", p_majorRuntimeVersion);
  fprintf(f, "   minorRuntimeVersion: %u\n", p_minorRuntimeVersion);
  fprintf(f, "   metaData: %llu\n", p_metaData);
  fprintf(f, "   flags: %u\n", p_flags);
  fprintf(f, "   entryPointToken: %u\n", p_entryPointToken);
  fprintf(f, "   resources: %llu\n", p_resources);
  fprintf(f, "   strongNameSignature: %llu\n", p_strongNameSignature);
  fprintf(f, "   codeManagerTable: %llu\n", p_codeManagerTable);
  fprintf(f, "   vTableFixups: %llu\n", p_vTableFixups);
  fprintf(f, "   exportAddressTableJumps: %llu\n", p_exportAddressTableJumps);
  fprintf(f, "   managedNativeHeader: %llu\n", p_managedNativeHeader);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
