// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/IndexedList.h>

#include <Sawyer/Assert.h>
#include <Sawyer/PoolAllocator.h>
#include <boost/foreach.hpp>
#include <iostream>
#include <set>
#include <string>

using namespace Sawyer::Container;

template<class T>
std::ostream& operator<<(std::ostream &o, const IndexedList<T> &list) {
    o <<"  {";
    BOOST_FOREACH (typename IndexedList<T>::Node const &node, list.nodes())
        o <<" [" <<node.id() <<"]=" <<node.value();
    o <<" }";
    return o;
}

template<class List>
void default_ctor() {
    std::cout <<"default constructor:\n";
    List list;
    ASSERT_always_require(list.isEmpty());
    std::cout <<list <<"\n";
}

template<class List>
void insert_values() {
    std::cout <<"value insertion:\n";
    List list;

    typename List::NodeIterator iter = list.insert(list.nodes().end(), "first");
    std::cout <<"  inserted [" <<iter->id() <<"] = " <<iter->value() <<"\n";
    ASSERT_always_require(1==list.size());
    ASSERT_always_require(0==iter->id());
    ASSERT_always_require(iter->value() == "first");

    iter = list.insert(list.values().end(), "second");
    std::cout <<"  inserted [" <<iter->id() <<"] = " <<**iter <<"\n";
    ASSERT_always_require(2==list.size());
    ASSERT_always_require(1==iter->id());
    ASSERT_always_require(iter->value() == "second");

    iter = list.insert(list.nodes().end(), "third");
    std::cout <<"  inserted [" <<iter->id() <<"] = " <<iter->value() <<"\n";
    ASSERT_always_require(3==list.size());
    ASSERT_always_require(2==iter->id());
    ASSERT_always_require(iter->value() == "third");

    std::cout <<list <<"\n";
}

template<class List>
void push_values() {
    std::cout <<"value pushing:\n";
    List list;

    list.pushFront("second");
    list.pushBack("third");
    list.pushFront("first");
    ASSERT_always_require(3==list.size());
    std::cout <<list <<"\n";

    typename List::NodeIterator iter = list.nodes().begin();
    ASSERT_always_require(iter->id()==2);
    ASSERT_always_require(iter==list.find(2));
    ASSERT_always_require(iter->value() == "first");
    ++iter;
    ASSERT_always_require(iter->id()==0);
    ASSERT_always_require(iter==list.find(0));
    ASSERT_always_require(iter->value() == "second");
    ++iter;
    ASSERT_always_require(iter->id()==1);
    ASSERT_always_require(iter==list.find(1));
    ASSERT_always_require(iter->value() == "third");
}

template<class List>
void capacity() {
    List list;
    list.capacity(100);
    ASSERT_always_require(list.capacity() >= 100);

    list.pushBack("aaa");
    list.pushBack("bbb");
    ASSERT_always_require(list.capacity() >= 100);

    list.capacity(1);
    ASSERT_always_require(list.capacity() >= 1);
    ASSERT_always_require(list.capacity() >= list.size());
}

