#ifndef ROSE_BinaryAnalysis_ByteCode_Jvm_H
#define ROSE_BinaryAnalysis_ByteCode_Jvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ByteCode/Analysis.h>

class SgAsmJvmConstantPool;
class SgAsmJvmField;
class SgAsmJvmFileHeader;
class SgAsmJvmMethod;
class SgAsmInstructionList;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

class JvmCode final : public Code {
public:
  virtual const uint8_t* bytes() const {
    return bytes_;
  }
  virtual size_t size() const {
    return size_;
  }
  Address offset() const {
    return offset_;
  }
  void bytes(const uint8_t* buf) {
    bytes_ = buf;
  }
  void size(size_t sz) {
    size_ = sz;
  }
  void offset(Address off) {
    offset_ = off;
  }

  explicit JvmCode(uint8_t* bytes, size_t size, Address offset)
    : bytes_{bytes}, size_{size}, offset_{offset} {
  }

private:
  const uint8_t* bytes_;
  size_t size_;
  Address offset_;
};

class JvmField final : public Field {
public:
  virtual std::string name() const;

  JvmField() = delete;
  explicit JvmField(SgAsmJvmFileHeader* jfh, SgAsmJvmField* field)
    : jfh_{jfh}, sgField_{field} {
  }

private:
  SgAsmJvmFileHeader* jfh_;
  SgAsmJvmField* sgField_;
};

class JvmMethod final : public Method {
public:
  virtual std::string name() const override;
  virtual bool isSystemReserved(const std::string &name) const override;

  virtual const Code & code() const override;
  virtual const SgAsmInstructionList* instructions() const override;
  virtual void decode(const Disassembler::BasePtr &disassembler) const override;

  virtual void annotate() override;

  JvmMethod() = delete;
  explicit JvmMethod(SgAsmJvmFileHeader*, SgAsmJvmMethod*, Address);

private:
  SgAsmJvmFileHeader* jfh_;
  SgAsmJvmMethod* sgMethod_;
  JvmCode code_;
};

class JvmInterface final : public Interface {
public:
  virtual std::string name() const;
  uint16_t index() const {return index_;}

  JvmInterface() = delete;
  explicit JvmInterface(SgAsmJvmFileHeader* jfh, uint16_t index)
    : jfh_{jfh}, index_{index} {
  }

private:
  SgAsmJvmFileHeader* jfh_;
  uint16_t index_;
};

class JvmAttribute final : public Attribute {
public:
  virtual std::string name() const;
  uint16_t index() const {return index_;}

  JvmAttribute() = delete;
  explicit JvmAttribute(SgAsmJvmFileHeader* jfh, uint16_t index)
    : jfh_{jfh}, index_{index} {
  }

private:
  SgAsmJvmFileHeader* jfh_;
  uint16_t index_;
};

class JvmClass final : public Class {
public:
  virtual std::string name() const;
  virtual std::string super_name() const;
  virtual std::string typeSeparator() const {
    return "::";
  }

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

  SgAsmJvmConstantPool* constant_pool();

  virtual void dump();

  static std::string name(uint16_t, const SgAsmJvmConstantPool*);

  JvmClass() = delete;
  explicit JvmClass(std::shared_ptr<Namespace> ns, SgAsmJvmFileHeader* jfh);

private:
  SgAsmJvmFileHeader* jfh_;
  std::vector<const Field*> fields_;
  std::vector<const Method*> methods_;
  std::vector<const Attribute*> attributes_;
  std::vector<const Interface*> interfaces_;
  std::vector<std::string> strings_;
};

class JvmContainer final : public Container {
public:
  virtual std::string name() const override;
  virtual bool isSystemReserved(const std::string &name) const override;
  static  bool isJvmSystemReserved(const std::string &name);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
