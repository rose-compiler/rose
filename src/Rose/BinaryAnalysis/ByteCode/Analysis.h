#ifndef ROSE_BinaryAnalysis_ByteCode_Analysis_H
#define ROSE_BinaryAnalysis_ByteCode_Analysis_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

class SgAsmInstructionList;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

using BasicBlockPtr = Partitioner2::BasicBlockPtr;
using PartitionerPtr = Partitioner2::PartitionerPtr;

// Forward references
class Class;
class Namespace;

class Code {
public:
  virtual const uint8_t* bytes() const = 0;
  virtual size_t size() const = 0;
  virtual Address offset() const = 0;

protected:
  Code() {}
};

class Field {
public:
  virtual std::string name() const = 0;
protected:
  Field() {}
};

class Method {
public:
  virtual std::string name() const = 0;
  virtual bool isSystemReserved(const std::string &name) const = 0;

  virtual const Code & code() const = 0;
  virtual const SgAsmInstructionList* instructions() const = 0;
  virtual void decode(const Disassembler::BasePtr&) const = 0;

  /* Annotate the AST (.e.g., add comments to instructions) */
  virtual void annotate() = 0;

  /* Set of instruction branch targets */
  std::set<Address> targets() const;

  // Methods associated with basic blocks (Rose::BinaryAnalysis::Partitioner2)
  //
  const std::vector<BasicBlockPtr>& blocks() const;
  void append(BasicBlockPtr bb);

  Method() = delete;

protected:
  Method(Address);
  ~Method();
  Address classAddr_;
  Partitioner2::FunctionPtr function_;
  std::vector<BasicBlockPtr> blocks_;
};

class Interface {
public:
  virtual std::string name() const = 0;
protected:
  Interface() {}
};

class Attribute {
public:
  virtual std::string name() const = 0;
protected:
  Attribute() {}
};

class Class {
public:
  virtual std::string name() const = 0;
  virtual std::string super_name() const = 0;
  virtual std::string typeSeparator() const = 0;
  virtual const std::vector<const Field*> &fields() const = 0;
  virtual const std::vector<const Method*> &methods() const = 0;
  virtual const std::vector<const Attribute*> &attributes() const = 0;
  virtual const std::vector<const Interface*> &interfaces() const = 0;
  virtual const std::vector<std::string> &strings() = 0;
  virtual void partition(const PartitionerPtr &, std::map<std::string,Address> &) const;
  virtual void digraph() const;
  virtual void dump() = 0;

  Address address() const {return address_;}

  Class() = delete;

protected:
  Address address_;
  std::shared_ptr<Namespace> namespace_;
  Class(std::shared_ptr<Namespace> ns, Address va) : address_{va}, namespace_{ns} {}
};

class Namespace {
public:
  virtual std::string name() const = 0;
  virtual void partition(const PartitionerPtr &partitioner, std::map<std::string,Address> &) const;

  void append(std::shared_ptr<Class> ptr) {
    classes_.push_back(ptr);
  }
  const std::vector<std::shared_ptr<Class>> &classes() const {
    return classes_;
  }

protected:
  Namespace() {}
  std::vector<std::shared_ptr<Class>> classes_;
};

class Container {
public:
  virtual std::string name() const = 0;
  virtual bool isSystemReserved(const std::string &name) const = 0;
  virtual void partition(const PartitionerPtr &partitioner) const;

  const std::vector<std::shared_ptr<Namespace>> &namespaces() const {return namespaces_;}

  /* A unique (per container) virtual address for system/library functions */
  static Address nextSystemReservedVa();

protected:
  Container() {}
  std::vector<std::shared_ptr<Namespace>> namespaces_;

private:
  static Address nextSystemReservedVa_;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
