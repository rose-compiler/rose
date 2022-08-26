#ifndef ROSE_BinaryAnalysis_ByteCode_Cil_H
#define ROSE_BinaryAnalysis_ByteCode_Cil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ByteCode/Analysis.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

class CilCode : public Code {
public:
  virtual const uint8_t* bytes() const {
    return nullptr; // UNIMPLEMENTED
  }
  virtual const size_t size() const {
    return 0; // UNIMPLEMENTED
  }

  explicit CilCode(uint8_t* bytes, size_t size, rose_addr_t offset)
    : bytes_{bytes}, size_{size}, offset_{offset} {
  }

private:
  const uint8_t* bytes_;
  size_t size_;
  rose_addr_t offset_;
};

class CilField : public Field {
public:
  virtual const std::string name() const {
    return "CilField::name():UNIMPLEMENTED";
  }

  CilField() = delete;

private:
};

class CilMethod : public Method {
public:
  virtual const std::string name() const {
    return "CilMethod::name():UNIMPLEMENTED";
  }
  virtual const Code & code() const {
    return code_;
  }
  virtual const void decode(Disassembler* disassembler) const {
    // UNIMPLEMENTED
  }
  virtual const SgAsmInstructionList* instructions() const {
    return nullptr; // UNIMPLEMENTED
  }

  CilMethod() = delete;
  explicit CilMethod(SgAsmCilMethodDef*);

private:
  SgAsmCilMethodDef* sgMethod_;
  CilCode code_;
};

class CilInterface : public Interface {
public:
  virtual const std::string name() const {
    return "CilInterface::name():UNIMPLEMENTED";
  }

  CilInterface() = delete;

private:
};

class CilAttribute : public Attribute {
public:
  virtual const std::string name() const {
    return "CilAttribute::name():UNIMPLEMENTED";
  }

  CilAttribute() = delete;

private:
};

class CilClass : public Class {
public:
  virtual const std::string name() const {
    return "CilClass::super_name():UNIMPLEMENTED";
  }
  virtual const std::string super_name() const {
    return "CilClass::super_name():UNIMPLEMENTED";
  }
  virtual const std::vector<std::string> &strings() const {
    return strings_
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

  CilClass() = delete;
  explicit CilClass(SgAsmCilTypeDef*);

private:
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
