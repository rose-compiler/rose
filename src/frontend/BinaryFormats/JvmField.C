/* JVM Fields */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "Jvm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmJvmField::SgAsmJvmField(SgAsmJvmFieldTable* table)
{
  ASSERT_not_null(table);
  set_parent(table);

  auto header = dynamic_cast<SgAsmJvmFileHeader*>(table->get_header());
  p_attribute_table = new SgAsmJvmAttributeTable(header, /*parent*/this);
}

SgAsmJvmField* SgAsmJvmField::parse(SgAsmJvmConstantPool* pool)
{
  ASSERT_not_null(pool);

  Jvm::read_value(pool, p_access_flags);
  Jvm::read_value(pool, p_name_index);
  Jvm::read_value(pool, p_descriptor_index);

  p_attribute_table->parse(pool);

  return this;
}

void SgAsmJvmField::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmField::dump::%d:%d:%d\n", p_access_flags, p_name_index, p_descriptor_index);
}

SgAsmJvmFieldTable::SgAsmJvmFieldTable(SgAsmJvmFileHeader* jfh)
{
  set_parent(jfh);
  set_header(jfh);
}

SgAsmJvmFieldTable* SgAsmJvmFieldTable::parse()
{
  uint16_t fields_count;

  auto jfh = dynamic_cast<SgAsmJvmFileHeader*>(get_parent());
  ROSE_ASSERT(jfh && "JVM file header is a nullptr");
  auto pool = jfh->get_constant_pool();
  ROSE_ASSERT(pool && "JVM constant_pool is a nullptr");

  Jvm::read_value(pool, fields_count);
  for (int ii = 0; ii < fields_count; ii++) {
    auto field = new SgAsmJvmField(this);
    field->parse(pool);
    get_fields().push_back(field);
  }
  return this;
}

void SgAsmJvmFieldTable::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s", prefix);
  for (auto field : get_fields()) {
    field->dump(stdout, "   ", idx++);
  }
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
