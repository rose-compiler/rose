// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/IndexedList.h>

#include <Sawyer/Assert.h>
#include <Sawyer/PoolAllocator.h>
#include <boost/foreach.hpp>
#include <iostream>
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
}

template<class List>
void list_assignment() {
    std::cout <<"list assignment:\n";

    List list2;
    list2.pushBack("aaa");                              // will be clobbered
    list2.pushBack("bbb");                              // ditto
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    Sawyer::initializeLibrary();

    typedef IndexedList<std::string> L1;
    
    default_ctor<L1>();
    insert_values<L1>();
    push_values<L1>();
    iterators<L1>();
    mid_insertion<L1>();
    fill_ctor<L1>();
    fill_insertion<L1>();
    list_insertion<L1>();
    node_erasure<L1>();
    erase_range<L1>();
    copy_ctor<L1>();
    list_assignment<L1>();
    iter_from_ptr<L1>();
    user_allocator();
}
