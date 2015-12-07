#ifndef ROSE_AstAttributeMechanism_H
#define ROSE_AstAttributeMechanism_H

#include "rosedll.h"
#include "rose_override.h"
#include <Sawyer/Attribute.h>
#include <list>
#include <set>

class SgNode;
class SgNamedType;
class SgJavaParameterizedType;
class SgTemplateParameter;
class SgTemplateParameterList;

/** Base class for all IR node attribute values.
 *
 *  This is the base class for all attribute values stored in the Sage IR node using the @ref AstAttributeMechanism. IR node
 *  attributes are polymorphic based on this abstract class, and are always allocated on the heap. Once the attribute value is
 *  handed to an attribute container method the container owns the value and is reponsible for deleting it.  In the case of
 *  methods that only optionally insert a value, the value is deleted immediately if it's not inserted.  But see @ref
 *  getOwnershipPolicy for additional information.
 *
 *  The underlying @ref Sawyer::Attribute mechanism can store values of any type, including POD, 3rd party types, and
 *  pointers. On the other hand, the @ref AstAttributeMechanism interface described here stores only pointers to values
 *  allocated on the stack, owns those values, and supports operations that are useful specifically in IR nodes.
 *
 *  Subclasses should each implement a virtual @ref copy constructor, which should allocate a new copy of the value.  The
 *  implementation in this base class returns a null pointer, which means that the attribute is not copied into a new AST node
 *  when its AST node is copied.  If a subclass fails to implement the virtual copy constructor and a superclass has an
 *  implementation that return non-null, then copying the AST node will copy only the superclass part of the attribute and the
 *  new attribute will have the dynamic type of the superclass--probably not what you want!
 *
 *
 *
 *  For a more detailed description of using attributes in ROSE (and your own classes) see @ref attributes. */
class ROSE_DLL_API AstAttribute {
public:
    /** Who owns this attribute.
     *
     *  See @ref getOwnershipPolicy. */
    enum OwnershipPolicy {
        CONTAINER_OWNERSHIP,                            /**< Container owns attribute. New subclasses should use this! */
        NO_OWNERSHIP,                                   /**< Attributes are always leaked. */
        CUSTOM_OWNERSHIP,                               /**< Subclass defines ownership policy. */
        UNKNOWN_OWNERSHIP                               /**< Default for old subclasses. */
    };

    /** Who owns this attribute.
     *
     *  The original implementation of this class from the early 2000's did not have clear rules about who owned a
     *  heap-allocated attribute.  The documentation was silent on the issue, and the implementation seemed to imply that
     *  container ownership was intended but was then commented out at some point.  Any ownership policy should have the
     *  following properties:
     *
     *  @li Attributes allocated on the heap should not be leaked. For instance, if an AST is deleted, then the attributes that
     *      were referenced by the AST nodes should also be eventually deleted.
     *  @li The mechanism should not place undue burden on the user.  For instance, if a user copies and later deletes an AST
     *      to which some analysis has attached attributes, the user should not need to be concerned with deleting attributes
     *      stored in the copy.
     *  @li The mechanism should be able to support either deep or shallow attribute copies as appropriate for the
     *      attribute. The deep vs. shallow copying policy is implemented by the virtual @ref copy method, which must
     *      coordinate with the ownership policy to ensure no leaks.
     *
     *  We define four ownership policies, although from the standpoint of an attribute container there are really only two:
     *  the container either deletes attributes or doesn't delete attributes.  The four ownership policies are:
     *
     *  @li @c CONTAINER_OWHERSHIP: The simple approach to ownership, and the one that we recommend for all new attribute
     *      subclasses, is that the attribute container owns the attributes.  When the container is copied (e.g., as part of
     *      copying an AST node) then it invokes the @ref copy methods of its attributes, and when the container is deleted
     *      (e.g., as part of deleting an AST node) then it explicitly deletes its attributes. This policy is an "allocate and
     *      forget" approach: once the creator inserts an attribute into the container it transfers/moves ownership to the
     *      container and the creator never needs to invoke @c delete.  This policy also means that users never need to
     *      explicitly delete attributes if they copy and then delete an AST.  Newly designed attribute subclasses should use
     *      this policy unless they have a very good reason to use another policy.
     *
     *  @li @c NO_OWNERSHIP: Another simple approach is that ownership is transfered to the operating system.  In other words,
     *     the attribute is <em>never</em> deleted by the program and its memory is reclaimed only when the program terminates.
     *     Attribute containers that are incorporated into objects that are frequently allocated and/or copied and then deleted
     *     will result in a large number of leaked attributes.  This approach is not recommended and is present only for
     *     laziness.
     *
     *  @li @c CUSTOM_OWNERSHIP: A third approach is that the attribute subclass implements its own ownership policy, which
     *     ideally should have the properties listed above. An attribute using this policy will never be deleted by an
     *     attribute container; the class must implement some other mechanism for tracking which attributes are allocated and
     *     whether they can be safely deleted.
     *
     *  @li @c UNKNOWN_OWNERSHIP: This final policy is for subclasses implemented before clear attribute ownership rules were
     *     defined.  Due to the ambiguity in the original AstAttributeMechanism implementation and the fact that attributes
     *     are used by code outside the ROSE library, this must be the default implementation. */
    virtual OwnershipPolicy getOwnershipPolicy() const;

