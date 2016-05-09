#include "sage3basic.h"
#include "AstAttributeMechanism.h"
#include "Diagnostics.h"

#include "roseInternal.h"
#include <boost/foreach.hpp>
#include <sstream>
#include <Sawyer/Map.h>
#include <Sawyer/Set.h>

using namespace rose;
using namespace rose::Diagnostics;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AstAttributeMechanism
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Types of warnings reported. This is to address concerns that there are too many warnings when running ROSE tools that use
// old attribute subclasses that didn't properly implement the AstAttribute interface (perhaps due to lack of documentation).
enum WarningType { HAS_MEMORY_LEAK, HAS_UNKNOWN_OWNERSHIP, HAS_NULL_COPY, HAS_SELF_COPY, HAS_NO_CLASS_NAME };
Sawyer::Container::Map<Sawyer::Attribute::Id, Sawyer::Container::Set<WarningType> > wasEmitted_;
static bool shouldEmit(Sawyer::Attribute::Id id, WarningType warning) {
    return wasEmitted_.insertMaybeDefault(id).insert(warning);
}

static std::string
attributeFullName(AstAttribute *value, Sawyer::Attribute::Id id) {
    std::string retval;

    if (value == NULL) {
        retval = "null attribute";
    } else if (id == Sawyer::Attribute::INVALID_ID) {
        retval = "invalid attribute";
    } else {
        retval = "attribute \"" + StringUtility::cEscape(Sawyer::Attribute::name(id)) + "\"";
    }

    if (value != NULL) {
        std::string className = value->attribute_class_name();
        if (className.compare("AstAttribute") != 0) {
            retval += " of type " + StringUtility::cEscape(className);
        } else {
            retval += " of unknown type (AstAttribute::attribute_class_name not overridden)";
        }
    }
    return retval;
}

static void
deleteAttributeValue(AstAttribute *value, Sawyer::Attribute::Id id) {
    if (value != NULL) {
        switch (value->getOwnershipPolicy()) {
            case AstAttribute::CONTAINER_OWNERSHIP:
                delete value;
                break;
            case AstAttribute::NO_OWNERSHIP:
                if (shouldEmit(id, HAS_MEMORY_LEAK))
                    mlog[WARN] <<attributeFullName(value, id) <<" is leaked\n";
                break;
            case AstAttribute::CUSTOM_OWNERSHIP:
                // assume ownership is properly implemented by the subclass
                break;
            case AstAttribute::UNKNOWN_OWNERSHIP: {
                if (shouldEmit(id, HAS_UNKNOWN_OWNERSHIP)) {
                    mlog[WARN] <<attributeFullName(value, id) <<" ownership is unknown and possibly leaked\n";

                    // Show details about how to fix this only once per program.
                    static bool detailsShown = false;
                    if (!detailsShown) {
                        mlog[WARN] <<"    This attribute's class should include a definition for the virtual\n"
                                   <<"    \"getOwnershipPolicy\" based on the intention the author had for how\n"
                                   <<"    memory is managed for this attribute type.  If the author intended\n"
                                   <<"    attribute memory to be managed by ROSE's AstAttributeMechanism, then\n"
                                   <<"    the following definition should be added to the attribute's class:\n"
                                   <<"        virtual AstAttribute::OwnershipPolicy\n"
                                   <<"        getOwnershipPolicy() const ROSE_OVERRIDE {\n"
                                   <<"            return CONTAINER_OWNERSHIP;\n"
                                   <<"        }\n"
                                   <<"    and the attribute should not be explicitly deleted by the attribute\n"
                                   <<"    creator or any users of the attribute. See documentation for\n"
                                   <<"    AstAttribute::getOwnershipPolicy for details.\n";
                        detailsShown = true;
                    }
                }
                break;
            }
        }
    }
}

AstAttributeMechanism&
AstAttributeMechanism::operator=(const AstAttributeMechanism &other) {
    assignFrom(other);
    return *this;
}

