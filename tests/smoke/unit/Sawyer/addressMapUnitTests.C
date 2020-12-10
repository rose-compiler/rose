// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/AddressMap.h>

#include <Sawyer/AllocatingBuffer.h>
#include <Sawyer/MappedBuffer.h>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace Sawyer;
using namespace Sawyer::Container;

template<typename T>
static std::string
show(const Interval<T> &x) {
    if (x.isEmpty()) {
        return "[]";
    } else if (x.isSingleton()) {
        return "[" + boost::lexical_cast<std::string>(x.least()) + "]";
    } else {
        return "[" + boost::lexical_cast<std::string>(x.least()) + "," + boost::lexical_cast<std::string>(x.greatest()) + "]";
    }
}

static std::string
showWhere(size_t got, size_t ans) {
    return "got=" + boost::lexical_cast<std::string>(got) + ", answer=" + boost::lexical_cast<std::string>(ans);
}

template<typename T>
static std::string
showWhere(size_t i, const Interval<T> &got, const Interval<T> &ans) {
    return "i=" + boost::lexical_cast<std::string>(i) + ": got=" + show(got) + ", answer=" + show(ans);
}

template<typename T>
static std::string
showWhere(size_t i, size_t j, const Interval<T> &got, const Interval<T> &ans) {
    return ("i=" + boost::lexical_cast<std::string>(i) +
            ", j=" + boost::lexical_cast<std::string>(j) +
            ": got=" + show(got) +
            ", answer=" + show(ans));
}

static std::string
showWhere(size_t i, size_t j, size_t got, size_t ans) {
    return ("i=" + boost::lexical_cast<std::string>(i) +
            ", j=" + boost::lexical_cast<std::string>(j) +
            ": got=" + boost::lexical_cast<std::string>(got) +
            ", answer=" + boost::lexical_cast<std::string>(ans));
}

template<class AMap>
void checkNodes(AMap &m, const Interval<typename AMap::Address> &interval,
                const boost::iterator_range<typename AMap::ConstNodeIterator> &nodes) {
    if (interval.isEmpty()) {
        ASSERT_always_require2(nodes.begin()==m.nodes().end(), "empty interval");
        ASSERT_always_require2(nodes.end()==m.nodes().end(), "empty interval");
    } else {
        typename AMap::ConstNodeIterator first = m.find(interval.least());
        ASSERT_always_require(first != m.nodes().end());
        ASSERT_always_require(first == nodes.begin());
        typename AMap::ConstNodeIterator last = m.find(interval.greatest());
        ASSERT_always_require(last != m.nodes().end());
        ++last;
        ASSERT_always_require(last == nodes.end());
    }
}
        
template<typename Address, typename Value>
struct SegPred: SegmentPredicate<Address, Value> {
    virtual bool operator()(bool chain, const typename SegmentPredicate<Address, Value>::Args&) /*override*/ {
        return chain;
    }
};

// Test that all operations that can be applied to constant maps compile whether the map is const or mutable
template<class Map>
void compile_test_const(Map &s) {
    typedef typename Map::Address Address;
    typedef typename Map::Value Value;
    typedef Interval<typename Map::Address> Addresses;
    //typedef typename Map::Segment Segment;

    // Test constructors
    Map s2;
    Map s3(s);

    SegPred<Address, Value> segPred;

    // Test that constraints can be generated from a map
    s.require(Access::READABLE);
    s.prohibit(Access::READABLE);
    s.access(Access::READABLE);
    s.substr("one");
    s.at(100);
    s.at(Addresses::hull(0, 10));
    s.limit(5);
    s.atOrAfter(200);
    s.atOrBefore(300);
    s.within(Addresses::hull(100, 200));
    s.within(100, 200);
    s.baseSize(0, 1);
    s.after(100);
    s.before(100);
    s.singleSegment();
    s.segmentPredicate(&segPred);
    s.any();
    s.none();

    // Test that constraints can be augmented
    s.any().require(Access::READABLE);
    s.any().prohibit(Access::READABLE);
    s.any().access(Access::READABLE);
    s.any().substr("one");
    s.any().at(100);
    s.any().at(Addresses::hull(0, 10));
    s.any().limit(5);
    s.any().atOrAfter(200);
    s.any().atOrBefore(300);
    s.any().within(Addresses::hull(100, 200));
    s.any().within(100, 200);
    s.any().baseSize(100, 100);
    s.any().after(100);
    s.any().before(100);
    s.any().singleSegment();
    s.any().segmentPredicate(&segPred);
    s.any().any();
    s.any().none();

    // Operations for const or non-const maps on maps and constraints
    s.nSegments();

    s.segments(s.any());
    s.segments(s.any(), MATCH_BACKWARD);
    s.any().segments();
    s.any().segments(MATCH_BACKWARD);

    s.nodes();
    s.nodes(s.any());
    s.nodes(s.any(), MATCH_BACKWARD);
    s.any().nodes();
    s.any().nodes(MATCH_BACKWARD);

    s.next(s.any());
    s.next(s.any(), MATCH_BACKWARD);
    s.any().next();
    s.any().next(MATCH_BACKWARD);

    s.available(s.any());
    s.available(s.any(), MATCH_BACKWARD);
    s.any().available();
    s.any().available(MATCH_BACKWARD);

    s.exists(s.any());
    s.exists(s.any(), MATCH_BACKWARD);
    s.any().exists();
    s.any().exists(MATCH_BACKWARD);

    s.findNode(s.any());
    s.findNode(s.any(), MATCH_BACKWARD);
    s.any().findNode();
    s.any().findNode(MATCH_BACKWARD);

    std::vector<typename Map::Value> v;

    s.read(NULL, s.any());
    s.read(NULL, s.any(), MATCH_BACKWARD);
    s.any().read(NULL);
    s.any().read(NULL, MATCH_BACKWARD);
    s.read(v, s.any());
    s.read(v, s.any(), MATCH_BACKWARD);
    s.any().read(v);
    s.any().read(v, MATCH_BACKWARD);

    // Operations for const and non-const maps (but not on constraints)
    s.unmapped(0);
    s.unmapped(0, MATCH_BACKWARD);

    s.findFreeSpace(1);
    s.findFreeSpace(1, 2);
    s.findFreeSpace(1, 2, Addresses::whole());
    s.findFreeSpace(1, 2, Addresses::whole(), MATCH_BACKWARD);
}

