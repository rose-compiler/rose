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

  std::cout << "SgAsmJvmAttribute::createAttribute() ...\n";

  read_jvm_value(pool, attribute_name_index, false);
  std::cout << "SgAsmJvmAttribute::attribute_name_index " << attribute_name_index << std::endl;
  std::string name = pool->get_utf8_string(attribute_name_index);
  std::cout << "SgAsmJvmAttribute::attribute name is " << name << std::endl;

  if (name == "Code") {
    std::cout << "SgAsmJvmAttribute:: returning new Code attribute ...\n";
    return new SgAsmJvmCodeAttribute();
  }

  return nullptr;
}

SgAsmJvmAttribute* SgAsmJvmAttribute::parse(SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmAttribute::parse() ...\n";

  read_jvm_value(pool, p_attribute_name_index, true);
  read_jvm_value(pool, p_attribute_length, true);

  return this;
}

SgAsmJvmAttribute* SgAsmJvmCodeAttribute::parse(SgAsmJvmConstantPool* pool)
{
  char* bytes{nullptr};

  SgAsmJvmAttribute::parse(pool);
  std::cout << "SgAsmJvmCodeAttribute::parse() ...\n";

  read_jvm_value(pool, p_max_stack, true);
  read_jvm_value(pool, p_max_locals, true);

  /* allocate and read the code array */
  p_code_length = read_jvm_bytes(pool, bytes);
  set_code(bytes);

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
  os << "SgAsmJvmAttribute:" << p_attribute_name_index << ":" << p_attribute_length << std::endl;
}

void SgAsmJvmCodeAttribute::dump(std::ostream &os)
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmCodeAttribute:" << p_max_stack << ":" << p_max_locals << ":" << p_code_length << std::endl;
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

#endif // ROSE_ENABLE_BINARY_ANALYSIS
