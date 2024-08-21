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
BoostSerializer::genPrologue(std::ostream &header, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&,
                             const Generator &generator) const {
    if (generator.name() != "rosetta") {
        header <<"\n"
               <<THIS_LOCATION <<"#include <rosePublicConfig.h>\n"
               <<"#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB\n"
               <<"#include <boost/serialization/access.hpp>\n"
               <<"#include <boost/serialization/base_object.hpp>\n"
               <<"#include <boost/serialization/map.hpp>\n"
               <<"#include <boost/serialization/set.hpp>\n"
               <<"#include <boost/serialization/shared_ptr.hpp>\n"
               <<"#include <boost/serialization/vector.hpp>\n"
               <<"#include <boost/serialization/version.hpp>\n"
               <<"#endif\n";
    }
}

void
BoostSerializer::genBody(std::ostream &header, std::ostream&, const Ast::Class::Ptr &c, const Hierarchy&,
                         const Generator &generator) const {
    ASSERT_not_null(c);

    const bool usingRosetta = generator.name() == "rosetta";

    header <<"\n"
           <<THIS_LOCATION <<"    //----------------------- Boost serialization for " <<c->name <<" -----------------------\n"
           <<"#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB\n"
           <<"private:\n"
           <<"    friend class boost::serialization::access;\n"
           <<"\n"
           <<"    template<class S>\n"
           <<"    void serialize(S &s, const unsigned /*version*/) {\n";
    if (usingRosetta)
        header <<"        debugSerializationBegin(\"" <<c->name <<"\");\n";

    // Serialize the base classes
    for (const auto &super: c->inheritance)
        header <<THIS_LOCATION <<"        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(" <<super.second <<");\n";

    // Serialize all properties that request serialization
    size_t nTemps = 0;
    for (const auto &p: c->properties) {
        if (!p->serializerBaseName) {
            // Default serialization for this property
            p->cppStack->emitOpen(header);
            const std::string memberName = generator.propertyDataMemberName(p());
            header <<locationDirective(p->findAncestor<Ast::File>(), p->startToken)
                   <<"        s & BOOST_SERIALIZATION_NVP(" <<memberName <<");\n";
            p->cppStack->emitClose(header);
        } else if (!p->serializerBaseName->empty()) {
            // User-controlled serialization for this property
            p->cppStack->emitOpen(header);
            const std::string memberName = generator.propertyDataMemberName(p());
            const std::string serializer = *p->serializerBaseName + "Serialize";
            const std::string deserializer = *p->serializerBaseName + "Deserialize";
            header <<locationDirective(p->findAncestor<Ast::File>(), p->startToken)
                   <<"        {\n"
                   <<"            auto temp = " <<serializer <<"(" <<memberName <<");\n"
                   <<"            s & boost::serialization::make_nvp(\"" <<memberName <<"\", temp);\n"
                   <<"            " <<memberName <<" = " <<deserializer <<"(temp);\n"
                   <<"        }\n";
            p->cppStack->emitClose(header);
        } else {
            // No serialization for this property
        }
    }

    if (usingRosetta)
        header <<THIS_LOCATION <<"        debugSerializationEnd(\"" <<c->name <<"\");\n";
    header <<"    }\n"
           <<"#endif // ROSE_HAVE_BOOST_SERIALIZATION_LIB\n";
}

void
BoostSerializer::genEpilogue(std::ostream&, std::ostream&, const Ast::Class::Ptr&, const Hierarchy&, const Generator&) const {}

} // namespace