// Test that all operations that can be applied to mutable maps compile
template<class Map>
void compile_test_mutable(Map &s) {
    std::vector<typename Map::Value> v;

    s.write(NULL, s.any());
    s.write(NULL, s.any(), MATCH_BACKWARD);
    s.any().write(NULL);
    s.any().write(NULL, MATCH_BACKWARD);
    s.write(v, s.any());
    s.write(v, s.any(), MATCH_BACKWARD);
    s.any().write(v);
    s.any().write(v, MATCH_BACKWARD);

    s.prune(s.any());
    s.prune(s.any(), MATCH_BACKWARD);
    s.any().prune();
    s.any().prune(MATCH_BACKWARD);

    s.keep(s.any());
    s.keep(s.any(), MATCH_BACKWARD);
    s.any().keep();
    s.any().keep(MATCH_BACKWARD);

    s.changeAccess(0, 0, s.any());
    s.changeAccess(0, 0, s.any(), MATCH_BACKWARD);
    s.any().changeAccess(0, 0);
    s.any().changeAccess(0, 0, MATCH_BACKWARD);
}

void compile_tests() {
    AddressMap<unsigned, char> s;
    compile_test_const(s);
    compile_test_mutable(s);
    const AddressMap<unsigned, char> &t = s;
    compile_test_const(t);
}

    // For loops that are safe when the index overflows
#define SAFE_FOR_UP(VAR, START, END)                                                                                           \
    for (Address start_=(START), end_=(END), oldIdx_=(START), VAR=(START);                                                     \
         VAR<=end_ && (VAR==start_ || oldIdx_<end_);                                                                           \
         oldIdx_=VAR++)
#define SAFE_FOR_DN(VAR, START, END)                                                                                           \
    for (Address start_=(START), end_=(END), oldIdx_=(START), VAR=(START);                                                     \
         VAR>=end_ && (VAR==start_ || oldIdx_>end_);                                                                           \
         oldIdx_=VAR--)
        