template<class List>
void accessors() {
    List list;
    list.pushBack("aaa");
    list.pushBack("bbb");
    const List clist(list);

    ASSERT_always_require(list.frontNode().value() == "aaa");
    ASSERT_always_require(clist.frontNode().value() == "aaa");

    ASSERT_always_require(list.backNode().value() == "bbb");
    ASSERT_always_require(clist.backNode().value() == "bbb");

    ASSERT_always_require(list.frontValue() == "aaa");
    ASSERT_always_require(clist.frontValue() == "aaa");

    ASSERT_always_require(list.backValue() == "bbb");
    ASSERT_always_require(clist.backValue() == "bbb");

    ASSERT_always_require(list.indexedNode(0).value() == "aaa");
    ASSERT_always_require(list.indexedNode(1).value() == "bbb");
    ASSERT_always_require(clist.indexedNode(0).value() == "aaa");
    ASSERT_always_require(clist.indexedNode(1).value() == "bbb");

    ASSERT_always_require(list.indexedValue(0) == "aaa");
    ASSERT_always_require(list.indexedValue(1) == "bbb");
    ASSERT_always_require(clist.indexedValue(0) == "aaa");
    ASSERT_always_require(clist.indexedValue(1) == "bbb");

    ASSERT_always_require(list[0] == "aaa");
    ASSERT_always_require(list[1] == "bbb");
    ASSERT_always_require(clist[0] == "aaa");
    ASSERT_always_require(clist[1] == "bbb");

    ASSERT_always_require(list.getOptional(0));
    ASSERT_always_require(*list.getOptional(0) == "aaa");
    ASSERT_always_require(list.getOptional(1));
    ASSERT_always_require(*list.getOptional(1) == "bbb");
    ASSERT_always_require(!list.getOptional(2));
    ASSERT_always_require(clist.getOptional(0));
    ASSERT_always_require(*clist.getOptional(0) == "aaa");
    ASSERT_always_require(clist.getOptional(1));
    ASSERT_always_require(*clist.getOptional(1) == "bbb");
    ASSERT_always_require(!clist.getOptional(2));

    std::string s = "zzz";
    ASSERT_always_require(list.getOrElse(0, s) == "aaa");
    ASSERT_always_require(list.getOrElse(1, s) == "bbb");
    ASSERT_always_require(list.getOrElse(2, s) == "zzz");
    
    ASSERT_always_require(clist.getOrElse(0, s) == "aaa");
    ASSERT_always_require(clist.getOrElse(1, s) == "bbb");
    ASSERT_always_require(clist.getOrElse(2, s) == "zzz");

    ASSERT_always_require(list.getOrDefault(0) == "aaa");
    ASSERT_always_require(list.getOrDefault(1) == "bbb");
    ASSERT_always_require(list.getOrDefault(2) == "");
    ASSERT_always_require(clist.getOrDefault(0) == "aaa");
    ASSERT_always_require(clist.getOrDefault(1) == "bbb");
    ASSERT_always_require(clist.getOrDefault(2) == "");
}

template<class List>
void iterators() {
    std::cout <<"iterators:\n";
    List list;

    ASSERT_always_require(list.nodes().begin() == list.nodes().end());

    list.pushFront("third");
    list.pushBack("fourth");
    list.pushFront("second");
    list.pushBack("fifth");
    list.pushFront("first");
    list.pushBack("sixth");
    std::cout <<list <<"\n";

    ASSERT_always_require(*list.frontNode()=="first");
    ASSERT_always_require(list.backNode().value()=="sixth");

    typename List::NodeIterator iter;
    iter = list.nodes().begin();
    ASSERT_always_require(**iter=="first");
    ++iter;
    ASSERT_always_require(**iter=="second");
    ++iter;
    ASSERT_always_require(**iter=="third");
    ++iter;
    ASSERT_always_require(**iter=="fourth");
    ++iter;
    ASSERT_always_require(**iter=="fifth");
    ++iter;
    ASSERT_always_require(**iter=="sixth");
    ++iter;
    ASSERT_always_require(iter==list.nodes().end());


#if 0 /* [Robb Matzke 2014-04-18] */
    ASSERT_always_require(list[0]=="third");
    ASSERT_always_require(list.at(1)=="fourth");
    ASSERT_always_require(list.at(2)=="second");
    ASSERT_always_require(list[3]=="fifth");
    ASSERT_always_require(list[4]=="first");
    ASSERT_always_require(list.at(5)=="sixth");
#endif
    
    std::cout <<"  using BOOST_FOREACH: ";
    BOOST_FOREACH (const typename List::Node &node, list.nodes())
        std::cout <<" " <<*node;
    std::cout <<"\n";

    std::cout <<"  using begin/end:     ";
    for (typename List::NodeIterator i=list.nodes().begin(); i!=list.nodes().end(); ++i)
        std::cout <<" " <<i->value();
    std::cout <<"\n";

#if 0 // [Robb Matzke 2014-04-18]: not implemented
    std::cout <<"  using rbegin/rend:   ";
    for (typename List::ReverseIterator i=list.rbegin(); i!=list.rend(); ++i)
        std::cout <<" " <<*i;
    std::cout <<"\n";
#endif
}

