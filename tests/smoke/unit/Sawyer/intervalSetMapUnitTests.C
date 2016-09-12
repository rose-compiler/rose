// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/IntervalSetMap.h>
#include <Sawyer/Set.h>
#include <iostream>

using namespace Sawyer::Container;

typedef Interval<int> IntRange;
typedef Set<char> CharSet;
typedef IntervalSetMap<IntRange, CharSet> IntCharMap;

static void
print(const std::string &title, const IntCharMap &icmap) {
    std::cout <<"  " <<title <<"\n";
    if (icmap.isEmpty())
        std::cout <<"    empty\n";
    BOOST_FOREACH (const IntCharMap::Node &node, icmap.nodes()) {
        std::cout <<"    [" <<node.key().least() <<".." <<node.key().greatest() <<"] = {";
        BOOST_FOREACH (char ch, node.value().values()) {
            std::cout <<" '" <<ch <<"'";
        }
        std::cout <<" }\n";
    }
}

static void
test1() {
    std::cout <<"test 1\n";
    IntCharMap map;
    ASSERT_always_require(map.isEmpty());

    map.insert(IntRange::hull(10, 11), 'a');
    print("inserted 'a' at [10..11]", map);
    ASSERT_always_require(map.nIntervals()==1);

    map.insert(IntRange::hull(8, 13), 'b');
    print("inserted 'b' at [8..13]", map);
    ASSERT_always_require(map.nIntervals()==3);

    map.insert(6, 'c');
    print("inserted 'c' at 6", map);
    ASSERT_always_require(map.nIntervals()==4);

    map.insert(IntRange::hull(16,17), 'd');
    print("inserted 'd' at [16..17]", map);
    ASSERT_always_require(map.nIntervals()==5);

    CharSet cs = map.getUnion(IntRange());
    ASSERT_always_require(cs.isEmpty());
    cs = map.getUnion(8);
    ASSERT_always_require(cs.size()==1 && cs.exists('b'));
    cs = map.getUnion(IntRange::hull(4, 10));
    ASSERT_always_require(cs.size()==3 && cs.exists('a') && cs.exists('b') && cs.exists('c'));

    cs = map.getIntersection(IntRange());
    ASSERT_always_require(cs.isEmpty());
    cs = map.getIntersection(8);
    ASSERT_always_require(cs.size()==1 && cs.exists('b'));
    cs = map.getIntersection(IntRange::hull(9, 12));
    ASSERT_always_require(cs.size()==1 && cs.exists('b'));

    ASSERT_always_require(!map.existsAnywhere(IntRange(), 'a'));
    ASSERT_always_require(!map.existsAnywhere(8, 'a'));
    ASSERT_always_require(map.existsAnywhere(11, 'b'));
    ASSERT_always_require(!map.existsAnywhere(IntRange::hull(9, 12), 'c'));
    ASSERT_always_require(map.existsAnywhere(IntRange::hull(9, 12), 'a'));

    ASSERT_always_require(!map.existsEverywhere(IntRange(), 'a'));
    ASSERT_always_require(!map.existsEverywhere(8, 'a'));
    ASSERT_always_require(map.existsEverywhere(11, 'b'));
    ASSERT_always_require(!map.existsEverywhere(IntRange::hull(9, 12), 'a'));
    ASSERT_always_require(map.existsEverywhere(IntRange::hull(9, 12), 'b'));

    map.erase(11, 'b');
    print("erased 'b' at 11", map);
    ASSERT_always_require(map.nIntervals()==6);

    map.insert(11, 'b');
    print("inserted 'b' at 11", map);
    ASSERT_always_require(map.nIntervals()==5);

    map.erase(10, 'a');
    print("erased 'a' at 10", map);
    ASSERT_always_require(map.nIntervals()==5);
    
    map.erase(IntRange::hull(0, 20), 'b');
    print("erased 'b' at [0..20]", map);
    ASSERT_always_require(map.nIntervals()==3);

    map.erase(IntRange::hull(0, 20), 'e');
    print("erased 'e' (non-existing) at [0..20]", map);
    ASSERT_always_require(map.nIntervals()==3);

    cs.clear();
    cs.insert('e');
    cs.insert('f');
    map.replace(IntRange::hull(11, 19), cs);
    print("set to {'e', 'f'} at [11..19]", map);
    ASSERT_always_require(map.nIntervals()==2);

    map.erase(IntRange::hull(14, 18));
    print("erase [14..18]", map);
    ASSERT_always_require(map.nIntervals()==3);

    map.clear();
    print("cleared", map);
    ASSERT_always_require(map.nIntervals()==0);
}

static void
test2() {
    std::cout <<"test 2\n";
    // This is the example from the documentation.
    IntCharMap map;
    print("default constructed", map);
    ASSERT_always_require(map.nIntervals() == 0);

    map.insert(IntRange::baseSize(0, 5000000), 'a');
    print("inserted 'a' at [0..4999999]", map);
    ASSERT_always_require(map.nIntervals() == 1);

    map.insert(IntRange::baseSize(0, 5000000), 'b');
    print("inserted 'b' at [0..4999999]", map);
    ASSERT_always_require(map.nIntervals() == 1);

    map.insert(5, 'c');
    print("inserted 'c' at 5", map);
    ASSERT_always_require(map.nIntervals() == 3);

    map.insert(IntRange::hull(10, 19), 'd');
    print("inserted 'd' at [10..19]", map);
    ASSERT_always_require(map.nIntervals() == 5);

    map.erase(map.hull(), 'b');
    print("erased 'b' from everywhere", map);
    ASSERT_always_require(map.nIntervals() == 5);

    map.erase(IntRange::hull(-1000000, 1000000), 'e');
    print("erased 'e' from [-1000000..1000000]", map);
    ASSERT_always_require(map.nIntervals() == 5);
}

int
main() {
    test1();
    test2();
}