template<class AddressMap>
void constraint_tests(AddressMap &m) {
    typedef typename AddressMap::Address                Address;
    //typedef typename AddressMap::Value                  Value;
    typedef typename AddressMap::Segment                Segment;
    typedef Interval<Address>                           AInterval;
    //typedef boost::iterator_range<typename AddressMap::ConstNodeIterator> ANodes;

    // Mapping from address to segment number so we can compute expected answers
    std::cout <<"AddressMap:\n";
    Sawyer::Container::Map<Address, const Segment*> check;
    size_t nSegments = 0;
    BOOST_FOREACH (const typename AddressMap::Node &node, m.nodes()) {
        const AInterval &interval = node.key();
        const Segment &segment = node.value();
        std::cout <<"    #" <<nSegments <<": " <<show(interval) <<"\t"
                  <<(segment.accessibility() & Access::READABLE ? "r" : "-")
                  <<(segment.accessibility() & Access::WRITABLE ? "w" : "-")
                  <<(segment.accessibility() & Access::EXECUTABLE ? "x" : "-")
                  <<" size=" <<interval.size()
                  <<" {buffer=" <<segment.buffer()->name() <<"+" <<segment.offset() <<" sz=" <<segment.buffer()->size() <<"}"
                  <<"\n";
        SAFE_FOR_UP(i, interval.least(), interval.greatest())
            check.insert(i, &segment);
        ++nSegments;
    }

    // Choose an interval over which to iterate, preferably a bit larger than the map hull
    static const Address delta = 5;
    AInterval ispace = m.hull();
    if (ispace.isEmpty()) {
        ispace = AInterval::baseSize(AInterval::whole().least()+5, delta);
    } else {
        if (ispace.least() > AInterval::whole().least()) {
            Address n = ispace.least() - AInterval::whole().least();
            ispace = AInterval::hull(ispace.least()-std::min(n, delta), ispace.greatest());
        }
        if (AInterval::whole().greatest() > ispace.greatest()) {
            Address n = AInterval::whole().greatest() - ispace.greatest();
            ispace = AInterval::hull(ispace.least(), ispace.greatest() + std::min(n, delta));
        }
    }
    std::cout <<"  iteration interval " <<show(ispace) <<"\n";

    //----------------------------------------------------------------------------------------------------------------

    std::cout <<"  check: at(i).available()\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        if (check.exists(i)) {
            minAddr = maxAddr = i;
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                if (check.exists(j)) {
                    maxAddr = j;
                } else {
                    break;
                }
            }
        }
        AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.at(i).available();
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.at(i).nodes());
    }

    std::cout <<"  check: at(i).available(NONCONTIGUOUS)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        if (check.exists(i)) {
            minAddr = maxAddr = i;
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                if (check.exists(j))
                    maxAddr = j;
            }
        }
        AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.at(i).available(MATCH_NONCONTIGUOUS);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.at(i).nodes(MATCH_NONCONTIGUOUS));
    }

    std::cout <<"  check: at(i).available(MATCH_BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        if (check.exists(i)) {
            minAddr = maxAddr = i;
            SAFE_FOR_DN(j, i, ispace.least()) {
                if (check.exists(j)) {
                    minAddr = j;
                } else {
                    break;
                }
            }
        }
        AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.at(i).available(MATCH_BACKWARD);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.at(i).nodes(MATCH_BACKWARD));
    }

    std::cout <<"  check: at(i).available(NONCONTIGUOUS|BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        if (check.exists(i)) {
            minAddr = maxAddr = i;
            SAFE_FOR_DN(j, i, ispace.least()) {
                if (check.exists(j))
                    minAddr = j;
            }
        }
        AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.at(i).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.at(i).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
    }

    //---------------------------------------------------------------------------------------------------------------- 

    std::cout <<"  check: at([i,j]).available()\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            if (check.exists(i)) {
                minAddr = maxAddr = i;
                SAFE_FOR_UP(k, i, j) {
                    if (check.exists(k)) {
                        maxAddr = k;
                    } else {
                        break;
                    }
                }
            }
            AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.at(AInterval::hull(i, j)).available();
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.at(AInterval::hull(i, j)).nodes());
        }
    }
    
    std::cout <<"  check: at([i,j]).available(NONCONTIGUOUS)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            if (check.exists(i)) {
                minAddr = maxAddr = i;
                SAFE_FOR_UP(k, i, j) {
                    if (check.exists(k))
                        maxAddr = k;
                }
            }
            AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.at(AInterval::hull(i, j)).available(MATCH_NONCONTIGUOUS);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.at(AInterval::hull(i, j)).nodes(MATCH_NONCONTIGUOUS));
        }
    }
    
    std::cout <<"  check: at([i,j]).available(BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            if (check.exists(j)) {
                minAddr = maxAddr = j;
                SAFE_FOR_DN(k, j, i) {
                    if (check.exists(k)) {
                        minAddr = k;
                    } else {
                        break;
                    }
                }
            }
            AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.at(AInterval::hull(i, j)).available(MATCH_BACKWARD);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.at(AInterval::hull(i, j)).nodes(MATCH_BACKWARD));
        }
    }
    
    std::cout <<"  check: at([i,j]).available(NONCONTIGUOUS|BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            if (check.exists(j)) {
                minAddr = maxAddr = j;
                SAFE_FOR_DN(k, j, i) {
                    if (check.exists(k))
                        minAddr = k;
                }
            }
            AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.at(AInterval::hull(i, j)).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.at(AInterval::hull(i, j)).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
        }
    }
    
    //---------------------------------------------------------------------------------------------------------------- 

    std::cout <<"  check: at(i).limit(j).available()\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, 0, ispace.greatest()-i+1) {
            Address minAddr=1, maxAddr=0;
            if (j>0 && check.exists(i)) {
                minAddr = maxAddr = i;
                size_t nFound = 0;
                SAFE_FOR_UP(k, i, ispace.greatest()) {
                    if (nFound >= j)
                        break;
                    if (check.exists(k)) {
                        maxAddr = k;
                        ++nFound;
                    } else {
                        break;
                    }
                }
            }
            AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.at(i).limit(j).available();
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.at(i).limit(j).nodes());
        }
    }
    
    std::cout <<"  check: at(i).limit(j).available(NONCONTIGUOUS)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, 0, ispace.greatest()-i+1) {
            Address minAddr=10, maxAddr=0;
            if (j>0 && check.exists(i)) {
                minAddr = maxAddr = i;
                size_t nFound = 0;
                SAFE_FOR_UP(k, i, ispace.greatest()) {
                    if (nFound >= j)
                        break;
                    if (check.exists(k)) {
                        maxAddr = k;
                        ++nFound;
                    }
                }
            }
            AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.at(i).limit(j).available(MATCH_NONCONTIGUOUS);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.at(i).limit(j).nodes(MATCH_NONCONTIGUOUS));
        }
    }

    std::cout <<"  check: at(i).limit(j).available(BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, 0, ispace.greatest()-i+1) {
            Address minAddr=1, maxAddr=0;
            if (j>0 && check.exists(i)) {
                minAddr = maxAddr = i;
                size_t nFound = 0;
                SAFE_FOR_DN(k, i, ispace.least()) {
                    if (nFound >= j)
                        break;
                    if (check.exists(k)) {
                        minAddr = k;
                        ++nFound;
                    } else {
                        break;
                    }
                }
            }
            AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.at(i).limit(j).available(MATCH_BACKWARD);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.at(i).limit(j).nodes(MATCH_BACKWARD));
        }
    }
    
    std::cout <<"  check: at(i).limit(j).available(NONCONTIGUOUS|BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, 0, ispace.greatest()-i+1) {
            Address minAddr=1, maxAddr=0;
            if (j>0 && check.exists(i)) {
                minAddr = maxAddr = i;
                size_t nFound = 0;
                SAFE_FOR_DN(k, i, ispace.least()) {
                    if (nFound >= j)
                        break;
                    if (check.exists(k)) {
                        minAddr = k;
                        ++nFound;
                    }
                }
            }
            AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.at(i).limit(j).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.at(i).limit(j).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
        }
    }
    
    //---------------------------------------------------------------------------------------------------------------- 

    std::cout <<"  check: limit(i).available()\n";
    SAFE_FOR_UP(i, 0, ispace.size()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        size_t nFound = 0;
        SAFE_FOR_UP(j, ispace.least(), ispace.greatest()) {
            if (nFound >= i)
                break;
            if (check.exists(j)) {
                maxAddr = j;
                if (!inside) {
                    minAddr = j;
                    inside = true;
                }
                ++nFound;
            } else if (inside) {
                break;
            }
        }
        AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.limit(i).available();
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.limit(i).nodes());
    }

    std::cout <<"  check: limit(i).available(NONCONTIGUOUS)\n";
    SAFE_FOR_UP(i, 0, ispace.size()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        size_t nFound = 0;
        SAFE_FOR_UP(j, ispace.least(), ispace.greatest()) {
            if (nFound >= i)
                break;
            if (check.exists(j)) {
                maxAddr = j;
                if (!inside) {
                    minAddr = j;
                    inside = true;
                }
                ++nFound;
            }
        }
        AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.limit(i).available(MATCH_NONCONTIGUOUS);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.limit(i).nodes(MATCH_NONCONTIGUOUS));
    }

    std::cout <<"  check: limit(i).available(BACKWARD)\n";
    SAFE_FOR_UP(i, 0, ispace.size()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        size_t nFound = 0;
        SAFE_FOR_DN(j, ispace.greatest(), ispace.least()) {
            if (nFound >= i)
                break;
            if (check.exists(j)) {
                minAddr = j;
                if (!inside) {
                    maxAddr = j;
                    inside = true;
                }
                ++nFound;
            } else if (inside) {
                break;
            }
        }
        AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.limit(i).available(MATCH_BACKWARD);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.limit(i).nodes(MATCH_BACKWARD));
    }

    std::cout <<"  check: limit(i).available(NONCONTIGUOUS|BACKWARD)\n";
    SAFE_FOR_UP(i, 0, ispace.size()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        size_t nFound = 0;
        SAFE_FOR_DN(j, ispace.greatest(), ispace.least()) {
            if (nFound >= i)
                break;
            if (check.exists(j)) {
                minAddr = j;
                if (!inside) {
                    maxAddr = j;
                    inside = true;
                }
                ++nFound;
            }
        }
        AInterval answer = minAddr<=maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.limit(i).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.limit(i).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
    }

    //---------------------------------------------------------------------------------------------------------------- 

    std::cout <<"  check: atOrAfter(i).available()\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            if (check.exists(j)) {
                maxAddr = j;
                if (!inside)
                    minAddr = j;
                inside = true;
            } else if (inside) {
                break;
            }
        }
        AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.atOrAfter(i).available();
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.atOrAfter(i).nodes());
    }
    
    std::cout <<"  check: atOrAfter(i).available(NONCONTIGUOUS)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            if (check.exists(j)) {
                maxAddr = j;
                if (!inside)
                    minAddr = j;
                inside = true;
            }
        }
        AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.atOrAfter(i).available(MATCH_NONCONTIGUOUS);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.atOrAfter(i).nodes(MATCH_NONCONTIGUOUS));
    }

    std::cout <<"  check: atOrAfter(i).available(BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        SAFE_FOR_DN(j, ispace.greatest(), i) {
            if (check.exists(j)) {
                minAddr = j;
                if (!inside)
                    maxAddr = j;
                inside = true;
            } else if (inside) {
                break;
            }
        }
        AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.atOrAfter(i).available(MATCH_BACKWARD);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.atOrAfter(i).nodes(MATCH_BACKWARD));
    }
    
    std::cout <<"  check: atOrAfter(i).available(NONCONTIGUOUS|BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        SAFE_FOR_DN(j, ispace.greatest(), i) {
            if (check.exists(j)) {
                minAddr = j;
                if (!inside)
                    maxAddr = j;
                inside = true;
            }
        }
        AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.atOrAfter(i).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.atOrAfter(i).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
    }

    //---------------------------------------------------------------------------------------------------------------- 
    
    
    std::cout <<"  check: atOrBefore(i).available()\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        SAFE_FOR_UP(j, ispace.least(), i) {
            if (check.exists(j)) {
                maxAddr = j;
                if (!inside)
                    minAddr = j;
                inside = true;
            } else if (inside) {
                break;
            }
        }
        AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.atOrBefore(i).available();
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.atOrBefore(i).nodes());
    }

    std::cout <<"  check: atOrBefore(i).available(NONCONTIGUOUS)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        SAFE_FOR_UP(j, ispace.least(), i) {
            if (check.exists(j)) {
                maxAddr = j;
                if (!inside)
                    minAddr = j;
                inside = true;
            }
        }
        AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.atOrBefore(i).available(MATCH_NONCONTIGUOUS);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.atOrBefore(i).nodes(MATCH_NONCONTIGUOUS));
    }

    std::cout <<"  check: atOrBefore(i).available(BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        SAFE_FOR_DN(j, i, ispace.least()) {
            if (check.exists(j)) {
                minAddr = j;
                if (!inside)
                    maxAddr = j;
                inside = true;
            } else if (inside) {
                break;
            }
        }
        AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.atOrBefore(i).available(MATCH_BACKWARD);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.atOrBefore(i).nodes(MATCH_BACKWARD));
    }

    std::cout <<"  check: atOrBefore(i).available(NONCONTIGUOUS|BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        Address minAddr=1, maxAddr=0;
        bool inside = false;
        SAFE_FOR_DN(j, i, ispace.least()) {
            if (check.exists(j)) {
                minAddr = j;
                if (!inside)
                    maxAddr = j;
                inside = true;
            }
        }
        AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
        AInterval got = m.atOrBefore(i).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
        ASSERT_always_require2(got==answer, showWhere(i, got, answer));
        checkNodes(m, got, m.atOrBefore(i).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
    }

    //---------------------------------------------------------------------------------------------------------------- 

    std::cout <<"  check: within(i,j).available()\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            bool inside = false;
            SAFE_FOR_UP(k, i, j) {
                if (check.exists(k)) {
                    maxAddr = k;
                    if (!inside) {
                        minAddr = k;
                        inside = true;
                    }
                } else if (inside) {
                    break;
                }
            }
            AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.within(i, j).available();
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.within(i, j).nodes());
        }
    }
    
    std::cout <<"  check: within(i,j).available(NONCONTIGUOUS)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            bool inside = false;
            SAFE_FOR_UP(k, i, j) {
                if (check.exists(k)) {
                    maxAddr = k;
                    if (!inside) {
                        minAddr = k;
                        inside = true;
                    }
                }
            }
            AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.within(i, j).available(MATCH_NONCONTIGUOUS);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.within(i, j).nodes(MATCH_NONCONTIGUOUS));
        }
    }
    
    std::cout <<"  check: within(i,j).available(BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            bool inside = false;
            SAFE_FOR_DN(k, j, i) {
                if (check.exists(k)) {
                    minAddr = k;
                    if (!inside) {
                        maxAddr = k;
                        inside = true;
                    }
                } else if (inside) {
                    break;
                }
            }
            AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.within(i, j).available(MATCH_BACKWARD);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.within(i, j).nodes(MATCH_BACKWARD));
        }
    }
    
    std::cout <<"  check: within(i,j).available(NONCONTIGUOUS|BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            bool inside = false;
            SAFE_FOR_DN(k, j, i) {
                if (check.exists(k)) {
                    minAddr = k;
                    if (!inside) {
                        maxAddr = k;
                        inside = true;
                    }
                }
            }
            AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.within(i, j).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.within(i, j).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
        }
    }

    //---------------------------------------------------------------------------------------------------------------- 

    std::cout <<"  check: within(i,j).singleSegment().available()\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            const Segment *firstSegment=NULL, *segment=NULL;
            SAFE_FOR_UP(k, i, j) {
                if (check.getOptional(k).assignTo(segment) && (!firstSegment || firstSegment==segment)) {
                    maxAddr = k;
                    if (!firstSegment) {
                        minAddr = k;
                        firstSegment = segment;
                    }
                } else if (firstSegment) {
                    break;
                }
            }
            AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.within(i, j).singleSegment().available();
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.within(i, j).singleSegment().nodes());
        }
    }

    std::cout <<"  check: within(i,j).singleSegment().available(NONCONTIGUOUS)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            const Segment *firstSegment=NULL, *segment=NULL;
            SAFE_FOR_UP(k, i, j) {
                if (check.getOptional(k).assignTo(segment) && (!firstSegment || firstSegment==segment)) {
                    maxAddr = k;
                    if (!firstSegment) {
                        minAddr = k;
                        firstSegment = segment;
                    }
                } else if (firstSegment) {
                    break;
                }
            }
            AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.within(i, j).singleSegment().available(MATCH_NONCONTIGUOUS);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.within(i, j).singleSegment().nodes(MATCH_NONCONTIGUOUS));
        }
    }
    
    std::cout <<"  check: within(i,j).singleSegment().available(BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            const Segment *firstSegment=NULL, *segment=NULL;
            SAFE_FOR_DN(k, j, i) {
                if (check.getOptional(k).assignTo(segment) && (!firstSegment || firstSegment==segment)) {
                    minAddr = k;
                    if (!firstSegment) {
                        maxAddr = k;
                        firstSegment = segment;
                    }
                } else if (firstSegment) {
                    break;
                }
            }
            AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.within(i, j).singleSegment().available(MATCH_BACKWARD);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.within(i, j).singleSegment().nodes(MATCH_BACKWARD));
        }
    }
    
    std::cout <<"  check: within(i,j).singleSegment().available(NONCONTIGUOUS|BACKWARD)\n";
    SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
        SAFE_FOR_UP(j, i, ispace.greatest()) {
            Address minAddr=1, maxAddr=0;
            const Segment *firstSegment=NULL, *segment=NULL;
            SAFE_FOR_DN(k, j, i) {
                if (check.getOptional(k).assignTo(segment) && (!firstSegment || firstSegment==segment)) {
                    minAddr = k;
                    if (!firstSegment) {
                        maxAddr = k;
                        firstSegment = segment;
                    }
                } else if (firstSegment) {
                    break;
                }
            }
            AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
            AInterval got = m.within(i, j).singleSegment().available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
            ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
            checkNodes(m, got, m.within(i, j).singleSegment().nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
        }
    }

    //---------------------------------------------------------------------------------------------------------------- 

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).require(READABLE).available()\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).require(READWRITE).available()\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_UP(k, i, j) {
                    if (check.getOptional(k).assignTo(segment) && access == (segment->accessibility() & access)) {
                        maxAddr = k;
                        if (!inside) {
                            minAddr = k;
                            inside = true;
                        }
                    } else if (inside) {
                        break;
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).require(access).available();
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).require(access).nodes());
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).require(READABLE).available(NONCONTIGUOUS)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).require(READWRITE).available(NONCONTIGUOUS)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_UP(k, i, j) {
                    if (check.getOptional(k).assignTo(segment)) {
                        if (access == (segment->accessibility() & access)) {
                            maxAddr = k;
                            if (!inside) {
                                minAddr = k;
                                inside = true;
                            }
                        } else if (inside) {
                            break;
                        }
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).require(access).available(MATCH_NONCONTIGUOUS);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).require(access).nodes(MATCH_NONCONTIGUOUS));
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).require(READABLE).available(BACKWARD)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).require(READWRITE).available(BACKWARD)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_DN(k, j, i) {
                    if (check.getOptional(k).assignTo(segment) && access == (segment->accessibility() & access)) {
                        minAddr = k;
                        if (!inside) {
                            maxAddr = k;
                            inside = true;
                        }
                    } else if (inside) {
                        break;
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).require(access).available(MATCH_BACKWARD);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).require(access).nodes(MATCH_BACKWARD));
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).require(READABLE).available(NONCONTIGUOUS|BACKWARD)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).require(READWRITE).available(NONCONTIGUOUS|BACKWARD)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_DN(k, j, i) {
                    if (check.getOptional(k).assignTo(segment)) {
                        if (access == (segment->accessibility() & access)) {
                            minAddr = k;
                            if (!inside) {
                                maxAddr = k;
                                inside = true;
                            }
                        } else if (inside) {
                            break;
                        }
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).require(access).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).require(access).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------- 

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).prohibit(READABLE).available()\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).prohibit(READWRITE).available()\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_UP(k, i, j) {
                    if (check.getOptional(k).assignTo(segment) && 0 == (segment->accessibility() & access)) {
                        maxAddr = k;
                        if (!inside) {
                            minAddr = k;
                            inside = true;
                        }
                    } else if (inside) {
                        break;
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).prohibit(access).available();
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).prohibit(access).nodes());
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).prohibit(READABLE).available(NONCONTIGUOUS)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).prohibit(READWRITE).available(NONCONTIGUOUS)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_UP(k, i, j) {
                    if (check.getOptional(k).assignTo(segment)) {
                        if (0 == (segment->accessibility() & access)) {
                            maxAddr = k;
                            if (!inside) {
                                minAddr = k;
                                inside = true;
                            }
                        } else if (inside) {
                            break;
                        }
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).prohibit(access).available(MATCH_NONCONTIGUOUS);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).prohibit(access).nodes(MATCH_NONCONTIGUOUS));
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).prohibit(READABLE).available(BACKWARD)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).prohibit(READWRITE).available(BACKWARD)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_DN(k, j, i) {
                    if (check.getOptional(k).assignTo(segment) && 0 == (segment->accessibility() & access)) {
                        minAddr = k;
                        if (!inside) {
                            maxAddr = k;
                            inside = true;
                        }
                    } else if (inside) {
                        break;
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).prohibit(access).available(MATCH_BACKWARD);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).prohibit(access).nodes(MATCH_BACKWARD));
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).prohibit(READABLE).available(NONCONTIGUOUS|BACKWARD)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).prohibit(READWRITE).available(NONCONTIGUOUS|BACKWARD)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_DN(k, j, i) {
                    if (check.getOptional(k).assignTo(segment)) {
                        if (0 == (segment->accessibility() & access)) {
                            minAddr = k;
                            if (!inside) {
                                maxAddr = k;
                                inside = true;
                            }
                        } else if (inside) {
                            break;
                        }
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).prohibit(access).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).prohibit(access).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------- 

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).access(READABLE).available()\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).access(READWRITE).available()\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_UP(k, i, j) {
                    if (check.getOptional(k).assignTo(segment) && access == segment->accessibility()) {
                        maxAddr = k;
                        if (!inside) {
                            minAddr = k;
                            inside = true;
                        }
                    } else if (inside) {
                        break;
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).access(access).available();
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).access(access).nodes());
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).access(READABLE).access(NONCONTIGUOUS)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).access(READWRITE).access(NONCONTIGUOUS)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_UP(k, i, j) {
                    if (check.getOptional(k).assignTo(segment)) {
                        if (access == segment->accessibility()) {
                            maxAddr = k;
                            if (!inside) {
                                minAddr = k;
                                inside = true;
                            }
                        } else if (inside) {
                            break;
                        }
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).access(access).available(MATCH_NONCONTIGUOUS);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).access(access).nodes(MATCH_NONCONTIGUOUS));
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).access(READABLE).available(BACKWARD)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).access(READWRITE).available(BACKWARD)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_DN(k, j, i) {
                    if (check.getOptional(k).assignTo(segment) && access == segment->accessibility()) {
                        minAddr = k;
                        if (!inside) {
                            maxAddr = k;
                            inside = true;
                        }
                    } else if (inside) {
                        break;
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).access(access).available(MATCH_BACKWARD);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).access(access).nodes(MATCH_BACKWARD));
            }
        }
    }

    for (int a=0; a<2; a++) {
        unsigned access = 0;
        switch (a) {
            case 0:
                std::cout <<"  check: within(i,j).access(READABLE).available(NONCONTIGUOUS|BACKWARD)\n";
                access = Access::READABLE;
                break;
            case 1:
                std::cout <<"  check: within(i,j).access(READWRITE).available(NONCONTIGUOUS|BACKWARD)\n";
                access = Access::READABLE | Access::WRITABLE;
                break;
        }
        SAFE_FOR_UP(i, ispace.least(), ispace.greatest()) {
            SAFE_FOR_UP(j, i, ispace.greatest()) {
                Address minAddr=1, maxAddr=0;
                bool inside = false;
                const Segment *segment;
                SAFE_FOR_DN(k, j, i) {
                    if (check.getOptional(k).assignTo(segment)) {
                        if (access == segment->accessibility()) {
                            minAddr = k;
                            if (!inside) {
                                maxAddr = k;
                                inside = true;
                            }
                        } else if (inside) {
                            break;
                        }
                    }
                }
                AInterval answer = minAddr <= maxAddr ? AInterval::hull(minAddr, maxAddr) : AInterval();
                AInterval got = m.within(i, j).access(access).available(MATCH_NONCONTIGUOUS|MATCH_BACKWARD);
                ASSERT_always_require2(got==answer, showWhere(i, j, got, answer));
                checkNodes(m, got, m.within(i, j).access(access).nodes(MATCH_NONCONTIGUOUS|MATCH_BACKWARD));
            }
        }
    }

}

