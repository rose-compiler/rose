// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/DistinctList.h>

using namespace Sawyer::Container;

// Test that things compile
template<typename T, typename U>
static void
testCompileHelper() {
    typedef DistinctList<T> TList;
    typedef DistinctList<U> UList;

    // Implicit convertion must exist
    {
        T t __attribute__((unused)) (0);
        U u(0);
        t = u;
    }

    // List constructors
    {
        TList t1;
        TList t2(t1);
        UList u1;
        TList t3(u1);
        const UList u2(u1);
        TList t4(u2);
    }

    // List operators
    {
        TList t1;
        t1 = t1; // compiler warning expected -- we're testing self-assignment

        UList u1;
        t1 = u1;

        const UList u2;
        t1 = u2;
    }

    // List methods
    {
        T t(0);
        TList t1;
        t1.clear();
        t1.isEmpty();
        t1.size();
        t1.exists(t);
        t = t1.front();
        t = t1.back();
        t1.items();
        t1.pushFront(t);
        t1.pushBack(t);
        t = t1.popFront();
        t = t1.popBack();
        t1.erase(t);

        const TList t2;
        t2.isEmpty();
        t2.size();
        t2.exists(t);
        t = t2.front();
        t = t2.back();
        t1.items();
    }
}

void
testCompile() {
    testCompileHelper<int, int>();
    testCompileHelper<double, int>();
}

static void
testMap() {
    typedef int Item;
    typedef std::list<Item> Items;
    typedef Sawyer::Container::Map<Item, Items::iterator> Map;

    Items items;
    Map map;

    items.push_front(5);
    map.insert(5, items.begin());
    ASSERT_always_require(map.size()==1);

    Map::NodeIterator mi = map.find(4);
    ASSERT_always_require(mi == map.nodes().end());
    items.push_front(4);
    map.insert(4, items.begin());
    ASSERT_always_require(map.size()==2);
}

static void
default_ctor() {
    DistinctList<int> dl;
    ASSERT_always_require(dl.isEmpty());
    ASSERT_always_require(dl.size()==0);
    ASSERT_always_forbid(dl.exists(0));
}

static void
push_pop() {
    typedef DistinctList<int> List;
    int item;

    List dl;

    dl.pushFront(5);                                    // ( 5 )
    ASSERT_always_forbid(dl.isEmpty());
    ASSERT_always_require(dl.size()==1);
    ASSERT_always_require(dl.exists(5));
    ASSERT_always_require(dl.front()==5);
    ASSERT_always_require(dl.back()==5);

    dl.pushFront(4);                                    // ( 4 5 )
    ASSERT_always_require(dl.size()==2);
    ASSERT_always_require(dl.exists(4));
    ASSERT_always_require(dl.front()==4);
    ASSERT_always_require(dl.back()==5);

    dl.pushFront(5);                                    // ( 4 5 )  no change
    ASSERT_always_require(dl.size()==2);
    ASSERT_always_require(dl.front()==4);
    ASSERT_always_require(dl.back()==5);

    dl.pushBack(5);                                     // ( 4 5 )  no change
    ASSERT_always_require(dl.size()==2);
    ASSERT_always_require(dl.front()==4);
    ASSERT_always_require(dl.back()==5);

    dl.pushFront(4);                                    // ( 4 5 )  no change
    ASSERT_always_require(dl.size()==2);
    ASSERT_always_require(dl.front()==4);
    ASSERT_always_require(dl.back()==5);

    dl.pushBack(4);                                     // ( 4 5 )  no change
    ASSERT_always_require(dl.size()==2);
    ASSERT_always_require(dl.front()==4);
    ASSERT_always_require(dl.back()==5);

    dl.pushFront(3);                                    // ( 3 4 5 )
    ASSERT_always_require(dl.size()==3);
    ASSERT_always_require(dl.front()==3);
    ASSERT_always_require(dl.exists(4));
    ASSERT_always_require(dl.back()==5);

    item = dl.popBack();                                // ( 3 4 )
    ASSERT_always_require(item==5);
    ASSERT_always_require(dl.size()==2);
    ASSERT_always_require(dl.front()==3);
    ASSERT_always_require(dl.back()==4);
    ASSERT_always_forbid(dl.exists(5));

    item = dl.popFront();                               // ( 4 )
    ASSERT_always_require(item==3);
    ASSERT_always_require(dl.size()==1);
    ASSERT_always_require(dl.front()==4);
    ASSERT_always_require(dl.back()==4);
    ASSERT_always_forbid(dl.exists(3));
}

