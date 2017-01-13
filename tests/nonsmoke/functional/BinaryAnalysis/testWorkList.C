#include "rose.h"
#include "WorkLists.h"

// Test that we can push and pop a single item
template<class WList>
static void
test1()
{
    WList list;
    typename WList::value_type item;

    ROSE_ASSERT(list.empty());
    ROSE_ASSERT(0==list.size());

    list.push(1);
    ROSE_ASSERT(!list.empty());
    ROSE_ASSERT(1==list.size());

    item = list.pop();
    ROSE_ASSERT(list.empty());
    ROSE_ASSERT(0==list.size());
    ROSE_ASSERT(1==item);
}

// Test that we can push and pop multiple unique items in the correct order
template<class WList>
static void
test2()
{
    WList list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        typename WList::value_type item(i);
        list.push(item);
        ROSE_ASSERT(i+1==list.size());
    }

    for (size_t i=10; i>0; --i) {
        typename WList::value_type item = list.pop();
        typename WList::value_type expected(i-1);
        ROSE_ASSERT(item==expected);
        ROSE_ASSERT(list.size()==i-1);
    }
}

// Test that we can unshift and shift multiple unique items in the correct order
template<class WList>
static void
test3()
{
    WList list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        typename WList::value_type item(i);
        list.unshift(item);
        ROSE_ASSERT(i+1==list.size());
    }

    for (size_t i=10; i>0; --i) {
        typename WList::value_type item = list.shift();
        typename WList::value_type expected(i-1);
        ROSE_ASSERT(item==expected);
        ROSE_ASSERT(list.size()==i-1);
    }
}

// Test that we can use the list as a queue with unique items
template<class WList>
static void
test4()
{
    WList list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        typename WList::value_type item(i);
        list.push(item);
        ROSE_ASSERT(i+1==list.size());
    }

    for (size_t i=0; i<nitems; ++i) {
        typename WList::value_type item = list.shift();
        typename WList::value_type expected(i);
        ROSE_ASSERT(item==expected);
        ROSE_ASSERT(list.size()==9-i);
    }
}

// Test that size works with duplicate items disallowed
template<class WList>
static void
test5()
{
    WList list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        typename WList::value_type item(111);
        list.push(item, true);
        ROSE_ASSERT(1==list.size());
    }

    typename WList::value_type item = list.pop();
    typename WList::value_type expected(111);
    ROSE_ASSERT(item==expected);
    ROSE_ASSERT(list.empty());
}

// Test that size works with duplicate items allowed
template<class WList>
static void
test6()
{
    WList list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        typename WList::value_type item(111);
        list.push(item, false);
        ROSE_ASSERT(i+1==list.size());
    }

    for (size_t i=0; i<nitems; ++i) {
        typename WList::value_type item = list.shift();
        typename WList::value_type expected(111);
        ROSE_ASSERT(item==expected);
        ROSE_ASSERT(list.size()==9-i);
    }
}

#define TEST(TESTNAME) do {                                                                                                    \
    TESTNAME<WorkListUnique<int> >();                                                                                          \
    TESTNAME<WorkListUnique<short> >();                                                                                        \
    TESTNAME<WorkListUnique<unsigned short> >();                                                                               \
    TESTNAME<WorkListUnique<unsigned> >();                                                                                     \
    TESTNAME<WorkListUnique<uint64_t> >();                                                                                     \
                                                                                                                               \
    TESTNAME<WorkListNonUnique<int> >();                                                                                       \
    TESTNAME<WorkListNonUnique<short> >();                                                                                     \
    TESTNAME<WorkListNonUnique<unsigned short> >();                                                                            \
    TESTNAME<WorkListNonUnique<unsigned> >();                                                                                  \
    TESTNAME<WorkListNonUnique<uint64_t> >();                                                                                  \
} while (0);

int
main()
{
    TEST(test1);
    TEST(test2);
    TEST(test3);
    TEST(test4);
    TEST(test5);
    TEST(test6);

    return 0;
}
