// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Set.h>

using namespace Sawyer::Container;

static void
testInsertErase() {
    Set<int> set;
    ASSERT_always_require2(set.isEmpty(), "a default-constructed set is empty");
    ASSERT_always_require2(set.size() == 0, "a default-constructed set has no members");
    ASSERT_always_require2(set.hull().isEmpty(), "a default-constructed set is empty");
    ASSERT_always_require2(!set.exists(0), "member zero has not been inserted yet");
    ASSERT_always_require2(!set.exists(1), "member one has not been inserted yet");
    ASSERT_always_require2(!set.exists(2), "member two has not been inserted yet");

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

    //-------
    // union
    //-------

    s3 = s1;
    s3 |= s2;
    ASSERT_always_require(s3.size()==6);                // {2, 3, 5, 6, 7, 12}

    s3 = s2;
    s3 |= s1;
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
}

int
main() {
    testInsertErase();
    testTheoryOperators();
}