static void
node_iterators() {
    typedef IndexedList<std::pair<int, int> > L1;

    L1 list;
    list.pushBack(std::make_pair(1,2));
    L1::NodeIterator i = list.nodes().begin();
    ASSERT_always_require(i != list.nodes().end());

    // Access value through the node
    L1::Node &node = *i;
    ASSERT_always_require(node.value().first == 1);
    ASSERT_always_require(node.value().second == 2);
    ASSERT_always_require((*node).first == 1);
    ASSERT_always_require((*node).second == 2);
    ASSERT_always_require(node->first == 1);
    ASSERT_always_require(node->second == 2);

    // Same, but constant node
    const L1::Node &cnode = *i;
    ASSERT_always_require(cnode.value().first == 1);
    ASSERT_always_require(cnode.value().second == 2);
    ASSERT_always_require((*cnode).first == 1);
    ASSERT_always_require((*cnode).second == 2);
    ASSERT_always_require(cnode->first == 1);
    ASSERT_always_require(cnode->second == 2);

    // assignment
    L1::NodeIterator i2 = i;
    ASSERT_always_require(i2 == i);

    L1::ConstNodeIterator ci = i;
    ASSERT_always_require(ci == i);
    L1::ConstNodeIterator ci2 = ci;
    ASSERT_always_require(ci2 == ci);

    // pre-increment
    L1::NodeIterator i3 = ++i2;
    ASSERT_always_require(i3 == i2);
    ASSERT_always_require(i3 == list.nodes().end());

    L1::ConstNodeIterator ci3 = ++ci2;
    ASSERT_always_require(ci3 == ci2);
    ASSERT_always_require(ci3 == list.nodes().end());

    // pre-decrement (from the end iterator nonetheless)
    L1::NodeIterator i4 = --i2;
    ASSERT_always_require(i4 == i2);
    ASSERT_always_require(i4 == list.nodes().begin());

    L1::ConstNodeIterator ci4 = --ci2;
    ASSERT_always_require(ci4 == ci2);
    ASSERT_always_require(ci4 == list.nodes().begin());

    // post-increment
    L1::NodeIterator i5 = i2++;
    ASSERT_always_require(i5 == list.nodes().begin());
    ASSERT_always_require(i2 == list.nodes().end());

    L1::ConstNodeIterator ci5 = ci2++;
    ASSERT_always_require(ci5 == list.nodes().begin());
    ASSERT_always_require(ci2 == list.nodes().end());

    // post-decrement
    L1::NodeIterator i6 = i2--;
    ASSERT_always_require(i6 == list.nodes().end());
    ASSERT_always_require(i2 == list.nodes().begin());

    L1::ConstNodeIterator ci6 = ci2--;
    ASSERT_always_require(ci6 == list.nodes().end());
    ASSERT_always_require(ci2 == list.nodes().begin());
}

