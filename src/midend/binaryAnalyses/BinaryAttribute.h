#ifndef ROSE_BinaryAnalysis_Attribute_H
#define ROSE_BinaryAnalysis_Attribute_H

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <string>

namespace rose {
namespace BinaryAnalysis {

/** Attributes attached to other objects.
 *
 *  This namespace defines a mechanism for extending objects at runtime through the use of name/value pairs called
 *  "attributes".  Attributes are an extension mechanism orthogonal to class derivation and compile-time containment. Among
 *  other things, they are a convenient mechanism for users to cache data that needs to be associated with some ROSE
 *  object. See also, @ref AstAttributeMechanism for attributes attached to Sage IR nodes. There are two steps to using
 *  attributes: registering an attribute name to obtain an ID number, and accessing attribute values stored in objects.
 *
 *
 *
 *  @section binary_attribute_declaring Registering attribute names
 *
 *  In order to be able to store many attribute name/value pairs per object, the attribute names must be declared by
 *  registering them with the library.  The names are entered into a global symbol table and must be unique. Each name will be
 *  given an ID number to use when values are stored and retrieved.
 *
 *  @snippet binaryAttribute.C setup
 *  @snippet binaryAttribute.C declaring
 *
 *  Once an ID number is created it will never be removed from the attribute symbol table; attribute ID numbers are stable
 *  for the life of the program.  If the same name is registered more than once then an @ref Attribute::AlreadyExists exception
 *  is thrown.  The @ref Attribute::id and @ref Attribute::name functions can be used to obtain the ID number for a name, or
 *  the name for an ID number, respectively.
 *
 *
 *
 *  @section binary_attribute_using Storing and retrieving attributes
 *
 *  Each object can store zero or one value per declared attribute, and those values can be any type. Since the attribute
 *  retrieval must use the exact same type, some care must be taken to ensure that the correct type is stored. E.g., be sure to
 *  construct <code>std::string</code> if you're passing a string literal otherwise the attribute value will be of type
 *  <code>const char*</code>.
 *
 *  @snippet binaryAttribute.C storing values
 *
 *  Since storing a value that can be interpreted as Boolean false is different than storing no value at all, the API has an
 *  @ref Storage::attributeExists "attributeExists" method that can be used to conditionally retrieve the value. If one calls
 *  @ref Storage::getAttribute "getAttribute" for an attribute that has not been stored in the object, then a @ref DoesNotExist
 *  exception is thrown.  If the attribute exists but the type parameter differs from the type of value that was stored, then a
 *  @ref WrongQueryType exception is thrown.
 *
 *  @snippet binaryAttribute.C retrieve with getAttribute
 *
 *  The @ref Storage::getAttribute "getAttribute" method of retrieval can be a bit cumbersome since one has to check for
 *  existence first, so the API has additional methods that provide a default value. The default value is either specified as
 *  an argument, or instantiated with a default constructor.
 *
 *  @snippet binaryAttribute.C retrieve with default
 *
 *  It's also possible to get a @ref Sawyer::Optional value, an object that stores either the attribute value or nothing.  One
 *  of the useful things with this approach is being able to check for existence and assign to a variable at the same time even
 *  if the attribute value could be interpreted as false:
 *
 *  @snippet binaryAttribute.C retrieve optional
 *
 *  Attributes can be erased from an object. Erasing an attribute value has no effect on which attribute ID numbers are
 *  registered in the global attribute symbol table.
 *
 *  @snippet binaryAttribute.C erasing
 *
 *
 *
 *  @section binary_attribute_providing Providing attribute storage capability
 * 
 *  Providing the ability to store attributes in your own classes is easy: just inherit from @ref Attribute::Storage. For a
 *  class hierarchy, only the base class should directly inherit from @ref Attribute::Storage.
 *
 *  @snippet binaryAttribute.C providing
 *
 *  When such an object is assigned or copy-constructed the attribute values are copied using their assignment or copy
 *  constructors. When an object is deleted, its attribute values are also deleted. */
namespace Attribute {

typedef size_t Id;

/** Invalid attribute ID. */
extern const Id INVALID_ID;

/** Register a new attribute key.
 *
 *  The attribute name is registered with the system and an identifier is returned.  The attribute name/ID association is
 *  stored in a single, global attribute symbol table. This method throws an @ref AlreadyExists error if the specified name
 *  already exists in that global table. Once an attribute is registered its ID never changes and it is never removed from the
 *  global attribute symbol table. There is no guarantee that attribute ID numbers are small consecutive integers, although
 *  that is how the current implementation works. */
Id declare(const std::string &name);

/** Returns the ID for an attribute name.
 *
 *  Looks up the specified name in the global attribute symbol table and returns its identification number.  Returns @ref
 *  INVALID_ID if the name does not exist. */
Id id(const std::string &name);

/** Returns the name for an attribute ID.
 *
 *  Looks up the specified attribute ID in the global attribute symbol table and returns its name.  Returns the empty string if
 *  the ID does not exist. */
const std::string& name(Id);

/** Exception for non-existing values.
 *
 *  This exception is thrown when querying an attribute value and no value is stored for the specified attribute ID. */
class DoesNotExist: public std::domain_error {
public:
    ~DoesNotExist() throw () {}

