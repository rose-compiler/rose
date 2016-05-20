// Tests ROSE's Map<T> class.

// This is a ROSE utility class, so there's no need to include "rose.h"
#include "Map.h"
#include <iostream>

#define REQUIRE(X, S) require((X), #X, S, __FILE__, __LINE__);

static void require(bool cond, const char *cond_str, const char *mesg, const char *filename, unsigned linenum)
{
    if (!cond) {
        std::cerr <<filename <<":" <<linenum <<": postcondition failed: " <<(mesg && *mesg ? mesg : cond_str) <<"\n";
        abort();
    }
}

static void test1_get()
{
    typedef Map<int, int> MyMap;
    MyMap m;

    m.insert(std::make_pair(1, 9));
    m.insert(std::make_pair(2, 8));
    m.insert(std::make_pair(3, 7));

    boost::optional<int> v1 = m.get(1);
    REQUIRE(v1.is_initialized(), "v1 should be initialized");
    REQUIRE(v1==9, NULL);

    boost::optional<int> v2 = m.get(999);
    REQUIRE(!v2, "v2 should be uninitialized");
}

static void test2_get_value_or()
{
    typedef Map<int, int> MyMap;
    MyMap m;

    m.insert(std::make_pair(1, 9));
    m.insert(std::make_pair(2, 8));
    m.insert(std::make_pair(3, 7));

    int v1 = m.get_value_or(1, 100);
    REQUIRE(v1==9, "map-contained value should have been returned");

    int v2 = m.get_value_or(999, 100);
    REQUIRE(v2==100, "default value should have been returned");
}

static void test3_if()
{
    typedef Map<int, int*> MyMap;
    MyMap m;
    static int ia[3] = {9, 8, 7};

    m.insert(std::make_pair(1, ia+0));
    m.insert(std::make_pair(2, ia+1));
    m.insert(std::make_pair(3, ia+2));

    if (int *p = m.get_value_or(1, NULL)) {
        REQUIRE(*p==9, NULL);
    } else {
        REQUIRE(false, "should have gotten a non-null pointer");
    }

    if (m.get_value_or(999, NULL)) {
        REQUIRE(false, "should have gotten a null pointer");
    }
}

static void test4_get_one()
{
    typedef Map<int, int> MyMap;
    MyMap m;

    m.insert(std::make_pair(1, 9));
    m.insert(std::make_pair(2, 8));
    m.insert(std::make_pair(3, 7));

    int v1 = m.get_one(1);
    REQUIRE(v1==9, "map-contained value should have been returned");

    try {
        m.get_one(999);
        REQUIRE(false, "should have thrown an exception");
    } catch (const std::domain_error &e) {
    } catch (...) {
        REQUIRE(false, "should have thrown a domain_error");
    }
}

static void test5_exists()
{
    typedef Map<int, int> MyMap;
    MyMap m;

    m.insert(std::make_pair(1, 9));
    m.insert(std::make_pair(2, 8));
    m.insert(std::make_pair(3, 7));

    REQUIRE(m.exists(1), "key should exist");
    REQUIRE(!m.exists(999), "key should not exist");
}


int main()
{
    test1_get();
    test2_get_value_or();
    test3_if();
    test4_get_one();
    test5_exists();
    return 0;
}

