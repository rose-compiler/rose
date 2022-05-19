/* JVM Methods */
#include <featureTests.h>

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "Jvm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmJvmMethod::SgAsmJvmMethod(SgAsmJvmMethodTable* table)
{
  ASSERT_not_null(table);
  set_parent(table);

  auto header = dynamic_cast<SgAsmJvmFileHeader*>(table->get_header());
  p_attribute_table = new SgAsmJvmAttributeTable(header, this);

  p_instruction_list = new SgAsmInstructionList;
  p_instruction_list->set_parent(this);
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


SgAsmJvmMethodTable::SgAsmJvmMethodTable(SgAsmJvmFileHeader* jfh)
{
  set_parent(jfh);
  set_header(jfh);
}

SgAsmJvmMethodTable* SgAsmJvmMethodTable::parse()
{
  uint16_t methods_count;

  auto jcf = dynamic_cast<SgAsmJvmFileHeader*>(get_parent());
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