    /** Support for attibutes to specify edges in the dot graphs. */
    class ROSE_DLL_API AttributeEdgeInfo {
    public:
        SgNode* fromNode;
        SgNode* toNode;
        std::string label;
        std::string options;

        AttributeEdgeInfo(SgNode* fromNode, SgNode* toNode, std::string label, std::string options)
            : fromNode(fromNode), toNode(toNode), label(label), options(options) {}

        ~AttributeEdgeInfo() {
            fromNode = NULL;
            toNode = NULL;
        };
    };

    /** Support for adding nodes to DOT graphs. */
    class ROSE_DLL_API AttributeNodeInfo {
    public:
        SgNode* nodePtr;
        std::string label;
        std::string options;

        AttributeNodeInfo(SgNode* nodePtr, std::string label, std::string options)
            : nodePtr(nodePtr), label(label), options(options) {}

        ~AttributeNodeInfo() {};
    };

public:
    AstAttribute() {}
    virtual ~AstAttribute() {}

    /** Virtual default constructor.
     *
     *  Default-constructs a new object on the heap and returns its pointer.  All subclasses <em>must</em> implement this in
     *  order to instantiate the correct dynamic type, although many don't.  Invoking this constructor in a subclass that fails
     *  to implement it will return an attribute that's an incorrect dynamic type.
     *
     *  It would be nice if we could make this pure virtual, but unfortunately ROSETTA-generated code fails to compile because
     *  it generates an instantiation of this interface (whether or not that code is ever executed is unknown). [Robb Matzke
     *  2015-11-10]. */
    virtual AstAttribute* constructor() const {
        return new AstAttribute;                        // it doesn't make much sense to instantiate this type!
    }

    /** Virtual copy constructor.
     *
     *  Copy-constructs a new object on the heap and returns its pointer.  All subclasses must implement this in order to
     *  instantiate the correct dynamic type, although many don't.  If this @ref copy method returns a null pointer (like the
     *  base implementation) then the attribute is not copied as part of copying its container.  E.g., an attribute stored in
     *  an AST will not be copied when the AST is copied if that attribute is directly derived from @ref AstAttribute and fails
     *  to implement @ref copy.  If a subclass fails to implement @ref copy and inherits from a class that does implement a
     *  @ref copy that returns non-null, then the copied attribute will have an incorrect dynamic type.
     *
     *  It would be nice if we could make this pure virtual, but unfortunately ROSETTA-generated code fails to compile because
     *  it generates an instantiation of this interface (whether or not that code is ever executed is unkown). [Robb Matzke
     *  2015-11-10] */
    virtual AstAttribute* copy() const {
        return NULL;                                    // attribute will not be copied when the containing obj is copied
    }

    // DO NOT DOCUMENT!  The original implementation used a non-const copy constructor and many subclasses that implemented a
    // copy constructor didn't use C++11's "override" (ROSE_OVERRIDE) word as a defensive measure. Since we don't have access
    // to all those subclasses, we must continue to support the non-const version.  Subclasses should only have to implement
    // one or the other, not both.
    virtual AstAttribute* copy() {
        return const_cast<const AstAttribute*>(this)->copy();
    }

    /** Attribute class name.
     *
     *  Returns the name of the dynamic type. All subclasses must implement this in order to return the correct type name,
     *  although many don't.  If a subclass fails to implement this then it will return an incorrect class name.
     *
     *  It would be nice if this could be pure virtual, but unfortunately ROSETTA-generated code fails to compile because it
     *  generates an instantiation of this interface (whether or not that code is ever executed is unknown). [Robb Matzke
     *  2015-11-10] */
    virtual std::string attribute_class_name() const {
        return "AstAttribute";                          // almost certainly not the right dynamic type name!
    }

