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
 *  ROSE has an older @ref AstAttributeMechanism that's derived from the AttributeMechanism class template. Attributes stored
 *  in IR nodes can use the attribute-accessing methods defined in @ref SgNode, which are essentially wrappers around the @ref
 *  AstAttributeMechanism interface.  This purpose of this table and the examples that follow is to compare and contrast
 *  attribute mechanisms in a broader context than just IR nodes--it looks at using an attribute mechanism in arbitrary
 *  objects. Most of the rows of the AstAttributeMechanism are also applicable to the SgNode attribute interface.
 *
 *  <table>
 *    <tr>
 *      <td><b>This API</b></td>
 *      <td><b>AstAttributeMechanism</b></td>
 *      <td><b>AttributeMechanism</b></td>
 *    </tr>
 *    <tr>
 *      <td>Can store multiple attributes with many different value types.</td>
 *      <td>Can store multiple attributes with many different value types as long as those types all derive from
 *          AstAttribute.</td>
 *      <td>Can store multiple attributes all having the same type (second template argument). To store multiple
 *          types additional AttributeMechanism data members must be used.</td>
 *    </tr>
 *    <tr>
 *      <td>Can directly store non-class values.</td>
 *      <td>Requires non-class values to be wrapped in a class derived from @ref AstAttribute</td>
 *      <td>Can directly store non-class values (one value type per AttributeMechanism)</td>
 *    </tr>
 *    <tr>
 *      <td>%Attribute getter/setter methods are directly in containing class.</td>
 *      <td>Must obtain the AstAttributeMechanism data member first (@ref SgNode::get_attributeMechanism), then the
 *          getter/setter. The @ref SgNode class provides wrapper methods around AstAttributeMechanism, but this interface is
 *          available only in IR node objects.</td>
 *      <td>Must get the AttributeMechanism data member first unless only one value type is allowed (each AttributeMechanism
 *          data member can store attribute values having a single type).</td>
 *    </tr>
 *    <tr>
 *      <td>Can store values whose type is not user-modifiable, such as STL containers.</td>
 *      <td>User must be able modify the value type so it inherits from AstAttribute, or he must wrap the type in his own
 *          subclass of AstAttribute, adding an extra level of indirection to access the value.</td>
 *      <td>Can store values whose type is not user-modifiable, such as STL containers.</td>
 *    </tr>
 *    <tr>
 *      <td>Ensures that two users don't declare the same attribute name.</td>
 *      <td>Some assurance that names don't collide: the @c add method is a no-op (with an error message) if an attribute is
 *          already stored with that name, but @c set makes no such check.</td>
 *      <td>Names don't collide for attributes storing different value types, otherwise the behavior is the same
 *          as for @ref AstAttributeMechanism.</td>
 *    </tr>
 *    <tr>
 *      <td>Uses normal C++ copy constructors and assignment operators for attribute values.</td>
 *      <td>Requires implementation of virtual @c copy method (non-pure), which returns a null pointer if the user forgets.</td>
 *      <td>Uses normal C++ copy constructors and assignment operators for attribute values.</td>
 *    </tr>
 *    <tr>
 *      <td>Errors are reported by dedicated exception types allowing the user to gracefully recover.</td>
 *      <td>Errors are reported by printing them to the standard error stream and then either doing nothing (production) or
 *          aborting the whole program (debug).</td>
 *      <td>Errors are reported by printing them to the standard error stream and then either doing nothing (production) or
 *          aborting the whole program (debug).</td>
 *    </tr>
 *    <tr>
 *      <td>%Attribute existence and retrieval can be performed on a reference to a const object.</td>
 *      <td>The API does not support non-modifying operations on a reference to a const object.</td>
 *      <td>The API does not support non-modifying operations on a reference to a const object.</td>
 *    </tr>
 *    <tr>
 *      <td>Attempting to retrieve a non-existing attribute without providing a default value throws a
 *          @ref DoesNotExist exception.</td>
 *      <td>Attempting to retrieve a non-existing attribute emits a message to standard error and then causes the attribute to
 *          exist but have a null pointer (production) or aborts the whole program (debug). The user must check existence
 *          before retreiving.</td>
 *      <td>Attempting to retrieve a non-existing attribute emits a message to standard error and then causes the attribute to
 *          spring into existence with a default-constructed value (production) or aborts the whole program (debug). The user
 *          must check existence before retreiving.</td>
 *    </tr>
 *    <tr>
 *      <td>Erasing an attribute that is not stored is a no-op similar to STL container erase methods.</td>
 *      <td>Erasing an attribute that is not stored emits a message to standard error and does nothing (production)
 *          or aborts the whole program (debug). The user must check existence before erasing.</td>
 *      <td>Erasing an attribute that is not stored emits a message to standard error and does nothing (production)
 *          or aborts the whole program (debug). The user must check existence before erasing.</td>
 *    </tr>
 *    <tr>
 *      <td>%Attribute value types are runtime checked. A mismatch between writing and reading is reported by a
 *          @ref BadQueryType exception.</td>
 *      <td>%Attribute values types are runtime checked. A mismatch is discovered by the user when they perform a
 *          @c dynamic_cast from the AstAttribute base type to their subclass.</td>
 *      <td>%Attribute values are compile-time checked since each AttributeMechanism can store only one value type.</td>
 *    </tr>
 *    <tr>
 *      <td>All casting is hidden behind the API.</td>
 *      <td>Requires user to use C++ @c dynamic_cast from the AstAttribute pointer to the user's subclass pointer.</td>
 *      <td>No dynamic casting necessary since each AttributeMechanism stores only one value type.</td>
 *    </tr>
 *    <tr>
 *      <td>%Attribute value destructors are called when the containing object is destroyed.</td>
 *      <td>Attributes (all allocated on the heap) are never deleted automatically; the user must figure out when a
 *          value is no longer in use and delete it explicitly.</td>
 *      <td>%Attribute value destructors are called when the containing object is destroyed.</td>
 *    </tr>
 *  </table>
 *
 *  Some examples may help illuminate the differences. Let us assume that two types exist in some library header file somewhere
 *  and the user wants to store these as attribute values in some object. The two value types are:
 *
 *  @snippet binaryAttribute.C comparison value types
 *
 *  Let us also assume that a ROSE developer has a class and wants the user to be able to store attributes in objects of that
 *  class.  IR nodes have an alternative attribute interface that's a slim wrapper around the @ref AstAttributeMechanism, but
 *  since they're only defined in IR nodes and since users cannot create new IR node subclasses without modifying and
 *  recompiling ROSE and EDG, this step doesn't apply there.
 *
 *  The first step is for the ROSE developer to prepare his class for storing attributes:
 *
 *  @snippet binaryAttribute.C comparison preparing storage
 *
 *  Notice that AttributeMechanism requires a data member for each type of value. AstAttributeMechanism, which derives from
 *  AttributeMechanism, avoids this by using pointers to polymorhic value types. So the AttributeMechanism is already not
 *  extensible unless the user can edit the type where the attributes are stored--but if he can edit the type, then why use
 *  attributes at all?
 *
 *  Now we jump into the user code. The user wants to be able to store two attributes, one of each value type. As mentioned
 *  above, the attribute value types are defined in some library header, and the class of objects in which to store them is
 *  defined in a ROSE header file.  Methods 1 and 3 can store values of any type, but the user has more work to do before he
 *  can use methods 2 or 4:
 *
 *  @snippet binaryAttribute.C comparison attribute wrappers
 *
 *  Notice that AstAttributeMechanism and the SgNode interface both need a substantial amount of boilerplate to store even a
 *  simple enum value. Since @c copy is not pure virtual, if the user forgets to implement it (or mistypes it without C++11 @c
 *  override, like accidentally adding the @c const qualifier which is normally present for a copy constructor), then attribute
 *  values of that type will not be copied when the containing object is copied, but the @c exists, @c get, @c add, and @c
 *  remove methods will say they do exist.
 *
 *  The user will also want to use descriptive strings for the attribute so error messages are informative, but shorter names
 *  in C++ code, so we declare the attribute names:
 *
 *  @snippet binaryAttribute.C comparison declare 1
 *  @snippet binaryAttribute.C comparison declare 2
 *  @snippet binaryAttribute.C comparison declare 3
 *  @snippet binaryAttribute.C comparison declare 4
 *
 *  The declarations in methods 2 and 3 are identical. Method 1 differs by using an integral type for attribute IDs, which has
 *  two benefits: (1) it gives the API an opportunity to check whether two different users are trying to declare the same
 *  attribute name for different purposes, and (2) it reduces the size and increases the speed of the underlying storage maps
 *  by storing integer keys rather than strings.
 *
 *  Let us see how to insert two attributes into an object assuming that the object came from somewhere far away and we don't
 *  know whether it already contains these attributes. If it does, we want to overwrite their old values with new
 *  values. Overwriting values is likely to be a more common operation than insert-if-nonexistent. After all, languages
 *  generally don't have a dedicated assign-value-if-none-assigned operator (Perl and Bash being exceptions).
 *
 *  @snippet binaryAttribute.C comparison insert 1
 *  @snippet binaryAttribute.C comparison insert 2
 *  @snippet binaryAttribute.C comparison insert 3
 *  @snippet binaryAttribute.C comparison insert 4
 *
 *  Methods 1 and 3 are able to insert a new value and clean up any old value if it existed by automatically calling the old
 *  value's destructor.  Methods 2 and 4 requires the user to check whether an old value existed and delete the value
 *  explicitly. Fortunately deleting a null pointer is a no-op, otherwise we'd have to check that also.
 *
 *  Eventually the user will want to retrieve an attribute's value. Perhaps he wants the attribute value if it exists, or some
 *  default if it doesn't:
 *
 *  @snippet binaryAttribute.C comparison retrieve 1
 *  @snippet binaryAttribute.C comparison retrieve 2
 *  @snippet binaryAttribute.C comparison retrieve 3
 *  @snippet binaryAttribute.C comparison retrieve 4
 *
 *  Notice that methods 2, 3, and 4 require that the user first checks for attribute existence since querying a non-existing
 *  attribute will emit a message on standard error and instantiate the attribute (production) or abort the whole program
 *  (debug).  In production mode, querying a non-existing attribute with methods 2 or 4 will insert the attribute with a
 *  null pointer which the other methods interpret as the attribute existing even though it doesn't, and method 3 will
 *  insert a default-constructed attribute.  Methods 2 and 4 also require a cumbersome dynamic cast, an additional pointer
 *  check, and then unrwapping the wrapped attribute.
 *
 *  A potential gotcha with method 3 is that since a separate AstAttributeMechanism data member is required for each attribute
 *  value type, the user must be careful that he uses the correct data member. For instance, the @c exists checks above will
 *  work regardless of whether we use @c approximationAttributes or @c analysisTimeAttributes. In fact, an early version of the
 *  example used the wrong one in one place and postcondition assertions (not shown) were failing.
 *
 *  Sooner or later a user will want to erase an attribute. Perhaps the attribute holds the result of some optional analysis
 *  which is no longer valid. The user wants to ensure that the attribute doesn't exist, but isn't sure whether it exists
 *  currently:
 *
 *  @snippet binaryAttribute.C comparison erase 1
 *  @snippet binaryAttribute.C comparison erase 2
 *  @snippet binaryAttribute.C comparison erase 3
 *  @snippet binaryAttribute.C comparison erase 4
 *
 *  The @c delete operators are necessary in Methods 2 and 4 to prevent memory leaks. Methods 2, 3, and 4 all require the user
 *  to check for existence first because the @c remove and @c removeAttribute methods print messages to standard error if the
 *  attribute doesn't exist.
 *
 *  Finally, when the object containing the attributes is destroyed the user needs to be able to clean up by destroying the
 *  attributes that are attached. For instance, if a function has local variables that might be storing attributes and the
 *  function calls something that might throw an exception we need to catch the exception, delete the attributes (checking if
 *  they exist before we access them), and rethrow the same exception. This also makes debugging more difficult because GDB
 *  will lose information about where the exception was originally thrown--it will give you only the location of the rethrow.
 *
 *  @snippet binaryAttribute.C comparison cleanup 1
 *  @snippet binaryAttribute.C comparison cleanup 2
 *  @snippet binaryAttribute.C comparison cleanup 3
 *  @snippet binaryAttribute.C comparison cleanup 4
 *
 *  Whether to use Method 1, 2, or 3 is up to the author of the class that wants to allow attributes to be stored. Whether to
 *  use Method 2 or 4 in an IR node is up to the user of the IR node. */
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
