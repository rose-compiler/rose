// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Attribute.h>

#define check(X) ASSERT_always_require(X)

using namespace Sawyer;

static void
attributeIdTests() {
    // Declare an attribrute
    Attribute::Id i1 = Attribute::declare("one");
    check(i1 != Attribute::INVALID_ID);

    // Declaring existing attribute throws
    try {
        (void) Attribute::declare("one");
        ASSERT_not_reachable("should have thrown");
    } catch (const Attribute::AlreadyExists &e) {
    }

    // Lookup existing attribute
    Attribute::Id i2 = Attribute::id("one");
    check(i2 == i1);

    // Lookup non-existing attribute
    Attribute::Id i3 = Attribute::id("non-existing");
    check(i3 == Attribute::INVALID_ID);

    // Lookup name by existing ID
    std::string s1 = Attribute::name(i1);
    check(s1 == "one");

    // Lookup name by non-existing ID
    Attribute::Id i4 = 100;
    check(i4 != i1);
    std::string s2 = Attribute::name(i4);
    check(s2 == "");

    std::string s3 = Attribute::name(Attribute::INVALID_ID);
    check(s3 == "");
}

template<typename SyncTag>
class Simple: public Attribute::Storage<SyncTag> {
public:
};

template<typename SyncTag>
static void
defaultConstructorTests() {
    Simple<SyncTag> foo;

    check(foo.nAttributes() == 0);
}

template<typename SyncTag>
static void
insertEraseTests() {
    Simple<SyncTag> mo;
    const Simple<SyncTag> &co = mo;

    Attribute::Id integer = Attribute::id("integer");
    check(integer != Attribute::INVALID_ID);
    Attribute::Id real = Attribute::id("real");
    check(real != Attribute::INVALID_ID);

    // Insert an attribute
    mo.setAttribute(integer, 100);
    check(co.nAttributes() == 1);
    check(co.attributeExists(integer));
    int i1 = co.template getAttribute<int>(integer);
    check(i1 == 100);

    // Re-insert an existing attribute
    mo.setAttribute(integer, 200);
    check(co.nAttributes() == 1);
    int i2 = co.template getAttribute<int>(integer);
    check(i2 == 200);

    // Don't insert an existing attribute
    mo.setAttributeMaybe(integer, 300);
    check(co.nAttributes() == 1);
    int i3 = co.template getAttribute<int>(integer);
    check(i3 == 200);

    // Insert another attribute
    mo.setAttribute(real, 0.125);                       // 0.125 has an exact representation in a double
    check(co.nAttributes() == 2);
    check(co.attributeExists(real));
    double d1 = co.template getAttribute<double>(real);
    check(d1 == 0.125);

    // Erase an attribute
    mo.eraseAttribute(integer);
    check(co.nAttributes() == 1);
    check(!co.attributeExists(integer));
    check(co.attributeExists(real));

    // Erase all attributes
    mo.clearAttributes();
    check(co.nAttributes() == 0);
    check(!co.attributeExists(integer));
    check(!co.attributeExists(real));
}

template<typename SyncTag>
static void
queryTests() {
    Simple<SyncTag> mo;
    const Simple<SyncTag> &co = mo;

    // Setup
    Attribute::Id integer = Attribute::id("integer");
    check(integer != Attribute::INVALID_ID);
    Attribute::Id integer2 = Attribute::id("integer2");
    check(integer2 != Attribute::INVALID_ID);

    mo.setAttribute(integer, 100);
    check(co.nAttributes() == 1);
    check(co.attributeExists(integer));
    check(100 == co.template getAttribute<int>(integer));

    // attributeOrElse
    int i1 = co.attributeOrElse(integer, 200);
    check(i1 == 100);
    int i2 = co.attributeOrElse(integer2, 200);
    check(i2 == 200);
}

int
main() {
    attributeIdTests();

    defaultConstructorTests<Sawyer::SingleThreadedTag>();
    defaultConstructorTests<Sawyer::MultiThreadedTag>();

    Attribute::Id i1 = Attribute::declare("integer");
    check(i1 != Attribute::INVALID_ID);
    Attribute::Id i2 = Attribute::declare("integer2");
    check(i2 != Attribute::INVALID_ID);
    Attribute::Id i3 = Attribute::declare("real");
    check(i3 != Attribute::INVALID_ID);

    insertEraseTests<Sawyer::SingleThreadedTag>();
    insertEraseTests<Sawyer::MultiThreadedTag>();

    queryTests<Sawyer::SingleThreadedTag>();
    queryTests<Sawyer::MultiThreadedTag>();
}
