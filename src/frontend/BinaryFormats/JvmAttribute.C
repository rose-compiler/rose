/* JVM Attributes */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Rose::Diagnostics;
using namespace ByteOrder;

#if 0
SgAsmJvmAttribute::SgAsmJvmAttribute(SgAsmGenericHeader* fhdr)
  : SgAsmGenericSection(fhdr->get_file(), fhdr)
{
  std::cout << "SgAsmJvmAttribute::ctor() ...\n";
}
#endif

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

void SgAsmJvmAttribute::dump(std::ostream &os)
{
  os << "SgAsmJvmAttribute::dump\n";
}

void SgAsmJvmConstantValue::dump(std::ostream &os)
{
  SgAsmJvmAttribute::dump(os);
  os << "SgAsmJvmConstantValue::dump\n";
}

#endif
