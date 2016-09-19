// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Unit tests for the DenseIntegerSet container
#include <Sawyer/DenseIntegerSet.h>

using namespace Sawyer::Container;

template<class T>
T sum(const DenseIntegerSet<T> &set) {
    T retval = 0;
    BOOST_FOREACH (T val, set.values())
        retval += val;
    return retval;
}

static void
testConstructors() {
    DenseIntegerSet<int> s1;
    ASSERT_always_require(s1.isEmpty());
    ASSERT_always_require(s1.domain().isEmpty());

    Interval<unsigned> i2 = Interval<unsigned>::hull(10, 20);
    DenseIntegerSet<unsigned> s2(i2);
    ASSERT_always_require(s2.isEmpty());
    ASSERT_always_require(s2.domain() == i2);
    ASSERT_always_require(!s2.isStorable(9));
    ASSERT_always_require(s2.isStorable(10));
    ASSERT_always_require(s2.isStorable(20));
    ASSERT_always_require(!s2.isStorable(21));

    DenseIntegerSet<unsigned> s3(i2.least(), i2.greatest());
    ASSERT_always_require(s3.isEmpty());
    ASSERT_always_require(s3.domain() == i2);

    DenseIntegerSet<unsigned char> s4(0, 255);
    ASSERT_always_require(s4.isEmpty());
    Interval<unsigned char> i4 = s4.domain();
    ASSERT_always_require(!i4.isEmpty());
    ASSERT_always_require(i4.isWhole());
}
    
static void
testInsertErase() {
    Interval<int> domain = Interval<int>::hull(-10, 50);
    DenseIntegerSet<int> set(domain);
    ASSERT_always_require(set.domain() == domain);

    ASSERT_always_require2(set.isEmpty(), "a just-constructed set is empty");
    ASSERT_always_require2(set.size() == 0, "a default-constructed set has no members");
    ASSERT_always_require2(!set.exists(0), "member zero has not been inserted yet");
    ASSERT_always_require2(!set.exists(1), "member one has not been inserted yet");
    ASSERT_always_require2(!set.exists(2), "member two has not been inserted yet");

    bool inserted = set.insert(0);
    ASSERT_always_require2(inserted, "member should have been inserted");
    ASSERT_always_require2(!set.isEmpty(), "a singleton set is not empty");
    ASSERT_always_require2(set.size() == 1, "a singleton set has one member");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(!set.exists(1), "member one has not been inserted yet");
    ASSERT_always_require2(!set.exists(2), "member two has not been inserted yet");

    inserted = set.insert(0); // again
    ASSERT_always_require2(!inserted, "member should have already existed");
    ASSERT_always_require2(!set.isEmpty(), "a singleton set is not empty");
    ASSERT_always_require2(set.size() == 1, "a singleton set has one member");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(!set.exists(1), "member one has not been inserted yet");
    ASSERT_always_require2(!set.exists(2), "member two has not been inserted yet");

    inserted = set.insert(2);
    ASSERT_always_require2(inserted, "member should have been inserted");
    ASSERT_always_require2(!set.isEmpty(), "a two-member set is not empty");
    ASSERT_always_require2(set.size() == 2, "a two-member set has two members");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(!set.exists(1), "member one has not been inserted yet");
    ASSERT_always_require2(set.exists(2), "member two has been inserted");
    
    inserted = set.insert(1);
    ASSERT_always_require2(inserted, "member should have been inserted");
    ASSERT_always_require2(!set.isEmpty(), "a three-member set is not empty");
    ASSERT_always_require2(set.size() == 3, "a three-member set has three members");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(set.exists(1), "member one has been inserted");
    ASSERT_always_require2(set.exists(2), "member two has been inserted");
    
    bool erased = set.erase(2);
    ASSERT_always_require2(erased, "member should have been erased");
    ASSERT_always_require2(!set.isEmpty(), "a two-member set is not empty");
    ASSERT_always_require2(set.size() == 2, "a two-member set has three members");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(set.exists(1), "member one has been inserted");
    ASSERT_always_require2(!set.exists(2), "member two has been erased");
    
    erased = set.erase(2); // again
    ASSERT_always_require2(!erased, "member has already been erased");
    ASSERT_always_require2(!set.isEmpty(), "a two-member set is not empty");
    ASSERT_always_require2(set.size() == 2, "a two-member set has three members");
    ASSERT_always_require2(set.exists(0), "member zero has been inserted");
    ASSERT_always_require2(set.exists(1), "member one has been inserted");
    ASSERT_always_require2(!set.exists(2), "member two has been erased");

    set.insertAll();
    ASSERT_always_require2(!set.isEmpty(), "a set containing all values is not empty");
    ASSERT_always_require2(set.size() == (size_t)domain.size(), "not all values inserted");

    set.clear();
    ASSERT_always_require2(set.isEmpty(), "a cleared set is empty");
    ASSERT_always_require2(set.size() == 0, "a cleared set has no members");
    ASSERT_always_require2(!set.exists(0), "member zero has been cleared");
    ASSERT_always_require2(!set.exists(1), "member one has been cleared");
    ASSERT_always_require2(!set.exists(2), "member two has been cleared");

    // Inserting a value that's not storable should result in an exception
    try {
        set.insert(set.domain().greatest()+1);
        ASSERT_not_reachable("insert outside domain should have failed");
    } catch (const Sawyer::Exception::DomainError&) {
    } catch (...) {
        ASSERT_not_reachable("wrong kind of exception thrown");
    }

    // Querying existence of a value outside the domain should return false
    ASSERT_always_require(!set.exists(set.domain().greatest()+1));
}

