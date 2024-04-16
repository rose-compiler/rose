#include <Rosebud/CerealSerializer.h>

#include <Rosebud/Generator.h>

#include <iostream>

namespace Rosebud {

CerealSerializer::Ptr
CerealSerializer::instance() {
    return Ptr(new CerealSerializer);
}

std::string
CerealSerializer::name() const {
    return "cereal";
}

std::string
CerealSerializer::purpose() const {
    return "Serializes using the Cereal library (experimental)";
}

bool
CerealSerializer::isSerializable(const Ast::Class::Ptr&) const {
    return true;
}

void
CerealSerializer::genPrologue(std::ostream &header, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&,
                              const Generator &generator) const {
    if (generator.name() != "rosetta") {
        header <<"\n"
               <<THIS_LOCATION <<"#include <rosePublicConfig.h>\n"
               <<"#ifdef ROSE_HAVE_CEREAL\n"
               <<"#include <cereal/access.hpp>\n"
               <<"#include <cereal/cereal.hpp>\n"
               <<"#include <cereal/types/memory.hpp>\n"
               <<"#endif\n";
    }
}

void
CerealSerializer::genBody(std::ostream &header, std::ostream&, const Ast::Class::Ptr &c, const Hierarchy &h,
                          const Generator &generator) const {
    ASSERT_not_null(c);

    header <<"\n"
           <<THIS_LOCATION <<"#ifdef ROSE_HAVE_CEREAL\n"
           <<"private:\n"
           <<"    friend class cereal::access;\n"
           <<"\n"
           <<"    // Automatically generated; do not modify!\n"
           <<"    template<class Archive>\n"
           <<"    void CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {\n";

    // The Cereal documentation says to use `archive(T...)` but I've found that the error messages are harder to understand when
    // something goes wrong. It works better to use `archive(T)` with one argument at a time and that way the error message points
    // to the argument where the error occurs rather than the beginning of a long argument list.
    std::ostringstream body;
    const Hierarchy::ConstVertexIterator self = h.findVertexValue(c);
    if (self != h.vertices().end()) {
        for (const auto &parent: self->inEdges())
            body <<THIS_LOCATION <<"        archive(" <<"cereal::base_class<" <<parent.source()->value()->name <<">(this));\n";
    }
    for (const auto &p: c->properties) {
        if (!p->findAttribute("Rosebud::no_serialize")) {
            p->cppStack->emitOpen(body);
            const std::string dataMember = generator.propertyDataMemberName(p());
            body <<THIS_LOCATION <<"        archive(" <<"cereal::make_nvp(\"" <<p->name <<"\", " <<dataMember <<"));\n";
            p->cppStack->emitClose(body);
        }
    }

    header <<THIS_LOCATION <<body.str()
           <<"    }\n"
           <<"\n"
           <<"    // Automatically generated; do not modify!\n"
           <<"    template<class Archive>\n"
           <<"    void CEREAL_LOAD_FUNCTION_NAME(Archive &archive) {\n"
           <<body.str()
           <<"    }\n"
           <<"#endif // ROSE_HAVE_CEREAL\n";
}

void
CerealSerializer::genEpilogue(std::ostream&, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&, const Generator&) const {}

} // namespace
