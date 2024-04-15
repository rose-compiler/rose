#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ByteCode/Jvm.h>

#include <Rose/BinaryAnalysis/Disassembler/Jvm.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>

#include <SgAsmJvmAttribute.h>
#include <SgAsmJvmAttributeTable.h>
#include <SgAsmJvmCodeAttribute.h>
#include <SgAsmJvmConstantPool.h>
#include <SgAsmJvmConstantPoolEntry.h>
#include <SgAsmJvmField.h>
#include <SgAsmJvmFieldTable.h>
#include <SgAsmJvmFileHeader.h>
#include <SgAsmJvmInstruction.h>
#include <SgAsmJvmMethod.h>
#include <SgAsmJvmMethodTable.h>
#include <SgAsmInstructionList.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmOperandList.h>

#include <Cxx_GrammarDowncast.h>

using PoolEntry = SgAsmJvmConstantPoolEntry;
using AddressSegment = Sawyer::Container::AddressSegment<rose_addr_t,uint8_t>;
using opcode = Rose::BinaryAnalysis::JvmInstructionKind;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

std::string JvmField::name() const
{
  auto pool = jfh_->get_constant_pool();
  if (sgField_->get_name_index() != 0) {
    return pool->get_utf8_string(sgField_->get_name_index());
  }
  return std::string{""};
}

JvmMethod::JvmMethod(SgAsmJvmFileHeader* jfh, SgAsmJvmMethod* method, rose_addr_t va)
  : Method{va}, jfh_{jfh}, sgMethod_{method}, code_{nullptr,0,0}
{
  ASSERT_not_null(jfh);
  ASSERT_not_null(method);

  auto attribute_table = method->get_attribute_table();
  for (auto attribute : attribute_table->get_attributes()) {
    if (auto codeAttr{isSgAsmJvmCodeAttribute(attribute)}) {
      const uint8_t* code{reinterpret_cast<const uint8_t*>(codeAttr->get_code())};
      // Set JvmCode object fields
      code_.bytes(code);
      code_.size(codeAttr->get_code_length());
      code_.offset(codeAttr->get_code_offset());
    }
  }
}

std::string JvmMethod::name() const
{
  auto pool = jfh_->get_constant_pool();
  if (sgMethod_->get_name_index() != 0) {
    return pool->get_utf8_string(sgMethod_->get_name_index());
  }
  return std::string{""};
}

bool JvmMethod::isSystemReserved(const std::string &name) const
{
  return JvmContainer::isJvmSystemReserved(name);
}

const Code &
JvmMethod::code() const {
    return code_;
}

const SgAsmInstructionList*
JvmMethod::instructions() const {
    return sgMethod_->get_instruction_list();
}

void
JvmMethod::decode(const Disassembler::Base::Ptr &disassembler) const {
  rose_addr_t va{classAddr_ + code_.offset()};
  rose_addr_t endVa{va + code_.size()};

  auto disassemblerJvm{disassembler.dynamicCast<Disassembler::Jvm>()};
  disassemblerJvm->codeOffset(va);

  MemoryMap::Ptr map = MemoryMap::instance();
  MemoryMap::Buffer::Ptr buf = MemoryMap::StaticBuffer::instance(code_.bytes(), code_.size());
  map->insert(AddressInterval::baseSize(va, buf->size()),
              AddressSegment(buf, /*offset*/0, MemoryMap::READABLE, "JVM code segment"));

  SgAsmInstruction* insn{nullptr};
  SgAsmInstructionList* insnList{sgMethod_->get_instruction_list()};

  while (va < endVa) {
    insn = disassembler->disassembleOne(map, va);
    if (insn) {
      va += insn->get_size();
      ASSERT_require2((666 != (int)insn->get_anyKind()), "unknown instruction");
      insnList->get_instructions().push_back(insn);
      insn->set_parent(insnList);
    }
    else {
      ASSERT_require2(false, "disassembly failed for lack of instruction");
    }
  }
  ASSERT_require(va == endVa);
}