    /** Convert an attribute to a string.
     *
     *  This is used by other components to print the value of an attribute. For example the pdf generation calls this function
     *  to print the value of an attribute in the pdf file. The default implementation is to return the attribute address in
     *  the heap as a string. */
    virtual std::string toString();

    /** Packing support.
     *
     * @{ */
    virtual int packed_size();
    virtual char* packed_data();
    virtual void unpacked_data( int size, char* data );
    /** @} */

    /** DOT support.
     *
     *  @{ */
    virtual std::string additionalNodeOptions();
    virtual std::vector<AttributeEdgeInfo> additionalEdgeInfo();
    virtual std::vector<AttributeNodeInfo> additionalNodeInfo();
    /** @} */

    /** Eliminate IR nodes in DOT graphs.
     *
     *  Or to tailor the presentation of information about ASTs. */
    virtual bool commentOutNodeInGraph();
};


/** Stores named attributes in Sage IR nodes.
 *
 *  This attribute container stores one non-null, heap-allocated attribute per user-specified name.  All values are derived
 *  from @ref AstAttribute.  Any object can have an attribute container data member. For example, each AST node (@ref SgNode)
 *  contains one attribute container named @ref SgNode::get_attributeMechanism.
 *
 *  The value class's @ref AstAttribute::getOwnershipPolicy "getOwnershipPolicy" method indicates whether the container owns
 *  the heap-allocated attribute and therefore whether the container is responsible for invoking @c delete on the attribute
 *  when the container is destroyed.  New attribute subclasses should use the @ref AstAttribute::CONTAINER_OWNERSHIP policy if
 *  possible.
 *
 *  IR node attribute values are always on the heap. Whenever an attribute container is copied, the container invokes the @ref
 *  AstAttribute::copy "copy" method on all its attributes. The attributes' @c copy should either allocate a new copy of the
 *  attribute or return a null pointer. Since these values must be derived from @ref AstAttribute, it is not possible to
 *  directly store values whose type the user cannot modify to inherit from @ref AstAttribute. This includes POD types and
 *  classes defined in 3rd party libraries (e.g., @c std::vector). To store such values, the user must wrap them in another
 *  class that does inherit from @ref AstAttribute and which implements the necessary virtual functions.
 *
 *  The names of attributes are strings and the container does not check whether the string supplied to various container
 *  methods is spelled correctly.  Using a misspelled attribute name is the same as using a different value name--in effect,
 *  operating on a completely different, unintended attribute.
 *
 *  The @ref AstAttributeMechanism is used by AST nodes (@ref SgNode) and is available via @ref SgNode::get_attributeMechanism,
 *  although that is not the preferred API.  Instead, @ref SgNode provides an additional methods that contain "attribute" as
 *  part of their name. These "attribute" methods are mostly just wrappers around @ref SgNode::get_attributeMechanism.
 *
 *  Users can also use @ref AstAttributeMechanism as a data member in their own classes. However, @ref Sawyer::Attribute is
 *  another choice: not only does it serve as the implementation for @ref AstAttributeMechanism, but it also supports checked
 *  attribute names and attributes that are values rather than pointers, including POD, 3rd-party types, and shared-ownership
 *  pointers. The amount of boilerplate that needs to be written in order to store a @ref Sawyer::Attribute is much less than
 *  that required to store an attribute with @ref AstAttributeMechanism.
 *
 *  For additional information, including examples, see @ref attributes. */
class ROSE_DLL_API AstAttributeMechanism {
    // Use containment because we want to keep the original API.
    Sawyer::Attribute::Storage attributes_;

public:
    /** Default constructor.
     *
     *  Constructs an attribute mechanism that holds no attributes. */
    AstAttributeMechanism() {}

    /** Copy constructor.
     *
     *  Copying an attribute container will copy the heap-allocated attribute values by invoking each value's @c copy
     *  method. If the method returns null then the new attribute container will not have that attribute; i.e., the @ref exists
     *  method returns false.
     *
     *  <b>New semantics:</b> The original behavior was that if the value's @c copy method returned null, the @ref exists
     *  predicate returned true even though no value existed. */
    AstAttributeMechanism(const AstAttributeMechanism &other) {
        assignFrom(other);
    }