template<class MemoryMap>
static void io_tests(MemoryMap &m) {
    typedef typename MemoryMap::Value Value;
    typedef typename MemoryMap::Address Address;
    typedef Interval<Address> Interval;
    //typedef typename MemoryMap::Segment Segment;

    // Write consecutive values to all addresses
    std::cout <<"  check: at(i).limit(3).write(buf)\n";
    Sawyer::Container::Map<Address, Value> check;
    Value buf[3], counter=0;
    Address addr = m.hull().least();
    while (m.atOrAfter(addr).next().assignTo(addr)) {
        Interval avail = m.at(addr).limit(3).available();
        ASSERT_always_require(!avail.isEmpty());
        ASSERT_always_require(avail.size()<=3);
        for (size_t i=0; i<3; ++i) {
            if (i<avail.size()) {
                check.insert(addr+i, (buf[i] = ++counter));
            } else {
                buf[i] = 0;
            }
        }
        Interval written = m.at(addr).limit(3).write(buf);
        ASSERT_always_require(written==avail);
        if (written.greatest()==m.hull().greatest())
            break;                                      // protect next line from overflow
        addr += written.size();
    }

    // Read values back one, two, and three at a time to compare
    for (size_t bufsz=1; bufsz<=3; bufsz++) {
        std::cout <<"  check: at(i).limit(" <<bufsz <<").read(buf)\n";
        addr = m.hull().least();
        while (m.atOrAfter(addr).next().assignTo(addr)) {
            Interval read = m.at(addr).limit(bufsz).read(buf);
            ASSERT_always_require(!read.isEmpty());
            ASSERT_always_require(read.size() <= bufsz);
            ASSERT_always_require(read.least()==addr);
            for (size_t i=0; i<read.size(); ++i) {
                ASSERT_always_require(check.exists(addr+i));
                ASSERT_always_require(buf[i]==check[addr+i]);
            }
            if (read.greatest()==m.hull().greatest())
                break;                                  // protect next line from overflow
            addr += read.size();
        }
    }
}