void
JvmMethod::annotate()
{
  auto pool = jfh_->get_constant_pool();

  for (auto insn: instructions()->get_instructions()) {
    std::string comment{};
    auto jvmInsn{isSgAsmJvmInstruction(insn)};

    switch (jvmInsn->get_kind()) {
      case opcode::checkcast:
      case opcode::getfield:
      case opcode::getstatic:
      case opcode::instanceof:
      case opcode::invokedynamic:
      case opcode::invokeinterface:
      case opcode::invokespecial:
      case opcode::invokestatic:
      case opcode::invokevirtual:
      case opcode::new_:
      case opcode::putfield:
      case opcode::putstatic: {
        if (auto expr = isSgAsmIntegerValueExpression(insn->get_operandList()->get_operands()[0])) {
          comment = JvmClass::name(expr->get_value(), pool);
          expr->set_comment(comment);
          // Also store the name in the instruction's comment for later convenience in partitioning
          insn->set_comment(comment);
        }
        break;
      }
      default: ;
    }
  }
}

std::string
JvmInterface::name() const
{
  auto pool = jfh_->get_constant_pool();
  if (0 != index()) {
    auto classEntry = pool->get_entry(index());
    if (classEntry->get_tag() == SgAsmJvmConstantPoolEntry::CONSTANT_Class) {
      if (classEntry->get_name_index() != 0) {
        return pool->get_utf8_string(classEntry->get_name_index());
      }
    }
  }
  return std::string{""};
}

std::string JvmAttribute::name() const
{
  auto pool = jfh_->get_constant_pool();
  if (0 != index()) {
    auto entry = pool->get_entry(index());
    if (entry->get_tag() == SgAsmJvmConstantPoolEntry::CONSTANT_Utf8) {
      return pool->get_utf8_string(index());
    }
  }
  return std::string{""};
}

JvmClass::JvmClass(std::shared_ptr<Namespace> ns, SgAsmJvmFileHeader* jfh)
  : Class{ns,jfh->get_baseVa()}, jfh_{jfh}, strings_{std::vector<std::string>()}
{
  ASSERT_not_null(jfh);
  ASSERT_not_null(jfh->get_field_table());
  ASSERT_not_null(jfh->get_method_table());
  ASSERT_not_null(jfh->get_attribute_table());

  auto fields = jfh->get_field_table()->get_fields();
  auto methods = jfh->get_method_table()->get_methods();
  auto attributes = jfh->get_attribute_table()->get_attributes();
  const std::list<uint16_t>& interfaces = jfh->get_interfaces();

  for (auto sgField : fields) {
    fields_.push_back(new JvmField{jfh, sgField});
  }
  for (auto sgMethod : methods) {
    methods_.push_back(new JvmMethod{jfh, sgMethod, address()});
  }
  for (uint16_t index  : interfaces) {
    interfaces_.push_back(new JvmInterface{jfh, index});
  }
  for (auto sgAttribute : attributes) {
    attributes_.push_back(new JvmAttribute{jfh, sgAttribute->get_attribute_name_index()});
  }
}

