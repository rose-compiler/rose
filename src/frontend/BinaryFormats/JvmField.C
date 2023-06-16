/* JVM Fields */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "Jvm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using Rose::BinaryAnalysis::ByteOrder::hostToBe;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

SgAsmJvmField::SgAsmJvmField(SgAsmJvmFieldTable* table)
{
  initializeProperties();

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

void SgAsmJvmField::unparse(std::ostream& os) const
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

  //  p_attribute_table->unparse(os);
}

void SgAsmJvmField::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmField::dump::%d:%d:%d\n", p_access_flags, p_name_index, p_descriptor_index);
}

SgAsmJvmFieldTable::SgAsmJvmFieldTable(SgAsmJvmFileHeader* jfh)
{
  initializeProperties();
  set_parent(jfh);
  set_header(jfh);
}

SgAsmJvmFieldTable* SgAsmJvmFieldTable::parse()
{
  uint16_t fields_count;

  auto jfh = dynamic_cast<SgAsmJvmFileHeader*>(get_parent());
  ASSERT_not_null(jfh);
  auto pool = jfh->get_constant_pool();
  ASSERT_not_null(pool);

  Jvm::read_value(pool, fields_count);
  for (int ii = 0; ii < fields_count; ii++) {
    auto field = new SgAsmJvmField(this);
    field->parse(pool);
    get_fields().push_back(field);
  }
  return this;
}

void SgAsmJvmFieldTable::unparse(std::ostream& os) const
{
  uint16_t fields_count = get_fields().size();

  hostToBe(fields_count, &fields_count);
  os.write(reinterpret_cast<const char*>(&fields_count), sizeof fields_count);

  for (auto field : get_fields()) {
    field->unparse(os);
  }
}

void SgAsmJvmFieldTable::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s", prefix);
  for (auto field : get_fields()) {
    field->dump(stdout, "   ", idx++);
  }
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
