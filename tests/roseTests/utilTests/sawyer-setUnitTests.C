// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Set.h>

using namespace Sawyer::Container;

static void
default_ctor() {
    Set<int> set;
    ASSERT_always_require2(set.isEmpty(), "a default-constructed set is empty");
    ASSERT_always_require2(set.size() == 0, "a default-constructed set has no members");
    ASSERT_always_require2(set.hull().isEmpty(), "a default-constructed set is empty");
    ASSERT_always_require2(!set.exists(0), "member zero has not been inserted yet");
    ASSERT_always_require2(!set.exists(1), "member one has not been inserted yet");
    ASSERT_always_require2(!set.exists(2), "member two has not been inserted yet");
}

static void
scalar_ctor() {
    Set<int> set(100);
    ASSERT_always_require(set.size() == 1);
    ASSERT_always_require(set.hull().least() == 100);
}

static void
testInsertErase() {
    Set<int> set;
    bool inserted = set.insert(0);
    ASSERT_always_require2(inserted, "member should have been inserted");
    ASSERT_always_require2(!set.isEmpty(), "a singleton set is not empty");
    ASSERT_always_require2(set.size() == 1, "a singleton set has one member");
    ASSERT_always_require2(set.hull().size() == 1, "singleton set has a singleton hull");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(!set.exists(1), "member one has not been inserted yet");
    ASSERT_always_require2(!set.exists(2), "member two has not been inserted yet");

    inserted = set.insert(0); // again
    ASSERT_always_require2(!inserted, "member should have already existed");
    ASSERT_always_require2(!set.isEmpty(), "a singleton set is not empty");
    ASSERT_always_require2(set.size() == 1, "a singleton set has one member");
    ASSERT_always_require2(set.hull().size() == 1, "singleton set has a singleton hull");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(!set.exists(1), "member one has not been inserted yet");
    ASSERT_always_require2(!set.exists(2), "member two has not been inserted yet");

    inserted = set.insert(1);
    ASSERT_always_require2(inserted, "member should have been inserted");
    ASSERT_always_require2(!set.isEmpty(), "a two-member set is not empty");
    ASSERT_always_require2(set.size() == 2, "a two-member set has two members");
    ASSERT_always_require2(set.hull().size() == 2, "hull of set {0,1} is [0..1]");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(set.exists(1), "member one has been inserted");
    ASSERT_always_require2(!set.exists(2), "member two has not been inserted yet");
    
    inserted = set.insert(2);
    ASSERT_always_require2(inserted, "member should have been inserted");
    ASSERT_always_require2(!set.isEmpty(), "a three-member set is not empty");
    ASSERT_always_require2(set.size() == 3, "a three-member set has three members");
    ASSERT_always_require2(set.hull().size() == 3, "hull of set {0,1,2} is [0..2]");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(set.exists(1), "member one has been inserted");
    ASSERT_always_require2(set.exists(2), "member two has been inserted");
    
    bool erased = set.erase(1);
    ASSERT_always_require2(erased, "member should have been erased");
    ASSERT_always_require2(!set.isEmpty(), "a two-member set is not empty");
    ASSERT_always_require2(set.size() == 2, "a two-member set has three members");
    ASSERT_always_require2(set.hull().size() == 3, "hull of set {0,2} is [0..2]");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(!set.exists(1), "member one has been erased");
    ASSERT_always_require2(set.exists(2), "member two has been inserted");
    
    erased = set.erase(1); // again
    ASSERT_always_require2(!erased, "member has already been erased");
    ASSERT_always_require2(!set.isEmpty(), "a two-member set is not empty");
    ASSERT_always_require2(set.size() == 2, "a two-member set has three members");
    ASSERT_always_require2(set.hull().size() == 3, "hull of set {0,2} is [0..2]");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(!set.exists(1), "member one has been erased");
    ASSERT_always_require2(set.exists(2), "member two has been inserted");

    set.clear();
    ASSERT_always_require2(set.isEmpty(), "a cleared set is empty");
    ASSERT_always_require2(set.size() == 0, "a cleared set has no members");
    ASSERT_always_require2(set.hull().isEmpty(), "a cleared set is empty");
    ASSERT_always_require2(!set.exists(0), "member zero has been cleared");
    ASSERT_always_require2(!set.exists(1), "member one has been cleared");
    ASSERT_always_require2(!set.exists(2), "member two has been cleared");
}