static void
pop_empty() {
    DistinctList<std::string> empty;

    try {
        empty.popFront();
        ASSERT_not_reachable("should have thrown");
    } catch (const std::runtime_error&) {
    }

    try {
        empty.popBack();
        ASSERT_not_reachable("should have thrown");
    } catch (const std::runtime_error&) {
    }

}

static void
copy_ctor() {
    DistinctList<int> s1;
    s1.pushBack(1);
    s1.pushBack(2);
    const DistinctList<int> &cs1 = s1;

    DistinctList<int> d1 = cs1;
    ASSERT_always_require(d1.size() == s1.size());

    DistinctList<double> d2 = cs1;
    ASSERT_always_require(d2.size() == s1.size());
}

static void
clear() {
    DistinctList<int> s1;
    s1.pushBack(1);
    s1.pushBack(2);
    ASSERT_always_require(s1.size() == 2);

    s1.clear();
    ASSERT_always_require(s1.size() == 0);
}

static void
assignment() {
    DistinctList<int> s1;
    s1.pushBack(1);
    s1.pushBack(2);
    const DistinctList<int> &cs1 = s1;

    DistinctList<double> d1;
    d1.pushBack(7);
    d1.pushBack(8);
    d1.pushBack(9);
    ASSERT_always_require(d1.size() == 3);
    d1 = cs1;
    ASSERT_always_require(d1.size() == cs1.size());
}

static void
position() {
    DistinctList<std::string> list;
    list.pushBack("bbb");
    list.pushFront("aaa");
    list.pushBack("ccc");
    const DistinctList<std::string> &clist = list;

    ASSERT_always_require(clist.position("aaa") == 0);
    ASSERT_always_require(clist.position("bbb") == 1);
    ASSERT_always_require(clist.position("ccc") == 2);
    ASSERT_always_require(clist.position("zzz") == list.size());
}

static void
reference() {
    DistinctList<std::string> list;
    list.pushBack("bbb");
    list.pushFront("aaa");
    list.pushBack("ccc");
    const DistinctList<std::string> &clist = list;
    ASSERT_always_require(clist.front() == "aaa");
    ASSERT_always_require(clist.back() == "ccc");

    const DistinctList<std::string> empty;
    try {
        empty.front();
        ASSERT_not_reachable("should have thrown");
    } catch (const std::runtime_error&) {
    }

    try {
        empty.back();
        ASSERT_not_reachable("should have thrown");
    } catch (const std::runtime_error&) {
    }
}

static void
erase() {
    DistinctList<std::string> list;
    list.pushBack("bbb");
    list.pushFront("aaa");
    list.pushBack("ccc");
    ASSERT_always_require(list.exists("aaa"));
    ASSERT_always_require(list.exists("bbb"));
    ASSERT_always_require(list.exists("ccc"));

    list.erase("bbb");
    ASSERT_always_require(list.exists("aaa"));
    ASSERT_always_require(!list.exists("bbb"));
    ASSERT_always_require(list.exists("ccc"));

    list.erase("zzz");
    ASSERT_always_require(list.exists("aaa"));
    ASSERT_always_require(!list.exists("bbb"));
    ASSERT_always_require(list.exists("ccc"));
}

static void
list() {
    DistinctList<std::string> l;
    l.pushBack("bbb");
    l.pushFront("aaa");
    l.pushBack("ccc");

    const std::list<std::string> &items = l.items();
    ASSERT_always_require(items.size() == 3);
}

int
main() {
    Sawyer::initializeLibrary();
    testMap();

    default_ctor();
    push_pop();
    pop_empty();
    copy_ctor();
    clear();
    assignment();
    position();
    reference();
    erase();
    list();
}
