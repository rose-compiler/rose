#include <boost/foreach.hpp>
#include <set>
#include <rangemap.h>
#include <sawyer/IntervalMap.h>

// We want a map that associates Key with ValueSet, compressing adjacent keys with equal ValueSet.
typedef size_t Key;
typedef int Value;
typedef std::set<Value> ValueSet;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RangeMap tests
// This API will probably be deprecated eventually in favor of Sawyer::Container::IntervalMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The RangeMap API needs a way to print the values stored in the map regardless of whether we ever try to print them. This has
// to be defined in the global namespace, or the namespace of ValueSet if Koenig lookup is used.
std::ostream& operator<<(std::ostream &out, const ValueSet &vs) {
    out <<"{";
    BOOST_FOREACH (Value v, vs)
        out <<" " <<v;
    out <<" }";
    return out;
}

namespace TestRangeMap {

typedef Range<Key> KeyRange;

// We need to subclass RangeMapValue in order to define the "split" method, whose return type is that of the subclass.
class ValueContainer: public RangeMapValue<KeyRange, ValueSet> {
public:
    ValueContainer() {}
    ValueContainer(const ValueSet &value): RangeMapValue<KeyRange, ValueSet>(value) {}

    ValueContainer split(const Range &my_range, const Key &new_end) {
        assert(my_range.contains(Range(new_end)));
        return *this;
    }
};

// Finally, we can define our Map type.
typedef RangeMap<KeyRange, ValueContainer> Map;

// Printing is a little easier since we've taken the trouble of defining an operator<< for std::set<int>
static void
printMap(std::ostream &out, const std::string &title, const Map &m) {
    out <<"  " <<title <<" ====\n";
    for (Map::const_iterator i=m.begin(); i!=m.end(); ++i) {
        const KeyRange keyRange = i->first;
        const ValueContainer &valueContainer = i->second;
        out <<"    " <<keyRange <<" = " <<valueContainer <<"\n";
    }
}

static void
test() {
    ValueSet vs1;
    vs1.insert(100);
    vs1.insert(101);
    vs1.insert(102);
    vs1.insert(103);

    Map m;

    // Insert something for a single key. The size_t cast is required.
    m.insert(KeyRange((size_t)0), vs1);
    printMap(std::cout, "inserted {100,101,102,103} at key 0", m);

    // Insert a neighboring key with the same value (they should be combined)
    m.insert(KeyRange((size_t)1), vs1);
    printMap(std::cout, "inserted {100,101,102,103} at key 1", m);

    // Insert another neighboring key with the same value (they should be combined)
    m.insert(KeyRange((size_t)2), vs1);
    printMap(std::cout, "inserted {100,101,102,103} at key 2", m);

    // Remove the middle key (should result in two values stored)
    m.erase(KeyRange((size_t)1));
    printMap(std::cout, "erased key 1", m);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Sawyer::Container::IntervalMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This example does the same thing as the RangeMap test.
namespace TestIntervalMap {

using namespace Sawyer::Container;

typedef Interval<Key> KeyInterval;

typedef IntervalMap<KeyInterval, ValueSet> Map;

static void
printMap(std::ostream &out, const std::string &title, const Map &m) {
    out <<"  " <<title <<"\n";
    BOOST_FOREACH (const Map::Node &node, m.nodes()) {
        const KeyInterval &keyInterval = node.key();
        const ValueSet &valueSet = node.value();
        out <<"    " <<keyInterval.least() <<".." <<keyInterval.greatest() <<" = {";
        BOOST_FOREACH (const Value &value, valueSet)
            out <<" " <<value;
        out <<" }\n";
    }
}

static void
test() {
    ValueSet vs1;
    vs1.insert(100);
    vs1.insert(101);
    vs1.insert(102);
    vs1.insert(103);

    Map m;

    // Insert something for a single key
    m.insert(0, vs1);
    printMap(std::cout, "inserted {100,101,102,103} at key 0", m);

    // Insert a neighboring key with the same value (they should be combined)
    m.insert(1, vs1);
    printMap(std::cout, "inserted {100,101,102,103} at key 1", m);

    // Insert another neighboring key with the same value (they should be combined)
    m.insert(2, vs1);
    printMap(std::cout, "inserted {100,101,102,103} at key 2", m);

    // Remove the middle key (should result in two values stored)
    m.erase(1);
    printMap(std::cout, "erased key 1", m);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main() {
    std::cout <<"Testing ROSE RangeMap:\n";
    TestRangeMap::test();

    std::cout <<"\nTesting Sawyer::Container::IntervalMap:\n";
    TestIntervalMap::test();
}

    
