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
  ROSE_ASSERT(get_attribute_table()->get_parent());
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
  jcf->set_method_table(this);
  set_parent(jcf);

#ifdef WORKING_ON
  ROSE_ASSERT(get_header() == nullptr);
  auto header = jcf->get_header(SgAsmGenericFile::FAMILY_JVM);
  ROSE_ASSERT(header);
  set_header(header);
#endif

  std::cout << "-->method_table: " << jcf->get_method_table() << std::endl;
}

SgAsmJvmMethodTable* SgAsmJvmMethodTable::parse()
{
  uint16_t methods_count;

  auto jcf = dynamic_cast<SgAsmJvmClassFile*>(get_parent());
  ROSE_ASSERT(jcf && "JVM class_file is a nullptr");
  auto pool = jcf->get_constant_pool();
  ROSE_ASSERT(pool && "JVM constant_pool is a nullptr");

  Jvm::read_value(pool, methods_count);
  for (int ii = 0; ii < methods_count; ii++) {
    auto method = new SgAsmJvmMethod(this);
    method->parse(pool);
    get_methods().push_back(method);
  }

  return this;
}

void SgAsmJvmMethodTable::dump(FILE* f, const char *prefix, ssize_t idx) const
{
  fprintf(f, "%s", prefix);
  for (auto method : get_methods()) {
    method->dump(stdout, "method->", idx++);
  }
}

SgAsmJvmAttribute* SgAsmJvmEnclosingMethod::parse(SgAsmJvmConstantPool* pool)
{
  ROSE_ASSERT(false && "SgAsmJvmEnclosingMethod::parse()");
  return nullptr;
}

void SgAsmJvmEnclosingMethod::dump(FILE* f, const char *prefix, ssize_t idx) const
{
  ROSE_ASSERT(false && "SgAsmJvmEnclosingMethod::dump()");
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
