/* JVM Attributes */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include "JvmClassFile.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Rose::Diagnostics;
using namespace ByteOrder;

SgAsmJvmAttribute* SgAsmJvmAttribute::create_attribute(SgAsmJvmConstantPool* pool)
{
  uint16_t attribute_name_index;
  uint32_t attribute_length;

  Jvm::read_value(pool, attribute_name_index, /*advance_offset*/false);
  std::string name = pool->get_utf8_string(attribute_name_index);
  std::cout << "\n--- " << name << " attribute ---\n";

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

  std::cout << "of length " << attribute_length << std::endl;

  SgAsmGenericHeader* header{pool->get_header()};
  ROSE_ASSERT(header);
  std::cout << "--> header is " << header->class_name() << std::endl;

  rose_addr_t offset{header->get_offset()};
  std::cout << "--> offset is " << offset << std::endl;

  header->set_offset(offset + attribute_length);

  return nullptr;
}

SgAsmJvmAttributeTable::SgAsmJvmAttributeTable()
{
  std::cout << "SgAsmJvmAttributeTable::ctor() ... WARNING: parent NOT set\n";
  p_attributes = new SgAsmJvmAttributeList;
  p_attributes->set_parent(this);
}
SgAsmJvmAttributeTable::SgAsmJvmAttributeTable(SgAsmJvmClassFile* parent)
{
  std::cout << "SgAsmJvmAttributeTable::ctor() ...\n";
  set_parent(parent);
  p_attributes = new SgAsmJvmAttributeList;
  p_attributes->set_parent(this);
}
SgAsmJvmAttributeTable::SgAsmJvmAttributeTable(SgAsmJvmNode* parent)
{
  std::cout << "SgAsmJvmAttributeTable::ctor() ...\n";
  set_parent(parent);
  p_attributes = new SgAsmJvmAttributeList;
  p_attributes->set_parent(this);
}

SgAsmJvmAttributeTable* SgAsmJvmAttributeTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t attributes_count;

  std::cout << "SgAsmJvmAttributeTable::parse() ...\n";

  Jvm::read_value(pool, attributes_count);
  auto attributes = get_attributes()->get_entries();
  for (int ii = 0; ii < attributes_count; ii++) {
    auto attribute = SgAsmJvmAttribute::create_attribute(pool);
    // attribute may not be implemented yet
    if (attribute) {
      attribute->set_parent(this);
      attribute->parse(pool);
      attributes.push_back(attribute);
    }
  }
  return this;
}

void SgAsmJvmAttributeTable::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s", prefix);
  for (auto attribute : get_attributes()->get_entries()) {
    attribute->dump(stdout, "   ", idx++);
  }
}

SgAsmJvmAttribute* SgAsmJvmAttribute::parse(SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmAttribute::parse() ...\n";

  Jvm::read_value(pool, p_attribute_name_index);
  Jvm::read_value(pool, p_attribute_length);

  return this;
}

SgAsmJvmAttribute* SgAsmJvmCodeAttribute::parse(SgAsmJvmConstantPool* pool)
{
  uint32_t length;
  char* bytes{nullptr};

  SgAsmJvmAttribute::parse(pool);
  std::cout << "SgAsmJvmCodeAttribute::parse() ...\n";

  Jvm::read_value(pool, p_max_stack);
  Jvm::read_value(pool, p_max_locals);

  /* allocate and read the code array */
  p_code_length = Jvm::read_bytes(pool, bytes, length);
  set_code(bytes);

  std::cout << "SgAsmJvmCodeAttribute::parse() attr length is " << p_attribute_length << std::endl;
  std::cout << "SgAsmJvmCodeAttribute::parse() code length is " << length << std::endl;

  /* exception table */
  p_exception_table = new SgAsmJvmExceptionTable(this);
  ROSE_ASSERT(p_exception_table && p_exception_table->get_exceptions());
  p_exception_table->parse(pool);

  /* attribute table */
  p_attribute_table = new SgAsmJvmAttributeTable(this);
  ROSE_ASSERT(p_attribute_table && p_attribute_table->get_attributes());
  p_attribute_table->parse(pool);

  dump(stdout, "", 0);

  // try creating an instruction

// Interface should be changed to
//
// SgAsmInstruction*
// Rose::BinaryAnalysis::Disassembler
//     ::disassembleOne(const unsigned char* buf,
//                      rose_addr_t buf_va,
//                      size_t buf_size,
//                      rose_addr_t start_va,
//                      AddressSet* successors=nullptr
//                     )
//
// auto inst = Rose::BinaryAnalysis::JvmDisassembler::disassembleOne(...);

  return this;
}

SgAsmJvmAttribute* SgAsmJvmConstantValue::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  std::cout << "SgAsmJvmConstantValue::parse() ...\n";

  // The value of the attribute_length item must be two (section 4.7.2)
  ROSE_ASSERT(p_attribute_length == 2);

  Jvm::read_value(pool, p_constantvalue_index);
  std::cout << "SgAsmJvmConstantValue::parse:constantvalue_index " << p_constantvalue_index << std::endl;

  return this;
}

SgAsmJvmAttribute* SgAsmJvmSignature::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  std::cout << "SgAsmJvmSignature::parse() ...\n";

  ROSE_ASSERT(false && "TODO");
  return this;
}

SgAsmJvmAttribute* SgAsmJvmSourceFile::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  std::cout << "SgAsmJvmSourceFile::parse() ...\n";
  Jvm::read_value(pool, p_sourcefile_index);
  return this;
}

void SgAsmJvmAttribute::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmAttribute:%d:%d\n", p_attribute_name_index, p_attribute_length);
}