static void
testIterators() {
    DenseIntegerSet<int> s1(-10, 10);                   // constructor is already tested
    s1.insert(-9);                                      // insert and size are already tested
    s1.insert(0);
    s1.insert(2);
    s1.insert(10);
    s1.erase(2);

    DenseIntegerSet<int>::ConstIterator i1 = s1.values().begin();
    DenseIntegerSet<int>::ConstIterator i2 = s1.values().end();

    ASSERT_always_require(i1 == i1);
    ASSERT_always_require(!(i1 != i1));
    ASSERT_always_require(!(i1 < i1));

    ASSERT_always_require(i2 == i2);
    ASSERT_always_require(!(i2 != i2));
    ASSERT_always_require(!(i2 < i2));

    ASSERT_always_require(!(i1 == i2));
    ASSERT_always_require(i1 != i2);
    ASSERT_always_require(i1 < i2 || i2 < i1);

    std::vector<bool> present(21, false);
    present[10 -9] = true;
    present[10 +0] = true;
    present[10 +10] = true;

    // First iterated item
    int val = *i1;
    ASSERT_always_require(-9 == val || 0 == val || 10 == val);
    present[10 + val] = false;

    // Second iterated item
    DenseIntegerSet<int>::ConstIterator i3 = ++i1;
    ASSERT_always_require(i3 == i1);
    ASSERT_always_require(i3 != i2);
    val = *i3;
    ASSERT_always_require(-9 == val || 0 == val || 10 == val);
    ASSERT_always_require(present[10 + val]);
    present[10 + val] = false;

    // Post increment
    DenseIntegerSet<int>::ConstIterator i1b = i1++;
    ASSERT_require(i1b != i1);
    i1b++;
    ASSERT_require(i1b == i1);

    // Third iterated item
    DenseIntegerSet<int>::ConstIterator i4(i1);         // copy constructor test
    ASSERT_always_require(i4 == i1);
    ASSERT_always_require(i4 != i2);
    ASSERT_always_require(i4 != i3);
    val = *i4;
    ASSERT_always_require(-9 == val || 0 == val || 10 == val);
    ASSERT_always_require(present[10 + val]);
    present[10 + val] = false;

#if 0 // [Robb Matzke 2016-03-10]
    // Dereferencing after erasing. This should result in a failed assertion (which is why this test is commented out). The
    // assertion will say something like "dereferencing erased member".
    s1.erase(val);
    *i4;
#endif

    // End iterator
    DenseIntegerSet<int>::ConstIterator i5 = ++i1;
    ASSERT_always_require(i5 == i1);
    ASSERT_always_require(i5 == i2);
    ASSERT_always_require(i5 != i3);
    ASSERT_always_require(i5 != i4);

    // Make sure BOOST_FOREACH works (and therefore C++11 "for" should also eventually work)
    int sum = 0;
    BOOST_FOREACH (int i, s1.values())
        sum += i;
    ASSERT_always_require(1 == sum);                    // -9 + 0 + 10
}

static void
testAssignment() {
    DenseIntegerSet<int> s1(-10, 10);
    s1.insert(-9);
    s1.insert(0);
    s1.insert(10);
    ASSERT_always_require(sum(s1) == 1);

    // Default c'tor
    DenseIntegerSet<int> s2(s1);
    ASSERT_always_require(s2.domain() == s1.domain());
    ASSERT_always_require(s2.size() == s1.size());
    ASSERT_always_require(sum(s2) == 1);

    // Assignment
    Interval<int> i3 = Interval<int>::hull(-9, 10);
    DenseIntegerSet<int> s3(i3);
    s3 = s1;                                            // all values fit
    ASSERT_always_require(s3.size() == s1.size());
    ASSERT_always_require(s3.domain() == i3);           // assignment doesn't change the domain
    ASSERT_always_require(sum(s3) == 1);

    // Assigment when a value cannot be copied must be exception-safe.
    Interval<int> i4 = Interval<int>::hull(-5, 5);
    DenseIntegerSet<int> s4(i4);
    s4.insert(-5);
    s4.insert(-1);
    s4.insert(5);
    try {
        s4 = s1;
        ASSERT_not_reachable("copying should have failed but didn't");
    } catch (const Sawyer::Exception::DomainError &) {
    } catch (...) {
        ASSERT_not_reachable("wrong type of exception thrown");
    }
    ASSERT_require(s4.size() == 3);
    ASSERT_require(sum(s4) == -1);
}

static void
testTheoryOperators() {
    DenseIntegerSet<int> s1(1, 20);;
    s1.insert(2);
    s1.insert(3);
    s1.insert(5);
    s1.insert(7);
    ASSERT_always_require(s1.size()==4);                // {2, 3, 5, 7}

    DenseIntegerSet<int> s2(2, 19);
    s2 = s1;
    s2.erase(3);
    s2.erase(5);
    s2.erase(7);
    s2.insert(6);
    s2.insert(12);
    ASSERT_always_require(s2.size()==3);                // {2, 6, 12}

    //---------------
    // intersection
    //---------------

    DenseIntegerSet<int> s3(-1, 18);
    s3 = s1;
    s3 &= s2;
    ASSERT_always_require(s3.size()==1);                // {2}

    s3 = s2;
    s3 &= s1;
    ASSERT_always_require(s3.size()==1);                // {2}

    DenseIntegerSet<int> empty;
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
    testConstructors();
    testInsertErase();
    testIterators();
    testAssignment();
    testTheoryOperators();
}
