/* JVM Attributes */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "Jvm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using std::cout;
using std::endl;

SgAsmJvmAttribute* SgAsmJvmAttribute::create_attribute(SgAsmJvmConstantPool* pool)
{
  uint16_t attribute_name_index;
  uint32_t attribute_length;

  Jvm::read_value(pool, attribute_name_index, /*advance_offset*/false);
  std::string name = pool->get_utf8_string(attribute_name_index);

  if (name == "Code") {
    return new SgAsmJvmCodeAttribute;
  }
  else if (name == "ConstantValue") { // 4.7.2
    return new SgAsmJvmConstantValue;
  }
  else if (name == "SourceFile") { // 4.7.10
    return new SgAsmJvmSourceFile;
  }
  else if (name == "LineNumberTable") { // 4.7.12
    return new SgAsmJvmLineNumberTable;
  }

  // skip attribute
  Jvm::read_value(pool, attribute_name_index);
  Jvm::read_value(pool, attribute_length);

#ifdef DEBUG_ON
  cout << "--- " << name << " attribute ---\n";
  cout << "SgAsmJvmAttribute::create_attribute(): skipping attribute of length " << attribute_length << endl;
#endif

  SgAsmGenericHeader* header{pool->get_header()};
  ASSERT_not_null(header);

  rose_addr_t offset{header->get_offset()};
  header->set_offset(offset + attribute_length);

  return nullptr;
}

SgAsmJvmAttributeTable::SgAsmJvmAttributeTable()
{
  // Default constructor needed for serialization
}

SgAsmJvmAttributeTable::SgAsmJvmAttributeTable(SgAsmJvmFileHeader* jfh, SgAsmNode* parent)
{
  set_parent(parent);
  set_header(jfh);
}

SgAsmJvmAttributeTable* SgAsmJvmAttributeTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t attributes_count;

  Jvm::read_value(pool, attributes_count);

  for (int ii = 0; ii < attributes_count; ii++) {
    auto attribute = SgAsmJvmAttribute::create_attribute(pool);
    // attribute may not be implemented yet
    if (attribute) {
      attribute->set_parent(this);
      attribute->parse(pool);
      get_attributes().push_back(attribute);
    }
  }
  return this;
}

void SgAsmJvmAttributeTable::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s", prefix);
  for (auto attribute : get_attributes()) {
    attribute->dump(stdout, "   ", idx++);
  }
}

SgAsmJvmAttribute* SgAsmJvmAttribute::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_attribute_name_index);
  Jvm::read_value(pool, p_attribute_length);

#ifdef DEBUG_ON
  cout << "SgAsmJvmAttribute::parse:attribute_name_index " << p_attribute_name_index << endl;
  cout << "SgAsmJvmAttribute::parse:attribute_length " << p_attribute_length << endl;
  ROSE_ASSERT(p_attribute_name_index == get_attribute_name_index());
  ROSE_ASSERT(p_attribute_length == get_attribute_length());
#endif

  return this;
}

SgAsmJvmAttribute* SgAsmJvmCodeAttribute::parse(SgAsmJvmConstantPool* pool)
{
  uint32_t length;
  char* bytes{nullptr};
  auto header = dynamic_cast<SgAsmJvmFileHeader*>(pool->get_header());

  SgAsmJvmAttribute::parse(pool);

  Jvm::read_value(pool, p_max_stack);
  Jvm::read_value(pool, p_max_locals);

  /* set the offset for the code array (used later for disassembly/decoding) */
  set_code_offset(header->get_offset());

  /* allocate and read the code array */
  p_code_length = Jvm::read_bytes(pool, bytes, length);
  set_code(bytes);

  /* exception table */
  p_exception_table = new SgAsmJvmExceptionTable(this);
  p_exception_table->parse(pool);

  /* attribute table */
  p_attribute_table = new SgAsmJvmAttributeTable(header, /*parent*/this);
  p_attribute_table->parse(pool);

  return this;
}

SgAsmJvmAttribute* SgAsmJvmConstantValue::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  // The value of the attribute_length item must be two (section 4.7.2)
  ROSE_ASSERT(p_attribute_length == 2);
  Jvm::read_value(pool, p_constantvalue_index);

  return this;
}

SgAsmJvmAttribute* SgAsmJvmSignature::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  cout << "SgAsmJvmSignature::parse() ...\n";

  ROSE_ASSERT(false && "TODO");
  return this;
}

SgAsmJvmAttribute* SgAsmJvmSourceFile::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, p_sourcefile_index);
  return this;
}

void SgAsmJvmAttribute::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld:%d:%d\n", prefix, idx, p_attribute_name_index, p_attribute_length);
}