static void test00() {
    typedef boost::uint16_t Address;
    typedef AddressMap<Address, char> AMap;
    typedef Interval<Address> AInterval;

    
    // basic test: {10-14, 20-24, 30-34, 40-44}
    AMap m1;
    for (size_t i=0; i<4; ++i) {
        unsigned access = (i<2?(unsigned)Access::READABLE:0) | (i%2?(unsigned)Access::WRITABLE:0);
        m1.insert(AInterval::baseSize(10*(i+1), 5), AMap::Segment::anonymousInstance(5, access));
    }
    ASSERT_always_require(m1.nIntervals()==4);
    ASSERT_always_require2(m1.size()==20, showWhere(m1.size(), 20));
    ASSERT_always_require(boost::distance(m1.nodes())==4);
    constraint_tests(m1);
    io_tests(m1);

    // adjacent segments: {10-14, 20-24, 25-29, 30-34, 40-44}
    AMap m2 = m1;
    m2.insert(AInterval::hull(25, 29), AMap::Segment::anonymousInstance(5, Access::READABLE));
    ASSERT_always_require(m2.nIntervals()==5);
    ASSERT_always_require2(m2.size()==25, showWhere(m2.size(), 25));
    ASSERT_always_require(boost::distance(m2.nodes())==5);
    constraint_tests(m2);
    io_tests(m2);

    // bottom of address space: {0-4, 10-14}
    AMap m3;
    m3.insert(AInterval::baseSize( 0, 5), AMap::Segment::anonymousInstance(5, Access::READABLE));
    m3.insert(AInterval::baseSize(10, 5), AMap::Segment::anonymousInstance(5, Access::READABLE));
    ASSERT_always_require(m3.nIntervals()==2);
    ASSERT_always_require2(m3.size()==10, showWhere(m3.size(), 10));
    ASSERT_always_require(boost::distance(m3.nodes())==2);
    constraint_tests(m3);
    io_tests(m3);

    // top of address space: { 65525-65529, 65531-65535 }
    AMap m4;
    m4.insert(AInterval::hull(65525, 65529), AMap::Segment::anonymousInstance(5, Access::READABLE));
    m4.insert(AInterval::hull(65531, 65535), AMap::Segment::anonymousInstance(5, Access::READABLE));
    ASSERT_always_require(m4.nIntervals()==2);
    ASSERT_always_require2(m4.size()==10, showWhere(m4.size(), 10));
    ASSERT_always_require(boost::distance(m4.nodes())==2);
    constraint_tests(m4);
    io_tests(m4);
}

