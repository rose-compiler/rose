#include <Rosebud/NoneSerializer.h>

#include <Rosebud/Generator.h>

#include <memory>
#include <string>

namespace Rosebud {

NoneSerializer::Ptr
NoneSerializer::instance() {
    return Ptr(new NoneSerializer);
}

std::string
NoneSerializer::name() const {
    return "none";
}

std::string
NoneSerializer::purpose() const {
    return "Generates no serialization code.";
}

bool
NoneSerializer::isSerializable(const Ast::ClassPtr&) const {
    return false;
}

void
NoneSerializer::genPrologue(std::ostream&, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&, const Generator&) const {}

void
NoneSerializer::genBody(std::ostream&, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&, const Generator&) const {}

void
NoneSerializer::genEpilogue(std::ostream&, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&, const Generator&) const {}

} // namespace