std::string
JvmClass::name(uint16_t index, const SgAsmJvmConstantPool* pool)
{
  SgAsmJvmConstantPoolEntry* entry = pool->get_entry(index);

  switch (entry->get_tag()) {
    case PoolEntry::CONSTANT_Class: // 4.4.1  CONSTANT_Class_info table entry
    case PoolEntry::CONSTANT_NameAndType: // 4.4.6 CONSTANT_NameAndType_info table entry
    case PoolEntry::CONSTANT_Module: // 4.4.11 CONSTANT_Module_info table entry
    case PoolEntry::CONSTANT_Package: // 4.4.12 CONSTANT_Package_info table entry
      return JvmClass::name(entry->get_name_index(), pool);

    case PoolEntry::CONSTANT_String: // 4.4.2 CONSTANT_String_info table entry
      return JvmClass::name(entry->get_string_index(), pool);

    case PoolEntry::CONSTANT_Fieldref: // 4.4.3 CONSTANT_Fieldref_info table entry
    case PoolEntry::CONSTANT_Methodref: // 4.4.3 CONSTANT_Methodref_info table entry
    case PoolEntry::CONSTANT_InterfaceMethodref: { // 4.4.3 CONSTANT_InterfaceMethodref_info table entry
      std::string className{JvmClass::name(entry->get_class_index(), pool)};
      return className + "::" + JvmClass::name(entry->get_name_and_type_index(), pool);
    }

    case PoolEntry::CONSTANT_Utf8: // 4.4.7 CONSTANT_Utf8_info table entry
      return pool->get_utf8_string(index);

    case PoolEntry::CONSTANT_MethodHandle: // 4.4.8 CONSTANT_MethodHandle_info table entry
      return JvmClass::name(entry->get_reference_index(), pool);

    case PoolEntry::CONSTANT_MethodType: // 4.4.9 CONSTANT_MethodType_info table entry
      return JvmClass::name(entry->get_descriptor_index(), pool);

    case PoolEntry::CONSTANT_Dynamic: // 4.4.10 CONSTANT_Dynamic_info table entry
    case PoolEntry::CONSTANT_InvokeDynamic: // 4.4.10 CONSTANT_InvokeDynamic_info table entry
      return std::string{"bootstrap_method::"} + JvmClass::name(entry->get_name_and_type_index(), pool);

    default: ;
  }

  return std::string{""};
}

std::string JvmClass::name() const
{
  auto pool = jfh_->get_constant_pool();
  auto class_index = jfh_->get_this_class();
  auto class_info = pool->get_entry(class_index);
  if (class_info->get_name_index() != 0) {
    return pool->get_utf8_string(class_info->get_name_index());
  }
  return std::string{""};
}

std::string JvmClass::super_name() const
{
  auto pool = jfh_->get_constant_pool();
  auto super_index = jfh_->get_super_class();
  auto class_info = pool->get_entry(super_index);
  if (class_info->get_name_index() != 0) {
    return pool->get_utf8_string(class_info->get_name_index());
  }
  return std::string{""};
}

const std::vector<std::string> &JvmClass::strings()
{
  strings_.clear();
  auto pool = jfh_->get_constant_pool();
  for (auto entry : pool->get_entries()) {
    auto tag{entry->get_tag()};
    if (entry && (tag==PoolEntry::CONSTANT_Utf8 || tag==PoolEntry::CONSTANT_Utf8)) {
      strings_.push_back(std::string{entry->get_utf8_bytes(), entry->get_length()});
    }
  }
  return strings_;
}

SgAsmJvmConstantPool* JvmClass::constant_pool() {
    return jfh_->get_constant_pool();
}

void JvmClass::dump()
{
  using std::cout;

  cout << "\n";
  cout << "----------------\n";
  cout << "class '" << name() << "'" << std::endl;
  cout << "----------------\n";
  cout << "   super: " << super_name() << "\n\n";

  cout << "constant pool\n";
  cout << "-----------\n";
  constant_pool()->dump(stdout, "", 1);
  cout << "-----------\n\n";

  if (interfaces().size() > 0) {
    cout << "interfaces\n";
    cout << "-----------\n";
    for (auto interface : interfaces()) {
      cout << "   interface: " << interface->name() << std::endl;
    }
    cout << "-----------\n\n";
  }

  if (fields().size() > 0) {
    cout << "fields\n";
    cout << "-----------\n";
    for (auto field : fields()) {
      cout << "   field: " << field->name() << std::endl;
    }
    cout << "-----------\n\n";
  }

  if (attributes().size() > 0) {
    cout << "attributes\n";
    cout << "-----------\n";
    for (auto attribute : attributes()) {
      cout << "   attribute: " << attribute->name() << std::endl;
    }
    cout << "-----------\n\n";
  }
}

bool
JvmContainer::isSystemReserved(const std::string &name) const
{
  return isJvmSystemReserved(name);
}

bool
JvmContainer::isJvmSystemReserved(const std::string &name)
{
  if (name.substr(0,5) == "java/" || name.substr(0,16) == "bootstrap_method") {
    return true;
  }
  return false;
}

std::string
JvmContainer::name() const {
  return "JvmContainer::name():UNIMPLEMENTED";
}

} // namespace
} // namespace
} // namespace

#endif
