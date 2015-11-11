#ifndef ROSE_BinaryAnalysis_Attribute_H
#define ROSE_BinaryAnalysis_Attribute_H

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <string>
#include <vector>

namespace rose {
namespace BinaryAnalysis {

/** Attributes attached to other objects.
 *
 *  This namespace defines a mechanism for extending objects at run-time through the use of name/value pairs called
 *  "attributes".  Attributes are an extension mechanism orthogonal to class derivation and compile-time containment. Among
 *  other things, they are a convenient mechanism for users to cache data that needs to be associated with some ROSE
 *  object. See also, @ref SgNode::getAttribute et al for attributes attached to Sage IR nodes. There are two steps
 *  to using attributes: registering an attribute name to obtain an ID number, and accessing attribute values stored in
 *  objects.
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
 *  constructors. When an object is deleted, its attribute values are also deleted.
 *
 *
 *
 *  @section binary_attribute_comparison Comparison with other attribute APIs
 *
 *  ROSE also has an interface within the @ref SgNode class for storing heap-based attributes in IR nodes. Here's how these two
 *  interfaces differ:
 *
 *  <table>
 *    <tr>
 *      <td><b>This API</b></td>
 *      <td><b>IR node attributes</b></td>
 *    </tr>
 *    <tr>
 *      <td>Class authors can add attribute-storing capability to any class by inheriting this interface.</td>
 *      <td>Applies only to IR nodes, although the underlying @ref AstAttributeMechanism with a slightly different
 *          interface can be used in any class. This column applies mostly to the @ref AstAttributeMechanism also.</td>
 *    </tr>
 *    <tr>
 *      <td>Can store multiple attributes with many different value types.</td>
 *      <td>Can store multiple attributes with many different value types as long as those types all derive from
 *          AstAttribute.</td>
 *    </tr>
 *    <tr>
 *      <td>Can directly store non-class values.</td>
 *      <td>Requires non-class values to be wrapped in a class derived from @ref AstAttribute</td>
 *    </tr>
 *    <tr>
 *      <td>Can store values whose type is not user-modifiable, such as STL containers.</td>
 *      <td>User must be able modify the value type so it inherits from AstAttribute, or he must wrap the type in his own
 *          subclass of AstAttribute, adding an extra level of indirection to access the value.</td>
 *    </tr>
 *    <tr>
 *      <td>Ensures that two users don't declare the same attribute name.</td>
 *      <td>No assurance that the same name is not used for two different purposes.</td>
 *    </tr>
 *    <tr>
 *      <td>Uses normal C++ copy constructors and assignment operators for attribute values.</td>
 *      <td>Requires implementation of virtual @c copy method (non-pure) if copying is intended.</td>
 *    </tr>
 *    <tr>
 *      <td>Errors are reported by dedicated exception types.</td>
 *      <td>Errors are reported by return values.</td>
 *    </tr>
 *    <tr>
 *      <td>Attempting to retrieve a non-existing attribute without providing a default value throws a
 *          @ref DoesNotExist exception.</td>
 *      <td>Attempting to retrieve a non-existing attribute without providing a default value returns a null attribute
 *          pointer</td>
 *    </tr>
 *    <tr>
 *      <td>%Attribute value types are runtime checked. A mismatch between writing and reading is reported by a
 *          @ref WrongQueryType exception.</td>
 *      <td>%Attribute values types are runtime checked. A mismatch is discovered by the user when they perform a
 *          @c dynamic_cast from the AstAttribute base type to their subclass.</td>
 *    </tr>
 *    <tr>
 *      <td>All casting is hidden behind the API.</td>
 *      <td>Requires user to use C++ @c dynamic_cast from the AstAttribute pointer to the user's subclass pointer.</td>
 *    </tr>
 *  </table>
 *
 *  Some examples may help illuminate the differences.  The examples show three methods of using attributes:
 *
 *  @li <b>Method 1</b> uses the BinaryAnalysis::Attribute interface.
 *  @li <b>Method 2</b> uses the @ref AstAttributeMechanism interface.
 *  @li <b>Method 3</b> uses the @ref SgNode attribute interface.
 *
 *  Let us assume that two types exist in some library header file somewhere and the user wants to store these as attribute
 *  values in some object. The two value types are:
 *
 *  @snippet binaryAttribute.C comparison value types
 *
 *  Let us also assume that a ROSE developer has a class and wants the user to be able to store attributes in objects of that
 *  class.  The first step is for the ROSE developer to prepare his class for storing attributes:
 *
 *  @snippet binaryAttribute.C comparison preparing storage
 *
 *  Method 1 is designed to use inheritance: all of its methods have the word "attribute" in their names. Method 2 could be
 *  used by inheritance, but is more commonly used with containment due to its short, common method names like @c size. Method
 *  3 applies only to Sage IR nodes, but creating a new subclass of SgNode is outside the scope of this document; instead,
 *  we'll just use an existing IR node type.
 *
 *  Now we jump into the user code. The user wants to be able to store two attributes, one of each value type. As mentioned
 *  above, the attribute value types are defined in some library header, and the class of objects in which to store them is
 *  defined in a ROSE header file.  Method 1 an store values of any type, but the user has more work to do before he
 *  can use methods 2 or 3:
 *
 *  @snippet binaryAttribute.C comparison attribute wrappers
 *
 *  Method 1 requires no additional wrapper code since it can store any value directly. Methods 2 and 3 both require a
 *  substantial amount of boilerplate to store even a simple enum value.  The @c copy method's purpose is to allocate a new
 *  copy of an attribute when the object holding the attribute is copied or assigned. The copy method should be implemented in
 *  every @ref AstAttribute subclass, although few do.  If it's not implemented then one of two things happen: either the
 *  attribute is not copied, or only a superclass of the attribute is copied. Subclasses must also implement @c
 *  attribute_class_name, although few do. Neither @c copy nor @c attribute_class_name are pure virtual because of limitations
 *  with ROSETTA code generation.
 *
 *  Next, the user will want to use descriptive strings for the attribute so error messages are informative, but shorter names
 *  in C++ code, so we declare the attribute names:
 *
 *  @snippet binaryAttribute.C comparison declare 1
 *  @snippet binaryAttribute.C comparison declare 2
 *  @snippet binaryAttribute.C comparison declare 3
 *
 *  The declarations in methods 2 and 3 are identical. Method 1 differs by using an integral type for attribute IDs, which has
 *  two benefits: (1) it prevents two users from using the same attribute name for different purposes, and (2) it reduces the
 *  size and increases the speed of the underlying storage maps by storing integer keys rather than strings. Method 1 has
 *  functions that convert between identification numbers and strings if necessary (e.g., error messages).
 *
 *  Now, let us see how to insert two attributes into an object assuming that the object came from somewhere far away and we
 *  don't know whether it already contains these attributes. If it does, we want to overwrite their old values with new
 *  values. Overwriting values is likely to be a more common operation than insert-if-nonexistent. After all, languages
 *  generally don't have a dedicated assign-value-if-none-assigned operator (Perl and Bash being exceptions).
 *
 *  @snippet binaryAttribute.C comparison insert 1
 *  @snippet binaryAttribute.C comparison insert 2
 *  @snippet binaryAttribute.C comparison insert 3
 *
 *  Method 1 stores the attribute directly while Methods 2 and 3 require the attribute value to be wrapped in a heap-allocated
 *  object first.
 *
 *  Eventually the user will want to retrieve an attribute's value. Users commonly need to obtain the attribute or a default
 *  value.
 *
 *  @snippet binaryAttribute.C comparison retrieve 1
 *  @snippet binaryAttribute.C comparison retrieve 2
 *  @snippet binaryAttribute.C comparison retrieve 3
 *
 *  Method 1 has a couple functions dedicated to this common scenario. Methods 2 and 3 return a null pointer if the attribute
 *  doesn't exist, but require a dynamic cast to the appropriate type otherwise.
 *
 *  Sooner or later a user will want to erase an attribute. Perhaps the attribute holds the result of some optional analysis
 *  which is no longer valid. The user wants to ensure that the attribute doesn't exist, but isn't sure whether it currently
 *  exists:
 *
 *  @snippet binaryAttribute.C comparison erase 1
 *  @snippet binaryAttribute.C comparison erase 2
 *  @snippet binaryAttribute.C comparison erase 3
 *
 *  If the attribute didn't exist then none of these methods do anything. If it did exist... With Method 1, the value's
 *  destructor is called. Methods 2 and 3 delete the heap-allocated value, which is allowed since the attribute container owns
 *  the object.
 *
 *  Finally, when the object containing the attributes is destroyed the user needs to be able to clean up by destroying the
 *  attributes that are attached:
 *
 *  @snippet binaryAttribute.C comparison cleanup 1
 *  @snippet binaryAttribute.C comparison cleanup 2
 *  @snippet binaryAttribute.C comparison cleanup 3
 *
 *  All three interfaces now properly clean up their attributes, although this wasn't always the case with methods 2 and 3. */
namespace Attribute {

/** Attribute identification.
 *
 *  Each attribute name has a unique identification number and vice versa. */
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

    /** Erase all attributes. */
    void clearAttributes() {
        values_.clear();
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

    /** Number of attributes stored. */
    size_t nAttributes() const {
        return values_.size();
    }

    /** Returns ID numbers for all IDs stored in this container. */
    std::vector<Id> attributeIds() const;
};

} // namespace
} // namespace
} // namespace

#endif
