/* JVM Attributes */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "Jvm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using Rose::BinaryAnalysis::ByteOrder::hostToBe;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7 Attributes. SgAsmJvmAttribute is the base class for all of the attributes.
//
SgAsmJvmAttribute* SgAsmJvmAttribute::instance(SgAsmJvmConstantPool* pool, SgAsmJvmAttributeTable* parent)
{
  uint16_t attribute_name_index;
  uint32_t attribute_length;

  Jvm::read_value(pool, attribute_name_index, /*advance_offset*/false);
  std::string name = pool->get_utf8_string(attribute_name_index);

  if (name == "Code") {
    return new SgAsmJvmCodeAttribute(parent);
  }
  else if (name == "ConstantValue") { // 4.7.2
    return new SgAsmJvmConstantValue(parent);
  }
  else if (name == "StackMapTable") { // 4.7.3
    return new SgAsmJvmStackMapTable(parent);
  }
  else if (name == "SourceFile") { // 4.7.10
    return new SgAsmJvmSourceFile(parent);
  }
  else if (name == "LineNumberTable") { // 4.7.12
    return new SgAsmJvmLineNumberTable(parent);
  }

  // skip attribute
  Jvm::read_value(pool, attribute_name_index);
  Jvm::read_value(pool, attribute_length);

  mlog[FATAL] << "--- " << name << " attribute ---\n"
              << "SgAsmJvmAttribute::instance(): skipping attribute of length " << attribute_length << "\n";
#ifdef DEBUG_ON
  ROSE_ABORT();
#endif

  SgAsmGenericHeader* header{pool->get_header()};
  ASSERT_not_null(header);

  rose_addr_t offset{header->get_offset()};
  header->set_offset(offset + attribute_length);

  return nullptr;
}

SgAsmJvmAttribute* SgAsmJvmAttribute::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_attribute_name_index);
  Jvm::read_value(pool, p_attribute_length);
  return this;
}

void SgAsmJvmAttribute::unparse(std::ostream& os) const
{
  auto attribute_name_index = p_attribute_name_index;
  auto attribute_length = p_attribute_length;

  hostToBe(attribute_name_index, &attribute_name_index);
  hostToBe(attribute_length, &attribute_length);

  os.write(reinterpret_cast<const char*>(&attribute_name_index), sizeof attribute_name_index);
  os.write(reinterpret_cast<const char*>(&attribute_length), sizeof attribute_length);
}

void SgAsmJvmAttribute::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld:%d:%d\n", prefix, idx, p_attribute_name_index, p_attribute_length);
}

SgAsmJvmAttributeTable::SgAsmJvmAttributeTable(SgAsmJvmFileHeader* jfh, SgAsmNode* parent)
{
  initializeProperties();
  set_parent(parent);
  set_header(jfh);
}

SgAsmJvmAttributeTable* SgAsmJvmAttributeTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t attributes_count;

  Jvm::read_value(pool, attributes_count);

  for (int ii = 0; ii < attributes_count; ii++) {
    auto attribute = SgAsmJvmAttribute::instance(pool, /*parent*/this);
    // attribute may not be implemented yet
    if (attribute) {
      attribute->parse(pool);
      get_attributes().push_back(attribute);
    }
  }
  return this;
}

void SgAsmJvmAttributeTable::unparse(std::ostream& os) const
{
  uint16_t attributes_count = get_attributes().size();
  hostToBe(attributes_count, &attributes_count);
  os.write(reinterpret_cast<const char*>(&attributes_count), sizeof attributes_count);

  for (auto attribute : get_attributes()) {
    attribute->unparse(os);
  }
}

void SgAsmJvmAttributeTable::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s", prefix);
  for (auto attribute : get_attributes()) {
    attribute->dump(stdout, "   ", idx++);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.2 The ConstantValue Attribute. ConstantValue_attribute is represented by the SgAsmJvmConstantValue class.
//
SgAsmJvmConstantValue::SgAsmJvmConstantValue(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmAttribute* SgAsmJvmConstantValue::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  // The value of the attribute_length item must be two (section 4.7.2)
  ASSERT_require(p_attribute_length == 2);
  Jvm::read_value(pool, p_constantvalue_index);

  return this;
}

void SgAsmJvmConstantValue::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmConstantValue is not implemented yet\n";
}

