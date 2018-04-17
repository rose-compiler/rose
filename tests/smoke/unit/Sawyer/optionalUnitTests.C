// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Optional.h>

#include <iostream>
#include <Sawyer/Assert.h>
#include <string>

using namespace Sawyer;

static void
testNothing() {
    const Nothing n1 = Nothing(), n2 = Nothing();

    ASSERT_always_require(n1 == n2);
    ASSERT_always_forbid(n1 != n2);

    ASSERT_always_forbid(n1 > n2);
    ASSERT_always_forbid(n2 > n1);

    ASSERT_always_require(n1 >= n2);
    ASSERT_always_require(n2 >= n1);

    ASSERT_always_forbid(n1 < n2);
    ASSERT_always_forbid(n2 < n1);

    ASSERT_always_require(n1 <= n2);
    ASSERT_always_require(n2 <= n1);
}

// Class that counts how many of its objects are currently instantiated.
static int nCounted = 0;
class Counted {
public:
    Counted() {
        ++nCounted;
    }

    Counted(const Counted&) {
        ++nCounted;
    }

    ~Counted() {
        --nCounted;
    }
};

static void
testConstructDestroy() {
    int originalCount = nCounted;

    Optional<Counted> x;
    ASSERT_always_require(nCounted == originalCount);

    x = Counted();
    ASSERT_always_require(nCounted == originalCount + 1);

    x = Counted();
    ASSERT_always_require(nCounted == originalCount + 1);

    x = Nothing();
    ASSERT_always_require(nCounted == originalCount);
}

static void
testEmptyDeref() {
    Optional<int> x;
    const Optional<int> &cx = x;

    try {
        x.get();
        ASSERT_not_reachable("should have thrown std::domain_error");
    } catch (const std::domain_error&) {
    }
    try {
        cx.get();
        ASSERT_not_reachable("should have thrown std::domain_error");
    } catch (const std::domain_error&) {
    }
}

static void
testOrElse() {
    Optional<int> x;

    int i = 11;
    const int &i2 = x.orElse(i);
    ASSERT_always_require(i2 == i);
    ASSERT_always_require(&i2 == &i);

    const int j = 12;
    const int &j2 = x.orElse(j);
    ASSERT_always_require(j2 == j);
    ASSERT_always_require(&j2 == &j);
}

static void
testEquality() {
    Optional<int> one_a = 1;
    Optional<int> one_b = 1;
    Optional<int> two_a = 2;
    Optional<int> empty_a, empty_b;

    ASSERT_always_require(one_a.isEqual(one_a));
    ASSERT_always_require(one_a.isEqual(one_b));
    ASSERT_always_require(!one_a.isEqual(two_a));
    ASSERT_always_require(!one_a.isEqual(empty_a));

    ASSERT_always_require(!empty_a.isEqual(one_a));
    ASSERT_always_require(empty_a.isEqual(empty_a));
    ASSERT_always_require(empty_a.isEqual(empty_b));

    ASSERT_always_require(one_a.isEqual(1));

    ASSERT_always_require(!one_a.isEqual(Nothing()));
    ASSERT_always_require(empty_a.isEqual(Nothing()));
}

int main() {
    Sawyer::initializeLibrary();

    testNothing();
    testConstructDestroy();
    testEmptyDeref();
    testOrElse();
    testEquality();

    Optional<int> x;
    ASSERT_always_forbid2(x, "default constructed Optional should be false");
    try {
        *x;
        ASSERT_not_reachable("should have thrown \"dereferencing nothing\"");
    } catch (const std::domain_error&) {
    }
    ASSERT_always_require(!x);
    ASSERT_always_require(x.orElse(5)==5);

    x = 0;
    ASSERT_always_require(x);
    ASSERT_always_require(*x==0);
    ASSERT_always_require(x.orElse(5)==0);

    Optional<int> x2 = x;
    ASSERT_always_require(x2);
    ASSERT_always_require(*x2==0);

    x = x2;
    ASSERT_always_require(x);
    ASSERT_always_require(*x==0);

    // x == x;                                             // should be an error
    // x + x;                                              // should be an error
    // int y = x;                                          // should be an error

    // This works, but because of operator<<; nothing we can do about it.
    std::cout <<x <<"\n";

    Optional<std::string> os;
    ASSERT_always_require(!os);
    std::string s = "abc";
    os = s;
    ASSERT_always_require(os);
    ASSERT_always_require(*os == "abc");
}