    /** Assignment operator.
     *
     *  Assigning one attribute container to another will cause the destination container's attributes to be erased and
     *  deleted, and the source container's attributes to be copied.  The assignment operator is exception safe: it will either
     *  successfully copy and assign all attributes or not assign any attributes. However, if an attribute uses anything other
     *  than the @ref AstAttribute::CONTAINER_OWNERHSIP ownership policy then it is up to the attribute type's designer to
     *  handle deletion of attributes that were copied before the exception occurred.
     *
     *  <b>New semantics:</b> The original implementation had a copy constructor but no assignment operator. Assignment of one
     *  attribute container to another caused both containers to share the attribute values allocated on the heap, making it
     *  nearly impossible to figure out when they could be safely deleted and therefore leading to memory leaks. */
    AstAttributeMechanism& operator=(const AstAttributeMechanism &other);

    /** Destructor.
     *
     *  Destroying the attribute container should cause unused attributes to be destroyed.  If an attribute implements the @ref
     *  AstAttribute::CONTAINER_OWNERHSIP policy then the container explicitly deletes the attribute, otherwise it is up to the
     *  attribute class's designer to implement a deletion policy that prevents leaks.
     *
     *  <b>New semantics:</b> The original implementation did not delete attributes when the container was destroyed, although
     *  it had commented-out code to do so. */
    ~AstAttributeMechanism();

    /** Test for attribute existence.
     *
     *  Test whether this container holds an attribute with the specified name.  This predicate returns true only if the name
     *  exists and points to a non-null attribute value.  The name need not be declared in the attribute system.
     *
     *  <b>New semantics:</b> It is now permissible to invoke this method on a const attribute container and this method no
     *  longer copies the name argument. */
    bool exists(const std::string &name) const;

    /** Insert an attribute.
     *
     *  Inserts the specified heap-allocated value for the given attribute name, replacing any previous value stored for
     *  that same name.
     *
     *  If the new value uses the @ref AstAttribute::CONTAINER_OWNERSHIP policy then ownership is immediately transferred/moved
     *  to this container, which becomes responsible for deleting the attribute as appropriate to prevent leasks.  Otherwise,
     *  the attribute's designer is responsible for implementing an ownership policy that safely prevents leaks.
     *
     *  If the old value (if present) uses the @ref AstAttribute::CONTAINER_OWNERSHIP policy then it is deleted. Otherwise the
     *  attribute's designer is responsible for implementing an ownership policy that safely prevents leaks.
     *
     *  <b>New semantics:</b> The old implementation didn't delete the previous attribute value.  The old implementation
     *  allowed setting a null value, in which case the old @c exists returned true but the @c operator[] returned no
     *  attribute. */
    void set(const std::string &name, AstAttribute *value);

    /** Insert a new value if the attribute doesn't already exist.
     *
     *  Tests whether an attribute with the specified name @ref exists and if not, invokes @ref set.  See @ref set for details
     *  about ownership of the new attribute. Returns true if an attribute with the specified name did not already existed.
     *
     *  <b>New semantics:</b> The old implementation was ambiguous about who owned the object after this call. It didn't take
     *  ownership of an attribute that wasn't inserted, but it also didn't indicate whether it was inserted.  The old
     *  implementation printed an error message on standard error if the attribute existed (even if only its name existed but
     *  it had no value) and then returned to the caller without doing anything. Inserting a null value was allowed by the old
     *  implementation, in which case the old @c exists returned true but the old @c operator[] returned no attribute. */
    bool add(const std::string &name, AstAttribute *value);

    /** Insert a new value if the attribute already exists.
     *
     *  Tests whether the specified attribute exists, and if so, invokes @ref set. See @ref set for details about ownership of
     *  the old and new attributes.  Returns true if an attribute with the specified name already existed.
     *
     *  <b>New semantics:</b> The old implementation was ambiguous about who owned the object after this call. It didn't take
     *  ownership of an attribute that wasn't inserted, but it also didn't indicate whether it was inserted. The old
     *  implementation printed an error message on standard error if the attribute didn't exist and then returned to the caller
     *  without doing anything. Inserting a null value was allowed by the old implementation, in which case the old @c exists
     *  returned true but the old @c operator[] returned no attribute. */
    bool replace(const std::string &name, AstAttribute *value);

