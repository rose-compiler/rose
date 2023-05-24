#include <Rosebud/NoneGenerator.h>

namespace Rosebud {

NoneGenerator::Ptr
NoneGenerator::instance() {
    return Ptr(new NoneGenerator);
}

std::string
NoneGenerator::name() const {
    return "none";
}

std::string
NoneGenerator::purpose() const {
    return "Do not generate code, but only check the input.";
}

void
NoneGenerator::generate(const Ast::Project::Ptr&) {}

} // namespace
