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
        t1 = t1;

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
test() {
    typedef DistinctList<int> List;
    int item;

    List dl;
    ASSERT_always_require(dl.isEmpty());
    ASSERT_always_require(dl.size()==0);
    ASSERT_always_forbid(dl.exists(0));

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

int
main() {
    Sawyer::initializeLibrary();
    testMap();
    test();
}
