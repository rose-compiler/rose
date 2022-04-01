/* JVM Constant Pool section (SgAsmJvmConstantPool and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Rose::Diagnostics;
using namespace ByteOrder;

using PoolEntry = SgAsmJvmConstantPoolEntry;

SgAsmJvmConstantPool::SgAsmJvmConstantPool(SgAsmJvmFileHeader* fhdr)
  : SgAsmGenericSection(fhdr->get_file(), fhdr)
{
  std::cout << "SgAsmJvmConstantPool::ctor() ...\n";
  p_entries = new SgAsmJvmConstantPoolEntryList;
  p_entries->set_parent(this);
}

// Constructor creating an object ready to be initialized via parse().
SgAsmJvmConstantPoolEntry::SgAsmJvmConstantPoolEntry(PoolEntry::ConstantPoolKind tag)
  : p_tag{tag}, p_bytes{0}, p_hi_bytes{0}, p_low_bytes{0}, p_bootstrap_method_attr_index{0}, p_class_index{0},
    p_descriptor_index{0}, p_name_index{0}, p_name_and_type_index{0}, p_reference_index{0}, p_reference_kind{0},
    p_string_index{0}, p_length{0}, p_utf8_bytes{nullptr}
{
}

std::string PoolEntry::to_string(PoolEntry::ConstantPoolKind kind)
{
  switch (kind) {
    case PoolEntry::CONSTANT_Utf8:               return "CONSTANT_Utf8";
    case PoolEntry::CONSTANT_Integer:            return "CONSTANT_Integer";
    case PoolEntry::CONSTANT_Float:              return "CONSTANT_Float";
    case PoolEntry::CONSTANT_Long:               return "CONSTANT_Long";
    case PoolEntry::CONSTANT_Double:             return "CONSTANT_Double";
    case PoolEntry::CONSTANT_Class:              return "CONSTANT_Class";
    case PoolEntry::CONSTANT_String:             return "CONSTANT_String";
    case PoolEntry::CONSTANT_Fieldref:           return "CONSTANT_Fieldref";
    case PoolEntry::CONSTANT_Methodref:          return "CONSTANT_Methodref";
    case PoolEntry::CONSTANT_InterfaceMethodref: return "CONSTANT_InterfaceMethodref";
    case PoolEntry::CONSTANT_NameAndType:        return "CONSTANT_NameAndType";
    case PoolEntry::CONSTANT_MethodHandle:       return "CONSTANT_MethodHandle";
    case PoolEntry::CONSTANT_MethodType:         return "CONSTANT_MethodType";
    case PoolEntry::CONSTANT_Dynamic:            return "CONSTANT_Dynamic";
    case PoolEntry::CONSTANT_InvokeDynamic:      return "CONSTANT_InvokeDynamic";
    case PoolEntry::CONSTANT_Module:             return "CONSTANT_Module";
    case PoolEntry::CONSTANT_Package:            return "CONSTANT_Package";
    case PoolEntry::EMPTY:
      // Ignore this entry as it is empty
      return std::string{""};
    default: return "Unknown constant pool kind";
  }
}

std::string cp_tag(PoolEntry* entry)
{
  return PoolEntry::to_string(entry->get_tag());
}

void PoolEntry::dump(std::ostream &os, int index)
{
  if (get_tag() != PoolEntry::EMPTY) {
    os << index << ":" << cp_tag(this) << "_info";
  }

  switch (get_tag()) {
    case PoolEntry::CONSTANT_Utf8:
      os << ":" << get_length();
      os << ":" << std::string{get_utf8_bytes(), get_length()};
      break;
    case PoolEntry::CONSTANT_Integer:
    case PoolEntry::CONSTANT_Float:
      os << ":" << get_bytes();
      break;
    case PoolEntry::CONSTANT_Long:
    case PoolEntry::CONSTANT_Double:
      os << ":" << get_hi_bytes();
      os << ":" << get_low_bytes();
      break;
    case PoolEntry::CONSTANT_Class:
    case PoolEntry::CONSTANT_Module:
    case PoolEntry::CONSTANT_Package:
      os << ":" << get_name_index();
      break;
    case PoolEntry::CONSTANT_String:
      os << ":" << get_string_index();
      break;
    case PoolEntry::CONSTANT_Fieldref:
    case PoolEntry::CONSTANT_Methodref:
    case PoolEntry::CONSTANT_InterfaceMethodref:
      os << ":" << get_class_index();
      os << ":" << get_name_and_type_index();
      break;
    case PoolEntry::CONSTANT_NameAndType:
      os << ":" << get_name_index();
      os << ":" << get_descriptor_index();
      break;
    case PoolEntry::CONSTANT_MethodHandle:
      os << ":" << get_reference_kind();
      os << ":" << get_reference_index();
      break;
    case PoolEntry::CONSTANT_MethodType:
      os << ":" << get_descriptor_index();
      break;
    case PoolEntry::CONSTANT_Dynamic:
    case PoolEntry::CONSTANT_InvokeDynamic:
      os << ":" << get_bootstrap_method_attr_index();
      os << ":" << get_name_and_type_index();
      break;
    case PoolEntry::EMPTY:
      os << index << ":" << "Empty";
      break;
    default:
      os << ":" <<  "Unknown tag";
      break;
  }
  os << std::endl;
}

PoolEntry* PoolEntry::parse(SgAsmJvmConstantPool* pool)
{
  size_t count;
  uint16_t name_index;
  auto h{pool->get_header()};
  auto offset = h->get_offset();

  switch (get_tag()) {
    case PoolEntry::CONSTANT_Class: // 4.4.1  CONSTANT_Class_info table entry
    case PoolEntry::CONSTANT_Module: // 4.4.11 CONSTANT_Module_info table entry
    case PoolEntry::CONSTANT_Package: // 4.4.12 CONSTANT_Package_info table entry
      /* name_index */
      count = h->read_content(offset, &p_name_index, sizeof p_name_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Class,Module,Package}_info::name_index");
      }
      p_name_index = be_to_host(p_name_index);
      offset += count;
      break;

    case PoolEntry::CONSTANT_String: // 4.4.2 CONSTANT_String_info table entry
      /* string_index */
      count = h->read_content(offset, &p_string_index, sizeof p_string_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_String_info::string_index");
      }
      p_string_index = be_to_host(p_string_index);
      offset += count;
      break;

    case PoolEntry::CONSTANT_Fieldref: // 4.4.3 CONSTANT_Fieldref_info table entry
    case PoolEntry::CONSTANT_Methodref: // 4.4.3 CONSTANT_Methodref_info table entry
    case PoolEntry::CONSTANT_InterfaceMethodref: // 4.4.3 CONSTANT_InterfeceMethodref_info table entry
      /* class_index */
      count = h->read_content(offset, &p_class_index, sizeof p_class_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Field,Method,InterfaceMethod}ref_info::class_index");
      }
      p_class_index = be_to_host(p_class_index);
      offset += count;
      /* name_and_type_index */
      count = h->read_content(offset, &p_name_and_type_index, sizeof p_name_and_type_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Field,Method,InterfaceMethod}ref_info::name_and_type_index");
      }
      p_name_and_type_index = be_to_host(p_name_and_type_index);
      offset += count;
      break;

    case PoolEntry::CONSTANT_Integer: // 4.4.4 CONSTANT_Integer_info table entry
    case PoolEntry::CONSTANT_Float: // 4.4.4 CONSTANT_Float_info table entry
      /* bytes */
      count = h->read_content(offset, &p_bytes, sizeof p_bytes);
      if (4 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Integer,Float}_info::bytes");
      }
      p_bytes = be_to_host(p_bytes);
      offset += count;
      break;

    case PoolEntry::CONSTANT_Long: // 4.4.5 CONSTANT_Long_info table entry
    case PoolEntry::CONSTANT_Double: // 4.4.5 CONSTANT_Double_info table entry
      /* hi_bytes */
      count = h->read_content(offset, &p_hi_bytes, sizeof p_hi_bytes);
      if (4 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Integer,Float}_info::hi_bytes");
      }
      p_hi_bytes = be_to_host(p_hi_bytes);
      offset += count;
      /* low_bytes */
      count = h->read_content(offset, &p_low_bytes, sizeof p_low_bytes);
      if (4 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Integer,Float}_info::low_bytes");
      }
      p_low_bytes = be_to_host(p_low_bytes);
      offset += count;
      break;

    case PoolEntry::CONSTANT_NameAndType: // 4.4.6 CONSTANT_NameAndType_info table entry
      /* name_index */
      count = h->read_content(offset, &p_name_index, sizeof p_name_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_NameAndType_info::name_index");
      }
      p_name_index = be_to_host(p_name_index);
      offset += count;
      /* descriptor_index */
      count = h->read_content(offset, &p_descriptor_index, sizeof p_descriptor_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_NameAndType_info::descriptor_index");
      }
      p_descriptor_index = be_to_host(p_descriptor_index);
      offset += count;
      break;

    case PoolEntry::CONSTANT_Utf8: // 4.4.7 CONSTANT_Utf8_info table entry
    {
      char* bytes{nullptr};
      /* length */
      count = h->read_content(offset, &p_length, sizeof p_length);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_Utf8_info::length");
      }
      p_length = be_to_host(p_length);
      offset += count;
      /* bytes string */
      bytes = new char[p_length];
      count = h->read_content(offset, bytes, p_length);
      if (p_length != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_Utf8_info::bytes");
      }
      set_utf8_bytes(bytes);
      offset += count;
      break;
    }

    case PoolEntry::CONSTANT_MethodHandle: // 4.4.8 CONSTANT_MethodHandle_info table entry
      /* reference_kind */
      count = h->read_content(offset, &p_reference_kind, sizeof p_reference_kind);
      if (1 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_MethodHandle_info::reference_kind");
      }
      p_reference_kind = be_to_host(p_reference_kind);
      offset += count;
      /* reference_index */
      count = h->read_content(offset, &p_reference_index, sizeof p_reference_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Field,Method,InterfaceMethod}ref_info::reference_index");
      }
      p_reference_index = be_to_host(p_reference_index);
      offset += count;
      break;

    case PoolEntry::CONSTANT_MethodType: // 4.4.9 CONSTANT_MethodType_info table entry
      /* descriptor_index */
      count = h->read_content(offset, &p_descriptor_index, sizeof p_descriptor_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_MethodType_info::descriptor_index");
      }
      p_descriptor_index = be_to_host(p_descriptor_index);
      offset += count;
      break;

    case PoolEntry::CONSTANT_Dynamic: // 4.4.10 CONSTANT_Dynamic_info table entry
    case PoolEntry::CONSTANT_InvokeDynamic: // 4.4.10 CONSTANT_InvokeDynamic_info table entry
      /* bootstrap_method_attr_index */
      count = h->read_content(offset, &p_bootstrap_method_attr_index, sizeof p_bootstrap_method_attr_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Dynamic,InvokeDynamic}_info::bootstrap_method_attr_index");
      }
      p_bootstrap_method_attr_index = be_to_host(p_bootstrap_method_attr_index);
      offset += count;
      /* name_and_type_index */
      count = h->read_content(offset, &p_name_and_type_index, sizeof p_name_and_type_index);
      if (2 != count) {
        throw SgAsmJvmConstantPool::FormatError("Bad CONSTANT_{Dynamic,InvokeDynamic}_info::name_and_type_index");
      }
      p_name_and_type_index = be_to_host(p_name_and_type_index);
      offset += count;
      break;

    default:
      set_tag(PoolEntry::EMPTY);
  }

  h->set_offset(offset);
  set_parent(pool);

  return this;
}

