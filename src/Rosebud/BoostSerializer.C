#include <Rosebud/BoostSerializer.h>

#include <Rosebud/Generator.h>

#include <memory>
#include <ostream>
#include <string>

namespace Rosebud {

BoostSerializer::Ptr
BoostSerializer::instance() {
    return Ptr(new BoostSerializer);
}

std::string
BoostSerializer::name() const {
    return "boost";
}

std::string
BoostSerializer::purpose() const {
    return "Generates boost::serialization code.";
}

bool
BoostSerializer::isSerializable(const Ast::Class::Ptr&) const {
    return true;
}

void
BoostSerializer::genPrologue(std::ostream&, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&, const Generator&) const {}

void
BoostSerializer::genBody(std::ostream &header, std::ostream&, const Ast::Class::Ptr &c, const Hierarchy&,
                         const Generator &generator) const {
    ASSERT_not_null(c);

    header <<"\n"
           <<THIS_LOCATION <<"    //----------------------- Boost serialization for " <<c->name <<" -----------------------\n"
           <<"#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB\n"
           <<"private:\n"
           <<"    friend class boost::serialization::access;\n"
           <<"\n"
           <<"    template<class S>\n"
           <<"    void serialize(S &s, const unsigned /*version*/) {\n"
           <<"        debugSerializationBegin(\"" <<c->name <<"\");\n";

    // Serialize the base classes
    for (const auto &super: c->inheritance)
        header <<THIS_LOCATION <<"        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(" <<super.second <<");\n";

    // Serialize all properties that request serialization
    for (const auto &p:c->properties) {
        if (!p->findAttribute("Rosebud::no_serialize")) {
            const std::string memberName = generator.propertyDataMemberName(p());
            header <<locationDirective(p->findAncestor<Ast::File>(), p->startToken)
                   <<"        s & BOOST_SERIALIZATION_NVP(" <<memberName <<");\n";
        }
    }

    header <<THIS_LOCATION <<"        debugSerializationEnd(\"" <<c->name <<"\");\n"
           <<"    }\n"
           <<"#endif // ROSE_HAVE_BOOST_SERIALIZATION_LIB\n";
}

void
BoostSerializer::genEpilogue(std::ostream&, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&, const Generator&) const {}

} // namespace
