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
  else if (name == "Exceptions") { // 4.7.5
    return new SgAsmJvmExceptions(parent);
  }
  else if (name == "InnerClasses") { // 4.7.6
    return new SgAsmJvmInnerClasses(parent);
  }
  else if (name == "EnclosingMethod") { // 4.7.7
    return new SgAsmJvmEnclosingMethod(parent);
  }
  else if (name == "Synthetic") { // 4.7.8
    return new SgAsmJvmSynthetic(parent);
  }
  else if (name == "Signature") { // 4.7.9
    return new SgAsmJvmSignature(parent);
  }
  else if (name == "SourceFile") { // 4.7.10
    return new SgAsmJvmSourceFile(parent);
  }
  else if (name == "LineNumberTable") { // 4.7.12
    return new SgAsmJvmLineNumberTable(parent);
  }
  else if (name == "LocalVariableTable") { // 4.7.13
    return new SgAsmJvmLocalVariableTable(parent);
  }
  else if (name == "LocalVariableTypeTable") { // 4.7.14
    return new SgAsmJvmLocalVariableTable(parent);
  }
  else if (name == "Deprecated") { // 4.7.15
    return new SgAsmJvmDeprecated(parent);
  }
  else if (name == "RuntimeVisibleAnnotations") { // 4.7.16
    return new SgAsmJvmRuntimeVisibleAnnotations(parent);
  }
  else if (name == "RuntimeInvisibleAnnotations") { // 4.7.17
    return new SgAsmJvmRuntimeInvisibleAnnotations(parent);
  }
  else if (name == "AnnotationDefault") { // 4.7.22
    //TODO
  }
  else if (name == "BootstrapMethods") { // 4.7.23
    return new SgAsmJvmBootstrapMethods(parent);
  }
  else if (name == "MethodParameters") { // 4.7.24
    return new SgAsmJvmMethodParameters(parent);
  }
  else if (name == "Module") { // 4.7.25
    //TODO
  }
  else if (name == "ModulePackages") { // 4.7.26
    //TODO
  }
  else if (name == "ModuleMainClass") { // 4.7.27
    //TODO
  }
  else if (name == "NestHost") { // 4.7.28
    return new SgAsmJvmNestHost(parent);
  }
  else if (name == "NestMembers") { // 4.7.29
    return new SgAsmJvmNestMembers(parent);
  }

  // skip attribute
  Jvm::read_value(pool, attribute_name_index);
  Jvm::read_value(pool, attribute_length);

  mlog[FATAL] << "--- attribute name: " << name << " ---\n"
              << "SgAsmJvmAttribute::instance(): skipping attribute of length " << attribute_length << "\n";
  ASSERT_require2(false, "Missing attribute\n");

  SgAsmGenericHeader* header{pool->get_header()};
  ASSERT_not_null(header);

  Rose::BinaryAnalysis::Address offset{header->get_offset()};
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
  Jvm::writeValue(os, p_attribute_name_index);
  Jvm::writeValue(os, p_attribute_length);
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
  uint16_t numAttributes;

  Jvm::read_value(pool, numAttributes);

  for (int ii = 0; ii < numAttributes; ii++) {
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
  uint16_t numAttributes = get_attributes().size();
  Jvm::writeValue(os, numAttributes);

  for (auto attribute : get_attributes()) {
    attribute->unparse(os);
  }
}

void SgAsmJvmAttributeTable::dump(FILE* f, const char* prefix, ssize_t idx) const
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
  SgAsmJvmAttribute::unparse(os);
  Jvm::writeValue(os, p_constantvalue_index);
}

void SgAsmJvmConstantValue::dump(FILE* f, const char* prefix, ssize_t idx) const
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

  Jvm::writeValue(os, p_max_stack);
  Jvm::writeValue(os, p_max_locals);
  Jvm::writeValue(os, p_code_length);

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

SgAsmJvmExceptionTable::SgAsmJvmExceptionTable(SgAsmJvmCodeAttribute* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmExceptionTable* SgAsmJvmExceptionTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t tableLength;
  Jvm::read_value(pool, tableLength);

  for (int ii = 0; ii < tableLength; ii++) {
    auto handler = new SgAsmJvmExceptionHandler(this);
    handler->parse(pool);
    get_handlers().push_back(handler);
  }
  return this;
}