static void test01() {
    typedef unsigned Address;
    typedef Interval<Address> Addresses;
    typedef Buffer<Address, char>::Ptr BufferPtr;
    typedef AddressSegment<Address, char> Segment;
    typedef AddressMap<Address, char> MemoryMap;

    // Allocate a couple distinct buffers.
    BufferPtr buf1 = Sawyer::Container::AllocatingBuffer<Address, char>::instance(5);
    BufferPtr buf2 = Sawyer::Container::AllocatingBuffer<Address, char>::instance(5);

    // Map them to neighboring locations in the address space
    MemoryMap map;
    map.insert(Addresses::hull(1000, 1004), Segment(buf2));
    map.insert(Addresses::hull(1005, 1009), Segment(buf1));

    // Write something across the two buffers using mapped I/O
    static const char *data1 = "abcdefghij";
    Addresses accessed = map.at(Addresses::hull(1000, 1009)).write(data1);
    ASSERT_always_require(accessed.size()==10);

    // Read back the data
    char data2[10];
    memset(data2, 0, sizeof data2);
    accessed = map.at(1000).limit(10).read(data2);
    ASSERT_always_require(accessed.size()==10);
    ASSERT_always_require(0==memcmp(data1, data2, 10));

    // See what's in the individual buffers
    memset(data2, 0, sizeof data2);
    Address nread = buf1->read(data2, 0, 5);
    ASSERT_always_require(nread==5);
    ASSERT_always_require(0==memcmp(data2, "fghij", 5));

    memset(data2, 0, sizeof data2);
    nread = buf2->read(data2, 0, 5);
    ASSERT_always_require(nread==5);
    ASSERT_always_require(0==memcmp(data2, "abcde", 5));
}