void SgAsmJvmConstantValue::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "SgAsmJvmConstantValue:%d\n", p_constantvalue_index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.3 The Code Attribute (Code_attribute represented by the SgAsmJvmCodeAttribute class)
//
SgAsmJvmCodeAttribute::SgAsmJvmCodeAttribute(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmAttribute* SgAsmJvmCodeAttribute::parse(SgAsmJvmConstantPool* pool)
{
  uint32_t length;
  char* bytes{nullptr};
  auto header = dynamic_cast<SgAsmJvmFileHeader*>(pool->get_header());

  SgAsmJvmAttribute::parse(pool);

  Jvm::read_value(pool, p_max_stack);
  Jvm::read_value(pool, p_max_locals);

  /* set the offset for the code array (used later for disassembly/decoding) */
  set_code_offset(header->get_offset());

  /* allocate and read the code array */
  p_code_length = Jvm::read_bytes(pool, bytes, length);
  set_code(bytes);

  /* exception table */
  p_exception_table = new SgAsmJvmExceptionTable(this);
  p_exception_table->parse(pool);

  /* attribute table */
  p_attribute_table = new SgAsmJvmAttributeTable(header, /*parent*/this);
  p_attribute_table->parse(pool);

  return this;
}

void SgAsmJvmCodeAttribute::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  auto max_stack = p_max_stack;
  auto max_locals = p_max_locals;
  auto code_length = p_code_length;

  hostToBe(max_stack, &max_stack);
  hostToBe(max_locals, &max_locals);
  hostToBe(code_length, &code_length);

  os.write(reinterpret_cast<const char*>(&max_stack), sizeof max_stack);
  os.write(reinterpret_cast<const char*>(&max_locals), sizeof max_stack);
  os.write(reinterpret_cast<const char*>(&code_length), sizeof code_length);

  // Write code
  os.write(p_code, p_code_length);

  p_exception_table->unparse(os);
  p_attribute_table->unparse(os);
}

void SgAsmJvmCodeAttribute::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "-->SgAsmJvmCodeAttribute:%d:%d:%d\n", p_max_stack, p_max_locals, p_code_length);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.4 The StackMapTable Attribute. StackMapTable_attribute represented by the SgAsmJvmStackMapTable class.
//
SgAsmJvmStackMapTable::SgAsmJvmStackMapTable(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmStackMapTable* SgAsmJvmStackMapTable::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  uint16_t numEntries;
  Jvm::read_value(pool, numEntries);

  for (int ii = 0; ii < numEntries; ii++) {
    auto frame = new SgAsmJvmStackMapFrame(this);
    frame->parse(pool);
    get_entries().push_back(frame);
  }
  return this;
}

void SgAsmJvmStackMapTable::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t numEntries = get_entries().size();
  hostToBe(numEntries, &numEntries);
  os.write(reinterpret_cast<const char*>(&numEntries), sizeof numEntries);

  for (auto frame : get_entries()) {
    frame->unparse(os);
  }
}

void SgAsmJvmStackMapTable::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  mlog[WARN] << "dump of SgAsmJvmStackMapTable not fully implemented\n";
  // fprintf(f, "-->SgAsmJvmStackMapTable:%d:%d:%d\n", p_max_stack, p_max_locals, p_code_length);
}

SgAsmJvmStackMapFrame::SgAsmJvmStackMapFrame(SgAsmJvmStackMapTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmStackMapFrame* SgAsmJvmStackMapFrame::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t numStack{0}, numLocals{0};

  Jvm::read_value(pool, p_frame_type);

  switch (p_frame_type) {
    case 0 ... 63: // frame_type = SAME
      break;
    case 64 ... 127: // frame_type = SAME_LOCALS_1_STACK_ITEM
      numStack = 1;
      break;
    case 247: // frame_type = SAME_LOCALS_1_STACK_ITEM_EXTENDED
      numStack = 1;
      Jvm::read_value(pool, p_offset_delta);
      break;
    case 248 ... 250: // frame_type = CHOP
      Jvm::read_value(pool, p_offset_delta);
      break;
    case 251: // frame_type = SAME_FRAME_EXTENDED
      Jvm::read_value(pool, p_offset_delta);
      break;
    case 252 ... 254: // frame_type = SAME_FRAME_EXTENDED
      Jvm::read_value(pool, p_offset_delta);
      numLocals = p_frame_type - 251;
      break;
    case 255: { // frame_type = FULL_FRAME
      Jvm::read_value(pool, p_offset_delta);
      Jvm::read_value(pool, numLocals);

      // Unfortunately, locals are read before number_of_stack_items
      for (int ii = 0; ii < numLocals; ii++) {
        auto type = new SgAsmJvmStackMapVerificationType(this);
        type->parse(pool);
        get_locals().push_back(type);
      }
      numLocals = 0;

      Jvm::read_value(pool, numStack);
      break;
    }
    default:
      mlog[ERROR] << "SgAsmJvmStackMapFrame::parse: illegal frame type\n";
      ROSE_ABORT();
      break;
  }

  for (int ii = 0; ii < numLocals; ii++) {
    auto type = new SgAsmJvmStackMapVerificationType(this);
    type->parse(pool);
    get_locals().push_back(type);
  }

  for (int ii = 0; ii < numStack; ii++) {
    auto type = new SgAsmJvmStackMapVerificationType(this);
    type->parse(pool);
    get_stack().push_back(type);
  }

  return this;
}

