/* JVM Constant Pool section (SgAsmJvmConstantPool and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "Jvm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using PoolEntry = SgAsmJvmConstantPoolEntry;
using Rose::BinaryAnalysis::ByteOrder::hostToBe;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

SgAsmJvmConstantPool::SgAsmJvmConstantPool(SgAsmJvmFileHeader* jfh)
    : SgAsmGenericSection(isSgAsmGenericFile(jfh->get_parent()), jfh) {
    initializeProperties();
    set_parent(jfh);
    set_header(jfh);
}

// Constructor creating an object ready to be initialized via parse().
SgAsmJvmConstantPoolEntry::SgAsmJvmConstantPoolEntry(PoolEntry::Kind tag) {
    initializeProperties();
    p_tag = tag;
}

SgAsmJvmConstantPoolEntry* SgAsmJvmConstantPool::get_entry(size_t index) const
{
  SgAsmJvmConstantPoolEntry* entry{nullptr};
  auto entries{get_entries()};

  if (index > 0 && index <= entries.size()) {
    // Indices starts at one
    entry = entries[index-1];
  }
  else {
    throw FormatError("Invalid index");
  }
  return entry;
}

std::string SgAsmJvmConstantPool::get_utf8_string(size_t index) const
{
  SgAsmJvmConstantPoolEntry* entry{get_entry(index)};
  if (entry && entry->get_tag() == PoolEntry::CONSTANT_Utf8) {
    return std::string{entry->get_utf8_bytes(), entry->get_length()};
  }
  else {
    throw FormatError("Invalid CONSTANT_Utf8 entry at requested index");
  }
}

std::string PoolEntry::to_string(PoolEntry::Kind kind)
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

void PoolEntry::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  if (get_tag() != PoolEntry::EMPTY) {
    fprintf(f, "%sPoolEntry [%ld]: \t\t\t = %s_info", prefix, idx, PoolEntry::to_string(this->get_tag()).c_str());
  }

  switch (get_tag()) {
    case PoolEntry::CONSTANT_Utf8:
      fprintf(f, ": %d: %s", get_length(), std::string{get_utf8_bytes(), get_length()}.c_str());
      break;
    case PoolEntry::CONSTANT_Integer:
    case PoolEntry::CONSTANT_Float:
      fprintf(f, ": %d", get_bytes());
      break;
    case PoolEntry:: CONSTANT_Long:
    case PoolEntry::CONSTANT_Double:
      fprintf(f, ": %d: %d", get_hi_bytes(), get_low_bytes());
      break;
    case PoolEntry::CONSTANT_Class:
    case PoolEntry::CONSTANT_Module:
    case PoolEntry::CONSTANT_Package:
      fprintf(f, ": %d", get_name_index());
      break;
    case PoolEntry::CONSTANT_String:
      fprintf(f, ": %d", get_string_index());
      break;
    case PoolEntry::CONSTANT_Fieldref:
    case PoolEntry::CONSTANT_Methodref:
    case PoolEntry::CONSTANT_InterfaceMethodref:
      fprintf(f, ": %d: %d", get_class_index(), get_name_and_type_index());
      break;
    case PoolEntry::CONSTANT_NameAndType:
      fprintf(f, ": %d: %d", get_name_index(), get_descriptor_index());
      break;
    case PoolEntry::CONSTANT_MethodHandle:
      fprintf(f, ": %d: %d", get_reference_kind(), get_reference_index());
      break;
    case PoolEntry::CONSTANT_MethodType:
      fprintf(f, ": %d", get_descriptor_index());
      break;
    case PoolEntry::CONSTANT_Dynamic:
    case PoolEntry::CONSTANT_InvokeDynamic:
      fprintf(f, ": %d: %d", get_bootstrap_method_attr_index(), get_name_and_type_index());
      break;
    case PoolEntry::EMPTY:
      fprintf(f, "%s%ld: Empty", prefix, idx);
      break;
    default:
      fprintf(f, "%s%ld: Unknown tag", prefix, idx);
      break;
  }
  fprintf(f, "\n");
}

PoolEntry* PoolEntry::parse(SgAsmJvmConstantPool* pool)
{
  set_parent(pool);

  switch (get_tag()) {
    case PoolEntry::CONSTANT_Class: // 4.4.1  CONSTANT_Class_info table entry
    case PoolEntry::CONSTANT_Module: // 4.4.11 CONSTANT_Module_info table entry
    case PoolEntry::CONSTANT_Package: // 4.4.12 CONSTANT_Package_info table entry
      Jvm::read_value(pool, p_name_index);
      break;

    case PoolEntry::CONSTANT_String: // 4.4.2 CONSTANT_String_info table entry
      Jvm::read_value(pool, p_string_index);
      break;

    case PoolEntry::CONSTANT_Fieldref: // 4.4.3 CONSTANT_Fieldref_info table entry
    case PoolEntry::CONSTANT_Methodref: // 4.4.3 CONSTANT_Methodref_info table entry
    case PoolEntry::CONSTANT_InterfaceMethodref: // 4.4.3 CONSTANT_InterfeceMethodref_info table entry
      Jvm::read_value(pool, p_class_index);
      Jvm::read_value(pool, p_name_and_type_index);
      break;

    case PoolEntry::CONSTANT_Integer: // 4.4.4 CONSTANT_Integer_info table entry
    case PoolEntry::CONSTANT_Float: // 4.4.4 CONSTANT_Float_info table entry
      Jvm::read_value(pool, p_bytes);
      break;

    case PoolEntry::CONSTANT_Long: // 4.4.5 CONSTANT_Long_info table entry
    case PoolEntry::CONSTANT_Double: // 4.4.5 CONSTANT_Double_info table entry
      Jvm::read_value(pool, p_hi_bytes);
      Jvm::read_value(pool, p_low_bytes);
      break;

    case PoolEntry::CONSTANT_NameAndType: // 4.4.6 CONSTANT_NameAndType_info table entry
      Jvm::read_value(pool, p_name_index);
      Jvm::read_value(pool, p_descriptor_index);
      break;

    case PoolEntry::CONSTANT_Utf8: // 4.4.7 CONSTANT_Utf8_info table entry
    {
      char* bytes{nullptr};

      Jvm::read_bytes(pool, bytes, p_length);
      set_utf8_bytes(bytes);
      ASSERT_not_null(bytes);
      break;
    }

    case PoolEntry::CONSTANT_MethodHandle: // 4.4.8 CONSTANT_MethodHandle_info table entry
      Jvm::read_value(pool, p_reference_kind);
      Jvm::read_value(pool, p_reference_index);
      break;

    case PoolEntry::CONSTANT_MethodType: // 4.4.9 CONSTANT_MethodType_info table entry
      Jvm::read_value(pool, p_descriptor_index);
      break;

    case PoolEntry::CONSTANT_Dynamic: // 4.4.10 CONSTANT_Dynamic_info table entry
    case PoolEntry::CONSTANT_InvokeDynamic: // 4.4.10 CONSTANT_InvokeDynamic_info table entry
      Jvm::read_value(pool, p_bootstrap_method_attr_index);
      Jvm::read_value(pool, p_name_and_type_index);
      break;

    default:
      set_tag(PoolEntry::EMPTY);
  }

  return this;
}

// Write entry back to a binary file
void PoolEntry::unparse(std::ostream& os) const
{
  switch (get_tag()) {
    case PoolEntry::CONSTANT_Class: // 4.4.1  CONSTANT_Class_info table entry
    case PoolEntry::CONSTANT_Module: // 4.4.11 CONSTANT_Module_info table entry
    case PoolEntry::CONSTANT_Package: { // 4.4.12 CONSTANT_Package_info table entry
      auto name_index = p_name_index;
      hostToBe(p_name_index, &name_index);
      os.write(reinterpret_cast<const char*>(&name_index), sizeof name_index);
      break;
    }
    case PoolEntry::CONSTANT_String: { // 4.4.2 CONSTANT_String_info table entry
      auto string_index = p_string_index;
      hostToBe(p_string_index, &string_index);
      os.write(reinterpret_cast<const char*>(&string_index), sizeof string_index);
      break;
    }
    case PoolEntry::CONSTANT_Fieldref: // 4.4.3 CONSTANT_Fieldref_info table entry
    case PoolEntry::CONSTANT_Methodref: // 4.4.3 CONSTANT_Methodref_info table entry
    case PoolEntry::CONSTANT_InterfaceMethodref: { // 4.4.3 CONSTANT_InterfaceMethodref_info table entry
      auto class_index = p_class_index;
      auto name_and_type_index = p_name_and_type_index;
      hostToBe(p_class_index, &class_index);
      hostToBe(p_name_and_type_index, &name_and_type_index);
      os.write(reinterpret_cast<const char*>(&class_index), sizeof class_index);
      os.write(reinterpret_cast<const char*>(&name_and_type_index), sizeof name_and_type_index);
      break;
    }
    case PoolEntry::CONSTANT_Integer: // 4.4.4 CONSTANT_Integer_info table entry
    case PoolEntry::CONSTANT_Float: { // 4.4.4 CONSTANT_Float_info table entry
      auto bytes = p_bytes;
      hostToBe(p_bytes, &bytes);
      os.write(reinterpret_cast<const char*>(&bytes), sizeof bytes);
      break;
    }
    case PoolEntry::CONSTANT_Long: // 4.4.5 CONSTANT_Long_info table entry
    case PoolEntry::CONSTANT_Double: { // 4.4.5 CONSTANT_Double_info table entry
      auto hi_bytes = p_hi_bytes;
      auto low_bytes = p_low_bytes;
      hostToBe(p_hi_bytes, &hi_bytes);
      hostToBe(p_low_bytes, &low_bytes);
      os.write(reinterpret_cast<const char*>(&hi_bytes), sizeof hi_bytes);
      os.write(reinterpret_cast<const char*>(&low_bytes), sizeof low_bytes);
      break;
    }
    case PoolEntry::CONSTANT_NameAndType: { // 4.4.6 CONSTANT_NameAndType_info table entry
      auto name_index = p_name_index;
      auto descriptor_index = p_descriptor_index;
      hostToBe(p_name_index, &name_index);
      hostToBe(p_descriptor_index, &descriptor_index);
      os.write(reinterpret_cast<const char*>(&name_index), sizeof name_index);
      os.write(reinterpret_cast<const char*>(&descriptor_index), sizeof descriptor_index);
      break;
    }
    case PoolEntry::CONSTANT_Utf8: // 4.4.7 CONSTANT_Utf8_info table entry
    {
      auto length = p_length;
      hostToBe(p_length, &length);
      os.write(reinterpret_cast<const char*>(&length), sizeof length);
      os.write(p_utf8_bytes, p_length);
      break;
    }
    case PoolEntry::CONSTANT_MethodHandle: { // 4.4.8 CONSTANT_MethodHandle_info table entry
      auto reference_kind = p_reference_kind;
      auto reference_index = p_reference_index;
      hostToBe(p_reference_kind, &reference_kind);
      hostToBe(p_reference_index, &reference_index);
      os.write(reinterpret_cast<const char*>(&reference_kind), sizeof reference_kind);
      os.write(reinterpret_cast<const char*>(&reference_index), sizeof reference_index);
      break;
    }
    case PoolEntry::CONSTANT_MethodType: { // 4.4.9 CONSTANT_MethodType_info table entry
      auto descriptor_index = p_descriptor_index;
      hostToBe(p_descriptor_index, &descriptor_index);
      os.write(reinterpret_cast<const char*>(&descriptor_index), sizeof descriptor_index);
      break;
    }
    case PoolEntry::CONSTANT_Dynamic: // 4.4.10 CONSTANT_Dynamic_info table entry
    case PoolEntry::CONSTANT_InvokeDynamic: { // 4.4.10 CONSTANT_InvokeDynamic_info table entry
      auto bootstrap_method_attr_index = p_bootstrap_method_attr_index;
      auto name_and_type_index = p_name_and_type_index;
      hostToBe(p_bootstrap_method_attr_index, &bootstrap_method_attr_index);
      hostToBe(p_name_and_type_index, &name_and_type_index);
      os.write(reinterpret_cast<const char*>(&bootstrap_method_attr_index), sizeof bootstrap_method_attr_index);
      os.write(reinterpret_cast<const char*>(&name_and_type_index), sizeof name_and_type_index);
      break;
    }
    case PoolEntry::EMPTY: // An extra entry following CONSTANT_Long or CONSTANT_Double (a noop)
    default:
      break;
  }
}

SgAsmJvmConstantPool* SgAsmJvmConstantPool::parse()
{
  PoolEntry* entry{nullptr};
  auto header = get_header();
  ASSERT_not_null(header);

  /* Constant pool count */
  uint16_t constant_pool_count;
  Jvm::read_value(this, constant_pool_count);

  // A constant_pool index is considered valid if it is greater than zero and less than constant_pool_count
  for (int ii = 1; ii < constant_pool_count; ii++) {
    /* tag */
    uint8_t tag;
    Jvm::read_value(this, tag);

    // Create and initialize (parse) a new entry
    auto kind = static_cast<PoolEntry::Kind>(tag);
    entry = new PoolEntry(kind);
    entry->parse(this);

    // Store the new entry
    get_entries().push_back(entry);

    // If this is CONSTANT_Long or CONSTANT_Double, store index location with empty entry
    // 4.4.5 "In retrospect, making 8-byte constants take two constant pool entries was a poor choice."
    //
    if (kind == PoolEntry::CONSTANT_Long || kind == PoolEntry::CONSTANT_Double) {
      // Create and store an empty entry
      entry = new PoolEntry(PoolEntry::EMPTY);
      get_entries().push_back(entry);
      ii += 1;
    }
  }
  return this;
}

// Write section back to a binary file
void SgAsmJvmConstantPool::unparse(std::ostream& os) const
{
  uint16_t count = get_entries().size() + 1; // constant_pool_count = #entries + 1
  hostToBe(count, &count);
  os.write(reinterpret_cast<const char*>(&count), sizeof count);

  for (auto entry : get_entries()) {
    uint8_t tag = entry->get_tag();
    auto kind = static_cast<PoolEntry::Kind>(tag);

    if (kind != PoolEntry::EMPTY) {
      hostToBe(tag, &tag);
      os.write(reinterpret_cast<const char*>(&tag), sizeof tag);
      entry->unparse(os);
    }
  }
}

void SgAsmJvmConstantPool::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  SgAsmGenericSection::dump(f, prefix, idx);

  // set idx for entry index
  idx = 0;
  for (auto entry : get_entries()) {
    entry->dump(f, prefix, idx++);
  }
}

#endif