void SgAsmJvmExceptionTable::unparse(std::ostream &os) const
{
  uint16_t tableLength = get_handlers().size();
  Jvm::writeValue(os, tableLength);

  for (auto handler : get_handlers()) {
    handler->unparse(os);
  }
}

void SgAsmJvmExceptionTable::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "SgAsmJvmExceptionTable::dump() not implemented yet\n";
}

SgAsmJvmExceptionHandler::SgAsmJvmExceptionHandler(SgAsmJvmExceptionTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmExceptionHandler* SgAsmJvmExceptionHandler::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_end_pc);
  Jvm::read_value(pool, p_handler_pc);
  Jvm::read_value(pool, p_catch_type);
  return this;
}

void SgAsmJvmExceptionHandler::unparse(std::ostream &os) const
{
  Jvm::writeValue(os, p_start_pc);
  Jvm::writeValue(os, p_end_pc);
  Jvm::writeValue(os, p_handler_pc);
  Jvm::writeValue(os, p_catch_type);
}

void SgAsmJvmExceptionHandler::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "SgAsmJvmExceptionHandler::dump() not implemented yet\n";
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
  Jvm::writeValue(os, numEntries);

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
  uint16_t numLocals = get_locals().size();
  uint16_t numStack = get_stack().size();

  Jvm::writeValue(os, p_frame_type);

  switch (p_frame_type) {
    case 0 ... 63:   // frame_type = SAME
    case 64 ... 127: // frame_type = SAME_LOCALS_1_STACK_ITEM
      break;
    case 247:         // frame_type = SAME_LOCALS_1_STACK_ITEM_EXTENDED
    case 248 ... 250: // frame_type = CHOP
    case 251:         // frame_type = SAME_FRAME_EXTENDED
    case 252 ... 254: // frame_type = SAME_FRAME_EXTENDED
      Jvm::writeValue(os, p_offset_delta);
      break;
    case 255: { // frame_type = FULL_FRAME
      Jvm::writeValue(os, p_offset_delta);
      Jvm::writeValue(os, numLocals);

      for (auto type : get_locals()) {
        type->unparse(os);
      }
      numLocals = 0;

      Jvm::writeValue(os, numStack);
      break;
    }
  }

  if (numLocals > 0) {
    for (auto type : get_locals()) {
      type->unparse(os);
    }
  }
  for (auto type : get_stack()) {
    type->unparse(os);
  }
}

void SgAsmJvmStackMapFrame::dump(FILE*, const char*, ssize_t) const
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
  Jvm::writeValue(os, p_tag);

  if (p_tag == 7) {
    Jvm::writeValue(os, p_cpool_index);
  }
  else if (p_tag == 8) {
    Jvm::writeValue(os, p_offset);
  }
}

void SgAsmJvmStackMapVerificationType::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "dump of SgAsmJvmStackMapVerificationType is not implemented yet\n";
  // fprintf(f, "-->SgAsmJvmStackMapVerificationType:%d:%d:%d\n", p_max_stack, p_max_locals, p_code_length);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.5 The Exceptions Attribute. Exceptions_attribute represented by the SgAsmJvmExceptions class.
//
SgAsmJvmExceptions::SgAsmJvmExceptions(SgAsmJvmAttributeTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmExceptions* SgAsmJvmExceptions::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  uint16_t numExceptions;
  Jvm::read_value(pool, numExceptions);

  SgUnsigned16List u16List;
  for (int ii = 0; ii < numExceptions; ii++) {
    uint16_t index;
    Jvm::read_value(pool, index);
    u16List.push_back(index);
  }
  set_exception_index_table(u16List);

  return this;
}

void SgAsmJvmExceptions::unparse(std::ostream &os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t numExceptions = get_exception_index_table().size();
  Jvm::writeValue(os, numExceptions);

  for (uint16_t index : get_exception_index_table()) {
    Jvm::writeValue(os, index);
  }
}

