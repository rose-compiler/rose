#ifndef ROSE_BinaryAnalysis_ByteCode_Analysis_H
#define ROSE_BinaryAnalysis_ByteCode_Analysis_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

using BasicBlockPtr = Partitioner2::BasicBlockPtr;
using PartitionerPtr = Partitioner2::PartitionerPtr;

// Forward reference for Method
class Class;

class Code {
public:
  virtual const uint8_t* bytes() const = 0;
  virtual const size_t size() const = 0;
  virtual const rose_addr_t offset() const = 0;

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
  virtual const void decode(const Disassembler::BasePtr&) const = 0;
  virtual const SgAsmInstructionList* instructions() const = 0;

  const Class* myClass() { return class_; }

  /* Annotate the AST (.e.g., add comments to instructions) */
  virtual void annotate() = 0;

  /* Set of instruction branch targets */
  std::set<rose_addr_t> targets() const;

  // Methods associated with basic blocks (Rose::BinaryAnalysis::Partitioner2)
  //
  const std::vector<BasicBlockPtr>& blocks() const;
  void append(BasicBlockPtr bb);

  Method() = delete;

protected:
  Method(const Class*);
  ~Method();
  const Class* class_;
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
  virtual void partition(const PartitionerPtr &partitioner) const;
  virtual void digraph();
protected:
  Class() {}
};

class Namespace {
public:
  virtual const std::string name() const = 0;
  virtual const std::vector<const Class*> &classes() const = 0;
  virtual void partition(const PartitionerPtr &partitioner) const;

protected:
  Namespace() {}
};

class Container {
public:
  virtual const std::string name() const = 0;
  virtual const std::vector<const Namespace*> &namespaces() const = 0;
  virtual void partition(const PartitionerPtr &partitioner) const;

protected:
  Container() {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