void SgAsmJvmCodeAttribute::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "SgAsmJvmCodeAttribute:%d:%d:%d\n", p_max_stack, p_max_locals, p_code_length);
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
  fprintf(f, "SgAsmJvmSourceFile:%d\n", p_sourcefile_index);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Exception nodes used by SgAsmJvmCodeAttribute
//
SgAsmJvmException::SgAsmJvmException(SgAsmJvmExceptionTable* table)
{
  std::cout << "\nSgAsmJvmException::ctor() ...\n";
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
  std::cout << "SgAsmJvmException::dump() ...\n";
}

SgAsmJvmExceptionTable::SgAsmJvmExceptionTable(SgAsmJvmCodeAttribute* parent)
{
  std::cout << "SgAsmJvmExceptionTable::ctor() ...\n";
  set_parent(parent);
  p_exceptions = new SgAsmJvmExceptionList;
  p_exceptions->set_parent(this);
}

SgAsmJvmExceptionTable* SgAsmJvmExceptionTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t exception_table_length;
  Jvm::read_value(pool, exception_table_length);

  //if (exception_table_length == 0) return this; // TODO: until message dump ends
  std::cout << "SgAsmJvmExceptionTable::parse() ...\n";
  std::cout << "SgAsmJvmExceptionTable::parse() exception table length is " << exception_table_length << std::endl;

  auto exceptions = get_exceptions()->get_entries();
  for (int ii = 0; ii < exception_table_length; ii++) {
    std::cout << "\n --- exception ---\n";
    auto exception = new SgAsmJvmException(this);
    exception->parse(pool);
    exceptions.push_back(exception);
  }
  std::cout << "SgAsmJvmExceptionTable::parse() exit ... \n";

  return this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// InnerClassTable used by SgAsmJvmInnerClass (InnerClassTable_attribute from the Jvm specification (4.7.6))
//
SgAsmJvmInnerClass::SgAsmJvmInnerClass(SgAsmJvmInnerClassTable* table)
{
  std::cout << "\nSgAsmJvmInnerClass::ctor() ...\n";
}

SgAsmJvmInnerClass* SgAsmJvmInnerClass::parse(SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmInnerClass::parse() ...\n";
  return this;
}

void SgAsmJvmInnerClass::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  std::cout << "SgAsmJvmInnerClass::dump() ...\n";
}

SgAsmJvmInnerClassTable::SgAsmJvmInnerClassTable(SgAsmJvmAttribute* parent)
{
  std::cout << "SgAsmJvmInnerClassTable::ctor() ...\n";
}

SgAsmJvmInnerClassTable* SgAsmJvmInnerClassTable::parse(SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmInnerClassTable::parse() ...\n";
  std::cout << "SgAsmJvmInnerClassTable::parse() exit ... \n";

  return this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SgAsmJvmLineNumberTable attribute node (LineNumberTable_attribute from the Jvm specification (4.7.12)
//
SgAsmJvmLineNumberTable::SgAsmJvmLineNumberTable(SgAsmJvmCodeAttribute* parent)
{
  std::cout << "\nSgAsmJvmLineNumberTable::ctor() ...\n";
  set_parent(parent);

  p_line_number_table = new SgAsmJvmLineNumberInnerTable(this);
  ROSE_ASSERT(get_line_number_table());
  //TODO:
  //  p_line_number_table->set_parent(this);

  ROSE_ASSERT(false);
}

SgAsmJvmLineNumberTable* SgAsmJvmLineNumberTable::parse(SgAsmJvmConstantPool* pool)
{
  ROSE_ASSERT(get_parent());
  std::cout << "SgAsmJvmLineNumberTable::parse() ...\n";

  uint16_t line_number_table_length;
  SgAsmJvmAttribute::parse(pool);

  Jvm::read_value(pool, line_number_table_length);
  std::cout << "SgAsmJvmLineNumberTable::parse() line_number_table_length is " << line_number_table_length << std::endl;

  //TODO: fix attribute constructor (see above)
  if (get_line_number_table() == nullptr) {
    p_line_number_table = new SgAsmJvmLineNumberInnerTable(this);
    ROSE_ASSERT(get_line_number_table());
    ROSE_ASSERT(get_line_number_table()->get_parent());
  }

  auto inner_table = get_line_number_table();
  ROSE_ASSERT(inner_table);
  auto entries = inner_table->get_line_numbers()->get_entries();
  for (int ii = 0; ii < line_number_table_length; ii++) {
    auto line_number = new SgAsmJvmLineNumber(inner_table);
    line_number->parse(pool);
    line_number->dump(stdout, "", 0);
    entries.push_back(line_number);
  }
  inner_table->dump(stdout, "", 0);

  return this;
}

void SgAsmJvmLineNumberTable::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmLineNumberTable::dump() ...\n");
}

SgAsmJvmLineNumber::SgAsmJvmLineNumber(SgAsmJvmLineNumberInnerTable* table)
{
  set_parent(table);
}

SgAsmJvmLineNumber* SgAsmJvmLineNumber::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_line_number);
  return this;
}

void SgAsmJvmLineNumber::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmLineNumber:%d:%d\n", p_start_pc, p_line_number);
}

SgAsmJvmLineNumberInnerTable::SgAsmJvmLineNumberInnerTable(SgAsmJvmLineNumberTable* parent)
{
  std::cout << "SgAsmJvmLineNumberInnerTable::ctor() ...\n";

  set_parent(parent);
  p_line_numbers = new SgAsmJvmLineNumberList;
  p_line_numbers->set_parent(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
