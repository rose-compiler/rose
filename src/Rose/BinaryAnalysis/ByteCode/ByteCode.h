#ifndef ROSE_BinaryAnalysis_ByteCode_H
#define ROSE_BinaryAnalysis_ByteCode_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

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
  virtual const void decode(Disassembler*) const = 0;
protected:
  Method() {}
};

class Interface {
public:
  virtual const std::string name() const = 0;
protected:
  Interface() {}
};

class Class {
public:
  virtual const std::string name() const = 0;
  virtual const std::string super_name() const = 0;
  virtual const std::vector<const Method*> &methods() const = 0;
  virtual const std::vector<const Field*> &fields() const = 0;
  virtual const std::vector<Interface*> &interfaces() const = 0;
  virtual const std::vector<std::string> &strings() = 0;
protected:
  Class() {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