SgAsmJvmConstantPool* SgAsmJvmConstantPool::parse()
{
  PoolEntry* entry{nullptr};
  auto header = get_header();
  rose_addr_t offset = header->get_offset();

  std::cout << "SgAsmJvmConstantPool::parse() ...\n";
  std::cout << "SgAsmJvmConstantPool::parse() header class name is " << header->class_name() << std::endl;
  std::cout << "SgAsmJvmConstantPool::parse() this offset is " << get_offset() << std::endl;
  std::cout << "SgAsmJvmConstantPool::parse() header offset is " << offset << std::endl;

  /* Constant pool count */
  uint16_t constant_pool_count;
  auto count = header->read_content(offset, &constant_pool_count, sizeof constant_pool_count);
  if (2 != count) throw FormatError("Bad Java class file constant_pool_count");
  constant_pool_count = be_to_host(constant_pool_count);
  offset +=count;
  std::cout << "SgAsmJvmConstantPool::parse() constant_pool_count is " << constant_pool_count << "\n\n";

  // A constant_pool index is considered valid if it is greater than zero and less than constant_pool_count
  for (int ii = 1; ii < constant_pool_count; ii++) {
    /* tag */
    uint8_t tag;
    count = header->read_content(offset, &tag, sizeof tag);
    if (1 != count) throw FormatError("Bad Java class file tag");

    offset +=count;
    header->set_offset(offset);

    // Create and initialize (parse) a new entry
    auto kind = static_cast<PoolEntry::ConstantPoolKind>(tag);
    entry = new PoolEntry(kind);
    entry->parse(this);

    // Store the new entry
    p_entries->get_entries().push_back(entry);

    // If this is CONSTANT_Long or CONSTANT_Double, store index location with empty entry
    // 4.4.5 "In retrospect, making 8-byte constants take two constant pool entries was a poor choice."
    //
    if (kind == PoolEntry::CONSTANT_Long || kind == PoolEntry::CONSTANT_Double) {
      // Create and store an empty entry
      entry = new PoolEntry(PoolEntry::EMPTY);
      p_entries->get_entries().push_back(entry);
      ii += 1;
    }
    offset = header->get_offset();
  }

  // Dump constant pool entries
  int ii{1};
  for (PoolEntry* entry : get_entries()->get_entries()) {
    entry->dump(std::cout, ii++);
  }

  std::cout << "\nSgAsmJvmConstantPool::parse finished ...\n\n";
  return this;
}

#endif