AstAttributeMechanism::~AstAttributeMechanism() {
    BOOST_FOREACH (Sawyer::Attribute::Id id, attributes_.attributeIds())
        deleteAttributeValue(attributes_.attributeOrElse<AstAttribute*>(id, NULL), id);
}

bool
AstAttributeMechanism::exists(const std::string &name) const {
    Sawyer::Attribute::Id id = Sawyer::Attribute::id(name);
    if (Sawyer::Attribute::INVALID_ID == id)
        return false;
    return attributes_.attributeExists(id);
}

void
AstAttributeMechanism::set(const std::string &name, AstAttribute *newValue) {
    Sawyer::Attribute::Id id = Sawyer::Attribute::id(name);
    if (Sawyer::Attribute::INVALID_ID == id)
        id = Sawyer::Attribute::declare(name);
    AstAttribute *oldValue = attributes_.attributeOrElse<AstAttribute*>(id, NULL);
    if (newValue != oldValue)
        deleteAttributeValue(oldValue, id);
    if (NULL == newValue) {
        attributes_.eraseAttribute(id);
    } else {
        attributes_.setAttribute(id, newValue);
    }
}

// insert if not already existing
bool
AstAttributeMechanism::add(const std::string &name, AstAttribute *value) {
    if (!exists(name)) {
        set(name, value);
        return true;
    } else {
        deleteAttributeValue(value, Sawyer::Attribute::id(name));
    }
    return false;
}

// insert only if already existing
bool
AstAttributeMechanism::replace(const std::string &name, AstAttribute *value) {
    if (exists(name)) {
        set(name, value);
        return true;
    } else {
        deleteAttributeValue(value, Sawyer::Attribute::id(name));
    }
    return false;
}

AstAttribute*
AstAttributeMechanism::operator[](const std::string &name) const {
    Sawyer::Attribute::Id id = Sawyer::Attribute::id(name);
    if (Sawyer::Attribute::INVALID_ID == id)
        return NULL;
    return attributes_.attributeOrElse<AstAttribute*>(id, NULL);
}

// erase
void
AstAttributeMechanism::remove(const std::string &name) {
    Sawyer::Attribute::Id id = Sawyer::Attribute::id(name);
    if (Sawyer::Attribute::INVALID_ID != id) {
        AstAttribute *oldValue = attributes_.attributeOrElse<AstAttribute*>(id, NULL);
        attributes_.eraseAttribute(id);                 // do this first in case deleteAttributeValue throws
        deleteAttributeValue(oldValue, id);
    }
}

// get attribute names
AstAttributeMechanism::AttributeIdentifiers
AstAttributeMechanism::getAttributeIdentifiers() const {
    AttributeIdentifiers retval;
    BOOST_FOREACH (Sawyer::Attribute::Id id, attributes_.attributeIds())
        retval.insert(Sawyer::Attribute::name(id));
    return retval;
}

size_t
AstAttributeMechanism::size() const {
    return attributes_.nAttributes();
}