    /** Constructor taking an attribute name or description. */
    explicit DoesNotExist(const std::string &attrName)
        : std::domain_error(attrName + " does not exist in object") {}
};

/** Exception thrown when redeclaring an existing attribute. */
class AlreadyExists: public std::runtime_error {
public:
    ~AlreadyExists() throw () {}

    /** Constructor taking an attribute name or description. */
    AlreadyExists(const std::string &attrName, Id id)
        : std::runtime_error(attrName + " is already a declared attribute (id=" + boost::lexical_cast<std::string>(id) + ")") {}
};

/** Exception thrown when wrong data type is queried. */
typedef boost::bad_any_cast WrongQueryType;

/** API and storage for attributes.
 *
 *  This is the interface inherited by objects that can store attributes.  See the @ref rose::BinaryAnalysis::Attribute
 *  "namespace" for usage and examples. */
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
     *  key. The attribute type can be almost anything and can be changed for each call, but the same type must be used when
     *  retrieving the attribute. */
    template<typename T>
    void setAttribute(Id id, const T &value) {
        values_.insert(id, boost::any(value));
    }

    /** Get an attribute that is known to exist.
     *
     *  Returns the value for the attribute with the specified @p id.  The attribute must exist or a @ref DoesNotExist
     *  exception is thrown.  The type must match the type used when the attribute was stored, or a @ref WrongQueryType
     *  exception is thrown. */
    template<typename T>
    T getAttribute(Id id) const {
        AttrMap::ConstNodeIterator found = values_.find(id);
        if (found == values_.nodes().end()) {
            std::string name = Attribute::name(id);
            if (name.empty()) {
                throw DoesNotExist("attribute id " + boost::lexical_cast<std::string>(id) + " [not declared]");
            } else {
                throw DoesNotExist(name);
            }
        }
        return boost::any_cast<T>(values_.getOptional(id).orDefault());
    }

    /** Return an attribute or a specified value.
     *
     *  If the attribute exists, return its value, otherwise return the specified value. Throws @ref WrongQueryType if the
     *  stored attribute's value type doesn't match the type of the provided default value (if no value is stored then the
     *  provided default type isn't checked). */
    template<typename T>
    T attributeOrElse(Id id, const T &dflt) const {
        return boost::any_cast<T>(values_.getOptional(id).orElse(dflt));
    }

    /** Return an attribute or a default-constructed value.
     *
     *  Returns the attribute value if it exists, or a default-constructed value otherwise. Throws
     *  @ref WrongQueryType if the stored attribute's value type doesn't match the specified type (if no value is stored then
     *  the default type isn't checked). */
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
        return boost::any_cast<T>(found->value());
    }
};

} // namespace
} // namespace
} // namespace

#endif
