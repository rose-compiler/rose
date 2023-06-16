/* JVM Attributes */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "Jvm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using Rose::BinaryAnalysis::ByteOrder::hostToBe;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

SgAsmJvmAttribute* SgAsmJvmAttribute::instance(SgAsmJvmConstantPool* pool)
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
  mlog[FATAL] << "--- " << name << " attribute ---\n"
              << "SgAsmJvmAttribute::instance(): skipping attribute of length " << attribute_length << "\n";
  ROSE_ABORT();
#endif

  SgAsmGenericHeader* header{pool->get_header()};
  ASSERT_not_null(header);

  rose_addr_t offset{header->get_offset()};
  header->set_offset(offset + attribute_length);

  return nullptr;
}

SgAsmJvmAttributeTable::SgAsmJvmAttributeTable(SgAsmJvmFileHeader* jfh, SgAsmNode* parent)
{
  initializeProperties();
  set_parent(parent);
  set_header(jfh);
}

SgAsmJvmAttributeTable* SgAsmJvmAttributeTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t attributes_count;

  Jvm::read_value(pool, attributes_count);

  for (int ii = 0; ii < attributes_count; ii++) {
    auto attribute = SgAsmJvmAttribute::instance(pool);
    // attribute may not be implemented yet
    if (attribute) {
      attribute->set_parent(this);
      attribute->parse(pool);
      get_attributes().push_back(attribute);
    }
  }
  return this;
}

void SgAsmJvmAttributeTable::unparse(std::ostream& os) const
{
  uint16_t attributes_count = get_attributes().size();
  hostToBe(attributes_count, &attributes_count);
  os.write(reinterpret_cast<const char*>(&attributes_count), sizeof attributes_count);

  for (auto attribute : get_attributes()) {
    attribute->unparse(os);
  }
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
  return this;
}

void SgAsmJvmAttribute::unparse(std::ostream& os) const
{
  auto attribute_name_index = p_attribute_name_index;
  auto attribute_length = p_attribute_length;

  hostToBe(attribute_name_index, &attribute_name_index);
  hostToBe(attribute_length, &attribute_length);

  os.write(reinterpret_cast<const char*>(&attribute_name_index), sizeof attribute_name_index);
  os.write(reinterpret_cast<const char*>(&attribute_length), sizeof attribute_length);
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

void SgAsmJvmCodeAttribute::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  auto max_stack = p_max_stack;
  auto max_locals = p_max_locals;
  auto code_length = p_code_length;

  hostToBe(max_stack, &max_stack);
  hostToBe(max_locals, &max_locals);
  hostToBe(code_length, &code_length);

  os.write(reinterpret_cast<const char*>(&max_stack), sizeof max_stack);
  os.write(reinterpret_cast<const char*>(&max_locals), sizeof max_stack);
  os.write(reinterpret_cast<const char*>(&code_length), sizeof code_length);

  // Write code
  os.write(p_code, p_code_length);

  p_exception_table->unparse(os);
  p_attribute_table->unparse(os);
}

SgAsmJvmAttribute* SgAsmJvmConstantValue::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  // The value of the attribute_length item must be two (section 4.7.2)
  ASSERT_require(p_attribute_length == 2);
  Jvm::read_value(pool, p_constantvalue_index);

  return this;
}

void SgAsmJvmConstantValue::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmConstantValue is not implemented yet\n";
}

SgAsmJvmAttribute* SgAsmJvmSignature::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  mlog[WARN] << "Parsing of SgAsmJvmSignature attribute is not implemented yet\n";

  ROSE_ASSERT(false && "TODO");
  return this;
}

void SgAsmJvmSignature::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmSignature attribute is not implemented yet\n";
}

SgAsmJvmAttribute* SgAsmJvmSourceFile::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, p_sourcefile_index);
  return this;
}

void SgAsmJvmSourceFile::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmSourceFile is not implemented yet\n";

  SgAsmJvmAttribute::unparse(os);

  auto sourcefile_index = p_sourcefile_index;
  hostToBe(sourcefile_index, &sourcefile_index);
  os.write(reinterpret_cast<const char*>(&sourcefile_index), sizeof sourcefile_index);
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
  mlog[INFO] << "\nSgAsmJvmException::ctor() ...\n";
  initializeProperties();
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

void SgAsmJvmException::unparse(std::ostream &os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmException is not implemented yet\n";
}

void SgAsmJvmException::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  mlog[WARN] << "SgAsmJvmException::dump() not implemented yet\n";
}

SgAsmJvmExceptionTable::SgAsmJvmExceptionTable(SgAsmJvmCodeAttribute* parent)
{
    initializeProperties();
    set_parent(parent);
}