void SgAsmJvmExceptions::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "SgAsmJvmExceptions::dump() not implemented yet\n";
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
  SgAsmJvmAttribute::parse(pool);

  uint16_t numClasses{0};
  Jvm::read_value(pool, numClasses);

  for (int ii = 0; ii < numClasses; ii++) {
    auto entry = new SgAsmJvmInnerClassesEntry(this);
    entry->parse(pool);
    get_classes().push_back(entry);
  }
  return this;
}

void SgAsmJvmInnerClasses::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t numClasses = get_classes().size();
  Jvm::writeValue(os, numClasses);

  for (auto entry : get_classes()) {
    entry->unparse(os);
  }
}

void SgAsmJvmInnerClasses::dump(FILE*, const char*, ssize_t) const
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
  Jvm::read_value(pool, p_inner_class_info_index);
  Jvm::read_value(pool, p_outer_class_info_index);
  Jvm::read_value(pool, p_inner_name_index);
  Jvm::read_value(pool, p_inner_class_access_flags);
  return this;
}

void SgAsmJvmInnerClassesEntry::unparse(std::ostream& os) const
{
  Jvm::writeValue(os, p_inner_class_info_index);
  Jvm::writeValue(os, p_outer_class_info_index);
  Jvm::writeValue(os, p_inner_name_index);
  Jvm::writeValue(os, p_inner_class_access_flags);
}

void SgAsmJvmInnerClassesEntry::dump(FILE*, const char*, ssize_t) const
{
  mlog[INFO] << "SgAsmJvmInnerClassesEntry::dump() is not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.7 The EnclosingMethod Attribute. EnclosingMethod_attribute is represented by the SgAsmJvmEnclosingMethod class.
//
SgAsmJvmEnclosingMethod::SgAsmJvmEnclosingMethod(SgAsmJvmAttributeTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmEnclosingMethod* SgAsmJvmEnclosingMethod::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, p_class_index);
  Jvm::read_value(pool, p_method_index);
  return this;
}

void SgAsmJvmEnclosingMethod::unparse(std::ostream &os) const
{
  SgAsmJvmAttribute::unparse(os);
  Jvm::writeValue(os, p_class_index);
  Jvm::writeValue(os, p_method_index);
}

void SgAsmJvmEnclosingMethod::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "%s    class_index:%d method_index:%d\n", prefix, p_class_index, p_method_index);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.8 The Synthetic Attribute. Synthetic_attribute represented by the SgAsmJvmSynthetic class.
//
SgAsmJvmSynthetic::SgAsmJvmSynthetic(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmAttribute* SgAsmJvmSynthetic::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  // The value of the attribute_length item must be zero (section 4.7.8)
  ASSERT_require(p_attribute_length == 0);
  return this;
}

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
  Jvm::read_value(pool, p_signature_index);
  return this;
}

void SgAsmJvmSignature::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);
  Jvm::writeValue(os, p_signature_index);
}

void SgAsmJvmSignature::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  SgAsmJvmAttribute::dump(f, prefix, idx);
  fprintf(f, "%s    signature_index:%d\n", prefix, p_signature_index);
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
  Jvm::writeValue(os, p_sourcefile_index);
}

void SgAsmJvmSourceFile::dump(FILE* f, const char* prefix, ssize_t idx) const
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
  uint16_t tableLength;
  ASSERT_not_null(get_parent());

  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, tableLength);

  for (int ii = 0; ii < tableLength; ii++) {
    auto entry = new SgAsmJvmLineNumberEntry(this);
    entry->parse(pool);
    get_line_number_table().push_back(entry);
  }
  return this;
}

void SgAsmJvmLineNumberTable::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t tableLength = get_line_number_table().size();
  Jvm::writeValue(os, tableLength);

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
  Jvm::writeValue(os, p_start_pc);
  Jvm::writeValue(os, p_line_number);
}

void SgAsmJvmLineNumberEntry::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: start_pc:%d line_number:%d\n", prefix, idx, p_start_pc, p_line_number);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.13 The LocalVariableTable Attribute. LocalVariableTable_attribute represented by the TODO class.
//
SgAsmJvmLocalVariableTable::SgAsmJvmLocalVariableTable(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmLocalVariableTable* SgAsmJvmLocalVariableTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t tableLength;
  ASSERT_not_null(get_parent());

  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, tableLength);

  for (int ii = 0; ii < tableLength; ii++) {
    auto entry = new SgAsmJvmLocalVariableEntry(this);
    entry->parse(pool);
    get_local_variable_table().push_back(entry);
  }
  return this;
}