// Construction and assignment. Must be exception-safe.
void
AstAttributeMechanism::assignFrom(const AstAttributeMechanism &other) {
    if (this == &other)
        return;
    AstAttributeMechanism tmp;                          // for exception safety
    BOOST_FOREACH (Sawyer::Attribute::Id id, other.attributes_.attributeIds()) {
        /*!const*/ AstAttribute *attr = other.attributes_.attributeOrElse<AstAttribute*>(id, NULL);
        ASSERT_not_null(attr);

        // Copy the attribute. This might throw, which is why we're using "tmp". If it throws, then we don't ever make it to
        // the std::swap below, and the destination is unchanged and tmp will be cleaned up according to the ownership policy
        // of each attribute. Also, as mentioned in the header file, be sure we invoke the non-const version of "copy" for the
        // sake of subclasses that have not been modified to define the const version.
        AstAttribute *copied = attr->copy();

        // Check various things about the attribute.  Some of these might be important in places other than copying, but we
        // report them here because there's a large amount of user code that doesn't follow the rules and we don't want to be
        // completely obnoxious.
        if (!copied && shouldEmit(id, HAS_NULL_COPY))
            mlog[WARN] <<attributeFullName(attr, id) <<" was not copied; no virtual copy function?\n";
        if (attr->attribute_class_name().compare("AstAttribute") == 0 && shouldEmit(id, HAS_NO_CLASS_NAME)) {
            mlog[WARN] <<"attribute \"" <<StringUtility::cEscape(Sawyer::Attribute::name(id)) <<"\""
                       <<" does not implement attribute_class_name\n";
        }
        switch (attr->getOwnershipPolicy()) {
            case AstAttribute::CONTAINER_OWNERSHIP:
                ASSERT_require2(copied != attr,
                                "virtual copy function for \"" + Sawyer::Attribute::name(id) + "\" did not copy attribute");
                break;
            case AstAttribute::NO_OWNERSHIP:
                // copy() is allowed to return itself; we don't care since we won't ever delete either attribute
                // A leak will be reported instead of deleting the attribute.
                break;
            case AstAttribute::CUSTOM_OWNERSHIP:
                // copy() is allowed to return itself; we don't care since we won't ever delete either attribute
                // Assume attribute ownership is properly implemented without leaking memory.
                break;
            case AstAttribute::UNKNOWN_OWNERSHIP:
                // Similar to CONTAINER_OWNERSHIP but only warn, don't assert.
                if (copied != attr && shouldEmit(id, HAS_SELF_COPY))
                    mlog[WARN] <<attributeFullName(attr, id) <<" virtual copy function did not copy the attribute (unknown ownership)\n";
                break;
        }

        if (copied)
            tmp.attributes_.setAttribute(id, copied);
    }
    std::swap(attributes_, tmp.attributes_);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AstAttribute
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AstAttribute::OwnershipPolicy
AstAttribute::getOwnershipPolicy() const {
    return UNKNOWN_OWNERSHIP;
}

std::string
AstAttribute::toString() {
    return StringUtility::numberToString((void*)(this));
}

int
AstAttribute::packed_size() {
    return 0;
}

char*
AstAttribute::packed_data() {
    return NULL;
}

void
AstAttribute::unpacked_data(int size, char* data) {}

std::string
AstAttribute::additionalNodeOptions() {
    return "";
}

std::vector<AstAttribute::AttributeEdgeInfo>
AstAttribute::additionalEdgeInfo() {
    return std::vector<AttributeEdgeInfo>();
}

std::vector<AstAttribute::AttributeNodeInfo>
AstAttribute::additionalNodeInfo() {
    return std::vector<AttributeNodeInfo>();
}

bool
AstAttribute::commentOutNodeInGraph() {
    return false;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MetricAttribute
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AstAttribute*
MetricAttribute::copy() const {
    return new MetricAttribute(*this);
}

std::string
MetricAttribute::attribute_class_name() const {
    return "MetricAttribute";
}

MetricAttribute&
MetricAttribute::operator+=(const MetricAttribute &other) {
    is_derived_ = true;
    value_ += other.value_;
    return *this;
}

MetricAttribute&
MetricAttribute::operator-=(const MetricAttribute &other) {
    is_derived_ = true;
    value_ -= other.value_;
    return *this;
}

MetricAttribute&
MetricAttribute::operator*=(const MetricAttribute &other) {
    is_derived_ = true;
    value_ *= other.value_;
    return *this;
}

MetricAttribute&
MetricAttribute::operator/=(const MetricAttribute &other) {
    is_derived_ = true;
    value_ /= other.value_;
    return *this;
}

std::string
MetricAttribute::toString() {
    std::ostringstream ostr;
    ostr << value_;
    std::string retval = ostr.str();

    if (is_derived_)
        retval += "**";
    return retval;
}

int
MetricAttribute::packed_size() {
    return toString().size()+1;                         // +1 because of NUL-character
}

char*
MetricAttribute::packed_data() {
    // Reimplemented because old version returned a dangling pointer. [Robb Matzke 2015-11-10]
    std::string str = toString();
    static char buf[256];
    ASSERT_require(str.size() < sizeof buf);            // remember NUL terminator
    memcpy(buf, str.c_str(), str.size()+1);
    return buf;
}

void
MetricAttribute::unpacked_data(int size, char* data) {
    if (size <= 0)
        return;
    // check tail **
    char * head = data;
    char * tail = head + strlen(head) - 1;
    if (*tail=='*')
    is_derived_ = true;

    // retrieve value
    // strtod() is smart enough to skip tab and ignore tailing **
    char * endp;
    value_=strtod(head,&endp);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AstSgNodeListAttribute
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AstSgNodeListAttribute::setNode(SgNode *node, int signedIndex) {
    size_t index = boost::numeric_cast<size_t>(signedIndex);
    while (get().size() <= index) // make sure the element at the specified index is available
        get().push_back(NULL); 
    get()[index] = node;
}

SgNode*
AstSgNodeListAttribute::getNode(int signedIndex) {
    size_t index = boost::numeric_cast<size_t>(signedIndex);
    return (index >= 0 && index < get().size() ? get()[index] : NULL);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AstParameterizedTypeAttribute
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AstParameterizedTypeAttribute::AstParameterizedTypeAttribute(SgNamedType *genericType_)
    : genericType(genericType_) {
    isSgClassType(genericType);
}

bool
AstParameterizedTypeAttribute::argumentsMatch(SgTemplateParameterList *type_arg_list,
                                              std::vector<SgTemplateParameter *> *new_args_ptr) {
    if (type_arg_list == NULL && new_args_ptr == NULL) { // two null argument list? ... then they match.
        return true;
    }
    if (type_arg_list == NULL || new_args_ptr == NULL) { // Only one of the argument lists is null?
        return false;
    }
    ROSE_ASSERT(type_arg_list != NULL && new_args_ptr != NULL);

    SgTemplateParameterPtrList args = type_arg_list -> get_args();
    if (args.size() != new_args_ptr -> size()) {
        return false;
    }
    SgTemplateParameterPtrList::iterator arg_it = args.begin(),
                                         new_arg_it = new_args_ptr -> begin();
    for (; arg_it != args.end(); arg_it++, new_arg_it++) {
        SgType *type1 = (*arg_it) -> get_type(),
               *type2 = (*new_arg_it) -> get_type();
        if (type1 != type2) {
            break;
        }
    }
    return (arg_it == args.end()); // The two argument lists match?
}

SgJavaParameterizedType*
AstParameterizedTypeAttribute::findOrInsertParameterizedType(std::vector<SgTemplateParameter *> *new_args_ptr) {
    //
    // Keep track of parameterized types in a table so as not to duplicate them.
    //
    for (std::list<SgJavaParameterizedType *>::iterator type_it = parameterizedTypes.begin();
         type_it != parameterizedTypes.end();
         type_it++) {
        if (argumentsMatch((*type_it) -> get_type_list(), new_args_ptr)) {
            return (*type_it);
        }
    }

    //
    // This parameterized type does not yet exist. Create it, store it in the table and return it.
    //
    SgClassDeclaration *classDeclaration = isSgClassDeclaration(genericType -> getAssociatedDeclaration());
    ROSE_ASSERT(classDeclaration != NULL);
    SgTemplateParameterList *typeParameterList = (new_args_ptr == NULL ? NULL : new SgTemplateParameterList());
    if (new_args_ptr != NULL) {
        typeParameterList -> set_args(*new_args_ptr);
    }
    SgJavaParameterizedType *parameterized_type = new SgJavaParameterizedType(classDeclaration, genericType, typeParameterList);
    parameterizedTypes.push_front(parameterized_type);

    return parameterized_type;
}