void SgAsmJvmStackMapFrame::unparse(std::ostream& os) const
{
  auto type = p_frame_type;
  hostToBe(type, &type);
  os.write(reinterpret_cast<const char*>(&type), sizeof type);

  // Warning: order is important (see FULL_FRAME above)

  if (auto offset = p_offset_delta) {
    hostToBe(offset, &offset);
    os.write(reinterpret_cast<const char*>(&offset), sizeof offset);
  }

  // uint16_t number_of_locals;
  // verification_type_info locals[number_of_locals];
  //
  uint16_t numLocals = get_locals().size();
  if (numLocals || p_frame_type==0xff) { // numLocals must be written for frame_type==FULL_FRAME
    hostToBe(numLocals, &numLocals);
    os.write(reinterpret_cast<const char*>(&numLocals), sizeof numLocals);
  }
  for (auto local : get_locals()) {
    local->unparse(os);
  }

  // uint16_t number_of_stack_items;
  // verification_type_info stack[number_of_stack_items];
  //
  uint16_t numStack = get_stack().size();
  if (numStack || p_frame_type==0xff) { // numStack must be written for frame_type==FULL_FRAME
    hostToBe(numStack, &numStack);
    os.write(reinterpret_cast<const char*>(&numStack), sizeof numStack);
  }
  for (auto stackEntry : get_stack()) {
    stackEntry->unparse(os);
  }
}

void SgAsmJvmStackMapFrame::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  mlog[WARN] << "dump of SgAsmJvmStackMapFrame is not implemented yet\n";
  // fprintf(f, "-->SgAsmJvmStackMapFrame:%d:%d:%d\n", p_max_stack, p_max_locals, p_code_length);
}

SgAsmJvmStackMapVerificationType::SgAsmJvmStackMapVerificationType(SgAsmJvmStackMapFrame* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmStackMapVerificationType* SgAsmJvmStackMapVerificationType::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_tag);

  switch (p_tag) {
    case 0: // ITEM_Top
    case 1: // ITEM_Integer
    case 2: // ITEM_Float
    case 3: // ITEM_Double
    case 4: // ITEM_Long
    case 5: // ITEM_Null
    case 6: // ITEM_UninitializedThis
      break;
    case 7: // ITEM_Object
      Jvm::read_value(pool, p_cpool_index);
      break;
    case 8: // ITEM_Uninitialized
      Jvm::read_value(pool, p_offset);
      break;
    default:
      mlog[ERROR] << "SgAsmJvmStackMapVerificationType::parse: illegal type\n";
      ROSE_ABORT();
      break;
  }
  return this;
}

void SgAsmJvmStackMapVerificationType::unparse(std::ostream& os) const
{
  auto tag = p_tag;
  hostToBe(tag, &tag);
  os.write(reinterpret_cast<const char*>(&tag), sizeof tag);

  if (auto index = p_cpool_index) {
    hostToBe(index, &index);
    os.write(reinterpret_cast<const char*>(&index), sizeof index);
  }
  if (auto offset = p_offset) {
    hostToBe(offset, &offset);
    os.write(reinterpret_cast<const char*>(&offset), sizeof offset);
  }
}

