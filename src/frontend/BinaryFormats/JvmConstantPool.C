/* JVM Constant Pool section (SgAsmJvmConstantPool and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Should go in JvmConstantPool.h or JvmConstantPoolEntries.h? */

/* Specific formats for constant_pool table entries. All fields are big endian. */
// 4.4.1
struct CONSTANT_Class_info : SgAsmJvmConstantPoolEntry {
  uint16_t name_index;
  CONSTANT_Class_info(SgAsmGenericHeader*);
};

// 4.4.2
struct CONSTANT_Methodref_info : SgAsmJvmConstantPoolEntry {
  uint16_t class_index;
  uint16_t name_and_type_index;
  CONSTANT_Methodref_info(SgAsmGenericHeader*);
};

// 4.4.6
struct CONSTANT_NameAndType_info : SgAsmJvmConstantPoolEntry {
  uint16_t name_index;
  uint16_t descriptor_index;
  CONSTANT_NameAndType_info(SgAsmGenericHeader*);
};

// 4.4.7
struct CONSTANT_Utf8_info : SgAsmJvmConstantPoolEntry {
  uint16_t length;
  uint8_t* bytes;
  CONSTANT_Utf8_info(SgAsmGenericHeader* h);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Rose::Diagnostics;
using namespace ByteOrder;

// 4.4.1
CONSTANT_Class_info::CONSTANT_Class_info(SgAsmGenericHeader* h)
  : SgAsmJvmConstantPoolEntry{CONSTANT_Class}
{
  auto offset = h->get_offset();
  auto count = h->read_content(offset, &name_index, sizeof name_index);
  if (2 != count) std::cout << "Bad CONSTANT_Class_info::name_index\n";
  name_index = be_to_host(name_index);
  offset += count;

  std::cout << "CONSTANT_Class_info:" << name_index << std::endl;
  h->set_offset(offset);
}

CONSTANT_Methodref_info::CONSTANT_Methodref_info(SgAsmGenericHeader* h)
  : SgAsmJvmConstantPoolEntry{CONSTANT_Methodref}
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

// 4.4.6
CONSTANT_NameAndType_info::CONSTANT_NameAndType_info(SgAsmGenericHeader* h)
  : SgAsmJvmConstantPoolEntry{CONSTANT_NameAndType}, name_index{0}, descriptor_index{0}
{
  auto offset = h->get_offset();

  /* name_index */
  auto count = h->read_content(offset, &name_index, sizeof name_index);
  if (2 != count) std::cout << "Bad CONSTANT_NameAndType_info::name_index\n";
  name_index = be_to_host(name_index);
  offset += count;

  /* descriptor_index */
  count = h->read_content(offset, &descriptor_index, sizeof descriptor_index);
  if (2 != count) std::cout << "Bad CONSTANT_NameAndType_info::descriptor_index\n";
  descriptor_index = be_to_host(descriptor_index);
  offset += count;

  std::cout << "CONSTANT_NameAndType_info:" << name_index << ":" << descriptor_index << std::endl;
  h->set_offset(offset);
}

// 4.4.7
CONSTANT_Utf8_info::CONSTANT_Utf8_info(SgAsmGenericHeader* h)
  : SgAsmJvmConstantPoolEntry{CONSTANT_Utf8}, length{0}, bytes{nullptr}
{
  auto offset = h->get_offset();

  /* length */
  auto count = h->read_content(offset, &length, sizeof length);
  if (2 != count) std::cout << "Bad CONSTANT_Utf8_info::length\n";
  length = be_to_host(length);
  offset += count;

  /* bytes string */
  bytes = new uint8_t[length+1]; // null terminated strings are easier to use
  count = h->read_content(offset, bytes, length);
  if (length != count) std::cout << "Bad CONSTANT_Utf8_info::bytes\n";
  bytes[length] = '\0';
  offset += count;

  std::cout << "CONSTANT_Utf8_info:" << length << ":" << bytes << std::endl;
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
  std::cout << "SgAsmJvmConstantPool::parse() constant_pool_count is " << constant_pool_count << "\n\n";

  SgAsmJvmConstantPoolEntry* entry{nullptr};
  /* A constant_pool index is considered valid if it is greater than zero and less than constant_pool_count */
  for (int ii = 1; ii < constant_pool_count; ii++) {
    /* tag */
    uint8_t tag;
    count = header->read_content(offset, &tag, sizeof tag);
    if (1 != count) throw FormatError("Bad Java class file tag");

    offset +=count;
    header->set_offset(offset);

    std::cout << ii << ":";
    switch (tag) {
      case SgAsmJvmConstantPoolEntry::CONSTANT_Class:
        entry = new CONSTANT_Class_info(header);
        if ((header->get_offset() - offset) != 2) {
          throw FormatError("Bad Java CONSTANT_Class");
        }
        break;
      case SgAsmJvmConstantPoolEntry::CONSTANT_Methodref:
        entry = new CONSTANT_Methodref_info(header);
        if ((header->get_offset() - offset) != 4) {
          throw FormatError("Bad Java CONSTANT_Methodref");
        }
        break;
      case SgAsmJvmConstantPoolEntry::CONSTANT_NameAndType:
        entry = new CONSTANT_NameAndType_info(header);
        break;
      case SgAsmJvmConstantPoolEntry::CONSTANT_Utf8:
        entry = new CONSTANT_Utf8_info(header);
        break;
      default:
        std::cout << "SgAsmJvmConstantPool::parse() unknown tag " << (int) tag << std::endl;
        return this;
    }
    offset = header->get_offset();
    // TODO: This need to be a list
    set_entry(entry);
  }

// NOTE: refactor this once SgJavaClassFile exists
#if 1
  /* access_flags */
  uint16_t access_flags;
  count = read_content(offset, &access_flags, sizeof access_flags);
  if (2 != count) std::cout << "Bad Java class file access_flags\n";
  access_flags = be_to_host(access_flags);
  offset += count;
  std::cout << "\nSgAsmJvmConstantPool::access_flags " << access_flags << std::endl;

  /* this_class */
  uint16_t this_class;
  count = read_content(offset, &this_class, sizeof this_class);
  if (2 != count) std::cout << "Bad Java class file this_class\n";
  this_class = be_to_host(this_class);
  offset += count;
  std::cout << "SgAsmJvmConstantPool::this_class " << this_class << std::endl;

  /* super_class */
  uint16_t super_class;
  count = read_content(offset, &super_class, sizeof super_class);
  if (2 != count) std::cout << "Bad Java class file super_class\n";
  super_class = be_to_host(super_class);
  offset += count;
  std::cout << "SgAsmJvmConstantPool::super_class " << super_class << std::endl;

  /* interfaces_count */
  uint16_t interfaces_count;
  count = read_content(offset, &interfaces_count, sizeof interfaces_count);
  if (2 != count) std::cout << "Bad Java class file interfaces_count\n";
  interfaces_count = be_to_host(interfaces_count);
  offset += count;
  std::cout << "SgAsmJvmConstantPool::interfaces_count " << interfaces_count << std::endl;

  //TODO: what's up with the different offsets? And they are diff by 1!
  //set_offset(offset);
#endif

  std::cout << "\nSgAsmJvmConstantPool::parse finished ...\n\n";
  return this;
}

#endif
