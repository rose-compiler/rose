#include "sage3basic.h"
#include "AstAttributeMechanism.h"

#include "roseInternal.h"
#include <boost/foreach.hpp>
#include <sstream>

using namespace rose;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AstAttributeMechanism
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AstAttributeMechanism::~AstAttributeMechanism() {
    BOOST_FOREACH (Sawyer::Attribute::Id id, attributes_.attributeIds())
        delete attributes_.getAttribute<AstAttribute*>(id);
}

bool
AstAttributeMechanism::exists(const std::string &name) const {
    Sawyer::Attribute::Id id = Sawyer::Attribute::id(name);
    if (Sawyer::Attribute::INVALID_ID == id)
        return false;
    return attributes_.attributeExists(id);
}

// insert if not already existing
bool
AstAttributeMechanism::add(const std::string &name, AstAttribute *value) {
    if (NULL == value)
        return false;
    Sawyer::Attribute::Id id = Sawyer::Attribute::id(name);
    if (Sawyer::Attribute::INVALID_ID == id)
        id = Sawyer::Attribute::declare(name);
    if (attributes_.attributeExists(id)) {
        delete value;
        return false;
    }
    attributes_.setAttribute(id, value);
    return true;
}

// insert only if already existing
bool
AstAttributeMechanism::replace(const std::string &name, AstAttribute *value) {
    if (NULL == value)
        return false;
    Sawyer::Attribute::Id id = Sawyer::Attribute::id(name);
    AstAttribute *oldValue = NULL;
    if (Sawyer::Attribute::INVALID_ID == id && (oldValue = attributes_.attributeOrElse<AstAttribute*>(id, NULL))) {
        delete oldValue;
        attributes_.setAttribute(id, value);
        return true;
    }
    delete value;
    return false;
}

void
AstAttributeMechanism::set(const std::string &name, AstAttribute *value) {
    Sawyer::Attribute::Id id = Sawyer::Attribute::id(name);
    if (Sawyer::Attribute::INVALID_ID == id)
        id = Sawyer::Attribute::declare(name);
    delete attributes_.attributeOrElse<AstAttribute*>(id, NULL);
    attributes_.setAttribute(id, value);
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
        delete attributes_.attributeOrElse<AstAttribute*>(id, NULL);
        attributes_.eraseAttribute(id);
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
        AstAttribute *attr = other.attributes_.getAttribute<AstAttribute*>(id);
        ASSERT_not_null(attr);
        if (AstAttribute *copied = attr->copy()) { // might throw; returning null means don't copy.
            ASSERT_forbid2(attr == copied, "copy attribute \'" + Sawyer::Attribute::name(id) + "\' returned itself");
            tmp.attributes_.setAttribute(id, copied);
        }
    }
    std::swap(attributes_, tmp.attributes_);
    // tmp's destructor will delete its attributes: either the partly copied list if exception, or the originals
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AstAttribute
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