static void
value_iterators() {
    typedef IndexedList<std::pair<int, int> > L1;

    L1 list;
    list.pushBack(std::make_pair(1, 2));

    L1::ValueIterator i = list.values().begin();
    ASSERT_always_require(i != list.values().end());

    // Access to the value
    L1::Value &v = *i;
    ASSERT_always_require(v.first == 1);
    ASSERT_always_require(v.second == 2);

    ASSERT_always_require(i->first == 1);
    ASSERT_always_require(i->second == 2);

    // assignment
    L1::ValueIterator i2 = i;
    ASSERT_always_require(i2 == i);

    L1::ConstValueIterator ci = i;
    ASSERT_always_require(ci == i);
    L1::ConstValueIterator ci2 = ci;
    ASSERT_always_require(ci2 == ci);

    // pre-increment
    L1::ValueIterator i3 = ++i2;
    ASSERT_always_require(i3 == i2);
    ASSERT_always_require(i3 == list.values().end());

    L1::ConstValueIterator ci3 = ++ci2;
    ASSERT_always_require(ci3 == ci2);
    ASSERT_always_require(ci3 == list.values().end());

    // pre-decrement (from the end iterator nonetheless)
    L1::ValueIterator i4 = --i2;
    ASSERT_always_require(i4 == i2);
    ASSERT_always_require(i4 == list.values().begin());

    L1::ConstValueIterator ci4 = --ci2;
    ASSERT_always_require(ci4 == ci2);
    ASSERT_always_require(ci4 == list.values().begin());

    // post-increment
    L1::ValueIterator i5 = i2++;
    ASSERT_always_require(i5 == list.values().begin());
    ASSERT_always_require(i2 == list.values().end());

    L1::ConstValueIterator ci5 = ci2++;
    ASSERT_always_require(ci5 == list.values().begin());
    ASSERT_always_require(ci2 == list.values().end());

    // post-decrement
    L1::ValueIterator i6 = i2--;
    ASSERT_always_require(i6 == list.values().end());
    ASSERT_always_require(i2 == list.values().begin());

    L1::ConstValueIterator ci6 = ci2--;
    ASSERT_always_require(ci6 == list.values().end());
    ASSERT_always_require(ci2 == list.values().begin());

    // Const value iterator from node iterator
    L1::ConstValueIterator i7 = list.nodes().begin();
    ASSERT_always_require(i7 != list.values().end());
    ASSERT_always_require(i7->first == 1);
    ASSERT_always_require(i7->second == 2);
    L1::ConstNodeIterator cni = list.nodes().begin();
    L1::ConstValueIterator ci7 = cni;
    ASSERT_always_require(ci7 != list.values().end());
    ASSERT_always_require(ci7->first == 1);
    ASSERT_always_require(ci7->second == 2);
}

template<class List>
void mid_insertion() {
    std::cout <<"insertion in the middle:\n";
    List list;

    list.pushFront("third");                            // id=0
    list.pushBack("fourth");                            // id=1
    list.pushFront("second");                           // id=2
    list.pushBack("fifth");                             // id=3
    list.pushFront("first");                            // id=4
    list.pushBack("sixth");                             // id=5
    std::cout <<"  initial list: " <<list <<"\n";

    typename List::NodeIterator iter = list.insert(list.find(0), "pre-three");// id=6
    std::cout <<"  inserted before \"three\": " <<list <<"\n";
    ASSERT_always_require(iter->id()==6);
    ASSERT_always_require(iter->value()=="pre-three");

    iter = list.insert(list.nodes().end(), "post-six");     // id=7
    std::cout <<"  inserted at end: " <<list <<"\n";
    ASSERT_always_require(iter->id()==7);
    ASSERT_always_require(iter->value()=="post-six");

    iter = list.insert(list.nodes().begin(), "pre-one");    // id=8
    std::cout <<"  inserted at beginning: " <<list <<"\n";
    ASSERT_always_require(iter->id()==8);
    ASSERT_always_require(iter->value()=="pre-one");
}

template<class List>
void fill_ctor() {
    std::cout <<"filling constructor:\n";
    List list(5, "ha!");
    std::cout <<"  " <<list <<"\n";

    ASSERT_always_require(5==list.size());
    BOOST_FOREACH (const typename List::Node &node, list.nodes())
        ASSERT_always_require(node.value()=="ha!");
}