static void test02() {
    typedef unsigned Address;
    typedef Interval<Address> Addresses;
    typedef Buffer<Address, char>::Ptr BufferPtr;
    typedef AddressSegment<Address, char> Segment;
    typedef AddressMap<Address, char> MemoryMap;

    // Create some buffer objects
    char data1[15];
    memcpy(data1, "---------------", 15);
    BufferPtr buf1 = Sawyer::Container::StaticBuffer<Address, char>::instance(data1, 15);
    char data2[10];
    memcpy(data2, "##########", 10);
    BufferPtr buf2 = Sawyer::Container::StaticBuffer<Address, char>::instance(data2, 5); // using only first 5 bytes

    // Map data2 into the middle of data1
    MemoryMap map;
    map.insert(Addresses::baseSize(1000, 15), Segment(buf1));
    map.insert(Addresses::baseSize(1005,  5), Segment(buf2)); 

    // Write across both buffers and check that data2 occluded data1
    Addresses accessed = map.at(1001).limit(13).write("bcdefghijklmn");
    ASSERT_always_require2(accessed.size()==13, showWhere(1001, 13, accessed.size(), 13));
    ASSERT_always_require(0==memcmp(data1, "-bcde-----klmn-", 15));
    ASSERT_always_require(0==memcmp(data2,      "fghij#####", 10));

    // Map the middle of data1 over the top of data2 again and check that the mapping has one element. I.e., the three
    // separate parts were recombined into a single entry since they are three consecutive areas of a single buffer.
    map.insert(Addresses::baseSize(1005, 5), Segment(buf1, 5));
    ASSERT_always_require(map.nSegments()==1);

    // Write some data again
    accessed = map.at(1001).limit(13).write("BCDEFGHIJKLMN");
    ASSERT_always_require2(accessed.size()==13, showWhere(1001, 13, accessed.size(), 13));
    ASSERT_always_require(0==memcmp(data1, "-BCDEFGHIJKLMN-", 15));
    ASSERT_always_require(0==memcmp(data2,      "fghij#####", 10));
}

static void test03() {
    typedef unsigned Address;
    typedef Interval<Address> Addresses;
    typedef Buffer<Address, char>::Ptr BufferPtr;
    typedef AddressSegment<Address, char> Segment;
    typedef AddressMap<Address, char> MemoryMap;

    boost::iostreams::mapped_file_params mfp;
    mfp.path = "/etc/passwd";
    mfp.flags = boost::iostreams::mapped_file_base::priv;
    BufferPtr buf1 = Sawyer::Container::MappedBuffer<Address, char>::instance(mfp);

    MemoryMap map;
    map.insert(Addresses::baseSize(1000, 64), Segment(buf1));

    char data[64];
    Address nread = map.at(1000).limit(64).read(data).size();
    data[sizeof(data)-1] = '\0';
    ASSERT_always_require(nread==64);
    std::cout <<data <<"...\n";
}

static void test04() {
    typedef unsigned Address;
    typedef Interval<Address> Addresses;
    typedef boost::uint8_t Value;
    typedef Buffer<Address, Value>::Ptr BufferPtr;
    typedef AddressSegment<Address, Value> Segment;
    typedef AddressMap<Address, Value> MemoryMap;

    BufferPtr buf1 = Sawyer::Container::AllocatingBuffer<Address, Value>::instance(50);
    Segment seg1 = Segment(buf1, 0, Access::READABLE);

    std::cout <<"Test that changing accessibility for the middle of a segment works\n";
    MemoryMap map;
    map.insert(Addresses::hull(50, 99), seg1);
    map.at(75).limit(5).changeAccess(Access::WRITABLE, 0);// add write permission
    int i=0;
    BOOST_FOREACH (const MemoryMap::Node &node, map.nodes()) {
        std::cout <<"  [" <<node.key().least() <<", " <<node.key().greatest() <<"]\n";
        switch (i) {
            case 0:
                ASSERT_always_require(node.key().least() == 50);
                ASSERT_always_require(node.key().greatest() == 74);
                ASSERT_always_require(node.value().accessibility() == Access::READABLE);
                break;
            case 1:
                ASSERT_always_require(node.key().least() == 75);
                ASSERT_always_require(node.key().greatest() == 79);
                ASSERT_always_require(node.value().accessibility() == (Access::READABLE | Access::WRITABLE));
                break;
            case 2:
                ASSERT_always_require(node.key().least() == 80);
                ASSERT_always_require(node.key().greatest() == 99);
                ASSERT_always_require(node.value().accessibility() == Access::READABLE);
                break;
            default:
                ASSERT_not_reachable("should have only three nodes");
        }
        ++i;
    }

    std::cout <<"Test that changing accessibility across a gap works\n";
    map.clear();
    map.insert(Addresses::hull(10, 19), seg1);
    map.insert(Addresses::hull(30, 39), seg1);
    map.at(15).before(35).changeAccess(Access::WRITABLE, 0);// add write permission
    i = 0;
    BOOST_FOREACH (const MemoryMap::Node &node, map.nodes()) {
        std::cout <<"  [" <<node.key().least() <<", " <<node.key().greatest() <<"]\n";
        switch (i) {
            case 0:
                ASSERT_always_require(node.key().least() == 10);
                ASSERT_always_require(node.key().greatest() == 14);
                ASSERT_always_require(node.value().accessibility() == Access::READABLE);
                break;
            case 1:
                ASSERT_always_require(node.key().least() == 15);
                ASSERT_always_require(node.key().greatest() == 19);
                ASSERT_always_require(node.value().accessibility() == (Access::READABLE | Access::WRITABLE));
                break;
            case 2:
                ASSERT_always_require(node.key().least() == 30);
                ASSERT_always_require(node.key().greatest() == 34);
                ASSERT_always_require(node.value().accessibility() == (Access::READABLE | Access::WRITABLE));
                break;
            case 3:
                ASSERT_always_require(node.key().least() == 35);
                ASSERT_always_require(node.key().greatest() == 39);
                ASSERT_always_require(node.value().accessibility() == Access::READABLE);
                break;
            default:
                ASSERT_not_reachable("should have only four nodes");
        }
        ++i;
    }
}

struct Traversal {
    std::vector<Interval<unsigned> > expected;
    size_t index;

    Traversal(): index(0) {}

    void insert(const Interval<unsigned> &interval) { expected.push_back(interval); }