void SgAsmJvmLocalVariableTable::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t tableLength = get_local_variable_table().size();
  Jvm::writeValue(os, tableLength);

  for (auto local_variable : get_local_variable_table()) {
    local_variable->unparse(os);
  }
}

void SgAsmJvmLocalVariableTable::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: SgAsmJvmLocalVariableTable::dump()\n", prefix, idx);
}

SgAsmJvmLocalVariableEntry::SgAsmJvmLocalVariableEntry(SgAsmJvmLocalVariableTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmLocalVariableEntry* SgAsmJvmLocalVariableEntry::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_length);
  Jvm::read_value(pool, p_name_index);
  Jvm::read_value(pool, p_descriptor_index);
  Jvm::read_value(pool, p_index);
  return this;
}

void SgAsmJvmLocalVariableEntry::unparse(std::ostream& os) const
{
  Jvm::writeValue(os, p_start_pc);
  Jvm::writeValue(os, p_length);
  Jvm::writeValue(os, p_name_index);
  Jvm::writeValue(os, p_descriptor_index);
  Jvm::writeValue(os, p_index);
}

void SgAsmJvmLocalVariableEntry::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: start_pc:%d length:%d name_index:%d descriptor_index:%d index:%d\n",
          prefix, idx, p_start_pc, p_length, p_name_index, p_descriptor_index, p_index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.14 The LocalVariableTypeTable Attribute. LocalVariableTypeTable_attribute represented by the TODO class.
//
SgAsmJvmLocalVariableTypeTable::SgAsmJvmLocalVariableTypeTable(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmLocalVariableTypeTable* SgAsmJvmLocalVariableTypeTable::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t tableLength;
  ASSERT_not_null(get_parent());

  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, tableLength);

  for (int ii = 0; ii < tableLength; ii++) {
    auto entry = new SgAsmJvmLocalVariableTypeEntry(this);
    entry->parse(pool);
    get_local_variable_type_table().push_back(entry);
  }
  return this;
}

void SgAsmJvmLocalVariableTypeTable::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t tableLength = get_local_variable_type_table().size();
  Jvm::writeValue(os, tableLength);

  for (auto local_variable_type : get_local_variable_type_table()) {
    local_variable_type->unparse(os);
  }
}

void SgAsmJvmLocalVariableTypeTable::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: SgAsmJvmLocalVariableTypeTable::dump()\n", prefix, idx);
}

SgAsmJvmLocalVariableTypeEntry::SgAsmJvmLocalVariableTypeEntry(SgAsmJvmLocalVariableTypeTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmLocalVariableTypeEntry* SgAsmJvmLocalVariableTypeEntry::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_start_pc);
  Jvm::read_value(pool, p_length);
  Jvm::read_value(pool, p_name_index);
  Jvm::read_value(pool, p_signature_index);
  Jvm::read_value(pool, p_index);
  return this;
}

void SgAsmJvmLocalVariableTypeEntry::unparse(std::ostream& os) const
{
  Jvm::writeValue(os, p_start_pc);
  Jvm::writeValue(os, p_length);
  Jvm::writeValue(os, p_name_index);
  Jvm::writeValue(os, p_signature_index);
  Jvm::writeValue(os, p_index);
}

void SgAsmJvmLocalVariableTypeEntry::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: start_pc:%d length:%d name_index:%d signature_index:%d index:%d\n",
          prefix, idx, p_start_pc, p_length, p_name_index, p_signature_index, p_index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.15 The Deprecated Attribute. Deprecated_attribute represented by the SgAsmJvmSynthetic class.
