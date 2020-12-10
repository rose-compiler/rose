// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/HashMap.h>

#include <boost/foreach.hpp>
#include <iostream>
#include <Sawyer/Assert.h>
#include <string>

template<class Key, class Value>
std::ostream& operator<<(std::ostream &o, const Sawyer::Container::HashMap<Key, Value> &map) {
    typedef Sawyer::Container::HashMap<Key, Value> Map;
    o <<"{";
    BOOST_FOREACH (const typename Map::Node &node, map.nodes())
        o <<" [" <<node.key() <<"]=" <<node.value();
    o <<" }";
    return o;
}

template<class Map>
void default_ctor() {
    std::cout <<"default constructor:\n";
    Map map;
    std::cout <<"    " <<map <<"\n";
    ASSERT_always_require(map.isEmpty());
    ASSERT_always_require(map.size()==0);
}

template<class Map>
void insert_one() {
    std::cout <<"insert one item at a time:\n";

    Map map;
    map.insert("one", 1);

    map
        .insert("two", 2)
        .insert("three", 3)
        .insert("four", 4);

    std::cout <<"    initial insertion: " <<map <<"\n";
    ASSERT_always_require(map.size()==4);
    ASSERT_always_require(map["one"]==1);
    ASSERT_always_require(map["two"]==2);
    ASSERT_always_require(map["three"]==3);
    ASSERT_always_require(map["four"]==4);

    typename Map::Value &two = map.insertMaybe("two", 22); // not inserted
    std::cout <<"    not inserted [two]=22: " <<map <<"\n";
    ASSERT_always_require(map.size()==4);
    ASSERT_always_require(map["two"]==2);
    ASSERT_always_require(two==2);

    typename Map::Value &uno = map.insertMaybe("uno", 1);
    std::cout <<"    inserted [uno]=1: " <<map <<"\n";
    ASSERT_always_require(map.size()==5);
    ASSERT_always_require(map["uno"]==1);
    ASSERT_always_require(uno==1);
}

static void
insert_default() {
    Sawyer::Container::HashMap<int, std::string> map;
    map.insert(1, "aaa");
    map.insert(2, "bbb");
    ASSERT_always_require(map.size()==2);

    map.insertDefault(1);
    ASSERT_always_require(map[1] == "");

    map.insertDefault(3);
    ASSERT_always_require(map.size()==3);
    ASSERT_always_require(map[3] == "");
}

template<class Map>
void insert_other() {
    std::cout <<"insert values from another map:\n";

    typedef Sawyer::Container::HashMap<const char*, double> OtherMap;
    OtherMap other;
    other.insert("pi", 3.14).insert("tau", 6.28).insert("e", 2.72);
    Map map;
    map.insert("one", 1).insert("two", 2).insert("three", 3);
    std::cout <<"    initial value for other: " <<other <<"\n";
    std::cout <<"    initial value for map: " <<map <<"\n";

    map.insertMultiple(other.nodes());
    std::cout <<"    inserted other into map: " <<map <<"\n";
    ASSERT_always_require(map.size()==6);
    ASSERT_always_require(map["e"]==2);
    ASSERT_always_require(map["one"]==1);
    ASSERT_always_require(map["pi"]==3);
    ASSERT_always_require(map["tau"]==6);
    ASSERT_always_require(map["three"]==3);
    ASSERT_always_require(map["two"]==2);

    // Const version without copy constructor
    Map map2;
    map2.insert("one", 1).insert("two", 2).insert("three", 3);
    const OtherMap *other2 = &other;
    map2.insertMultiple(other2->nodes());
    std::cout <<"    inserted other into map2: " <<map2 <<"\n";
    ASSERT_always_require(map2.size()==6);
    ASSERT_always_require(map2["e"]==2);
    ASSERT_always_require(map2["one"]==1);
    ASSERT_always_require(map2["pi"]==3);
    ASSERT_always_require(map2["tau"]==6);
    ASSERT_always_require(map2["three"]==3);
    ASSERT_always_require(map2["two"]==2);
}

