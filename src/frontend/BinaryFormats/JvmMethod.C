/* JVM Methods */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include "JvmClassFile.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Rose::Diagnostics;
using namespace ByteOrder;

SgAsmJvmMethod::SgAsmJvmMethod(SgAsmJvmMethodTable* table)
{
  ROSE_ASSERT(table);
  std::cout << "\nSgAsmJvmMethod::ctor() ...\n";

  p_attribute_table = new SgAsmJvmAttributeTable;
  p_attribute_table->set_parent(this);
  set_parent(table);

  ROSE_ASSERT(get_attribute_table()->get_attributes());
}

SgAsmJvmMethod* SgAsmJvmMethod::parse(SgAsmJvmConstantPool* pool)
{
  ROSE_ASSERT(pool);
  std::cout << "SgAsmJvmMethod::parse() ...\n";

  Jvm::read_value(pool, p_access_flags);
  Jvm::read_value(pool, p_name_index);
  Jvm::read_value(pool, p_descriptor_index);

  /* Attributes */
  p_attribute_table->parse(pool);

  dump(std::cout);

  std::cout << "SgAsmJvmMethod::parse() exit ... \n\n";

  return this;
}

void SgAsmJvmMethod::dump(std::ostream &os) const
{
  os << "SgAsmJvmMethod::" << p_access_flags << ":"
     << p_name_index  << ":" << p_descriptor_index << "\n";
}


SgAsmJvmMethodTable::SgAsmJvmMethodTable(SgAsmJvmClassFile* jcf)
{
  std::cout << "SgAsmJvmMethodTable::ctor() ...\n";
}

SgAsmJvmMethodTable* SgAsmJvmMethodTable::parse()
{
  std::cout << "SgAsmJvmMethodTable::parse() ...\n";
  std::cout << "SgAsmJvmMethodTable::parse() exit ... \n\n";

  return this;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
