// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Attribute.h>
#include <Sawyer/BiMap.h>
#include <boost/foreach.hpp>
#include <boost/version.hpp>

namespace Sawyer {
namespace Attribute {

const Id INVALID_ID(-1);

typedef Sawyer::Container::BiMap<Id, std::string> DefinedAttributes;
static DefinedAttributes definedAttributes;
static Id nextId = 0;
static SAWYER_THREAD_TRAITS::Mutex mutex;

SAWYER_EXPORT Id
declare(const std::string &name) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    Id retval = INVALID_ID;
    if (definedAttributes.reverse().getOptional(name).assignTo(retval))
        throw AlreadyExists(name, retval);
    retval = nextId++;
    definedAttributes.insert(retval, name);
    return retval;
}

SAWYER_EXPORT Id
id(const std::string &name) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    return definedAttributes.reverse().getOptional(name).orElse(INVALID_ID);
}

SAWYER_EXPORT const std::string&
name(Id id) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    return definedAttributes.forward().getOrDefault(id);
}

} // namespace
} // namespace