//
SgAsmJvmDeprecated::SgAsmJvmDeprecated(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmAttribute* SgAsmJvmDeprecated::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  // The value of the attribute_length item must be zero (section 4.7.15)
  ASSERT_require(p_attribute_length == 0);
  return this;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.16 The RuntimeVisibleAnnotations Attribute. RuntimeVisibleAnnotations_attribute is represented by the
// SgAsmJvmRuntimeVisibleAnnotations class.
//
SgAsmJvmRuntimeVisibleAnnotations::SgAsmJvmRuntimeVisibleAnnotations(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmRuntimeVisibleAnnotations* SgAsmJvmRuntimeVisibleAnnotations::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t numAnnotations;
  ASSERT_not_null(get_parent());

  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, numAnnotations);

  for (int ii = 0; ii < numAnnotations; ii++) {
    auto annotation = new SgAsmJvmRuntimeAnnotation(this);
    annotation->parse(pool);
    get_annotations().push_back(annotation);
  }

  return this;
}

void SgAsmJvmRuntimeVisibleAnnotations::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t numAnnotations = get_annotations().size();
  Jvm::writeValue(os, numAnnotations);

  for (auto annotation : get_annotations()) {
    annotation->unparse(os);
  }
}

void SgAsmJvmRuntimeVisibleAnnotations::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: SgAsmJvmRuntimeVisibleAnnotations::dump()\n", prefix, idx);
  for (auto annotation : get_annotations()) {
    annotation->dump(f, prefix, idx);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The RuntimeAnnotation class used by the RuntimeVisibleAnnotations_attribute and the
// RuntimeInvisibleAnnotations_attribute.
//
SgAsmJvmRuntimeAnnotation::SgAsmJvmRuntimeAnnotation(SgAsmJvmRuntimeVisibleAnnotations* parent)
{
  initializeProperties();
  set_parent(parent);
}
SgAsmJvmRuntimeAnnotation::SgAsmJvmRuntimeAnnotation(SgAsmJvmRuntimeInvisibleAnnotations* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmRuntimeAnnotation* SgAsmJvmRuntimeAnnotation::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t numPairs;
  Jvm::read_value(pool, p_type_index);
  Jvm::read_value(pool, numPairs);

  for (int ii = 0; ii < numPairs; ii++) {
    auto pair = new SgAsmJvmRuntimeAnnotationPair(this);
    pair->parse(pool);
    get_element_value_pairs().push_back(pair);
  }

  return this;
}

void SgAsmJvmRuntimeAnnotation::unparse(std::ostream& os) const
{
  Jvm::writeValue(os, p_type_index);

  uint16_t numPairs = get_element_value_pairs().size();
  Jvm::writeValue(os, numPairs);

  for (auto pair : get_element_value_pairs()) {
    pair->unparse(os);
  }
}

void SgAsmJvmRuntimeAnnotation::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: type_index:%d\n", prefix, idx, p_type_index);
  for (auto pair : get_element_value_pairs()) {
    pair->dump(f, prefix, idx);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The RuntimeAnnotationPair class is used by the RuntimeAnnotation class.
//
SgAsmJvmRuntimeAnnotationPair::SgAsmJvmRuntimeAnnotationPair(SgAsmJvmRuntimeAnnotation* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmRuntimeAnnotationPair* SgAsmJvmRuntimeAnnotationPair::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_element_name_index);

  p_value = new SgAsmJvmRuntimeAnnotationValue(this);
  p_value->parse(pool);

  return this;
}

void SgAsmJvmRuntimeAnnotationPair::unparse(std::ostream& os) const
{
  Jvm::writeValue(os, p_element_name_index);
  p_value->unparse(os);
}

void SgAsmJvmRuntimeAnnotationPair::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: RuntimeAnnotationPair: element_name_index:%d\n", prefix, idx, p_element_name_index);
  p_value->dump(f, prefix, idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The RuntimeAnnotationValue class is used by the RuntimeAnnotation class.
//
SgAsmJvmRuntimeAnnotationValue::SgAsmJvmRuntimeAnnotationValue(SgAsmJvmRuntimeAnnotationValue* parent)
{
  initializeProperties();
  set_parent(parent);
}
SgAsmJvmRuntimeAnnotationValue::SgAsmJvmRuntimeAnnotationValue(SgAsmJvmRuntimeAnnotationPair* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmRuntimeAnnotationValue* SgAsmJvmRuntimeAnnotationValue::parse(SgAsmJvmConstantPool* pool)
{
    Jvm::read_value(pool, p_tag);

    switch (p_tag) {
      // const_value_index tags
      case 'B': case 'C': case 'D': case 'F': // byte, char, double, float
      case 'I': case 'J': case 'S': case 'Z': // int, long, short, boolean
      case 's': // String
        Jvm::read_value(pool, p_const_value_index);
        break;
      case 'e': // enum_const_value
        Jvm::read_value(pool, p_type_name_index);
        Jvm::read_value(pool, p_const_name_index);
        break;
      case 'c': // class_info_index
        Jvm::read_value(pool, p_class_info_index);
        break;
      case '@': // nested annotation
        set_is_annotation_value(true);
        ASSERT_require2(p_tag!='@', "tag '@' annotation_value unsupported\n");
        break;
      case '[': { // array_value
        uint16_t numArrayValues;
        Jvm::read_value(pool, numArrayValues);
        for (uint16_t i = 0; i < numArrayValues; ++i) {
          auto value = new SgAsmJvmRuntimeAnnotationValue(this);
          value->parse(pool);
          get_values().push_back(value);
        }
        break;
      }
      default:
        throw std::runtime_error("Unknown element_value tag byte: " + std::to_string(p_tag));
    }

  return this;
}

void SgAsmJvmRuntimeAnnotationValue::unparse(std::ostream& os) const
{
  Jvm::writeValue(os, p_tag);

  // Unparsing the rest depends on the tag value
  switch (p_tag) {
    // const_value_index tags
    case 'B': case 'C': case 'D': case 'F': // byte, char, double, float
    case 'I': case 'J': case 'S': case 'Z': // int, long, short, boolean
    case 's': // String
      Jvm::writeValue(os, p_const_value_index);
      break;
    case 'e': // enum_const_value
        Jvm::writeValue(os, p_type_name_index);
        Jvm::writeValue(os, p_const_name_index);
        break;
    case 'c': // class_info_index
      Jvm::writeValue(os, p_class_info_index);
      break;
    case '@': // nested annotation
      ASSERT_require(get_is_annotation_value());
      ASSERT_require2(p_tag!='@', "tag '@' annotation_value unsupported\n");
      break;
    case '[': { // array_value
      uint16_t numArrayValues = get_values().size();
      Jvm::writeValue(os, numArrayValues);
      for (auto value : get_values()) {
        value->unparse(os);
      }
      break;
    }
    default:
      throw std::runtime_error("Unknown element_value tag byte: " + std::to_string(p_tag));
  }
}

void SgAsmJvmRuntimeAnnotationValue::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: RuntimeAnnotationValue: tag:%d\n", prefix, idx, p_tag);

  switch (p_tag) {
    // const_value_index tags
    case 'B': case 'C': case 'D': case 'F': // byte, char, double, float
    case 'I': case 'J': case 'S': case 'Z': // int, long, short, boolean
    case 's': // String
      fprintf(f, "%s:%ld:   RuntimeAnnotationValue: const_value_index:%d\n", prefix, idx, p_const_value_index);
      break;
    case 'e': // enum_const_value
      fprintf(f, "%s:%ld:   RuntimeAnnotationValue: const_value_index:%d\n", prefix, idx, p_type_name_index);
      fprintf(f, "%s:%ld:   RuntimeAnnotationValue: const_value_index:%d\n", prefix, idx, p_const_name_index);
      break;
    case 'c': // class_info_index
      fprintf(f, "%s:%ld:   RuntimeAnnotationValue: class_info_index:%d\n", prefix, idx, p_class_info_index);
      break;
    case '@': // nested annotation
      ASSERT_require(get_is_annotation_value());
      fprintf(f, "%s:%ld:   RuntimeAnnotationValue: annotation_value unsupported:\n", prefix, idx);
      break;
    case '[': { // array_value
      fprintf(f, "%s:%ld:   RuntimeAnnotationValue: num_values:%zu\n", prefix, idx, get_values().size());
      for (auto value : get_values()) {
        value->dump(f, prefix, idx);
      }
      break;
    }
    default:
      throw std::runtime_error("Unknown element_value tag byte: " + std::to_string(p_tag));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.17 The RuntimeInvisibleAnnotations Attribute. RuntimeInvisibleAnnotations_attribute is represented by the
// SgAsmJvmRuntimeInvisibleAnnotations class.
//
SgAsmJvmRuntimeInvisibleAnnotations::SgAsmJvmRuntimeInvisibleAnnotations(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmRuntimeInvisibleAnnotations* SgAsmJvmRuntimeInvisibleAnnotations::parse(SgAsmJvmConstantPool* pool)
{
  uint16_t numAnnotations;
  ASSERT_not_null(get_parent());

  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, numAnnotations);

  for (int ii = 0; ii < numAnnotations; ii++) {
    auto annotation = new SgAsmJvmRuntimeAnnotation(this);
    annotation->parse(pool);
    get_annotations().push_back(annotation);
  }

  return this;
}

void SgAsmJvmRuntimeInvisibleAnnotations::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t numAnnotations = get_annotations().size();
  Jvm::writeValue(os, numAnnotations);

  for (auto annotation : get_annotations()) {
    annotation->unparse(os);
  }
}

void SgAsmJvmRuntimeInvisibleAnnotations::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: SgAsmJvmRuntimeInvisibleAnnotations::dump()\n", prefix, idx);
  for (auto annotation : get_annotations()) {
    annotation->dump(f, prefix, idx);
  }
}

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
// 4.7.22 The AnnotationDefault Attribute. AnnotationDefault_attribute represented by the SgAsmJvmAnnotationDefault class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.23 The BootstrapMethods Attribute. BootstrapMethods_attribute represented by the SgAsmJvmBootstrapMethods class.
//
SgAsmJvmBootstrapMethods::SgAsmJvmBootstrapMethods(SgAsmJvmAttributeTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmBootstrapMethods* SgAsmJvmBootstrapMethods::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  uint16_t numMethods;
  Jvm::read_value(pool, numMethods);

  for (int ii = 0; ii < numMethods; ii++) {
    auto method = new SgAsmJvmBootstrapMethod(this);
    method->parse(pool);
    get_bootstrap_methods().push_back(method);
  }
  return this;
}

void SgAsmJvmBootstrapMethods::unparse(std::ostream &os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t numMethods = get_bootstrap_methods().size();
  Jvm::writeValue(os, numMethods);

  for (auto method : get_bootstrap_methods()) {
    method->unparse(os);
  }
}

void SgAsmJvmBootstrapMethods::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "SgAsmJvmBootstrapMethods::dump() not implemented yet\n";
}

// Note: the previous class name is plural, the following singular

SgAsmJvmBootstrapMethod::SgAsmJvmBootstrapMethod(SgAsmJvmBootstrapMethods* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmBootstrapMethod* SgAsmJvmBootstrapMethod::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_bootstrap_method_ref);

  uint16_t numArgs;
  Jvm::read_value(pool, numArgs);

  SgUnsigned16List u16List;
  for (int ii = 0; ii < numArgs; ii++) {
    uint16_t index;
    Jvm::read_value(pool, index);
    u16List.push_back(index);
  }
  set_bootstrap_arguments(u16List);

  return this;
}