    bool operator()(const AddressMap<unsigned, int> &/*map*/, const Interval<unsigned> &interval) {
        std::cout <<"    processing interval #" <<index <<": [" <<interval.least() <<", " <<interval.greatest() <<"]\n";
        ASSERT_always_require2(index < expected.size(),
                               "expected only " + boost::lexical_cast<std::string>(expected.size()) +
                               (1==expected.size() ? " interval" : " intervals"));
        ASSERT_always_require2(interval == expected[index],
                               "expected interval [" + boost::lexical_cast<std::string>(expected[index].least()) +
                               ", " + boost::lexical_cast<std::string>(expected[index].greatest()) + "]");
        ++index;
        return true;
    }

    void check() const {
        ASSERT_always_require2(index >= expected.size(),
                               "expected " + boost::lexical_cast<std::string>(expected.size()) +
                               (1==expected.size()?" interval":" intervals") + " but got " +
                               boost::lexical_cast<std::string>(index));
    }
};

typedef unsigned test05_Address;
typedef Interval<test05_Address> test05_Addresses;
typedef int test05_Value;
typedef AddressMap<test05_Address, test05_Value> test05_MemoryMap;

struct test05_Traversal: test05_MemoryMap::Visitor {
    bool operator()(const test05_MemoryMap &m, const test05_Addresses &interval) {
        std::cout <<"    interval [" <<interval.least() <<", " <<interval.greatest() <<"]\n";
        const test05_MemoryMap::Segment &segment = m.at(interval.least()).findNode()->value();
        std::cout <<"      segment name = \"" <<segment.name() <<"\"\n";
        return true;
    }
};

static void test05() {
    typedef test05_Address Address;
    typedef test05_Addresses Addresses;
    typedef test05_Value Value;
    typedef Buffer<Address, Value>::Ptr BufferPtr;
    typedef AddressSegment<Address, Value> Segment;
    typedef test05_MemoryMap MemoryMap;

    Value values[50];
    for (size_t i=0; i<50; ++i)
        values[i] = i;
    BufferPtr buf1 = Sawyer::Container::StaticBuffer<Address, Value>::instance(values, 50);
    Segment seg1 = Segment(buf1, 0, Access::READABLE);

    std::cout <<"Test traversal\n";
    MemoryMap map;
    map.insert(Addresses::baseSize(100, 50), Segment(buf1, 0, 0, "first"));
    map.insert(Addresses::baseSize(150, 50), Segment(buf1, 0, 0, "second"));
    map.insert(Addresses::baseSize(200, 50), Segment(buf1, 0, 0, "third"));
    map.insert(Addresses::baseSize(10000, 50), Segment(buf1, 0, 0, "fourth"));
    map.checkConsistency();

    std::cerr <<"  traverse everything\n";
    {
        Traversal t1;
        t1.insert(Addresses::baseSize(100, 50));
        t1.insert(Addresses::baseSize(150, 50));
        t1.insert(Addresses::baseSize(200, 50));
        t1.insert(Addresses::baseSize(10000, 50));
        map.any().traverse(t1);
        t1.check();
    }

    std::cerr <<"  traverse contiguous\n";
    {
        Traversal t1;
        t1.insert(Addresses::baseSize(100, 50));
        t1.insert(Addresses::baseSize(150, 50));
        t1.insert(Addresses::baseSize(200, 50));
        map.any().traverse(t1, MATCH_CONTIGUOUS);
        t1.check();
    }

    std::cerr <<"  traverse subset\n";
    {
        Traversal t1;
        t1.insert(Addresses::baseSize(125, 25));
        t1.insert(Addresses::baseSize(150, 25));
        map.atOrAfter(125).limit(50).traverse(t1);
        t1.check();
    }

    std::cerr <<"  traverse backward\n";
    {
        Traversal t1;
        t1.insert(Addresses::hull(100, 149));
        t1.insert(Addresses::hull(150, 175));
        map.atOrBefore(175).traverse(t1, MATCH_BACKWARD);
        t1.check();
    }

    std::cerr <<"  local traversal\n";
    test05_Traversal t2;
    map.any().traverse(t2);
    
}

static void testCopyOnWrite() {
    typedef unsigned Address;
    typedef char Value;
    typedef AddressMap<Address, Value> Map;
    typedef Interval<Address> Addresses;

    std::cout <<"Test copy-on-write\n";

    char s1[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
    Map::Buffer::Ptr buf1 = AllocatingBuffer<Address, Value>::instance(10);

    Map map1;
    map1.insert(Addresses::baseSize(100, 10), Map::Segment(buf1));
    map1.at(100).limit(10).write(s1);

    // Copy map1 to create map2
    Map map2(map1);
    char s2[10];
    map2.at(100).limit(10).read(s2);
    for (size_t i=0; i<10; ++i)
        ASSERT_always_require(s2[i]==s1[i]);

    // map1 and map2 share buffers, so changing one will change the other
    s2[0] = 'A';
    map2.at(100).limit(1).write(s2);
    map1.at(100).limit(1).read(s1);
    ASSERT_always_require(s2[0]==s1[0]);

    // Copy map1 to map3, but set copy-on-write.
    Map map3(map1, true);
    char s3[10];
    map3.at(100).limit(10).read(s3);
    for (size_t i=0; i<10; ++i)
        ASSERT_always_require(s3[i]==s1[i]);

    // Writing to any map will now cause the buffer to be copied first.
    s3[0] = '3';
    map3.at(100).limit(1).write(s3);
    s3[0] = '\0';
    map3.at(100).limit(1).read(s3);
    ASSERT_always_require(s3[0] == '3');
    map1.at(100).limit(1).read(s1);
    ASSERT_always_require(s1[0] == 'A');                // unchanged
    map2.at(100).limit(1).read(s2);
    ASSERT_always_require(s2[0] == 'A');                // unchanged

    // Now that copy-on-write is set for the buffer, writing to either map1 or map2 will break their sharing.
    s1[0] = '1';
    map1.at(100).limit(1).write(s1);
    s1[0] = '\0';
    map1.at(100).limit(1).read(s1);
    ASSERT_always_require(s1[0] == '1');
    s2[0] = '\0';
    map2.at(100).limit(1).read(s2);
    ASSERT_always_require(s2[0] == 'A');                // still not changed
    s3[0] = '\0';
    map3.at(100).limit(1).read(s3);
    ASSERT_always_require(s3[0] == '3');                // unchanged

    // We've written to map1 and map3, therefore their copy-on-write bits should be cleared now but map2's
    // copy-on-write is still set (even though map2 is the only thing using that buffer).
    ASSERT_always_require(map1.find(100)->value().buffer()->copyOnWrite() == false);
    ASSERT_always_require(map2.find(100)->value().buffer()->copyOnWrite() == true);
    ASSERT_always_require(map3.find(100)->value().buffer()->copyOnWrite() == false);
}

int main() {
    Sawyer::initializeLibrary();

    test00();
    test01();
    test02();
    test03();
    test04();
    test05();
    testCopyOnWrite();
}
