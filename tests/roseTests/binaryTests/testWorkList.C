#include "rose.h"
#include "WorkList.h"

// Test that we can push and pop a single item
template<typename T>
static void
test1()
{
    T item;
    WorkList<T> list;


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
template<typename T>
static void
test2()
{
    WorkList<T> list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        T item(i);
        list.push(item);
        ROSE_ASSERT(i+1==list.size());
    }

    for (size_t i=10; i>0; --i) {
        T item = list.pop();
        ROSE_ASSERT(item==T(i-1));
        ROSE_ASSERT(list.size()==i-1);
    }
}

// Test that we can unshift and shift multiple unique items in the correct order
template<typename T>
static void
test3()
{
    WorkList<T> list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        T item(i);
        list.unshift(item);
        ROSE_ASSERT(i+1==list.size());
    }

    for (size_t i=10; i>0; --i) {
        T item = list.shift();
        ROSE_ASSERT(item==T(i-1));
        ROSE_ASSERT(list.size()==i-1);
    }
}

// Test that we can use the list as a queue with unique items
// Test that we can unshift and shift multiple unique items in the correct order
template<typename T>
static void
test4()
{
    WorkList<T> list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        T item(i);
        list.push(item);
        ROSE_ASSERT(i+1==list.size());
    }

    for (size_t i=0; i<nitems; ++i) {
        T item = list.shift();
        ROSE_ASSERT(item==T(i));
        ROSE_ASSERT(list.size()==9-i);
    }
}

// Test that size works with duplicate items disallowed
template<typename T>
static void
test5()
{
    WorkList<T> list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        T item(111);
        list.push(item);
        ROSE_ASSERT(1==list.size());
    }

    T item = list.pop();
    ROSE_ASSERT(T(111)==item);
    ROSE_ASSERT(list.empty());
}

// Test that size works with duplicate items allowed
template<typename T>
static void
test6()
{
    WorkList<T> list;
    static const size_t nitems = 10;
    for (size_t i=0; i<nitems; ++i) {
        T item(111);
        list.push(item, false);
        ROSE_ASSERT(i+1==list.size());
    }

    for (size_t i=0; i<nitems; ++i) {
        T item = list.shift();
        ROSE_ASSERT(T(111)==item);
        ROSE_ASSERT(list.size()==9-i);
    }
}

#define TEST(TESTNAME) do {                                                                                                    \
    TESTNAME<int>();                                                                                                           \
    TESTNAME<short>();                                                                                                         \
    TESTNAME<unsigned short>();                                                                                                \
    TESTNAME<unsigned>();                                                                                                      \
    TESTNAME<uint64_t>();                                                                                                      \
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
