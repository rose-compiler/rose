#ifndef ROSE_BinaryAnalysis_ByteCode_Jvm_H
#define ROSE_BinaryAnalysis_ByteCode_Jvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ByteCode/ByteCode.h>

// Sage nodes
class SgAsmJvmClassFile;

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

  explicit JvmCode(SgAsmJvmClassFile* jcf, uint8_t* bytes, size_t size, rose_addr_t offset)
    : jcf_{jcf}, bytes_{bytes}, size_{size}, offset_{offset} {
  }

private:
  SgAsmJvmClassFile* jcf_;
  const uint8_t* bytes_;
  size_t size_;
  rose_addr_t offset_;
};

class JvmField : public Field {
public:
  virtual const std::string name() const;

  JvmField() = delete;
  explicit JvmField(SgAsmJvmClassFile* jcf, SgAsmJvmField* field)
    : jcf_{jcf}, sgField_{field} {
  }

private:
  SgAsmJvmClassFile* jcf_;
  SgAsmJvmField* sgField_;
};

class JvmMethod : public Method {
public:
  virtual const std::string name() const;
  virtual const Code & code() const {
    return code_;
  }
  virtual const void decode(Disassembler* disassembler) const;

  JvmMethod() = delete;
  explicit JvmMethod(SgAsmJvmClassFile* jcf, SgAsmJvmMethod* method);

private:
  SgAsmJvmClassFile* jcf_;
  SgAsmJvmMethod* sgMethod_;
  JvmCode code_;
};

class JvmInterface : public Interface {
public:
public:
  virtual const std::string name() const;

  JvmInterface() = delete;
  explicit JvmInterface(SgAsmJvmClassFile* jcf, uint16_t index);

private:
  SgAsmJvmClassFile* jcf_;
  uint16_t index_;
};

class JvmClass : public Class {
public:
  virtual const std::string name() const;
  virtual const std::string super_name() const;
  virtual const std::vector<std::string> &strings();
  virtual const std::vector<Interface*> &interfaces() const {
    return interfaces_;
  }
  virtual const std::vector<const Field*> &fields() const {
    return fields_;
  }
  virtual const std::vector<const Method*> &methods() const {
    return methods_;
  }

  JvmClass() = delete;
  explicit JvmClass(SgAsmJvmClassFile* jcf);

private:
  SgAsmJvmClassFile* jcf_;
  std::vector<const Field*> fields_;
  std::vector<const Method*> methods_;
  std::vector<Interface*> interfaces_;
  std::vector<std::string> strings_;
};


} // namespace
} // namespace
} // namespace

#endif
#endif
