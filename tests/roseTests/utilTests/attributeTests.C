// Unit tests for AstAttributeMechanism

#include <rose.h>
#include <sstream>

// Counts how many instances of class T are currently allocated.
template<class T>
class AllocationCounter {
public:
    static int nAllocated;
    static void* operator new(size_t size) {
        ++nAllocated;
        return ::operator new(size);
    }
    static void operator delete(void *ptr, size_t size) {
        if (ptr)
            --nAllocated;
        ::operator delete (ptr);
    }
};

template<class T>
int AllocationCounter<T>::nAllocated = 0;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Make sure that allocation count works correctly.

class AllocTest: public AllocationCounter<AllocTest> {
public:
    int x;

    AllocTest(): x(0) {}
    explicit AllocTest(int x): x(x) {}
};

static void
test_allocation_counter() {
    std::ostringstream unused;

    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 0, "initial allocation count is zero");
    {
        AllocTest o1;
        ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 0, "'new' is not called by constructors");
        unused <<o1.x;
    }
    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 0, "'delete' is not called by destructors");
    
    AllocTest *o2 = new AllocTest;
    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 1, "'new' should have incremented allocation count");

    AllocTest o3(*o2);
    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 1, "copy constructor should not have changed count");

    *o2 = o3;
    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 1, "assignment should not have changed count");
    o3 = *o2;
    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 1, "assignment should not have changed count");

    AllocTest *o4 = new AllocTest;
    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 2, "'new' should have incremented count");

    delete o4;
    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 1, "'delete' should have decremented count");

    delete o2;
    ASSERT_always_require2(AllocationCounter<AllocTest>::nAllocated == 0, "'delete' should have decremented count");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Verify that all operations compile and that non-modifying operations can be applied to const containers.

class Attr1: public AstAttribute {
public:
    virtual AstAttribute* copy() const ROSE_OVERRIDE {
        return new Attr1(*this);
    }

    virtual std::string attribute_class_name() const ROSE_OVERRIDE {
        return "Attr1";
    }
};

// This function is not intended to be called.
void
compile_tests(const AstAttributeMechanism &c, AstAttributeMechanism &m) {
    std::ostringstream unused;                          // to defeat "unused variable" warnings

    // Default constructor
    AstAttributeMechanism a1;

    // Copy constructor
    AstAttributeMechanism a2(c);
    AstAttributeMechanism a3(m);

    // Assignment
    a1 = c;
    a1 = m;

    // Other methods
    unused <<c.exists("x");
    unused <<m.add("x", new Attr1);
    unused <<m.replace("x", new Attr1);
    m.set("x", new Attr1);
    unused <<c["x"];
    m.remove("x");
    unused <<c.getAttributeIdentifiers().size();
    unused <<c.size();

    unused <<a1.size();
    unused <<a2.size();
    unused <<a3.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Verify that the container deletes its attributes.

class Attr2: public AstAttribute, public AllocationCounter<Attr2> {
public:
    virtual AstAttribute* copy() const ROSE_OVERRIDE {
        return new Attr2(*this);
    }

    virtual std::string attribute_class_name() const ROSE_OVERRIDE {
        return "Attr2";
    }
};

static void
test_value_deletion() {
    AstAttributeMechanism a;

    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 0, "initial count");

    ASSERT_always_require2(a.exists("x") == false, "x is not stored yet");
    Attr2 *v1 = dynamic_cast<Attr2*>(a["x"]);
    ASSERT_always_require2(v1 == NULL, "no value has been stored yet");
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 0, "no value allocated yet");

    v1 = new Attr2;
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 1, "one value allocated for storage");
    
    a.set("x", v1);
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 1, "ownership should have transfered to container");

    (void) a["x"];
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 1, "container should retain ownership");
    ASSERT_always_require2(a["x"] == v1, "value should have been stored");

    Attr2 *v2 = new Attr2;
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 2, "second value is allocated by user");
    bool wasAdded = a.add("x", v2);
    ASSERT_always_require2(wasAdded == false, "not inserted because value by that name already exists");
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 1, "ownership should have transfered to container");
    ASSERT_always_require2(a["x"] == v1, "v1 should remain in storage");

    v2 = new Attr2;
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 2, "object created by user");
    wasAdded = a.replace("y", v2);
    ASSERT_always_require2(wasAdded == false, "not inserted because value by that name does not exist");
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 1, "ownership should have transfered to container");
    ASSERT_always_require(a.exists("y") == false);

    v2 = new Attr2;
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 2, "object created by user");
    a.set("x", v2);
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 1, "ownership should have transfered to container");
    ASSERT_always_require2(a["x"] == v2, "v2 should have been stored");
    
    a.remove("x");
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 0, "all values deleted");
}

static void
test_container_destruction() {
    {
        AstAttributeMechanism a;
        ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 0, "initial count");
        a.set("x", new Attr2);
        ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 1, "one value inserted");
    }
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 0, "container destroyed");
}