template<class List>
void fill_insertion() {
    std::cout <<"filling insertion:\n";

    List list;
    list.pushFront("third");                            // id=0
    list.pushBack("fourth");                            // id=1
    list.pushFront("second");                           // id=2
    list.pushBack("fifth");                             // id=3
    list.pushFront("first");                            // id=4
    list.pushBack("sixth");                             // id=5
    std::cout <<"  initial list: " <<list <<"\n";

    list.insert(list.find(1), 5, "X");
    std::cout <<"  inserted 5 before 'fourth': " <<list <<"\n";
    ASSERT_always_require(list.size()==11);
    for (size_t id=0; id<11; ++id) {
        if (id>=6) {
            ASSERT_always_require(list.find(id)->value()=="X");
        } else {
            ASSERT_always_require(list.find(id)->value()!="X");
        }
    }
}

template<class List>
void list_insertion() {
    std::cout <<"list insertion:\n";

    List list;
    list.pushFront("third");                            // id=0
    list.pushBack("fourth");                            // id=1
    list.pushFront("second");                           // id=2
    list.pushBack("fifth");                             // id=3
    list.pushFront("first");                            // id=4
    list.pushBack("sixth");                             // id=5
    std::cout <<"  initial list: " <<list <<"\n";

    List toInsert;
    toInsert.pushBack("A");
    toInsert.pushBack("B");
    std::cout <<"  to insert:    " <<toInsert <<"\n";

    list.insertMultiple(list.find(3), toInsert.values());
    std::cout <<"  inserted before 'fifth': " <<list <<"\n";
    ASSERT_always_require(list.size()==8);
    ASSERT_always_require(list.indexedNode(6).value()=="A");
    ASSERT_always_require(list.indexedNode(7).value()=="B");
    for (size_t id=0; id<6; ++id)
        ASSERT_always_require(list.find(id)->value()!="A" && list.find(id)->value()!="B");
}

template<class List>
void node_erasure() {
    std::cout <<"erase one element:\n";

    List list;
    list.pushFront("third");                            // id=0
    list.pushBack("fourth");                            // id=1
    list.pushFront("second");                           // id=2
    list.pushBack("fifth");                             // id=3
    list.pushFront("first");                            // id=4
    list.pushBack("sixth");                             // id=5
    std::cout <<"  initial list: " <<list <<"\n";

    typename List::NodeIterator iter = list.eraseAt(list.nodes().begin());
    std::cout <<"  erased beginning: " <<list <<"\n";
    ASSERT_always_require(5==list.size());
    ASSERT_always_require(list.frontNode().value()=="second");
    ASSERT_always_require(iter==list.nodes().begin());
    ASSERT_always_require(list.indexedNode(4).value()=="sixth"); // renumbered

    iter = list.eraseAt(list.find(4));
    std::cout <<"  erased last item: " <<list <<"\n";
    ASSERT_always_require(4==list.size());
    ASSERT_always_require(list.indexedNode(3).value()=="fifth"); // not renumbered

    iter = list.eraseAt(list.find(1));
    std::cout <<"  erased 'fourth': " <<list <<"\n";
    ASSERT_always_require(3==list.size());
    ASSERT_always_require(list.indexedNode(0).value()=="third"); // not renumbered
    ASSERT_always_require(*list.indexedNode(1)=="fifth");        // renumbered
    ASSERT_always_require(*list.indexedNode(2)=="second");       // not renumbered
}

template<class List>
void erase_index() {
    List list;

    list.pushBack("3");
    list.pushFront("2");
    list.pushBack("4");
    list.pushFront("1");
    list.pushBack("5");
    list.pushFront("0");
    ASSERT_always_require(list.size() == 6);

    std::set<std::string> set;
    set.insert("0");
    set.insert("1");
    set.insert("2");
    set.insert("3");
    set.insert("4");
    set.insert("5");

    set.erase(list[4]);
    list.erase(4);

    BOOST_FOREACH (const std::string &s, list.values()) {
        std::set<std::string>::iterator si = set.find(s);
        ASSERT_always_require(si != set.end());
        set.erase(si);
    }
    ASSERT_always_require(set.empty());
}

