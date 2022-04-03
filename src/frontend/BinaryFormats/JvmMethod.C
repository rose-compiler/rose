/* JVM Methods */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Rose::Diagnostics;
using namespace ByteOrder;

SgAsmJvmMethod::SgAsmJvmMethod(SgAsmJavaClassFile* jcf, SgAsmGenericHeader* fhdr)
  : SgAsmGenericSection(fhdr->get_file(), fhdr)
{
  std::cout << "SgAsmJvmMethod::ctor() ...\n";
  set_parent(jcf);
}

SgAsmJvmMethod* SgAsmJvmMethod::parse(SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmMethod::parse() ...\n";

  auto jcf = dynamic_cast<SgAsmJavaClassFile*>(get_parent());
  ROSE_ASSERT(jcf);
#ifdef PUT_BACK
  auto pool = jcf->get_constant_pool();
#endif
  ROSE_ASSERT(pool && "JVM constant pool is a nullptr");

  SgAsmGenericHeader* header = get_header();
  rose_addr_t offset{header->get_offset()};
  std::cout << "SgAsmJvmMethod::parse() offset is " << offset << std::endl;

  // access_flags
  //
  auto count = read_content(offset, &p_access_flags, sizeof p_access_flags);
  if (2 != count) {
    throw FormatError("Bad Java class file access_flags");
  }
  p_access_flags = be_to_host(p_access_flags);
  offset += count;
  header->set_offset(offset);

  std::cout << "SgAsmJvmMethod::p_access_flags " << p_access_flags << std::endl;

  // name_index
  //
  count = read_content(offset, &p_name_index, sizeof p_name_index);
  if (2 != count) {
    throw FormatError("Bad Java class file name_index");
  }
  p_name_index = be_to_host(p_name_index);
  offset += count;
  header->set_offset(offset);

  std::cout << "SgAsmJvmMethod::p_name_index " << p_name_index << std::endl;

  // descriptor_index
  //
  count = read_content(offset, &p_descriptor_index, sizeof p_descriptor_index);
  if (2 != count) {
    throw FormatError("Bad Java class file descriptor_index");
  }
  p_descriptor_index = be_to_host(p_descriptor_index);
  offset += count;
  header->set_offset(offset);

  std::cout << "SgAsmJvmMethod::p_descriptor_index " << p_descriptor_index << std::endl;

  // Attributes
  //
  uint16_t attributes_count;
  count = header->read_content(offset, &attributes_count, sizeof attributes_count);
  if (2 != count) {
    throw FormatError("Bad Java class file attributes_count");
  }
  attributes_count = be_to_host(attributes_count);
  offset += count;
  header->set_offset(offset);

  std::cout << "SgAsmJvmFileHeader::attributes_count " << attributes_count << std::endl;
  //TODO:loop
  if (attributes_count) {
    auto attribute = SgAsmJvmAttribute::create_attribute(pool);
  }

  std::cout << "SgAsmJvmMethod::parse() exit ... \n\n";

  return this;
}

void SgAsmJvmMethod::dump(std::ostream &os)
{
  os << "SgAsmJvmMethod::dump\n";
}

#endif