static void
test_container_copying() {
    {
        AstAttributeMechanism a;
        ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 0, "initial count");
        a.set("x", new Attr2);
        ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 1, "one value inserted");

        AstAttributeMechanism b(a);
        ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 2, "container copy-constructed");

        AstAttributeMechanism c;
        ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 2, "no change");
        c = a;
        ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 3, "assignment to empty container");
        c = b;
        ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 3, "old value deleted during assignment");
    }
    ASSERT_always_require2(AllocationCounter<Attr2>::nAllocated == 0, "values deleted when containers destroyed");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test what happens when attributes don't define a virtual copy constructor.

class Attr3: public AstAttribute, public AllocationCounter<Attr3> {
public:
    // no virtual copy constructor

    virtual std::string attribute_class_name() const ROSE_OVERRIDE {
        return "Attr3";
    }
};

static void
test_missing_copy() {
    {
        AstAttributeMechanism a;
        ASSERT_always_require2(AllocationCounter<Attr3>::nAllocated == 0, "initial count");

        a.set("x", new Attr3);
        ASSERT_always_require2(AllocationCounter<Attr3>::nAllocated == 1, "one value inserted");
        ASSERT_always_require(a.exists("x"));

        AstAttributeMechanism b(a);
        ASSERT_always_require2(!b.exists("x"), "value lacking copy constructor should not have been copied");
        ASSERT_always_require2(AllocationCounter<Attr3>::nAllocated == 1, "one value inserted");
    }
    ASSERT_always_require2(AllocationCounter<Attr3>::nAllocated == 0, "containers destroyed");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test what happens if a container's virtual copy constructor doesn't make a copy

class Attr4: public AstAttribute, public AllocationCounter<Attr4> {
public:
    // incorrect virtual copy constructor
    virtual AstAttribute* copy() const ROSE_OVERRIDE {
        return const_cast<Attr4*>(this);
    }

    virtual std::string attribute_class_name() const ROSE_OVERRIDE {
        return "Attr4";
    }
};

static void
test_self_copy() {
    {
        AstAttributeMechanism a;
        ASSERT_always_require2(AllocationCounter<Attr4>::nAllocated == 0, "initial count");

        a.set("x", new Attr4);
        ASSERT_always_require2(AllocationCounter<Attr4>::nAllocated == 1, "one value inserted");
        ASSERT_always_require(a.exists("x"));

#if 0 // [Robb Matzke 2015-11-11]: a self-returning copy constructor is caught by the library now
        AstAttributeMechanism b(a);
        ASSERT_always_require2(!a.exists("x"), "value lacking proper copy constructor should not have been copied");
        ASSERT_always_require2(AllocationCounter<Attr4>::nAllocated == 1, "one value inserted");
#endif
    }
    ASSERT_always_require2(AllocationCounter<Attr4>::nAllocated == 0, "containers destroyed");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test that assignment is exception-safe by having a virtual copy constructor throw an exception part way through copying
// the attribute container.

class Attr5: public AstAttribute, public AllocationCounter<Attr5> {
public:
    bool shouldThrow;

    explicit Attr5(bool shouldThrow)
        : shouldThrow(shouldThrow) {}

    virtual AstAttribute* copy() const ROSE_OVERRIDE {
        if (shouldThrow)
            throw std::runtime_error("virtual copy constructor failed");
        return new Attr5(*this);
    }

    virtual std::string attribute_class_name() const ROSE_OVERRIDE {
        return "Attr5";
    }
};

static void
test_exception_safety() {
    {
        AstAttributeMechanism a;
        ASSERT_always_require2(AllocationCounter<Attr5>::nAllocated == 0, "initial count");

        a.set("x", new Attr5(false));
        a.set("y", new Attr5(true));
        a.set("z", new Attr5(false));
        ASSERT_always_require2(AllocationCounter<Attr5>::nAllocated == 3, "three values inserted");

        AstAttributeMechanism b;
        Attr5 *v1 = new Attr5(false);
        b.set("v1", v1);
        ASSERT_always_require2(AllocationCounter<Attr5>::nAllocated == 4, "inserted value into destination container");

        try {
            b = a;
            ASSERT_not_reachable("y's copy constructor should have thrown an exception");
        } catch (const std::runtime_error &e) {
        }
        ASSERT_always_require2(b["v1"] == v1, "destination container should not have been modified");
        ASSERT_always_require2(b.size() == 1, "destination container should not have been modified");
        ASSERT_always_require2(a.size() == 3, "source container should not have been modified");
        ASSERT_always_require2(AllocationCounter<Attr5>::nAllocated == 4, "assignment cleaned up extra copies");
    }
    ASSERT_always_require2(AllocationCounter<Attr5>::nAllocated == 0, "containers destroyed");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main() {
    test_allocation_counter();
    test_value_deletion();
    test_container_destruction();
    test_container_copying();
    test_missing_copy();
    test_self_copy();
    test_exception_safety();
}
