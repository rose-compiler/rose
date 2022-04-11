/* JVM Fields */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include "JvmClassFile.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Rose::Diagnostics;
using namespace ByteOrder;

SgAsmJvmField::SgAsmJvmField(SgAsmJvmFieldTable* table)
{
  std::cout << "SgAsmJvmField::ctor() ...\n";

  p_attribute_table = new SgAsmJvmAttributeTable;
  p_attribute_table->set_parent(this);
  set_parent(table);

  ROSE_ASSERT(get_attribute_table()->get_attributes());
}

SgAsmJvmField* SgAsmJvmField::parse(SgAsmJvmConstantPool* pool)
{
  std::cout << "SgAsmJvmField::parse() ...\n";
  return this;
}

void SgAsmJvmField::dump(std::ostream &os) const
{
  std::cout << "SgAsmJvmField::dump() ...\n";
}


SgAsmJvmFieldTable::SgAsmJvmFieldTable(SgAsmJvmClassFile* jcf)
{
  std::cout << "SgAsmJvmFieldTable::ctor() ...\n";
}

SgAsmJvmFieldTable* SgAsmJvmFieldTable::parse()
{
  std::cout << "SgAsmJvmFieldTable::parse() ...\n";
  std::cout << "SgAsmJvmFieldTable::parse() exit ... \n\n";

  return this;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
