#include <Partitioner2/Attribute.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Attribute {

typedef Sawyer::Container::Map<std::string, Id> NameToId;
typedef Sawyer::Container::Map<Id, std::string> IdToName;

const Id INVALID_ID(-1);
static NameToId nameToId;
static IdToName idToName;
static Id nextId = 0;


Id
registerName(const std::string &name) {
    if (nameToId.insertMaybe(name, nextId) != nextId)
        throw std::runtime_error("partitioner attribute \""+name+"\" alread exists");
    idToName.insert(nextId, name);
    return nextId++;
}

size_t
nRegisteredAttributes() {
    return (size_t)nextId;
}

Id
attributeId(const std::string &name) {
    return nameToId.getOptional(name).orElse(INVALID_ID);
}

const std::string&
attributeName(Id id) {
    static const std::string no_name;
    IdToName::ConstNodeIterator found = idToName.find(id);
    return found==idToName.nodes().end() ? no_name : found->value();
}

} // namespace
} // namespace
} // namespace
} // namespace
