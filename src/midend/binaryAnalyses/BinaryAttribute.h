#ifndef ROSE_BinaryAnalysis_Attribute_H
#define ROSE_BinaryAnalysis_Attribute_H

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <string>

namespace rose {
namespace BinaryAnalysis {

/** Manages attributes.
 *
 *  %Attributes are user-defined, ad-hoc data attached to other objects.  Each object that supports attributes has an @c attr
 *  method to read or write attributes.  Each attribute value is identified by an integer attribute key.  Attribute keys are
 *  obtained by registering an attribute name, and the key/name association is shared across all of ROSE.  This implementation
 *  of attributes should not be confused with AST attributes (@ref AstAttributeMechanism), which is a better choice for
 *  attaching attributes to AST nodes as it supports saving and restoring those attributes from files.
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
 *  function->attribute(CFG_FILE_NAME, fileName);
 * @endcode
 *
 *  And later, retrieve the file name, or a default if no such attribute was stored.
 *
 * @code
 *  std::string fileName = function.attribute<std::string>(CFG_FILE_NAME).orElse("/dev/null");
 * @endcode */
namespace Attribute {

typedef size_t Id;

/** Invalid attribute ID. */
extern const Id INVALID_ID;

/** Register a new attribute key.
 *
 *  The attribute name is registered with the system and an identifier is returned.  Throws an <code>std::runtime_error</code>
 *  if the specified name already exists.  Attributes are numbered consecutively from zero in the order they are
 *  registered. Once registered, an attribute ID is never unregistered. */
Id define(const std::string &name);

/** Number of attribute keys.
 *
 *  Returns the number of attribute names registered with the system.  Attributes are numbered consecutively starting at zero. */
size_t nDefined();

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
class Storage {
    typedef Sawyer::Container::Map<Id, boost::any> AttrMap;
    AttrMap values_;
public:
    /** Check attribute existence.
     *
     *  Returns true if an attribute with the specified identification number exists in this object, false otherwise. */
    bool attributeExists(Id id) const {
        return values_.exists(id);
    }
    
    /** Erase an attribute.
     *
     *  Causes the attribute to not be stored anymore. Does nothing if the attribute was not stored to begin with. Upon return,
     *  the @ref attributeExists method will return false for this @p id. */
    void eraseAttribute(Id id) {
        values_.erase(id);
    }

    /** Store an attribute.
     *
     *  Stores the specified value for the specified attribute, overwriting any previously stored value for the specified
     *  key. The attribute type can be almost anything, but getting and setting operations should use the same type to avoid
     *  exceptions. */
    template<typename T>
    void setAttribute(Id id, const T &value) {
        values_.insert(id, boost::any(value));
    }

    /** Get an attribute that is known to exist.
     *
     *  The return type must match the type of value that was stored for this attribute or a <code>boost::bad_any_cast</code>
     *  exception is thrown.  An <code>std::domain_error</code> is thrown if the attribute does not exist. */
    template<typename T>
    T getAttribute(Id id) const {
        AttrMap::ConstNodeIterator found = values_.find(id);
        if (found == values_.nodes().end()) {
            std::string name = attributeName(id);
            if (name.empty()) {
                throw std::domain_error("attribute id=" + boost::lexical_cast<std::string>(id) + " not defined");
            } else {
                throw std::domain_error(name + " attribute not present");
            }
        }
        return boost::any_cast<T>(values_.getOptional(id).orDefault());
    }

    /** Return an attribute or a specified value.
     *
     *  If the attribute exists, return its value, otherwise return the specified value. Throws
     *  <code>boost::bad_any_cast</code> if the stored attribute's value type doesn't match the type of the provided default
     *  value. */
    template<typename T>
    T attributeOrElse(Id id, const T &dflt) const {
        return boost::any_cast<T>(values_.getOptional(id).orElse(dflt));
    }

    /** Return an attribute or a default-constructed value.
     *
     *  Returns the attribute value if it exists, or a default-constructed value otherwise. Throws
     *  <code>boost::bad_any_cast</code> if the stored attribute's value type doesn't match the specified type. */
    template<typename T>
    T attributeOrDefault(Id id) const {
        AttrMap::ConstNodeIterator found = values_.find(id);
        if (found == values_.nodes().end())
            return T();
        return boost::any_cast<T>(found->value());
    }

    /** Return the attribute as an optional value.
     *
     *  Returns the attribute value if it exists, or returns nothing. */
    template<typename T>
    Sawyer::Optional<T> optionalAttribute(Id id) const {
        AttrMap::ConstNodeIterator found = values_.find(id);
        if (found == values_.nodes().end())
            return Sawyer::Nothing();
        return Sawyer::Optional<T>(boost::any_cast<T>(found->value()));
    }
};

} // namespace
} // namespace
} // namespace

#endif
