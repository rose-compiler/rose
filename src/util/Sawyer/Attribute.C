// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Attribute.h>
#include <Sawyer/BiMap.h>
#include <boost/foreach.hpp>

namespace Sawyer {
namespace Attribute {

const Id INVALID_ID(-1);

typedef Sawyer::Container::BiMap<Id, std::string> DefinedAttributes;
static DefinedAttributes definedAttributes;
static Id nextId = 0;

SAWYER_EXPORT Id
declare(const std::string &name) {
    Id retval = INVALID_ID;
    if (definedAttributes.reverse().getOptional(name).assignTo(retval))
        throw AlreadyExists(name, retval);
    retval = nextId++;
    definedAttributes.insert(retval, name);
    return retval;
}

SAWYER_EXPORT Id
id(const std::string &name) {
    return definedAttributes.reverse().getOptional(name).orElse(INVALID_ID);
}

SAWYER_EXPORT const std::string&
name(Id id) {
    return definedAttributes.forward().getOrDefault(id);
}

SAWYER_EXPORT std::vector<Id>
Storage::attributeIds() const {
    std::vector<Id> retval;
    retval.reserve(values_.size());
    BOOST_FOREACH (Id id, values_.keys())
        retval.push_back(id);
    return retval;
}

} // namespace
} // namespace
