#ifndef ROSE_Partitioner2_Attributes_H
#define ROSE_Partitioner2_Attributes_H

#include <boost/any.hpp>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <string>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Manages attributes.
 *
 *  %Attributes are user-defined, ad-hoc data attached to various objects that exist in a partitioner.  Each object that
 *  supports attributes has an @c attr method to read or write attributes.  Each attribute value is identified by an integer
 *  attribute key.  Attribute keys are obtained by registering an attribute name, and the key/name association is shared across
 *  all partitioners.
 *
 *  The use of attributes supports a quick and easy way to add data to existing objects and is complementary to the use of
 *  class derivation.  For instance, to store the name of a PNG file that has a picture of a function's control flow graph, one
 *  might do the following.  First, choose a descriptive name for the attribute and register it in order to get an attribute ID
 *  number that can be used as a lookup key:
 *
 * @code
 *  using namespace rose::BinaryAnalysis::Partitioner2;
 *  const Attribute::Id CFG_FILE_NAME = Attribute::registerName("Control flow graph PNG file name");
 * @endcode
 *
 *  Then store the filename for some function:
 *
 * @code
 *  Function::Ptr function = ...;
 *  std::string fileName = ...;
 *  function->attr(CFG_FILE_NAME, fileName);
 * @endcode
 *
 *  And later, retrieve the file name, or a default if no such attribute was stored.
 *
 * @code
 *  std::string fileName = function.attr<std::string>(CFG_FILE_NAME).orElse("/dev/null");
 * @endcode */
namespace Attribute {

typedef size_t Id;

/** Invalid attribute ID. */
extern const Id INVALID_ID;

/** Register a new attribute key.
 *
 *  The attribute name is registered with the system and an identifier is returned.  Throws an <code>std::runtime_error</code>
 *  if the specified name already exists.  Attributes are numbered consecutively from zero in the order they are
 *  registered. Once registered, an attribute key/ID binding is never removed. */
Id registerName(const std::string &name);

/** Number of attribute keys.
 *
 *  Returns the number of attribute names registered with the system.  Attributes are numbered consecutively starting at zero. */
size_t nRegistered();

/** Returns the ID for an attribute name.
 *
 *  Looks up the name in the attribute name table and returns its identification number.  Returns @ref INVALID_ID if
 *  the name does not exist. */
Id attributeId(const std::string &name);

/** Returns the name for an attribute ID.
 *
 *  Looks up the specified attribute ID in the attribute name table and returns its name.  Returns the empty string if the ID
 *  does not exist. */
const std::string& attributeName(Id);

/** List of attribute values.
 *
 *  This is the interface inherited by objects that can store attributes. */
class StoredValues {
    Sawyer::Container::Map<Id, boost::any> values_;
public:
    /** Obtain the value stored for an attribute.
     *
     *  Returns the attribute value if it exists, or nothing.  The stored value must be convertible to the specified type or
     *  else a <code>boost::bad_any_cast</code> will be thrown.  Existence can be tested like this:
     *
     * @code
     *  const Attribute::Id MY_ATTR_ID = ...;   // ID number for the attribute
     *  std::string value;                      // lets say the attribute is string valued
     *  Function::Ptr function = ...;           // some object supporting attributes
     *  if (function->attr(MY_ATTR_ID)) {
     *      value = function->attr<std::string>(MyAttr).get();
     *  } else {
     *      value = "default string";
     *  }
     * @endcode
     *
     *  The same thing can be done more efficiently and with less code by using the @ref Sawyer::Optional API:
     *
     * @code
     *  std::string value = function->attr<std::string>(MyAttr).orElse("default string");
     * @endcode */
    template<typename T> const Sawyer::Optional<T> attr(Id id) const {
        boost::any v = values_.getOptional(id).orDefault();
        return v.empty() ? Sawyer::Nothing() : Sawyer::Optional<T>(boost::any_cast<T>(v));
    }

    /** Store an attribute value.
     *
     *  Stores the specified value for the specified attribute, overwriting any previously stored value for the specified key. */
    template<typename T> void attr(Id id, const T &value) {
        values_.insert(id, boost::any(value));
    }

    /** Erase an attribute.
     *
     *  Causes the attribute to not be stored anymore. */
    void attrErase(Id id) {
        values_.erase(id);
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