template<class Map>
void accessors() {
    Map map;
    map.insert("aaa", 1);
    map.insert("bbb", 2);
    const Map &cmap = map;

    // Array-like
    ASSERT_always_require(map["aaa"] == 1);
    ASSERT_always_require(map["bbb"] == 2);
    try {
        map["ccc"];
        ASSERT_not_reachable("should have thrown std::domain_error");
    } catch (const std::domain_error&) {
    }
    ASSERT_always_require(cmap["aaa"] == 1);
    ASSERT_always_require(cmap["bbb"] == 2);
    try {
        cmap["ccc"];
        ASSERT_not_reachable("should have thrown std::domain_error");
    } catch (const std::domain_error&) {
    }

    // get
    ASSERT_always_require(map.get("aaa") == 1);
    ASSERT_always_require(map.get("bbb") == 2);
    try {
        map.get("ccc");
        ASSERT_not_reachable("should have thrown std::domain_error");
    } catch (const std::domain_error&) {
    }
    ASSERT_always_require(cmap.get("aaa") == 1);
    ASSERT_always_require(cmap.get("bbb") == 2);
    try {
        cmap.get("ccc");
        ASSERT_not_reachable("should have thrown std::domain_error");
    } catch (const std::domain_error&) {
    }

    // getOrElse
    int dflt = 911;
    const int cdflt = 911;
    ASSERT_always_require(map.getOrElse("aaa", dflt) == 1);
    ASSERT_always_require(map.getOrElse("bbb", dflt) == 2);
    ASSERT_always_require(map.getOrElse("ccc", dflt) == 911);
    ASSERT_always_require(map.getOrElse("ccc", cdflt) == 911);
    ASSERT_always_require(cmap.getOrElse("aaa", dflt) == 1);
    ASSERT_always_require(cmap.getOrElse("bbb", dflt) == 2);
    ASSERT_always_require(cmap.getOrElse("ccc", dflt) == 911);
    ASSERT_always_require(cmap.getOrElse("ccc", cdflt) == 911);
}

template<class Map>
void find() {
    std::cout <<"find values by key:\n";
    Map map;
    map.insert("eye", 1).insert("nose", 2).insert("mouth", 3).insert("neck", 4).insert("", 100);
    std::cout <<"    initial map: " <<map <<"\n";

    const Map &cmap = map;

    typename Map::NodeIterator iter;
    iter = map.find("mouth");
    ASSERT_always_require(iter!=map.nodes().end());
    ASSERT_always_require(iter->key()=="mouth");
    ASSERT_always_require(iter->value()==3);

    typename Map::ConstNodeIterator citer;
    citer = cmap.find("mouth");
    ASSERT_always_require(citer!=cmap.nodes().end());
    ASSERT_always_require(citer->key()=="mouth");
    ASSERT_always_require(citer->value()==3);

    iter = map.find("");
    ASSERT_always_require(iter!=map.nodes().end());
    ASSERT_always_require(iter->key()=="");
    ASSERT_always_require(iter->value()==100);

    citer = cmap.find("");
    ASSERT_always_require(citer!=cmap.nodes().end());
    ASSERT_always_require(citer->key()=="");
    ASSERT_always_require(citer->value()==100);

    iter = map.find("forehead");
    ASSERT_always_require(iter==map.nodes().end());

    citer = cmap.find("forehead");
    ASSERT_always_require(citer==cmap.nodes().end());
}

template<class Map>
void test_existence() {
    std::cout <<"test whether nodes exist:\n";
    Map map;
    map.insert("eye", 1).insert("nose", 2).insert("mouth", 3).insert("neck", 4).insert("", 100);
    std::cout <<"    initial map: " <<map <<"\n";

    ASSERT_always_require(map.exists("eye"));
    ASSERT_always_require(map.exists("nose"));
    ASSERT_always_require(map.exists("mouth"));
    ASSERT_always_require(map.exists("neck"));
    ASSERT_always_require(map.exists(""));

    ASSERT_always_forbid(map.exists("shoulders"));
}

template<class Map>
void clear_all() {
    std::cout <<"remove all nodes:\n";
    Map map;
    map.insert("eye", 1).insert("nose", 2).insert("mouth", 3).insert("neck", 4).insert("", 100);
    std::cout <<"    initial map: " <<map <<"\n";

    map.clear();
    std::cout <<"    cleared map: " <<map <<"\n";
    ASSERT_always_require(map.isEmpty());
    ASSERT_always_require(map.size()==0);
    ASSERT_always_require(!map.exists("mouth"));
}