void SgAsmJvmBootstrapMethod::unparse(std::ostream &os) const
{
  Jvm::writeValue(os, p_bootstrap_method_ref);

  uint16_t numArgs = get_bootstrap_arguments().size();
  Jvm::writeValue(os, numArgs);

  for (uint16_t index : get_bootstrap_arguments()) {
    Jvm::writeValue(os, index);
  }
}

void SgAsmJvmBootstrapMethod::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "SgAsmJvmBootstrapMethod::dump() not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.24 The MethodParameters Attribute. MethodParameters_attribute is represented by the SgAsmJvmMethodParameters class.
//
SgAsmJvmMethodParameters::SgAsmJvmMethodParameters(SgAsmJvmAttributeTable* parent)
{
  initializeProperties();
  set_parent(parent);
}

SgAsmJvmMethodParameters* SgAsmJvmMethodParameters::parse(SgAsmJvmConstantPool* pool)
{
  uint8_t parametersCount;
  ASSERT_not_null(get_parent());

  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, parametersCount);

  for (int ii = 0; ii < parametersCount; ii++) {
    auto entry = new SgAsmJvmMethodParametersEntry(this);
    entry->parse(pool);
    get_parameters().push_back(entry);
  }
  return this;
}

void SgAsmJvmMethodParameters::unparse(std::ostream& os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint8_t parametersCount = get_parameters().size();
  Jvm::writeValue(os, parametersCount);

  for (auto parameter : get_parameters()) {
    parameter->unparse(os);
  }
}