SgAsmJvmExceptionTable* SgAsmJvmExceptionTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t exception_table_length;
  Jvm::read_value(pool, exception_table_length);

  auto exceptions = get_exceptions();
  for (int ii = 0; ii < exception_table_length; ii++) {
    mlog[INFO] << "\n --- exception ---\n";
    auto exception = new SgAsmJvmException(this);
    exception->parse(pool);
    exceptions.push_back(exception);
  }

  return this;
}

void SgAsmJvmExceptionTable::unparse(std::ostream &os) const
{
  uint16_t exception_table_length = get_exceptions().size();
  hostToBe(exception_table_length, &exception_table_length);
  os.write(reinterpret_cast<const char*>(&exception_table_length), sizeof exception_table_length);

  for (auto exception : get_exceptions()) {
    exception->unparse(os);
  }
}

void SgAsmJvmExceptionTable::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  mlog[WARN] << "SgAsmJvmExceptionTable::dump() not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// InnerClassTable used by SgAsmJvmInnerClass (InnerClassTable_attribute from the Jvm specification (4.7.6))
//
SgAsmJvmInnerClassesEntry::SgAsmJvmInnerClassesEntry(SgAsmJvmInnerClasses* table)
{
  mlog[INFO] << "\nSgAsmJvmInnerClassesEntry::ctor() ...\n";
  initializeProperties();
}

SgAsmJvmInnerClassesEntry* SgAsmJvmInnerClassesEntry::parse(SgAsmJvmConstantPool* pool)
{
  mlog[INFO] << "SgAsmJvmInnerClassesEntry::parse() ...\n";
  return this;
}

void SgAsmJvmInnerClassesEntry::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmInnerClassesEntry is not implemented yet\n";
}

void SgAsmJvmInnerClassesEntry::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  mlog[INFO] << "SgAsmJvmInnerClassesEntry::dump() is not implemented yet\n";
}

SgAsmJvmInnerClasses::SgAsmJvmInnerClasses(SgAsmJvmAttribute* parent)
{
  mlog[INFO] << "SgAsmJvmInnerClasses::ctor() ...\n";
  initializeProperties();
}

SgAsmJvmInnerClasses* SgAsmJvmInnerClasses::parse(SgAsmJvmConstantPool* pool)
{
  mlog[INFO] << "SgAsmJvmInnerClasses::parse() ...\n";
  mlog[INFO] << "SgAsmJvmInnerClasses::parse() exit ... \n";

  return this;
}

void SgAsmJvmInnerClasses::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmInnerClasses is not implemented yet\n";
}

void SgAsmJvmInnerClasses::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  mlog[INFO] << "SgAsmJvmInnerClasses::dump() ...\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SgAsmJvmLineNumberTable attribute node (LineNumberTable_attribute from the Jvm specification (4.7.12)
//
SgAsmJvmLineNumberTable::SgAsmJvmLineNumberTable(SgAsmJvmAttribute* parent)
{
  mlog[INFO] << "\nSgAsmJvmLineNumberTable::ctor() ...\n";
  initializeProperties();
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

void SgAsmJvmLineNumberTable::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t line_number_table_length = get_line_number_table().size();
  hostToBe(line_number_table_length, &line_number_table_length);
  os.write(reinterpret_cast<const char*>(&line_number_table_length), sizeof line_number_table_length);

  for (auto line_number : get_line_number_table()) {
    line_number->unparse(os);
  }
}

void SgAsmJvmLineNumberTable::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: SgAsmJvmLineNumberTable::dump()\n", prefix, idx);
}

SgAsmJvmLineNumberEntry::SgAsmJvmLineNumberEntry(SgAsmJvmLineNumberTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmLineNumberEntry* SgAsmJvmLineNumberEntry::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_line_number);
  return this;
}

void SgAsmJvmLineNumberEntry::unparse(std::ostream& os) const
{
  auto start_pc = p_start_pc;
  auto line_number = p_line_number;
  hostToBe(start_pc, &start_pc);
  hostToBe(line_number, &line_number);
  os.write(reinterpret_cast<const char*>(&start_pc), sizeof start_pc);
  os.write(reinterpret_cast<const char*>(&line_number), sizeof line_number);
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

void SgAsmJvmModuleMainClass::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmModuleMainClass is not implemented yet\n";
}

void SgAsmJvmModuleMainClass::dump(FILE* f, const char *prefix, ssize_t idx) const
{
  ROSE_ASSERT(false && "SgAsmJvmModuleMainClass::dump()");
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
