#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/ByteCode/Jvm.h>
#include <Rose/BinaryAnalysis/DisassemblerJvm.h>

using namespace Rose::Diagnostics;
using PoolEntry = SgAsmJvmConstantPoolEntry;
using AddressSegment = Sawyer::Container::AddressSegment<rose_addr_t,uint8_t>;
using std::cout;
using std::endl;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

#ifdef TRIALS
JvmField::JvmField(SgAsmJvmClassFile* jcf, uint16_t index)
  : jcf_{jcf}
{
  ROSE_ASSERT(jcf && jcf->get_field_table());

  auto fields = jcf->get_field_table()->get_fields();
  sgField_ = fields[index];
  ROSE_ASSERT(sgField_ && "JvmField ctor(): sgField_ is null");

  cout << "JvmField ctor(): name_index:" << sgField_->get_name_index() << endl;
}
#endif

const std::string JvmField::name() const
{
  auto pool = jcf_->get_constant_pool();
  if (sgField_->get_name_index() != 0) {
    return pool->get_utf8_string(sgField_->get_name_index());
  }
  return std::string{""};
}

JvmMethod::JvmMethod(SgAsmJvmClassFile* jcf, SgAsmJvmMethod* method)
  : jcf_{jcf}, sgMethod_{method}, code_{jcf,nullptr,0,0}
{
  ROSE_ASSERT(jcf && method);

  auto attribute_table = method->get_attribute_table();
  for (auto attribute : attribute_table->get_attributes()) {

    attribute->dump(stdout, "", 0);
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
  auto pool = jcf_->get_constant_pool();
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
  while (va < endVa) {
    insn = disassembler->disassembleOne(map, va);
    if (insn) {
      cout << "    : " << insn->get_anyKind() << ": " << insn->get_mnemonic() << ": '"
           << insn->description() << "' " << " size:" << insn->get_size()
           << " va:" << insn->get_address() << endl;
      va += insn->get_size();
      ROSE_ASSERT((666 != (int)insn->get_anyKind()) && "unknown instruction");
    }
    else {
      ROSE_ASSERT(false && "disassembly failed");
    }
  }
  cout << "JvmMethod:decode() finished: va-offset:" << va-code_.offset() << endl;
}

JvmInterface::JvmInterface(SgAsmJvmClassFile* jcf, uint16_t index)
  : jcf_{jcf}, index_{index}
{
  cout << "JvmInterface ctor(): # interfaces: " << jcf->get_interfaces().size() << endl;
}

const std::string JvmInterface::name() const
{
  auto pool = jcf_->get_constant_pool();
  if (index_ != 0) {
    auto classEntry = pool->get_entry(index_);
    if (classEntry->get_tag() == SgAsmJvmConstantPoolEntry::CONSTANT_Class) {
      if (classEntry->get_name_index() != 0) {
        return pool->get_utf8_string(classEntry->get_name_index());
      }
    }
  }
  return std::string{""};
}

JvmClass::JvmClass(SgAsmJvmClassFile* jcf)
  : jcf_{jcf}, strings_{std::vector<std::string>()}
{
  cout << "JvmClass ctor(): jcf:" << jcf << endl;
  ROSE_ASSERT(jcf && jcf->get_field_table());
  ROSE_ASSERT(jcf && jcf->get_method_table());

  auto fields = jcf->get_field_table()->get_fields();
  cout << "JvmClass ctor(): field table:" << jcf->get_field_table() << endl;
  auto methods = jcf->get_method_table()->get_methods();
  cout << "JvmClass ctor(): method table:" << jcf->get_method_table() << endl;
  const std::list<uint16_t>& interfaces = jcf->get_interfaces();
  cout << "JvmClass ctor(): interface list count: " << interfaces.size() << endl;

  cout << "JvmClass ctor(): # fields:" << fields.size() << endl;
  for (auto sgField : fields) {
    fields_.push_back(new JvmField{jcf, sgField});
  }
  for (auto field : fields_) {
    cout << "    field_name:" << field->name() << endl;
  }

  cout << "JvmClass ctor(): # methods:" << methods.size() << endl;
  for (auto sgMethod : methods) {
    methods_.push_back(new JvmMethod{jcf, sgMethod});
  }
  for (auto method : methods_) {
    cout << "    method_name:" << method->name() << endl;
  }

  cout << "JvmClass ctor(): # interfaces:" << interfaces.size() << endl;
  for (uint16_t index  : interfaces) {
    interfaces_.push_back(new JvmInterface{jcf, index});
  }
  for (auto interface : interfaces_) {
    cout << "    interface_name:" << interface->name() << endl;
  }
}

const std::string JvmClass::name() const
{
  auto pool = jcf_->get_constant_pool();
  auto class_index = jcf_->get_this_class();
  auto class_info = pool->get_entry(class_index);
  if (class_info->get_name_index() != 0) {
    return pool->get_utf8_string(class_info->get_name_index());
  }
  return std::string{""};
}

const std::string JvmClass::super_name() const
{
  auto pool = jcf_->get_constant_pool();
  auto super_index = jcf_->get_super_class();
  auto class_info = pool->get_entry(super_index);
  if (class_info->get_name_index() != 0) {
    return pool->get_utf8_string(class_info->get_name_index());
  }
  return std::string{""};
}

const std::vector<std::string> &JvmClass::strings()
{
  strings_.clear();
  auto pool = jcf_->get_constant_pool();
  cout << "... strings: pool: " << pool << endl;
  cout << "... strings: # pool entries: " << pool->get_entries().size() << endl;
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