void SgAsmJvmCodeAttribute::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "-->SgAsmJvmCodeAttribute:%d:%d:%d\n", p_max_stack, p_max_locals, p_code_length);
}

void SgAsmJvmConstantValue::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "SgAsmJvmConstantValue:%d\n", p_constantvalue_index);
}

void SgAsmJvmSignature::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "SgAsmJvmSignature::dump\n");
}

void SgAsmJvmSourceFile::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "SgAsmJvmSourceFile::dump():%d\n", p_sourcefile_index);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Exception nodes used by SgAsmJvmCodeAttribute
//
SgAsmJvmException::SgAsmJvmException(SgAsmJvmExceptionTable* table)
{
  cout << "\nSgAsmJvmException::ctor() ...\n";
  set_parent(table);
}

SgAsmJvmException* SgAsmJvmException::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_end_pc);
  Jvm::read_value(pool, p_handler_pc);
  Jvm::read_value(pool, p_catch_type);
  return this;
}

void SgAsmJvmException::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  cout << "SgAsmJvmException::dump() ...\n";
}

SgAsmJvmExceptionTable::SgAsmJvmExceptionTable(SgAsmJvmCodeAttribute* parent)
{
  set_parent(parent);
}

SgAsmJvmExceptionTable* SgAsmJvmExceptionTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t exception_table_length;
  Jvm::read_value(pool, exception_table_length);

  auto exceptions = get_exceptions();
  for (int ii = 0; ii < exception_table_length; ii++) {
    cout << "\n --- exception ---\n";
    auto exception = new SgAsmJvmException(this);
    exception->parse(pool);
    exceptions.push_back(exception);
  }

  return this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// InnerClassTable used by SgAsmJvmInnerClass (InnerClassTable_attribute from the Jvm specification (4.7.6))
//
SgAsmJvmInnerClassesEntry::SgAsmJvmInnerClassesEntry(SgAsmJvmInnerClasses* table)
{
  cout << "\nSgAsmJvmInnerClassesEntry::ctor() ...\n";
}

SgAsmJvmInnerClassesEntry* SgAsmJvmInnerClassesEntry::parse(SgAsmJvmConstantPool* pool)
{
  cout << "SgAsmJvmInnerClassesEntry::parse() ...\n";
  return this;
}

void SgAsmJvmInnerClassesEntry::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  cout << "SgAsmJvmInnerClassesEntry::dump() ...\n";
}

SgAsmJvmInnerClasses::SgAsmJvmInnerClasses(SgAsmJvmAttribute* parent)
{
  cout << "SgAsmJvmInnerClasses::ctor() ...\n";
}

SgAsmJvmInnerClasses* SgAsmJvmInnerClasses::parse(SgAsmJvmConstantPool* pool)
{
  cout << "SgAsmJvmInnerClasses::parse() ...\n";
  cout << "SgAsmJvmInnerClasses::parse() exit ... \n";

  return this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SgAsmJvmLineNumberTable attribute node (LineNumberTable_attribute from the Jvm specification (4.7.12)
//
SgAsmJvmLineNumberTable::SgAsmJvmLineNumberTable(SgAsmJvmAttribute* parent)
{
  cout << "\nSgAsmJvmLineNumberTable::ctor() ...\n";
  set_parent(parent);
}

SgAsmJvmLineNumberTable* SgAsmJvmLineNumberTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t line_number_table_length;
  ASSERT_not_null(get_parent());

  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, line_number_table_length);

  auto line_number_table = get_line_number_table();
  for (int ii = 0; ii < line_number_table_length; ii++) {
    auto entry = new SgAsmJvmLineNumberEntry(this);
    entry->parse(pool);
    line_number_table.push_back(entry);
  }

  return this;
}

void SgAsmJvmLineNumberTable::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: SgAsmJvmLineNumberTable::dump()\n", prefix, idx);
}

SgAsmJvmLineNumberEntry::SgAsmJvmLineNumberEntry(SgAsmJvmLineNumberTable* table)
{
  set_parent(table);
}

SgAsmJvmLineNumberEntry* SgAsmJvmLineNumberEntry::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_line_number);
  return this;
}

void SgAsmJvmLineNumberEntry::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: start_pc:%d line_number:%d\n", prefix, idx, p_start_pc, p_line_number);
}

SgAsmJvmAttribute* SgAsmJvmModuleMainClass::parse(SgAsmJvmConstantPool* pool)
{
  ROSE_ASSERT(false && "SgAsmJvmModuleMainClass::parse()");
  return nullptr;
}

void SgAsmJvmModuleMainClass::dump(FILE* f, const char *prefix, ssize_t idx) const
{
  ROSE_ASSERT(false && "SgAsmJvmModuleMainClass::dump()");
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
