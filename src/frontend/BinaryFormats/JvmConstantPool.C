/* JVM Constant Pool section (SgAsmJvmConstantPool and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

using namespace Rose::Diagnostics;
using namespace ByteOrder;

SgAsmJvmConstantPool::
CONSTANT_Class_info::CONSTANT_Class_info(SgAsmGenericHeader* h)
  : SgAsmJvmConstantPool::cp_info{cp_info::CONSTANT_Class}
{
  auto offset = h->get_offset();
  auto count = h->read_content(offset, &name_index, sizeof name_index);
  if (2 != count) std::cout << "Bad CONSTANT_Class_info::name_index\n";
  name_index = be_to_host(name_index);
  offset += count;

  std::cout << "CONSTANT_Class_info:" << name_index << std::endl;
  h->set_offset(offset);
}

SgAsmJvmConstantPool::
CONSTANT_Methodref_info::CONSTANT_Methodref_info(SgAsmGenericHeader* h)
  : SgAsmJvmConstantPool::cp_info{cp_info::CONSTANT_Methodref}
{
  auto offset = h->get_offset();
  auto count = h->read_content(offset, &class_index, sizeof class_index);
  if (2 != count) std::cout << "Bad CONSTANT_Methodref_info::class_index\n";
  class_index = be_to_host(class_index);
  offset += count;

  count = h->read_content(offset, &name_and_type_index, sizeof name_and_type_index);
  if (2 != count) std::cout << "Bad CONSTANT_Methodref_info::name_and_type_index\n";
  name_and_type_index = be_to_host(name_and_type_index);
  offset += count;

  std::cout << "CONSTANT_Methodref_info:" << class_index << ":" << name_and_type_index << std::endl;
  h->set_offset(offset);
}

SgAsmJvmConstantPool* SgAsmJvmConstantPool::parse()
{
  std::cout << "SgAsmJvmConstantPool::parse() ...\n";

  auto header = get_header();
  rose_addr_t offset = header->get_offset();

  std::cout << "SgAsmJvmConstantPool::parse() this offset is " << get_offset() << std::endl;
  std::cout << "SgAsmJvmConstantPool::parse() header offset is " << offset << std::endl;

// NOTE: refactor this once SgJavaClassFile exists
#if 1
  /* Ensure magic number in file is correct */
  unsigned char magic[4];
  auto count = read_content(offset, magic, sizeof magic);
  if (4!=count || 0xCA!=magic[0] || 0xFE!=magic[1] || 0xBA!=magic[2] || 0xBE!=magic[3]) {
    std::cout << "Bad Java class file magic number\n";
  }
  offset += count;

  /* Minor version */
  uint16_t minor_version;
  count = read_content(offset, &minor_version, sizeof minor_version);
  if (2 != count) std::cout << "Bad Java class file minor version\n";
  minor_version = be_to_host(minor_version);
  offset += count;
  std::cout << "SgAsmJvmConstantPool::parse() offset is " << get_offset() << std::endl;

  /* Major version */
  uint16_t major_version;
  count = read_content(offset, &major_version, sizeof major_version);
  if (2 != count) std::cout << "Bad Java class file major version\n";
  major_version = be_to_host(major_version);
  offset += count;

  set_offset(offset);
  std::cout << "SgAsmJvmConstantPool::parse() offset is " << get_offset() << std::endl;
#endif

  /* Constant pool count */
  uint16_t constant_pool_count;
  count = header->read_content(offset, &constant_pool_count, sizeof constant_pool_count);
  if (2 != count) throw FormatError("Bad Java class file constant_pool_count");
  constant_pool_count = be_to_host(constant_pool_count);
  offset +=count;
  std::cout << "SgAsmJvmConstantPool::parse() constant_pool_count is " << constant_pool_count << std::endl;

  /* tag */
  uint8_t tag;
  count = header->read_content(offset, &tag, sizeof tag);
  if (1 != count) throw FormatError("Bad Java class file tag");
  tag = be_to_host(tag);
  offset +=count;

  header->set_offset(offset);
  std::cout << "SgAsmJvmConstantPool::parse() header offset is " << header->get_offset() << std::endl;

  switch (tag) {
    case cp_info::CONSTANT_Methodref: {
      std::cout << "SgAsmJvmConstantPool::parse() tag is CONSTANT_Methodref\n";
      auto cpinfo = new CONSTANT_Methodref_info(header);
      std::cout << "SgAsmJvmConstantPool::parse() header offset is " << header->get_offset() << std::endl;
      if ((header->get_offset() - offset) != 4) {
        throw FormatError("Bad Java CONSTANT_Methodref");
      }
      offset = header->get_offset();
      break;
    }
    default:
      std::cout << "SgAsmJvmConstantPool::parse() unknown tag " << (int) tag << std::endl;
  }

  std::cout << "SgAsmJvmConstantPool::parse finished ...\n\n";
  return this;
}

#endif