static void
iterator_ctors() {
    std::set<int> s1;
    s1.insert(2);
    s1.insert(3);
    s1.insert(1);
    s1.insert(4);
    s1.insert(0);
    const std::set<int> &cs1 = s1;

    Set<float> d1(s1.begin(), s1.end());
    ASSERT_always_require(d1.size() == s1.size());

    Set<float> d2(cs1.begin(), cs1.end());
    ASSERT_always_require(d2.size() == cs1.size());

    Set<double> d3(d1.values());
    ASSERT_always_require(d3.size() == d1.size());

    const Set<float> &d4 = d2;
    Set<double> d5(d4.values());
    ASSERT_always_require(d5.size() == d4.size());

    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(2);
    v.push_back(1);
    Set<int> d6(v.begin(), v.end());
    ASSERT_always_require(d6.size() == 2);
}

static void
predicates() {
    Set<int> s1;
    s1.insert(1);
    s1.insert(2);
    s1.insert(3);

    Set<int> s2;
    s2.insert(3);
    s2.insert(4);

    Set<int> s3;
    s3.insert(7);
    s3.insert(8);
    s3.insert(9);

    const Set<int> &cs1 = s1;
    const Set<int> &cs2 = s2;
    const Set<int> &cs3 = s3;

    ASSERT_always_require(cs1.existsAny(cs2));
    ASSERT_always_require(cs2.existsAny(cs1));
    ASSERT_always_require(cs1.existsAny(cs1));
    ASSERT_always_require(!cs1.existsAny(cs3));

    ASSERT_always_require(!cs1.existsAll(cs2));
    ASSERT_always_require(!cs2.existsAll(cs1));
    ASSERT_always_require(cs1.existsAll(cs1));

    ASSERT_always_require(cs1 == cs1);
    ASSERT_always_forbid(cs1 == cs2);

    ASSERT_always_require(cs1 != cs2);
    ASSERT_always_forbid(cs1 != cs1);
}


static void
testTheoryOperators() {
    Set<int> s1;
    s1.insert(2);
    s1.insert(3);
    s1.insert(5);
    s1.insert(7);
    ASSERT_always_require(s1.size()==4);                // {2, 3, 5, 7}

    Set<int> s2 = s1;
    s2.erase(3);
    s2.erase(5);
    s2.erase(7);
    s2.insert(6);
    s2.insert(12);
    ASSERT_always_require(s2.size()==3);                // {2, 6, 12}

    //---------------
    // intersection
    //---------------

    Set<int> s3;
    s3 = s1;
    s3 &= s2;
    ASSERT_always_require(s3.size()==1);                // {2}

    s3 = s2;
    s3 &= s1;
    ASSERT_always_require(s3.size()==1);                // {2}

    Set<int> empty;
    s3 &= empty;
    ASSERT_always_require(s3.size()==0);                // {}
    s3 &= s2;
    ASSERT_always_require(s3.size()==0);                // {}

    s3 = s1 & s2;
    ASSERT_always_require(s3.size()==1);                // {2}

    //-------
    // union
    //-------

    s3 = s1;
    s3 |= s2;
    ASSERT_always_require(s3.size()==6);                // {2, 3, 5, 6, 7, 12}

    s3 = s2;
    s3 |= s1;
    ASSERT_always_require(s3.size()==6);                // {2, 3, 5, 6, 7, 12}

    s3 = s1 | s2;
    ASSERT_always_require(s3.size()==6);                // {2, 3, 5, 6, 7, 12}

    //------------
    // difference
    //------------

    s3 = s1;
    s3 -= s2;
    ASSERT_always_require(s3.size()==3);                // {3, 5, 7}

    s3 = s2;
    s3 -= s1;
    ASSERT_always_require(s3.size()==2);                // {6, 12}

    s3 = s1 - s2;
    ASSERT_always_require(s3.size()==3);                // {3, 5, 7}
}

int
main() {
    default_ctor();
    scalar_ctor();
    testInsertErase();
    iterator_ctors();
    predicates();
    testTheoryOperators();
}
