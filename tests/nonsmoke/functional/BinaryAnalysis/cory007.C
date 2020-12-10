// Test that user-defined attributes can be added to symbolic expressions.
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>

#include <BinarySymbolicExpr.h>
#include <Sawyer/Attribute.h>
#include <SageBuilderAsm.h>
#include <sstream>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

struct MyCompoundAttribute {
    int x, y;
};

typedef std::vector<double> MyVectorAttribute;

int
main() {
    // Register a few attributes.  Once attribute IDs are created they never change and are never deleted.  ID numbers are not
    // guaranteed to be sequential, altough the current implementation allocates them that way.
    const Sawyer::Attribute::Id DUCKS     = Sawyer::Attribute::declare("Number of ducks");
    const Sawyer::Attribute::Id TYPE      = Sawyer::Attribute::declare("Pharos type");
    const Sawyer::Attribute::Id POSITION  = Sawyer::Attribute::declare("Position in windowing system");
    const Sawyer::Attribute::Id TENSILES  = Sawyer::Attribute::declare("Tensile strength of each rope");

    // Trying to declare the same attribute again will get you an error
    try {
        Sawyer::Attribute::declare("Number of ducks");
        ASSERT_not_reachable("should have thrown an error when creating a duplicate attribute");
    } catch (const Sawyer::Attribute::AlreadyExists&) {
    }

    // You can query attribute names and ID numbers:
    ASSERT_always_require(Sawyer::Attribute::id("Number of ducks") == DUCKS);
    ASSERT_always_require(Sawyer::Attribute::name(DUCKS) == "Number of ducks");

    // Make a symbolic expression to which we can attach some attributes.
    SymbolicExpr::Ptr a = SymbolicExpr::makeIntegerVariable(32, "a");
    SymbolicExpr::Ptr b = SymbolicExpr::makeIntegerConstant(32, 4);
    SymbolicExpr::Ptr c = SymbolicExpr::makeAdd(a, b);

    // Every expression has a hash value. Save it for comparison later.
    uint64_t hash1 = c->hash();

    // Attach some attributes to parts of the expression
    a->setAttribute(DUCKS, 0);                             // different than not storing an integer
    a->setAttribute(TYPE, (SgType*)NULL);                  // different than not storing a pointer

    MyCompoundAttribute pos1;
    pos1.x = pos1.y = 10;
    b->setAttribute(TYPE, (SgType*)SageBuilderAsm::buildTypeX86DoubleWord());
    b->setAttribute(POSITION, pos1);

    MyVectorAttribute strengths(5, 1.2);
    c->setAttribute(POSITION, pos1);
    c->setAttribute(TENSILES, strengths);

    // Attributes do not affect hashed value.  I.e., they're a little like comments.
    uint64_t hash2 = c->hash();
    ASSERT_always_require(hash1 == hash2);

    // Check whether attributes exist.
    ASSERT_always_require(a->attributeExists(DUCKS));
    ASSERT_always_require(a->attributeExists(TYPE));
    ASSERT_always_forbid(a->attributeExists(POSITION));
    ASSERT_always_forbid(a->attributeExists(TENSILES));

    ASSERT_always_forbid(b->attributeExists(DUCKS));
    ASSERT_always_require(b->attributeExists(TYPE));
    ASSERT_always_require(b->attributeExists(POSITION));
    ASSERT_always_forbid(b->attributeExists(TENSILES));

    ASSERT_always_forbid(c->attributeExists(DUCKS));
    ASSERT_always_forbid(c->attributeExists(TYPE));
    ASSERT_always_require(c->attributeExists(POSITION));
    ASSERT_always_require(c->attributeExists(TENSILES));


    // Check that we can retrieve attributes.  This demos a few different ways to get them.

    // Method 1: getting an attribute when it's known to exist
    ASSERT_always_require(a->getAttribute<int>(DUCKS) == 0);

    try {
        b->getAttribute<int>(DUCKS);
        ASSERT_not_reachable("DUCKS attribute does not exist for object b");
    } catch (const Sawyer::Attribute::DoesNotExist&) {
    }

    // Method 2: getting an attribute or some other value if it does't exist. The type of the second argument must match the
    // value stored in the attribute if the attribute happens to exist, or it throws boost::bad_any_cast.
    MyCompoundAttribute pos2;
    pos2.x = pos2.y = 20;
    ASSERT_always_require(a->attributeOrElse(POSITION, pos2).x == 20); // attribute doesn't exist, so use pos2
    ASSERT_always_require(b->attributeOrElse(POSITION, pos2).x == 10); // attribute exists, so use it instead of pos2

    // Method 3: getting an attribute or a default-constructed value.
    ASSERT_always_require(b->attributeOrDefault<MyVectorAttribute>(TENSILES).size() == 0); // no value stored, use default
    ASSERT_always_require(c->attributeOrDefault<MyVectorAttribute>(TENSILES).size() == 5); // value is stored

    // Method 4: getting an attribute value or nothing as a Sawyer::Optional. This allows to check for existence at the same
    // time as making an assignment.
    int nDucks = 911;
    if (a->optionalAttribute<int>(DUCKS).assignTo(nDucks)) {
        ASSERT_always_require(nDucks == 0);
    } else {
        ASSERT_not_reachable("cannot get here even though DUCKS attribute value is zero");
    }

    // Type conversions are possible only after the getAttribute call returns.
    std::ostringstream ss;
    double realDucks = a->getAttribute<int>(DUCKS);     // normal C++ conversion
    ss <<realDucks;                                     // to avoid compiler warning about not using realDucks

    try {
        realDucks = a->getAttribute<double>(DUCKS);     // cannot do a conversion "on the fly"
        ASSERT_not_reachable("query double property should not have worked");
    } catch (const Sawyer::Attribute::WrongQueryType&) {
    }

    // Attributes can be removed from an object.
    a->eraseAttribute(DUCKS);
    ASSERT_always_forbid(a->attributeExists(DUCKS));
}

#endif