template<class Map>
void erase_one() {
    std::cout <<"erase one node at a time:\n";
    Map map;
    map.insert("eye", 1).insert("nose", 2).insert("mouth", 3).insert("neck", 4).insert("", 100);
    std::cout <<"    initial map: " <<map <<"\n";

    map.erase("eye");
    std::cout <<"    removed \"eye\": " <<map <<"\n";
    ASSERT_always_require(map.size()==4);
    ASSERT_always_forbid(map.exists("eye"));

    map.erase("");
    std::cout <<"    removed \"\": " <<map <<"\n";
    ASSERT_always_require(map.size()==3);
    ASSERT_always_forbid(map.exists(""));

    map
        .erase("neck")
        .erase("mouth");
    std::cout <<"    removed \"neck\" and \"mouth\": " <<map <<"\n";
    ASSERT_always_require(map.size()==1);
    ASSERT_always_forbid(map.exists("neck"));
    ASSERT_always_forbid(map.exists("mouth"));

    map.erase("shoulder");
    std::cout <<"    removed \"shoulder\" (not): " <<map <<"\n";
    ASSERT_always_require(map.size()==1);

    map.erase("nose");
    std::cout <<"    removed \"nose\": " <<map <<"\n";
    ASSERT_always_require(map.isEmpty());
}

template<class Map>
void erase_other() {
    std::cout <<"erase keys that are in another map:\n";
    
    Map map;
    map.insert("eye", 1).insert("nose", 2).insert("mouth", 3).insert("neck", 4).insert("", 100);
    std::cout <<"    initial map: " <<map <<"\n";

    typedef Sawyer::Container::HashMap<const char*, double> Other;
    Other other;
    other.insert("neck", 1.2).insert("forehead", 99.99).insert("eye", 1.1).insert("", 2.2);
    std::cout <<"    to remove: " <<other <<"\n";

    map.eraseMultiple(other.keys());
    std::cout <<"    result: " <<map <<"\n";
    ASSERT_always_require(map.size()==2);
    ASSERT_always_require(map["mouth"]==3);
    ASSERT_always_require(map["nose"]==2);
}

template<class Map>
void insert_multiple() {
    Map m1;
    m1.insert("aaa", 1);
    m1.insert("bbb", 2);

    Map m2;
    m2.insert("bbb", 3);
    m2.insert("ccc", 4);

    m1.insertMaybeMultiple(m2.nodes());
    ASSERT_always_require(m1.size()==3);
    ASSERT_always_require(m1["aaa"] == 1);
    ASSERT_always_require(m1["bbb"] == 2);
    ASSERT_always_require(m1["ccc"] == 4);
}

void stdMapIterators() {
    typedef std::map<std::string, int> Map;
    Map map;

    map.insert(std::make_pair("eye", 1));
    map.insert(std::make_pair("nose", 2));
    map.insert(std::make_pair("mouth", 3));
    map.insert(std::make_pair("neck", 4));
    map.insert(std::make_pair("", 5));

    Map::iterator ni = map.find("mouth");
    Map::const_iterator cni __attribute__((unused)) = ni;

    ASSERT_always_require(ni != map.end());
    ASSERT_always_require(ni->first == "mouth");
    ASSERT_always_require(ni->second == 3);

    ni->second = 33;
    ASSERT_always_require(ni->second == 33);
    ASSERT_always_require(map["mouth"] == 33);
}