    /** Get an attribute value.
     *
     *  Returns the value associated with the given attribute, or null if the attribute does not exist.  This method does not
     *  copy the attribute before returning it, therefore the caller should not delete the attribute. Erasing the attribute
     *  from the container may cause the pointer to become invalid, depending on the attribute's ownership policy. If the
     *  attribute uses the @ref AstAttribute::CONTAINER_OWNERSHIP method then the attribute can be modified through its pointer
     *  without affecting attributes in other containers, otherwise the behavior is up to the attribute's designer.
     *
     *  The caller will need to @c dynamic_cast the returned pointer to the appropriate subclass of @ref AstAttribute.
     *
     *  <b>New semantics:</b> The old implementation partly created an attribute if it didn't exist: @c exists started
     *  returning true although @c operator[] continued to return no attribute. The old implementation printed an error message
     *  to standard error if the attribute did not exist. */
    AstAttribute* operator[](const std::string &name) const;

    /** Erases the specified attribute.
     *
     *  If an attribute with the specified name exists then it is removed from the container. If the attribute implements the
     *  @ref AstAttribute::CONTAINER_OWNERSHIP policy then this container deletes the attribute, otherwise it is up to the
     *  attribute's designer to implement a safe way to prevent attribute leaks.
     *
     *  <b>New semantics:</b> The old implementation did not delete the attribute value. It also printed an error message
     *  to standard error if the attribute did not exist. */
    void remove(const std::string &name);

    /** Set of attribute names. */
    typedef std::set<std::string> AttributeIdentifiers;

    /** List of stored attribute names.
     *
     *  Returns the set of names for attributes stored in this container.
     *
     *  <b>New semantics:</b> The old implementation also returned some names that had no attribute values. For instance, if
     *  @c operator[] was invoked for an attribute that didn't exist then that name was also returned. */
    AttributeIdentifiers getAttributeIdentifiers() const;

    /** Number of attributes stored.
     *
     *  Returns the number of attributes stored in this container.
     *
     *  <b>New semantics:</b> The old implementation returned a signed integer instead of @c size_t as is customary for size
     *  measurements. It also could not be invoked on a const container. It also could return a value larger larger than the
     *  number of stored attributes (such as when a previous query for a non-existing attribute occurred). */
    size_t size() const;

private:
    // Called by copy constructor and assignment.
    void assignFrom(const AstAttributeMechanism &other);
};



/** Attribute corresponding to a metric.
 *
 *  A metric attribute represents a numeric value obtained by either dynamic analysis (gprof or hpct) or static analysis (for
 *  example number of flop operations in a function) It MetricAttribute can be a raw (observed), or a propagated (derived)
 *  count It containes no name-string, because the attribute is stored in an AttributeMechanism-map, where the name is the
 *  key. */
class ROSE_DLL_API MetricAttribute: public AstAttribute {
protected:
    bool is_derived_;
    double value_;

public:
    MetricAttribute()
        : is_derived_(false), value_(0) {}
    
    MetricAttribute(double value, bool is_derived=false)
        : is_derived_(is_derived), value_(value) {}
    
    virtual AstAttribute* copy() const ROSE_OVERRIDE;
    virtual std::string attribute_class_name() const ROSE_OVERRIDE;

    virtual bool isDerived() const { return is_derived_; }
    virtual double getValue()  const { return value_; }
    virtual void setValue(double newVal) { value_ = newVal; }

    virtual std::string toString();

    virtual int packed_size();
    virtual char* packed_data();
    virtual void unpacked_data(int size, char* data);

    MetricAttribute& operator+=(const MetricAttribute &other);
    MetricAttribute& operator-=(const MetricAttribute &other);
    MetricAttribute& operator*=(const MetricAttribute &other);
    MetricAttribute& operator/=(const MetricAttribute &other);
};


/** IR node attribute that stores a copyable value.
 *
 *  Since IR node attributes must all inherit from @ref AstAttribute we need to write wrappers around POD types and 3rd party
 *  types when we want to store them as IR attributes. We've ended up with a proliferation of such attributes that all do
 *  almost the same thing but have different names to access their data members. Hopefully by providing one class template we
 *  can avoid further proliferation. */
template<class T>
class ROSE_DLL_API AstValueAttribute: public AstAttribute {
public:
    /** Type of value this wrapper holds. */
    typedef T Value;
private:
    Value value_;
public:
    /** Default constructor. Constructs an attribute wrapper having a default-constructed value. */
    AstValueAttribute() {}

    /** Constructs an attribute containing a specified value. */
    explicit AstValueAttribute(const T &value): value_(value) {}

    /** Copy constructor. Invokes the copy-constructor of the contained value. */
    AstValueAttribute(const AstValueAttribute &other): value_(other.value_) {}

    virtual AstAttribute* copy() const ROSE_OVERRIDE { return new AstValueAttribute(*this); }
    virtual std::string attribute_class_name() const ROSE_OVERRIDE { return "AstValueAttribute"; }

