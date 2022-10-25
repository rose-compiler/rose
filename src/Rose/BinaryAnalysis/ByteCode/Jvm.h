#ifndef ROSE_BinaryAnalysis_ByteCode_Jvm_H
#define ROSE_BinaryAnalysis_ByteCode_Jvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ByteCode/Analysis.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

class JvmCode : public Code {
public:
  virtual const uint8_t* bytes() const {
    return bytes_;
  }
  virtual const size_t size() const {
    return size_;
  }
  rose_addr_t const offset() const {
    return offset_;
  }
  void bytes(const uint8_t* buf) {
    bytes_ = buf;
  }
  void size(size_t sz) {
    size_ = sz;
  }
  void offset(rose_addr_t off) {
    offset_ = off;
  }

  explicit JvmCode(uint8_t* bytes, size_t size, rose_addr_t offset)
    : bytes_{bytes}, size_{size}, offset_{offset} {
  }

private:
  const uint8_t* bytes_;
  size_t size_;
  rose_addr_t offset_;
};

class JvmField : public Field {
public:
  virtual const std::string name() const;

  JvmField() = delete;
  explicit JvmField(SgAsmJvmFileHeader* jfh, SgAsmJvmField* field)
    : jfh_{jfh}, sgField_{field} {
  }

private:
  SgAsmJvmFileHeader* jfh_;
  SgAsmJvmField* sgField_;
};

class JvmMethod : public Method {
public:
  virtual const std::string name() const;
  virtual const Code & code() const {
    return code_;
  }
  virtual const void decode(const Disassembler::BasePtr &disassembler) const; // blame rasmussen17
  virtual const SgAsmInstructionList* instructions() const {
    return sgMethod_->get_instruction_list();
  }

  JvmMethod() = delete;
  explicit JvmMethod(SgAsmJvmFileHeader*, SgAsmJvmMethod*);

private:
  SgAsmJvmFileHeader* jfh_;
  SgAsmJvmMethod* sgMethod_;
  JvmCode code_;
};

class JvmInterface : public Interface {
public:
  virtual const std::string name() const;
  const uint16_t index() const {return index_;}

  JvmInterface() = delete;
  explicit JvmInterface(SgAsmJvmFileHeader* jfh, uint16_t index)
    : jfh_{jfh}, index_{index} {
  }

private:
  SgAsmJvmFileHeader* jfh_;
  uint16_t index_;
};

class JvmAttribute : public Attribute {
public:
  virtual const std::string name() const;
  const uint16_t index() const {return index_;}

  JvmAttribute() = delete;
  explicit JvmAttribute(SgAsmJvmFileHeader* jfh, uint16_t index)
    : jfh_{jfh}, index_{index} {
  }

private:
  SgAsmJvmFileHeader* jfh_;
  uint16_t index_;
};

class JvmClass : public Class {
public:
  virtual const std::string name() const;
  virtual const std::string super_name() const;
  virtual const std::vector<std::string> &strings();
  virtual const std::vector<const Interface*> &interfaces() const {
    return interfaces_;
  }
  virtual const std::vector<const Field*> &fields() const {
    return fields_;
  }
  virtual const std::vector<const Method*> &methods() const {
    return methods_;
  }
  virtual const std::vector<const Attribute*> &attributes() const {
    return attributes_;
  }

  SgAsmJvmConstantPool* constant_pool() {
    return jfh_->get_constant_pool();
  }

  JvmClass() = delete;
  explicit JvmClass(SgAsmJvmFileHeader* jfh);

private:
  SgAsmJvmFileHeader* jfh_;
  std::vector<const Field*> fields_;
  std::vector<const Method*> methods_;
  std::vector<const Attribute*> attributes_;
  std::vector<const Interface*> interfaces_;
  std::vector<std::string> strings_;
};


} // namespace
} // namespace
} // namespace

#endif
#endif
