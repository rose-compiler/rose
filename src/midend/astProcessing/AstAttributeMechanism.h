#ifndef ROSE_AstAttributeMechanism_H
#define ROSE_AstAttributeMechanism_H

#include "rosedll.h"
#include "rose_override.h"
#include <Sawyer/Attribute.h>

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
 *  methods that only optionally insert a value, the value is deleted immediately if it's not inserted.
 *
 *  The underlying @ref Sawyer::Attribute mechanism can store values of any type, including POD, 3rd party types, and
 *  pointers. On the other hand, the @ref AstAttributeMechanism interface described here stores only pointers to values
 *  allocated on the stack, owns those values, and supports operations that are useful specifically in IR nodes.
 *
 *  Subclasses must each implement a virtual @ref copy constructor, which should allocate a new copy of the value.
 *
 *  For a more detailed description of using attributes in ROSE (and your own classes) see @ref attributes. */
class ROSE_DLL_API AstAttribute {
public:
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
     *  Default-constructs a new object on the heap and returns its pointer.  All subclasses must implement this in order to
     *  instantiate the correct dynamic type, although many don't.
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
     *  instantiate the correct dynamic type, although many don't.
     *
     *  It would be nice if we could make this pure virtual, but unfortunately ROSETTA-generated code fails to compile because
     *  it generates an instantiation of this interface (whether or not that code is ever executed is unkown). [Robb Matzke
     *  2015-11-10] */
    virtual AstAttribute* copy() const {
        return NULL;                                    // attribute will not be copied when the containing obj is copied
    }

    /** Attribute class name.
     *
     *  Returns the name of the dynamic type. All subclasses must implement this in order to return the correct type name,
     *  although many don't.
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

    /** DOT support. */
    virtual std::string additionalNodeOptions();

    virtual std::vector<AttributeEdgeInfo> additionalEdgeInfo();
    virtual std::vector<AttributeNodeInfo> additionalNodeInfo();

    /** Eliminate IR nodes in DOT graphs.
     *
     *  Or to tailor the presentation of information about ASTs. */
    virtual bool commentOutNodeInGraph();
};