template<class List>
void erase_range() {
    std::cout <<"range erasure:\n";
    
    List list;
    list.pushFront("third");                            // id=0
    list.pushBack("fourth");                            // id=1
    list.pushFront("second");                           // id=2
    list.pushBack("fifth");                             // id=3
    list.pushFront("first");                            // id=4
    list.pushBack("sixth");                             // id=5
    std::cout <<"  initial list: " <<list <<"\n";

    typename List::NodeIterator from = list.find(2);        // "second", inclusive
    typename List::NodeIterator to   = list.find(5);        // "sixth", exclusive
    boost::iterator_range<typename List::NodeIterator> range(from, to);
    typename List::NodeIterator next = list.eraseAtMultiple(range);
    std::cout <<"  erased second-fifth: " <<list <<"\n";
    ASSERT_always_require(list.size()==2);
    ASSERT_always_require(*list.indexedNode(0)=="first");
    ASSERT_always_require(*list.indexedNode(1)=="sixth");
    ASSERT_always_require(next==list.find(1));

    next = list.eraseAtMultiple(list.nodes());
    std::cout <<"  erased all nodes: " <<list <<"\n";
    ASSERT_always_require(list.isEmpty());
    ASSERT_always_require(next==list.nodes().end());
}

class MyString {
public:
    std::string s;

    MyString() {}
    MyString(const std::string &s): s(s) {}
};

template<class List>
void copy_ctor() {
    std::cout <<"copy constructor:\n";

    List list;
    list.pushFront("third");                            // id=0
    list.pushBack("fourth");                            // id=1
    list.pushFront("second");                           // id=2
    list.pushBack("fifth");                             // id=3
    list.pushFront("first");                            // id=4
    list.pushBack("sixth");                             // id=5
    std::cout <<"  initial list:     " <<list <<"\n";

    // Copy construct from the same type
    List list2(list);
    std::cout <<"  copy constructed: " <<list2 <<"\n";
    ASSERT_always_require(list2.size()==6);
    ASSERT_always_require(list2.frontNode().value()=="first");
    ASSERT_always_require(list2.backNode().value()=="sixth");
    ASSERT_always_require(list2.nodes().begin()!=list.nodes().begin());

    std::cout <<"  original list:    " <<list <<"\n";
    ASSERT_always_require(list.size()==6);
    ASSERT_always_require(list.frontNode().value()=="first");
    ASSERT_always_require(list.backNode().value()=="sixth");

    // Copy construct from a different type
    IndexedList<MyString> l2(list);
    ASSERT_always_require(l2.size()==6);
    ASSERT_always_require(l2.frontNode().value().s == "first");
    ASSERT_always_require(l2.backNode().value().s == "sixth");
}

template<class List>
void assignment() {
    List src;
    src.pushBack("aaa");
    src.pushBack("bbb");
    src.pushBack("ccc");

    List dst1;
    dst1.pushBack("xxx");
    dst1.pushBack("yyy");
    dst1 = src;
    ASSERT_always_require(dst1.size() == 3);
    ASSERT_always_require(dst1.frontValue() == "aaa");
    ASSERT_always_require(dst1.backValue() == "ccc");
}

