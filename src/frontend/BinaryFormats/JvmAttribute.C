/* JVM Attributes */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Rose::Diagnostics;
using namespace ByteOrder;

SgAsmJvmAttribute::SgAsmJvmAttribute(const SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmAttribute::ctor() ...\n";
}

SgAsmJvmAttribute* SgAsmJvmAttribute::create_attribute(SgAsmJvmConstantPool* pool)
{
  SgAsmGenericHeader* header{pool->get_header()};
  rose_addr_t offset{header->get_offset()};
  uint16_t attribute_name_index;

  std::cout << "SgAsmJvmAttribute::createAttribute() ...\n";
  std::cout << "SgAsmJvmAttribute:createAttribute() offset is " << offset << std::endl;

  // attribute_name_index
  //
  auto count = header->read_content(offset, &attribute_name_index, sizeof attribute_name_index);
  if (2 != count) {
    //throw FormatError("Bad Java class file attribute_name_index");
    ROSE_ASSERT(false && "Bad Java class file attribute_name_index");
  }
  attribute_name_index = be_to_host(attribute_name_index);
  offset += count;
  // Don't advance offset, just need to see what attribute type to create?
  // header->set_offset(offset);

  std::cout << "SgAsmJvmAttribute::attribute_name_index " << attribute_name_index << std::endl;

}

SgAsmJvmAttribute* SgAsmJvmAttribute::parse(SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmAttribute::parse() ...\n";

  return this;
}

SgAsmJvmAttribute* SgAsmJvmConstantValue::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmConstantValue::parse(pool);
  std::cout << "SgAsmJvmConstantValue::parse() ...\n";

  return this;
}

SgAsmJvmAttribute* SgAsmJvmSignature::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmSignature::parse(pool);
  std::cout << "SgAsmJvmSignature::parse() ...\n";

  return this;
}

SgAsmJvmAttribute* SgAsmJvmSourceFile::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmSourceFile::parse(pool);
  std::cout << "SgAsmJvmSourceFile::parse() ...\n";

  return this;
}

void SgAsmJvmAttribute::dump(std::ostream &os)
{
  os << "SgAsmJvmAttribute::dump\n";
}

void SgAsmJvmConstantValue::dump(std::ostream &os)
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmConstantValue::dump\n";
}

void SgAsmJvmSignature::dump(std::ostream &os)
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmSignature::dump\n";
}

void SgAsmJvmSourceFile::dump(std::ostream &os)
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmSourceFile::dump\n";
}

#endif
