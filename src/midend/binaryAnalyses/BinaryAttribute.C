#include <BinaryAttribute.h>
#include <Sawyer/BiMap.h>

namespace rose {
namespace BinaryAnalysis {
namespace Attribute {

const Id INVALID_ID(-1);

typedef Sawyer::Container::BiMap<Id, std::string> DefinedAttributes;
static DefinedAttributes definedAttributes;
static Id nextId = 0;


Id
declare(const std::string &name) {
    Id retval = INVALID_ID;
    if (definedAttributes.reverse().getOptional(name).assignTo(retval))
        throw AlreadyExists(name, retval);
    retval = nextId++;
    definedAttributes.insert(retval, name);
    return retval;
}

Id
id(const std::string &name) {
    return definedAttributes.reverse().getOptional(name).orElse(INVALID_ID);
}

const std::string&
name(Id id) {
    return definedAttributes.forward().getOrDefault(id);
}

} // namespace
} // namespace
} // namespace
