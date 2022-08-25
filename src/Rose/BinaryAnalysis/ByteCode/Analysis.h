#ifndef ROSE_BinaryAnalysis_ByteCode_Analysis_H
#define ROSE_BinaryAnalysis_ByteCode_Analysis_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/DisassemblerJvm.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

using BasicBlockPtr = Partitioner2::BasicBlock::Ptr;

class Code {
public:
  virtual const uint8_t* bytes() const = 0;
  virtual const size_t size() const = 0;
protected:
  Code() {}
};

class Field {
public:
  virtual const std::string name() const = 0;
protected:
  Field() {}
};

class Method {
public:
  virtual const std::string name() const = 0;
  virtual const Code & code() const = 0;
  virtual const void decode(Disassembler::Base*) const = 0;
  virtual const SgAsmInstructionList* instructions() const = 0;

  /* Set of instruction branch targets */
  std::set<rose_addr_t> targets() const;

  // Methods associated with basic blocks (Rose::BinaryAnalysis::Partitioner2)
  //
  const std::vector<BasicBlockPtr>& blocks() const {
    return blocks_;
  }
  void append(BasicBlockPtr bb) {
    blocks_.push_back(bb);
  }

protected:
  Method() {}
  Partitioner2::FunctionPtr function_;
  std::vector<BasicBlockPtr> blocks_;
};

class Interface {
public:
  virtual const std::string name() const = 0;
protected:
  Interface() {}
};

class Attribute {
public:
  virtual const std::string name() const = 0;
protected:
  Attribute() {}
};

class Class {
public:
  virtual const std::string name() const = 0;
  virtual const std::string super_name() const = 0;
  virtual const std::vector<const Field*> &fields() const = 0;
  virtual const std::vector<const Method*> &methods() const = 0;
  virtual const std::vector<const Attribute*> &attributes() const = 0;
  virtual const std::vector<const Interface*> &interfaces() const = 0;
  virtual const std::vector<std::string> &strings() = 0;
  virtual void partition();
  virtual void digraph();
protected:
  Class() {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
