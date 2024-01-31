/* JVM Methods */
#include <featureTests.h>

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "Jvm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using Rose::BinaryAnalysis::ByteOrder::hostToBe;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

SgAsmJvmMethod::SgAsmJvmMethod(SgAsmJvmMethodTable* table)
{
  initializeProperties();
  ASSERT_not_null(table);
  set_parent(table);

  auto header = dynamic_cast<SgAsmJvmFileHeader*>(table->get_header());
  p_attribute_table = new SgAsmJvmAttributeTable(header, this);
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

void SgAsmJvmMethod::unparse(std::ostream& os) const
{
  auto access_flags = p_access_flags;
  auto name_index = p_name_index;
  auto descriptor_index = p_descriptor_index;

  hostToBe(access_flags, &access_flags);
  hostToBe(name_index, &name_index);
  hostToBe(descriptor_index, &descriptor_index);

  os.write(reinterpret_cast<const char*>(&access_flags), sizeof access_flags);
  os.write(reinterpret_cast<const char*>(&name_index), sizeof name_index);
  os.write(reinterpret_cast<const char*>(&descriptor_index), sizeof descriptor_index);

  p_attribute_table->unparse(os);
}

void SgAsmJvmMethod::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%d:%d:%d\n", prefix, p_access_flags, p_name_index, p_descriptor_index);
}


SgAsmJvmMethodTable::SgAsmJvmMethodTable(SgAsmJvmFileHeader* jfh)
{
  initializeProperties();
  set_parent(jfh);
  set_header(jfh);
}

SgAsmJvmMethodTable* SgAsmJvmMethodTable::parse()
{
  uint16_t methods_count;

  auto jcf = dynamic_cast<SgAsmJvmFileHeader*>(get_parent());
  ASSERT_not_null(jcf);
  auto pool = jcf->get_constant_pool();
  ASSERT_not_null(pool);

  Jvm::read_value(pool, methods_count);
  for (int ii = 0; ii < methods_count; ii++) {
    auto method = new SgAsmJvmMethod(this);
    method->parse(pool);
    get_methods().push_back(method);
  }

  return this;
}

void SgAsmJvmMethodTable::unparse(std::ostream& os) const
{
  uint16_t methods_count = get_methods().size();
  hostToBe(methods_count, &methods_count);
  os.write(reinterpret_cast<const char*>(&methods_count), sizeof methods_count);

  for (auto method : get_methods()) {
    method->unparse(os);
  }
}

void SgAsmJvmMethodTable::dump(FILE* f, const char *prefix, ssize_t idx) const
{
  fprintf(f, "%s", prefix);
  for (auto method : get_methods()) {
    method->dump(stdout, "method->", idx++);
  }
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