template<class List>
void list_assignment() {
    std::cout <<"list assignment:\n";

    List list2;
    list2.pushBack("aaa");                              // will be clobbered
    list2.pushBack("bbb");                              // ditto

    // Copy constructor
    List list3(list2);
    ASSERT_always_require(list3.size()==2);
    ASSERT_always_require(list3.frontValue() == "aaa");
    ASSERT_always_require(list3.backValue() == "bbb");

    list3.frontValue() = "zzz";
    ASSERT_always_require(list3.frontValue() == "zzz");
    ASSERT_always_require(list2.frontValue() == "aaa");

    // Assignment operator
    {
        List list;                                      // destroyed at end of scope
        list.pushFront("third");                        // id=0
        list.pushBack("fourth");                        // id=1
        list.pushFront("second");                       // id=2
        list.pushBack("fifth");                         // id=3
        list.pushFront("first");                        // id=4
        list.pushBack("sixth");                         // id=5
        std::cout <<"  initial list:     " <<list <<"\n";

        list2 = list;
        std::cout <<"  assignment:       " <<list2 <<"\n";
        ASSERT_always_require(list2.size()==6);
        ASSERT_always_require(list2.frontNode().value()=="first");
        ASSERT_always_require(list2.backNode().value()=="sixth");
        ASSERT_always_require(list2.nodes().begin()!=list.nodes().begin());

        std::cout <<"  original list:    " <<list <<"\n";
        ASSERT_always_require(list.size()==6);
        ASSERT_always_require(list.frontNode().value()=="first");
        ASSERT_always_require(list.backNode().value()=="sixth");
    }

    // iterate over list2 now that list has been destroyed.
    size_t x = list2.size();
    for (size_t i=0; i<x; ++i) {
        (void) list2.find(i)->id();
        (void) list2.find(i)->value();
    }
    BOOST_FOREACH (const typename List::Node &node, list2.nodes()) {
        x += node.id();
    }

}

template<class List>
void iter_from_ptr() {
#if 0 // [Robb Matzke 2014-04-18]: not a public API
    std::cout <<"iterator from node ptr:\n";

    List list;
    list.pushFront("third");                            // id=0
    list.pushBack("fourth");                            // id=1
    list.pushFront("second");                           // id=2
    list.pushBack("fifth");                             // id=3
    list.pushFront("first");                            // id=4
    list.pushBack("sixth");                             // id=5
    std::cout <<"  initial list:      " <<list <<"\n";

    typename List::Value &third = list[0];
    ASSERT_always_require(third=="third");
    typename List::Iterator third_iter(list, &third);
    ASSERT_always_require(third_iter==list.node(0));

#if 0 // [Robb Matzke 2014-04-18]: This should fail (and does)
    List list2;
    list2.pushBack("foo");
    typename List::Iterator bad_iter(list2, &third);
#endif
#endif
}

// This tests that the STL std::list elements can have an iterator to themselves.
struct Elmt1 {
    int payload;
    std::list<Elmt1>::iterator self;
};

static std::list<Elmt1> std_self_referent_test;

// This tests that the IndexList elements can have an iterator to themselves
struct Elmt2 {
    int payload;
    Sawyer::Container::IndexedList<Elmt2>::NodeIterator self;
};

Sawyer::Container::IndexedList<Elmt2> index_list_self_referent_test;

// Tests user-specified allocator
void user_allocator() {
    Sawyer::UnsynchronizedPoolAllocator pools;
    typedef Sawyer::ProxyAllocator<Sawyer::UnsynchronizedPoolAllocator> PoolProxy;

    Sawyer::Container::IndexedList<int, PoolProxy> list(pools);
    list.pushBack(123);
}

template<class List>
void debugging() {
    std::cout <<"debugging:\n";
    List list;
    list.pushBack("one");
    list.pushBack("two");
    list.pushFront("zero");
    list.dump(std::cout);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    Sawyer::initializeLibrary();

    typedef IndexedList<std::string> L1;
    
    default_ctor<L1>();
    insert_values<L1>();
    push_values<L1>();
    capacity<L1>();
    accessors<L1>();
    node_iterators();
    value_iterators();
    iterators<L1>();
    mid_insertion<L1>();
    fill_ctor<L1>();
    fill_insertion<L1>();
    list_insertion<L1>();
    node_erasure<L1>();
    erase_index<L1>();
    erase_range<L1>();
    copy_ctor<L1>();
    assignment<L1>();
    list_assignment<L1>();
    iter_from_ptr<L1>();
    user_allocator();
    debugging<L1>();
}
