#include <Rosebud/Serializer.h>

#include <Rosebud/BoostSerializer.h>
#include <Rosebud/CerealSerializer.h>
#include <Rosebud/NoneSerializer.h>

#include <boost/range/adaptor/reversed.hpp>

namespace Rosebud {

std::vector<Serializer::Ptr>
Serializer::registry_;

void
Serializer::initRegistry() {
    static bool initialized = false;
    if (!initialized) {
        registry_.push_back(BoostSerializer::instance());
        registry_.push_back(CerealSerializer::instance());
        registry_.push_back(NoneSerializer::instance());
        initialized = true;
    }
}

void
Serializer::registerSerializer(const Ptr &serializer) {
    ASSERT_not_null(serializer);
    initRegistry();
    registry_.push_back(serializer);
}

const std::vector<Serializer::Ptr>&
Serializer::registeredSerializers() {
    initRegistry();
    return registry_;
}

Serializer::Ptr
Serializer::lookup(const std::string &name) {
    initRegistry();
    for (const Ptr &serializer: boost::adaptors::reverse(registry_)) {
        if (serializer->name() == name)
            return serializer;
    }
    return {};
}

} // namespace
