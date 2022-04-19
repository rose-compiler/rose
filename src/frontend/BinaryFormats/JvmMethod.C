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
  set_parent(table);
  p_attribute_table = new SgAsmJvmAttributeTable(this);
  ROSE_ASSERT(get_attribute_table()->get_attributes());
  ROSE_ASSERT(get_attribute_table()->get_attributes()->get_parent());
}

SgAsmJvmMethod* SgAsmJvmMethod::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_access_flags);
  Jvm::read_value(pool, p_name_index);
  Jvm::read_value(pool, p_descriptor_index);

  /* Method attributes */
  p_attribute_table->parse(pool);

  return this;
}

void SgAsmJvmMethod::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%d:%d:%d\n", prefix, p_access_flags, p_name_index, p_descriptor_index);
}


SgAsmJvmMethodTable::SgAsmJvmMethodTable(SgAsmJvmClassFile* jcf)
{
  std::cout << "SgAsmJvmMethodTable::ctor() ...\n";
}

SgAsmJvmMethodTable* SgAsmJvmMethodTable::parse()
{
  uint16_t methods_count;

  std::cout << "SgAsmJvmMethodTable::parse() ...\n";
  std::cout << "SgAsmJvmMethodTable::parse() exit ... \n";

  return this;
}

void SgAsmJvmMethodTable::dump(FILE* f, const char *prefix, ssize_t idx) const
{
  fprintf(f, "%s", prefix);
  for (auto method : get_methods()->get_entries()) {
    method->dump(stdout, "   ", idx++);
  }
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