template<class Map>
void iterators() {
    std::cout <<"iterator functionality:\n";

    Map map;
    map.insert("eye", 1).insert("nose", 2).insert("mouth", 3).insert("neck", 4).insert("", 100);
    std::cout <<"    initial map: " <<map <<"\n";

    // Implicit conversions
    typename Map::NodeIterator ni = map.find("mouth");

    typename Map::ConstNodeIterator cni = ni;

    typename Map::ConstKeyIterator cki = ni;
    typename Map::ConstKeyIterator ck2 = cni;
    ASSERT_always_require(ck2 == cni);

    typename Map::ValueIterator vi = ni;

    typename Map::ConstValueIterator cvi = vi;
    typename Map::ConstValueIterator cvi2 = ni;
    ASSERT_always_require(cvi2 == ni);
    typename Map::ConstValueIterator cvi3 = cni;
    ASSERT_always_require(cvi3 == cni);

    // Node iterators
    ASSERT_always_require(ni!=map.nodes().end());

    ASSERT_always_require(ni->key()=="mouth");
    ASSERT_always_require((*ni).key()=="mouth");
    ASSERT_always_require(ni->value()==3);
    ASSERT_always_require((*ni).value()==3);

    ASSERT_always_require(ni!=map.find("eye"));
    ASSERT_always_forbid(ni==map.find("eye"));

    ASSERT_always_require(ni==ni);
    ASSERT_always_forbid(ni!=ni);

    // Const node iterators
    ASSERT_always_require(cni!=map.nodes().end());

    ASSERT_always_require(cni->key()=="mouth");
    ASSERT_always_require((*cni).key()=="mouth");
    ASSERT_always_require(cni->value()==3);
    ASSERT_always_require((*cni).value()==3);

    ASSERT_always_require(cni!=map.find("eye"));
    ASSERT_always_forbid(cni==map.find("eye"));

    ASSERT_always_require(cni==cni);
    ASSERT_always_forbid(cni!=cni);
    ASSERT_always_require(cni==ni);
    ASSERT_always_forbid(cni!=ni);

    // Const key iterators
    ASSERT_always_require(cki!=map.nodes().end());

    ASSERT_always_require(*cki=="mouth");
    ASSERT_always_require(cki->size()==5);

    ASSERT_always_require(cki!=map.find("eye"));
    ASSERT_always_forbid(cki==map.find("eye"));

    ASSERT_always_require(cki==cki);
    ASSERT_always_forbid(cki!=cki);
    ASSERT_always_require(cki==ni);
    ASSERT_always_forbid(cki!=ni);
    ASSERT_always_require(cki==cni);
    ASSERT_always_forbid(cki!=cni);

    // Value iterators
    ASSERT_always_require(vi!=map.nodes().end());

    ASSERT_always_require(*vi==3);

    ASSERT_always_require(vi!=map.find("eye"));
    ASSERT_always_forbid(vi==map.find("eye"));

    ASSERT_always_require(vi==vi);
    ASSERT_always_forbid(vi!=vi);
    ASSERT_always_require(vi==ni);
    ASSERT_always_forbid(vi!=ni);
    ASSERT_always_require(vi==cni);
    ASSERT_always_forbid(vi!=cni);
    ASSERT_always_require(vi==cki);
    ASSERT_always_forbid(vi!=cki);

    // Const value iterators
    ASSERT_always_require(cvi!=map.nodes().end());

    ASSERT_always_require(*cvi==3);

    ASSERT_always_require(cvi!=map.find("eye"));
    ASSERT_always_forbid(cvi==map.find("eye"));

    ASSERT_always_require(cvi==cvi);
    ASSERT_always_forbid(cvi!=cvi);
    ASSERT_always_require(cvi==ni);
    ASSERT_always_forbid(cvi!=ni);
    ASSERT_always_require(cvi==cni);
    ASSERT_always_forbid(cvi!=cni);
    ASSERT_always_require(cvi==cki);
    ASSERT_always_forbid(cvi!=cki);
    ASSERT_always_require(cvi==vi);
    ASSERT_always_forbid(cvi!=vi);

    // The following two tests which modify a value through a node iterator and verify that the value read through the
    // corresponding value iterator changed, and vice versa, don't compile correctly on GCC 4.5 when optimizations are turned
    // on. I think this is because of the static cast from std::pair to Map::Node but since C++ doesn't have type enrichment
    // I'm not sure how to fix it. Fortunately this feature doesn't seem to be used, so I'm just commenting it out.
#undef SKIP_ITERATOR_TYPE_ALIAS_TESTS
#ifdef __GNUC__
    #if __GNUC__ == 4 && __GNUC_MINOR__ == 5
        #define SKIP_ITERATOR_TYPE_ALIAS_TESTS
    #endif
#endif
#ifndef SKIP_ITERATOR_TYPE_ALIAS_TESTS
    // Modification via node iterator
    ni->value() = 33;
    ASSERT_always_require(ni->value()==33);
    ASSERT_always_require(cni->value()==33);
    ASSERT_always_require(*vi==33);
    ASSERT_always_require(*cvi==33);

    // Modification via value iterator
    *vi = 44;
    ASSERT_always_require(ni->value()==44);
    ASSERT_always_require(cni->value()==44);
    ASSERT_always_require(*vi==44);
    ASSERT_always_require(*cvi==44);
#endif

    // Node iterator increment
    typename Map::NodeIterator ni2 = ni++;
    ASSERT_always_require(ni != ni2);
    if (ni2 != map.nodes().end()) {
        ASSERT_always_require(ni2->key() != ni->key());
        ASSERT_always_require(ni2->value() != ni->value());
    }
}

