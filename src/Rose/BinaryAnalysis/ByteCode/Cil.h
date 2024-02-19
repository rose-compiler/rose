#ifndef ROSE_BinaryAnalysis_ByteCode_Cil_H
#define ROSE_BinaryAnalysis_ByteCode_Cil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ByteCode/Analysis.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

class CilCode final : public Code {
public:
  virtual const uint8_t* bytes() const {
    return bytes_; // UNIMPLEMENTED
  }
  virtual size_t size() const {
    return size_; // UNIMPLEMENTED
  }
  virtual rose_addr_t offset() const {
    return offset_; // UNIMPLEMENTED
  }

  explicit CilCode(uint8_t* bytes, size_t size, rose_addr_t offset)
    : bytes_{bytes}, size_{size}, offset_{offset} {
  }

private:
  const uint8_t* bytes_;
  size_t size_;
  rose_addr_t offset_;
};

class CilField final : public Field {
public:
  virtual std::string name() const {
    return "CilField::name():UNIMPLEMENTED";
  }

  CilField() = delete;

private:
};

class CilMethod final : public Method {
public:
  virtual std::string name() const override;
  virtual bool isSystemReserved(const std::string &name) const override;

  virtual const Code & code() const override;
  virtual const SgAsmInstructionList* instructions() const override;
  virtual void decode(const Disassembler::BasePtr &disassembler) const override;

  virtual void annotate() override;

  CilMethod() = delete;
  explicit CilMethod(SgAsmCilMetadataRoot*, SgAsmCilMethodDef*, rose_addr_t);

private:
  SgAsmCilMetadataRoot* mdr_;
  SgAsmCilMethodDef* sgMethod_;
  SgAsmInstructionList* insns_;
  CilCode code_;
};

class CilInterface final : public Interface {
public:
  virtual std::string name() const {
    return "CilInterface::name():UNIMPLEMENTED";
  }

  CilInterface() = delete;

private:
};

class CilAttribute final : public Attribute {
public:
  virtual std::string name() const {
    return "CilAttribute::name():UNIMPLEMENTED";
  }

  CilAttribute() = delete;

private:
};

class CilClass final : public Class {
public:
  virtual std::string name() const {
    return name_;
  }
  virtual std::string super_name() const {
    return "CilClass::super_name():UNIMPLEMENTED";
  }
  virtual std::string typeSeparator() const {
    return ".";
  }

  virtual const std::vector<std::string> &strings() {
    return strings_;
  }
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

  virtual void dump();

  static std::string objectName(const SgAsmCilMetadata*, SgAsmCilMetadataRoot*);

  CilClass() = delete;
  explicit CilClass(std::shared_ptr<Namespace> ns, SgAsmCilMetadataRoot*, const std::string &, size_t, size_t);

private:
    std::vector<const Field*> fields_;
    std::vector<const Method*> methods_;
    std::vector<const Attribute*> attributes_;
    std::vector<const Interface*> interfaces_;
    std::vector<std::string> strings_;
    std::string name_;
    SgAsmCilMetadataRoot* mdr_;
    //WARNING: not used yet!
    //SgAsmCilTypeDef* sgCilTypeDef_;
};

class CilNamespace final : public Namespace {
public:
    virtual std::string name() const;

    CilNamespace() = delete;
    explicit CilNamespace(SgAsmCilMetadataRoot*, const std::string &);

private:
    //WARNING: not used yet!
    //SgAsmCilMetadataRoot* mdr_;
    std::string name_;
};

class CilContainer final : public Container {
public:
  virtual std::string name() const override;
  virtual bool isSystemReserved(const std::string &name) const override;
  static  bool isCilSystemReserved(const std::string &name);

  void printAssemblies(std::ostream& os) const;
  void printMethods(std::ostream& os, size_t beg, size_t lim) const;
  void printModules(std::ostream& os) const;
  void printTypeDefs(std::ostream& os) const;

  static SgAsmCilMetadata* resolveToken(SgAsmIntegerValueExpression*, SgAsmCilMetadataRoot*);

  CilContainer() = delete;
  explicit CilContainer(SgAsmCilMetadataRoot*);

private:
  SgAsmCilMetadataRoot* mdr_;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