void SgAsmJvmMethodParameters::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: SgAsmJvmMethodParameters::dump()\n", prefix, idx);
}

SgAsmJvmMethodParametersEntry::SgAsmJvmMethodParametersEntry(SgAsmJvmMethodParameters* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmMethodParametersEntry* SgAsmJvmMethodParametersEntry::parse(SgAsmJvmConstantPool* pool)
{
  Jvm::read_value(pool, p_name_index);
  Jvm::read_value(pool, p_access_flags);
  return this;
}

void SgAsmJvmMethodParametersEntry::unparse(std::ostream& os) const
{
  Jvm::writeValue(os, p_name_index);
  Jvm::writeValue(os, p_access_flags);
}

void SgAsmJvmMethodParametersEntry::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "%s:%ld: start_pc:%d line_number:%d\n", prefix, idx, p_name_index, p_access_flags);
}

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
SgAsmJvmAttribute* SgAsmJvmModuleMainClass::parse(SgAsmJvmConstantPool*)
{
  mlog[WARN] << "Parsing of SgAsmJvmModuleMainClass is not implemented yet\n";
  return nullptr;
}

void SgAsmJvmModuleMainClass::unparse(std::ostream&) const
{
  mlog[WARN] << "Unparsing of SgAsmJvmModuleMainClass is not implemented yet\n";
}

