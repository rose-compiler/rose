#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/ByteCode/Jvm.h>

using PoolEntry = SgAsmJvmConstantPoolEntry;
using AddressSegment = Sawyer::Container::AddressSegment<rose_addr_t,uint8_t>;
using std::cout;
using std::endl;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

const std::string JvmField::name() const
{
  auto pool = jfh_->get_constant_pool();
  if (sgField_->get_name_index() != 0) {
    return pool->get_utf8_string(sgField_->get_name_index());
  }
  return std::string{""};
}

JvmMethod::JvmMethod(SgAsmJvmFileHeader* jfh, SgAsmJvmMethod* method)
  : jfh_{jfh}, sgMethod_{method}, code_{jfh,nullptr,0,0}
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

const std::string JvmMethod::name() const
{
  auto pool = jfh_->get_constant_pool();
  if (sgMethod_->get_name_index() != 0) {
    return pool->get_utf8_string(sgMethod_->get_name_index());
  }
  return std::string{""};
}

void const JvmMethod::decode(Disassembler* disassembler) const {
  rose_addr_t va{code_.offset()};
  rose_addr_t endVa{code_.offset() + code_.size()};

  auto disassemblerJvm{dynamic_cast<DisassemblerJvm*>(disassembler)};
  disassemblerJvm->code_offset(va);

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
      ROSE_ASSERT((666 != (int)insn->get_anyKind()) && "unknown instruction");
      insnList->get_instructions().push_back(insn);
      insn->set_parent(insnList);
    }
    else {
      ROSE_ASSERT(false && "disassembly failed");
    }
  }
  ROSE_ASSERT((va - code_.offset()) == code_.size());
}

const std::string JvmInterface::name() const
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

const std::string JvmAttribute::name() const
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

JvmClass::JvmClass(SgAsmJvmFileHeader* jfh)
  : jfh_{jfh}, strings_{std::vector<std::string>()}
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
    methods_.push_back(new JvmMethod{jfh, sgMethod});
  }
  for (uint16_t index  : interfaces) {
    interfaces_.push_back(new JvmInterface{jfh, index});
  }
  for (auto sgAttribute : attributes) {
    attributes_.push_back(new JvmAttribute{jfh, sgAttribute->get_attribute_name_index()});
  }
}

const std::string JvmClass::name() const
{
  auto pool = jfh_->get_constant_pool();
  auto class_index = jfh_->get_this_class();
  auto class_info = pool->get_entry(class_index);
  if (class_info->get_name_index() != 0) {
    return pool->get_utf8_string(class_info->get_name_index());
  }
  return std::string{""};
}

const std::string JvmClass::super_name() const
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

} // namespace
} // namespace
} // namespace

#endif