template<class Map>
void erase_iterator() {
    std::cout <<"erase nodes according to iterator:\n";

    Map map;
    map.insert("aaa", 1);
    map.insert("bbb", 2);
    map.insert("ccc", 3);

    typename Map::NodeIterator ni = map.find("bbb");
    map.eraseAt(ni);
    ASSERT_always_require(map.size()==2);
    ASSERT_always_require(map.exists("aaa"));
    ASSERT_always_require(!map.exists("bbb"));
    ASSERT_always_require(map.exists("ccc"));
}

static void
copy_ctor() {
    typedef const char* SrcKey;
    typedef std::string DstKey;
    typedef int SrcValue;
    typedef double DstValue;
    typedef Sawyer::Container::HashMap<SrcKey, SrcValue> SrcMap;
    typedef Sawyer::Container::HashMap<DstKey, DstValue> DstMap;

    SrcMap m1;
    m1.insert("aaa", 1);
    m1.insert("bbb", 2);
    ASSERT_always_require(m1.size()==2);

    const SrcMap m2(m1);
    ASSERT_always_require(m2.size()==m1.size());

    DstMap m3(m1);
    ASSERT_always_require(m3.size()==m1.size());

    DstMap m4(m2);
    ASSERT_always_require(m4.size()==m2.size());
}

static void
assignment() {
    typedef const char* SrcKey;
    typedef std::string DstKey;
    typedef int SrcValue;
    typedef double DstValue;
    typedef Sawyer::Container::HashMap<SrcKey, SrcValue> SrcMap;
    typedef Sawyer::Container::HashMap<DstKey, DstValue> DstMap;

    SrcMap m1;
    m1.insert("aaa", 1);
    m1.insert("bbb", 2);
    ASSERT_always_require(m1.size() == 2);

    SrcMap m2;
    m2.insert("xxx", 7);
    m2.insert("yyy", 8);
    m2.insert("zzz", 9);
    ASSERT_always_require(m2.size() == 3);
    m2 = m1;
    ASSERT_always_require(m2.size() == m1.size());

    const SrcMap m3 = m1;
    ASSERT_always_require(m3.size() == m1.size());
    SrcMap m4;
    m4.insert("xxx", 7);
    m4.insert("yyy", 8);
    m4.insert("zzz", 9);
    ASSERT_always_require(m4.size() == 3);
    m4 = m3;
    ASSERT_always_require(m4.size() == m3.size());

    DstMap m5;
    m5.insert("xxx", 7);
    m5.insert("yyy", 8);
    m5.insert("zzz", 9);
    ASSERT_always_require(m5.size() == 3);
    m5 = m1;
    ASSERT_always_require(m5.size() == m1.size());
    
    DstMap m6;
    m6.insert("xxx", 7);
    m6.insert("yyy", 8);
    m6.insert("zzz", 9);
    ASSERT_always_require(m6.size() == 3);
    m6 = m3;
    ASSERT_always_require(m6.size() == m3.size());
}

int main() {
    typedef std::string Key;
    typedef int Value;
    typedef Sawyer::Container::HashMap<Key, Value> Map;

    Sawyer::initializeLibrary();

    default_ctor<Map>();
    insert_one<Map>();
    insert_default();
    insert_other<Map>();
    accessors<Map>();
    find<Map>();
    test_existence<Map>();
    clear_all<Map>();
    erase_one<Map>();
    erase_other<Map>();
    insert_multiple<Map>();
    stdMapIterators();
    iterators<Map>();
    erase_iterator<Map>();
    copy_ctor();
    assignment();
}