void SgAsmJvmModuleMainClass::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "dump (of SgAsmJvmModuleMainClass) is not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.28 The NestHost Attribute. NestHost_attribute represented by the SgAsmJvmNestHost class.
//
SgAsmJvmNestHost::SgAsmJvmNestHost(SgAsmJvmAttributeTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmNestHost* SgAsmJvmNestHost::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);
  Jvm::read_value(pool, p_host_class_index);
  return this;
}

void SgAsmJvmNestHost::unparse(std::ostream &os) const
{
  SgAsmJvmAttribute::unparse(os);
  Jvm::writeValue(os, p_host_class_index);
}

void SgAsmJvmNestHost::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "SgAsmJvmNestHost::dump() not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.29 The NestMembers Attribute. NestMembers_attribute represented by the SgAsmJvmNestMembers class.
//
SgAsmJvmNestMembers::SgAsmJvmNestMembers(SgAsmJvmAttributeTable* table)
{
  initializeProperties();
  set_parent(table);
}

SgAsmJvmNestMembers* SgAsmJvmNestMembers::parse(SgAsmJvmConstantPool* pool)
{
  SgAsmJvmAttribute::parse(pool);

  uint16_t numClasses;
  Jvm::read_value(pool, numClasses);

  SgUnsigned16List u16List;
  for (int ii = 0; ii < numClasses; ii++) {
    uint16_t index;
    Jvm::read_value(pool, index);
    u16List.push_back(index);
  }
  set_classes(u16List);

  return this;
}

void SgAsmJvmNestMembers::unparse(std::ostream &os) const
{
  SgAsmJvmAttribute::unparse(os);

  uint16_t numClasses = get_classes().size();
  Jvm::writeValue(os, numClasses);

  for (uint16_t index : get_classes()) {
    Jvm::writeValue(os, index);
  }
}

void SgAsmJvmNestMembers::dump(FILE*, const char*, ssize_t) const
{
  mlog[WARN] << "SgAsmJvmNestMembers::dump() not implemented yet\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.30 The Record Attribute. Record_attribute represented by the TODO class.
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 4.7.31 The PermittedSubclasses Attribute. PermittedSubclasses_attribute represented by the TODO class.
//

#endif // ROSE_ENABLE_BINARY_ANALYSIS