void SgAsmJvmStackMapVerificationType::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  mlog[WARN] << "dump of SgAsmJvmStackMapVerificationType is not implemented yet\n";
  // fprintf(f, "-->SgAsmJvmStackMapVerificationType:%d:%d:%d\n", p_max_stack, p_max_locals, p_code_length);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.5 The Exceptions Attribute. Exceptions_attribute represented by the SgAsmJvmException class.
//
SgAsmJvmException::SgAsmJvmException(SgAsmJvmExceptionTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmException* SgAsmJvmException::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_end_pc);
  Jvm::read_value(pool, p_handler_pc);
  Jvm::read_value(pool, p_catch_type);
  return this;
}

void SgAsmJvmException::unparse(std::ostream &os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmException is not implemented yet\n";
}

void SgAsmJvmException::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  mlog[WARN] << "SgAsmJvmException::dump() not implemented yet\n";
}

SgAsmJvmExceptionTable::SgAsmJvmExceptionTable(SgAsmJvmCodeAttribute* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmExceptionTable* SgAsmJvmExceptionTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t exception_table_length;
  Jvm::read_value(pool, exception_table_length);

  for (int ii = 0; ii < exception_table_length; ii++) {
    mlog[INFO] << "\n --- exception ---\n";
    auto exception = new SgAsmJvmException(this);
    exception->parse(pool);
    get_exceptions().push_back(exception);
  }

  return this;
}

void SgAsmJvmExceptionTable::unparse(std::ostream &os) const
{
  uint16_t exception_table_length = get_exceptions().size();
  hostToBe(exception_table_length, &exception_table_length);
  os.write(reinterpret_cast<const char*>(&exception_table_length), sizeof exception_table_length);

  for (auto exception : get_exceptions()) {
    exception->unparse(os);
  }
}

void SgAsmJvmExceptionTable::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  mlog[WARN] << "SgAsmJvmExceptionTable::dump() not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.6 The InnerClasses Attribute. InnerClasses_attribute is represented by the SgAsmJvmInnerClasses class.
//
SgAsmJvmInnerClasses::SgAsmJvmInnerClasses(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmInnerClasses* SgAsmJvmInnerClasses::parse(SgAsmJvmConstantPool* pool)
{
  mlog[WARN] << "Parsing of SgAsmJvmInnerClasses is not implemented yet\n";
  return this;
}

void SgAsmJvmInnerClasses::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmInnerClasses is not implemented yet\n";
}

void SgAsmJvmInnerClasses::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  mlog[WARN] << "SgAsmJvmInnerClasses::dump() ...\n";
}

SgAsmJvmInnerClassesEntry::SgAsmJvmInnerClassesEntry(SgAsmJvmInnerClasses* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmInnerClassesEntry* SgAsmJvmInnerClassesEntry::parse(SgAsmJvmConstantPool* pool)
{
  mlog[WARN] << "Parsing of SgAsmJvmInnerClassesEntry is not implemented yet\n";
  return this;
}

void SgAsmJvmInnerClassesEntry::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmInnerClassesEntry is not implemented yet\n";
}

void SgAsmJvmInnerClassesEntry::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  mlog[INFO] << "SgAsmJvmInnerClassesEntry::dump() is not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.7 The EnclosingMethod Attribute. EnclosingMethod_attribute represented by the TODO.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.8 The Synthetic Attribute. Synthetic_attribute represented by the TODO.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.9 The Signature Attribute. Signature_attribute represented by the SgAsmJvmSignature class.
//
SgAsmJvmSignature::SgAsmJvmSignature(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmAttribute* SgAsmJvmSignature::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  mlog[WARN] << "Parsing of SgAsmJvmSignature attribute is not implemented yet\n";
  return this;
}

void SgAsmJvmSignature::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmSignature attribute is not implemented yet\n";
}

void SgAsmJvmSignature::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  mlog[WARN] << "dump of SgAsmJvmSignature is not implemented yet\n";
  //fprintf(f, "SgAsmJvmSignature::dump\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.10 The SourceFile Attribute. SourceFile_attribute represented by the SgAsmJvmSourceFile class.
//
SgAsmJvmSourceFile::SgAsmJvmSourceFile(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmAttribute* SgAsmJvmSourceFile::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, p_sourcefile_index);
  return this;
}

void SgAsmJvmSourceFile::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  auto index = p_sourcefile_index;
  hostToBe(index, &index);
  os.write(reinterpret_cast<const char*>(&index), sizeof index);
}

