#include <Rosebud/Generator.h>

#include <Rosebud/NoneGenerator.h>
#include <Rosebud/RoseGenerator.h>
#include <Rosebud/RosettaGenerator.h>
#include <Rosebud/YamlGenerator.h>

#include <boost/range/adaptor/reversed.hpp>

namespace Rosebud {

std::vector<Generator::Ptr>
Generator::registry_;

void
Generator::initRegistry() {
    static bool initialized = false;
    if (!initialized) {
        registry_.push_back(YamlGenerator::instance());
        registry_.push_back(RosettaGenerator::instance());
        registry_.push_back(RoseGenerator::instance());
        registry_.push_back(NoneGenerator::instance());
        initialized = true;
    }
}

void
Generator::registerGenerator(const Ptr &generator) {
    ASSERT_not_null(generator);
    initRegistry();
    registry_.push_back(generator);
}

const std::vector<Generator::Ptr>&
Generator::registeredGenerators() {
    initRegistry();
    return registry_;
}

Generator::Ptr
Generator::lookup(const std::string &name) {
    initRegistry();
    for (const Ptr &generator: boost::adaptors::reverse(registry_)) {
        if (generator->name() == name)
            return generator;
    }
    return {};
}

void
Generator::addAllToParser(Sawyer::CommandLine::Parser &parser) {
    for (const Ptr &generator: registry_)
        generator->adjustParser(parser);
}

std::string
Generator::propertyDataMemberName(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (p->dataMemberName) {
        return *p->dataMemberName;
    } else {
        return p->name + "_";
    }
}

std::vector<std::string>
Generator::propertyAccessorNames(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (p->accessorNames) {
        return *p->accessorNames;
    } else {
        return {p->name};
    }
}

std::vector<std::string>
Generator::propertyMutatorNames(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (p->mutatorNames) {
        return *p->mutatorNames;
    } else {
        return {p->name};
    }
}


} // namespace
