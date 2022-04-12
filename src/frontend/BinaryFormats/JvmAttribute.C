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

  std::cout << "SgAsmJvmAttribute::create_attribute() ...\n";

  Jvm::read_value(pool, attribute_name_index, false);
  std::cout << "SgAsmJvmAttribute::attribute_name_index " << attribute_name_index << std::endl;
  std::string name = pool->get_utf8_string(attribute_name_index);
  std::cout << "SgAsmJvmAttribute::attribute name is " << name << std::endl;

  if (name == "Code") {
    std::cout << "SgAsmJvmAttribute:: returning new Code attribute ...\n";
    return new SgAsmJvmCodeAttribute();
  }
  else if (name == "ConstantValue") {
    std::cout << "SgAsmJvmAttribute:: returning new ConstantValue attribute ...\n";
    return new SgAsmJvmConstantValue();
  }
  else if (name == "LineNumberTable") {
    std::cout << "SgAsmJvmAttribute:: skipping LineNumberTable attribute ";
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
  p_attributes = new SgAsmJvmAttributeList;
  p_attributes->set_parent(this);
}

SgAsmJvmAttributeTable* SgAsmJvmAttributeTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t attributes_count;

  std::cout << "SgAsmJvmAttributeTable::parse() ...\n";

  Jvm::read_value(pool, attributes_count, true);

  std::cout << "SgAsmJvmAttributeTable::attributes_count " << attributes_count << std::endl;

  auto attributes = get_attributes()->get_entries();
  for (int ii = 0; ii < attributes_count; ii++) {
    auto attribute = SgAsmJvmAttribute::create_attribute(pool);
    // attribute may not be implemented yet
    if (attribute) {
      attribute->parse(pool);
      attributes.push_back(attribute);
    }
  }

  return this;
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

  // skip exception table
  uint16_t exception_table_length;
  Jvm::read_value(pool, exception_table_length);
  std::cout << "SgAsmJvmCodeAttribute::parse(): exception_table_length is " << exception_table_length << std::endl;
  ROSE_ASSERT(exception_table_length == 0);

  // skip attributes
  uint16_t attributes_count;
  Jvm::read_value(pool, attributes_count);
  std::cout << "SgAsmJvmCodeAttribute::parse(): attributes_count is " << attributes_count << std::endl;

  //TODO
  //auto attributes = get_attributes()->get_entries();
  for (int ii = 0; ii < attributes_count; ii++) {
    auto attribute = SgAsmJvmAttribute::create_attribute(pool);
    // attribute may not be implemented yet
    if (attribute) {
      //TODO
      // attribute->parse(pool);
      // attributes.push_back(attribute);
    }
  }

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

  dump(std::cout);

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

  ROSE_ASSERT(false && "TODO");
  return this;
}

void SgAsmJvmAttribute::dump(std::ostream &os) const
{
  os << "SgAsmJvmAttribute:" << p_attribute_name_index << ":" << p_attribute_length << std::endl;
}

void SgAsmJvmCodeAttribute::dump(std::ostream &os) const
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmCodeAttribute:" << p_max_stack << ":" << p_max_locals << ":" << p_code_length << std::endl;
}

void SgAsmJvmConstantValue::dump(std::ostream &os) const
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmConstantValue:" << p_constantvalue_index << std::endl;
}

void SgAsmJvmSignature::dump(std::ostream &os) const
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmSignature::dump\n";
}

void SgAsmJvmSourceFile::dump(std::ostream &os) const
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmSourceFile::dump\n";
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
  std::cout << "SgAsmJvmException::parse() ...\n";
  return this;
}

void SgAsmJvmException::dump(std::ostream &os) const
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
  std::cout << "SgAsmJvmExceptionTable::parse() ...\n";
  std::cout << "SgAsmJvmExceptionTable::parse() exit ... \n\n";

  return this;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