void SgAsmJvmSourceFile::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "SgAsmJvmSourceFile::dump():%d\n", p_sourcefile_index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.11 The SourceDebugExtension Attribute. SourceDebugExtension_attribute represented by the TODO.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.12 The LineNumberTable Attribute.  LineNumberTable_attribute represented by the SgAsmJvmLineNumberTable class.
//
SgAsmJvmLineNumberTable::SgAsmJvmLineNumberTable(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmLineNumberTable* SgAsmJvmLineNumberTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t length;
  ASSERT_not_null(get_parent());

  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, length);

  for (int ii = 0; ii < length; ii++) {
    auto entry = new SgAsmJvmLineNumberEntry(this);
    entry->parse(pool);
    get_line_number_table().push_back(entry);
  }
  return this;
}

void SgAsmJvmLineNumberTable::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t line_number_table_length = get_line_number_table().size();
  hostToBe(line_number_table_length, &line_number_table_length);
  os.write(reinterpret_cast<const char*>(&line_number_table_length), sizeof line_number_table_length);

  for (auto line_number : get_line_number_table()) {
    line_number->unparse(os);
  }
}

void SgAsmJvmLineNumberTable::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: SgAsmJvmLineNumberTable::dump()\n", prefix, idx);
}

SgAsmJvmLineNumberEntry::SgAsmJvmLineNumberEntry(SgAsmJvmLineNumberTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmLineNumberEntry* SgAsmJvmLineNumberEntry::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_line_number);
  return this;
}

void SgAsmJvmLineNumberEntry::unparse(std::ostream& os) const
{
  auto start_pc = p_start_pc;
  auto line_number = p_line_number;
  hostToBe(start_pc, &start_pc);
  hostToBe(line_number, &line_number);
  os.write(reinterpret_cast<const char*>(&start_pc), sizeof start_pc);
  os.write(reinterpret_cast<const char*>(&line_number), sizeof line_number);
}

void SgAsmJvmLineNumberEntry::dump(FILE*f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: start_pc:%d line_number:%d\n", prefix, idx, p_start_pc, p_line_number);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.13 The LocalVariableTable Attribute. LocalVariableTable_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.14 The LocalVariableTypeTable Attribute. LocalVariableTypeTable_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.15 The Deprecated Attribute. Deprecated_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.16 The RuntimeVisibleAnnotations Attribute. RuntimeVisibleAnnotations_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.17 The RuntimeInvisibleAnnotations Attribute. RuntimeInvisibleAnnotations_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.18 The RuntimeVisibleParameterAnnotations Attribute. RuntimeVisibleParameterAnnotations_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.19 The RuntimeInvisibleParameterAnnotations Attribute. RuntimeInvisibleParameterAnnotations_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.20 The RuntimeVisibleTypeAnnotations Attribute. RuntimeVisibleTypeAnnotations_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.21 The RuntimeInvisibleTypeAnnotations Attribute. RuntimeInvisibleTypeAnnotations_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.22 The AnnotationDefault Attribute. AnnotationDefault_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.23 The BootstrapMethods Attribute. BootstrapMethods_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.24 The MethodParameters Attribute. MethodParameters_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.25 The Module Attribute. Module_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.26 The ModulePackages Attribute. ModulePackages_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.27 The ModuleMainClass Attribute. ModuleMainClass_attribute represented by the SgAsmJvmModuleMainClass class.
//
SgAsmJvmAttribute* SgAsmJvmModuleMainClass::parse(SgAsmJvmConstantPool* pool)
{
  mlog[WARN] << "Parsing of SgAsmJvmModuleMainClass is not implemented yet\n";
  return nullptr;
}

void SgAsmJvmModuleMainClass::unparse(std::ostream& os) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmModuleMainClass is not implemented yet\n";
}

void SgAsmJvmModuleMainClass::dump(FILE* f, const char *prefix, ssize_t idx) const
{
  mlog[WARN] << "dump (of SgAsmJvmModuleMainClass) is not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.28 The NestHost Attribute. NestHost_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.29 The NestMembers Attribute. NestMembers_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.30 The Record Attribute. Record_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.31 The PermittedSubclasses Attribute. PermittedSubclasses_attribute represented by the TODO class.
//

#endif // ROSE_ENABLE_BINARY_ANALYSIS