/** Stores named attributes in Sage IR nodes.
 *
 *  Attributes have a name and a value, the value being derived from @ref AstAttribute. IR node attribute values are always
 *  allocated on the stack and are owned by the IR node containing the attribute. Since attribute values are not reference
 *  counted, they must be copied (new value allocated on the heap) whenever the object containing them is copied, otherwise we
 *  would lose track of which containing object owns the attribute value.
 *
 *  Since IR attribute values must be derived from @ref AstAttribute, it is not possible to directly store values whose type
 *  the user cannot modify to inherit from @ref AstAttribute. This includes POD types and classes defined in 3rd party
 *  libraries. To store such values, the user must wrap them in another class that does inherit from @ref AstAttribute.
 *
 *  For the purpose of backward compatibility, IR node attributes have string names that need not be registered prior to using
 *  them. Therefore, a misspelled name is treated as a different attribute. The underlying @ref Sawyer::Attribute
 *  mechanism checks for misspelled names, therefore this class does extra work to bypass that check.
 *
 *  This interface applies only to IR nodes of type @ref SgNode and its derivatives.  This interface is built on the @ref
 *  Sawyer::Attribute interface, which can store POD and 3rd party types, and stores values of such types instead
 *  of pointers.
 *
 *  This interface was originally implemented by Dan Quinlan in terms of a template-based AttributeMechanism class
 *  designed by Markus Schordan in the early 2000's.  The current implementation aims to fix a number of issues identified with
 *  the previous implementation, including ambiguity about who owns the heap-allocated attribute values.
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
     *  deleted, and the source container's attributes to be copied.
     *
     *  <b>New semantics:</b> The original implementation had a copy constructor but no assignment operator. Assignment of one
     *  attribute container to another caused both containers to share the attribute values allocated on the heap, making it
     *  nearly impossible to figure out when they could be safely deleted and therefore leading to memory leaks. */
    AstAttributeMechanism& operator=(const AstAttributeMechanism &other) {
        assignFrom(other);
        return *this;
    }

    /** Destructor.
     *
     *  Destroying the attribute container will cause all attribute values in the container to be deleted. */
    ~AstAttributeMechanism();

    /** Test for attribute existence.
     *
     *  Test whether this container holds an attribute with the specified name.  This predicate returns true only if the name
     *  exists and points to a non-null heap-allocated attribute value.  The name need not be declared in the attribute
     *  system.
     *
     *  <b>New semantics:</b> It is now permissible to invoke this method on a const attribute container and this method no
     *  longer copies the name argument. */
    bool exists(const std::string &name) const;

    /** Insert a new value if the attribute doesn't already exist.
     *
     *  Tests whether an attribute with the specified name @ref exists and if not, inserts this attribute into the
     *  container. If @p value is null then this method is a no-op. The caller relinquishes ownership of the @p value whether
     *  or not it's inserted. If the value is not inserted then this method immediately deletes it.  Returns true if the value
     *  was inserted, false if not inserted.
     *
     *  <b>New semantics:</b> The old implementation was ambiguous about who owned the object after this call. It didn't take
     *  ownership of an attribute that wasn't inserted, but it also didn't indicate whether it was inserted.  The old
     *  implementation printed an error message on standard error if the attribute existed (even if only its name existed but
     *  it had no value) and then returned to the caller without doing anything. Inserting a null value was allowed by the old
     *  implementation, in which case the old @c exists returned true but the old @c operator[] returned no attribute. */
    bool add(const std::string &name, AstAttribute *value);

    /** Insert a new value if the attribute already exists.
     *
     *  Tests whether the specified attribute exists, and if so, replaces the old value with a new one.  The new value must be
     *  allocated on the heap and non-null, or else this method is a no-op.  The old value is deleted. The caller relinquishes
     *  ownership of the new value; if the new value is not inserted then this method immediately deletes it.  Returns true if
     *  the new value is inserted, false if not inserted.
     *
     *  <b>New semantics:</b> The old implementation was ambiguous about who owned the object after this call. It didn't take
     *  ownership of an attribute that wasn't inserted, but it also didn't indicate whether it was inserted. The old
     *  implementation printed an error message on standard error if the attribute didn't exist and then returned to the caller
     *  without doing anything. Inserting a null value was allowed by the old implementation, in which case the old @c exists
     *  returned true but the old @c operator[] returned no attribute. */
    bool replace(const std::string &name, AstAttribute *value);

    /** Insert a value regardless of whether the attribute already exists.
     *
     *  Inserts the specified value for an attribute, overwriting and deleting any previous value stored for that same
     *  attribute name.  The owner relinquishes ownership of the value, which must be allocated on the heap.
     *
     *  <b>New semantics:</b> The old implementation didn't delete the previous attribute value.  The old implementation
     *  allowed setting a null value, in which case the old @c exists returned true but the @c operator[] returned no
     *  attribute. */
    void set(const std::string &name, AstAttribute *value);

    /** Get an attribute value.
     *
     *  Returns the value associated with the given attribute, or null if the attribute does not exist.  The container retains
     *  ownership of the attribute--the user most not delete it.  The user will need to @c dynamic_cast the returned pointer to
     *  the appropriate subclass of @ref AstAttribute.  The returned value containues to be owned by the attribute container;
     *  the user must not delete it, although he may modify its contents since attributes are not shared between containers.
     *
     *  <b>New semantics:</b> The old implementation partly created an attribute if it didn't exist: @c exists started
     *  returning true although @c operator[] continued to return no attribute. The old implementation printed an error message
     *  to standard error if the attribute did not exist. */
    AstAttribute* operator[](const std::string &name) const;

    /** Erases the specified attribute.
     *
     *  If an attribute with the specified name exists then it is removed from the container and deleted.
     *
     *  <b>New semantics:</b> The old implementation did not delete the attribute value.  Now that attribute values are never
     *  shared with other containers or the caller it is safe to delete them. The old implementation printed an error message
     *  to standard error if the attribute did not exist. */
    void remove(const std::string &name);

    /** Set of attribute identifiers. */
    typedef std::set<std::string> AttributeIdentifiers;

    /** List of stored attribute names.
     *
     *  Returns the set of attribute names. */
    AttributeIdentifiers getAttributeIdentifiers() const;

    /** Number of attributes stored.
     *
     *  Returns the number of attributes stored in this container.
     *
     *  <b>New semantics:</b> The old implementation returned a signed integer instead of @c size_t as is customary for size
     *  measurements. It also could not be invoked on a const container. */
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