    /** Return the stored value by reference.
     *
     * @{ */
    const T& get() const { return value_; }
    T& get() { return value_; }
    /** @} */

    /** Assign a new value. */
    void set(const T& value) { value_ = value; }
};

// DQ (11/21/2009): Added new kind of attribute for handling regex trees.
/** Attribute containing a regex expression as a string.
 *
 *  An AstRegExAttribute is added to each part of an AST tree pattern specification to a RegEx tree. */
class ROSE_DLL_API AstRegExAttribute: public AstAttribute {
public:
    std::string expression;

    AstRegExAttribute() {}
    
    explicit AstRegExAttribute(const std::string & s)
        : expression(s) {}

    virtual AstAttribute* copy() const ROSE_OVERRIDE {
        return new AstRegExAttribute(*this);
    }

    virtual std::string attribute_class_name() const ROSE_OVERRIDE {
        return "AstRegExAttribute";
    }
};


// PC (10/21/2012): Added new kind of attribute for handling regex trees.
/** Attribute storing an SgNode.
 *
 *  A SgNode attribute may be used to "dynamically add" an SgNode field to an AST node. */
class ROSE_DLL_API AstSgNodeAttribute: public AstValueAttribute<SgNode*> {
public:
    typedef AstValueAttribute<SgNode*> Super;
    AstSgNodeAttribute(): Super(NULL) {}
    explicit AstSgNodeAttribute(SgNode *value): Super(value) {}
    AstSgNodeAttribute(const AstSgNodeAttribute &other): Super(other) {}
    virtual AstAttribute* copy() const ROSE_OVERRIDE { return new AstSgNodeAttribute(*this); }
    virtual std::string attribute_class_name() const ROSE_OVERRIDE { return "AstSgNodeAttribute"; }
    SgNode* getNode() { return get(); }
    void setNode(SgNode *node) { set(node); }
};

class ROSE_DLL_API AstSgNodeListAttribute: public AstValueAttribute<std::vector<SgNode*> > {
public:
    typedef AstValueAttribute<std::vector<SgNode*> > Super;
    AstSgNodeListAttribute() {}
    explicit AstSgNodeListAttribute(std::vector<SgNode *> &value): Super(value) {}
    AstSgNodeListAttribute(const AstSgNodeListAttribute &other): Super(other) {}
    virtual AstAttribute* copy() const ROSE_OVERRIDE { return new AstSgNodeListAttribute(*this); }
    virtual std::string attribute_class_name() const ROSE_OVERRIDE { return "AstSgNodeListAttribute"; }
    std::vector<SgNode*> &getNodeList() { return get(); }
    void addNode(SgNode *n) { get().push_back(n); }
    void setNode(SgNode*, int);
    SgNode *getNode(int);
    int size() { return get().size(); }
};

class ROSE_DLL_API AstIntAttribute : public AstValueAttribute<int> {
public:
    typedef AstValueAttribute<int> Super;
    AstIntAttribute(): Super(0) {}
    explicit AstIntAttribute(int value): Super(value) {}
    AstIntAttribute(const AstIntAttribute &other): Super(other) {}
    virtual AstAttribute* copy() const ROSE_OVERRIDE { return new AstIntAttribute(*this); }
    virtual std::string attribute_class_name() const ROSE_OVERRIDE { return "AstIntAttribute"; }
    int getValue() { return get(); }
};

class ROSE_DLL_API AstParameterizedTypeAttribute: public AstAttribute {
    SgNamedType *genericType;
    std::list<SgJavaParameterizedType *> parameterizedTypes;

public:
    AstParameterizedTypeAttribute()
        : genericType(NULL) {}
    
    explicit AstParameterizedTypeAttribute(SgNamedType *genericType);

    AstParameterizedTypeAttribute(const AstParameterizedTypeAttribute &other)
        : genericType(other.genericType), parameterizedTypes(other.parameterizedTypes) {}

    virtual AstAttribute* copy() const ROSE_OVERRIDE {
        return new AstParameterizedTypeAttribute(*this);
    }

    virtual std::string attribute_class_name() const ROSE_OVERRIDE {
        return "AstParameterizedTypeAttribute";
    }

    bool argumentsMatch(SgTemplateParameterList *type_arg_list, std::vector<SgTemplateParameter *> *new_args);

    SgJavaParameterizedType *findOrInsertParameterizedType(std::vector<SgTemplateParameter *> *new_args);
};

#endif
