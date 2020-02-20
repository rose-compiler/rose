// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Interval.h>
#include <Sawyer/IntervalSet.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Stopwatch.h>
#include <boost/foreach.hpp>
#include <boost/range/distance.hpp>
#include <iostream>

// Use std::pair as a value.  Works since it satisfies the minimal API: copy constructor, assignment operator, and equality.
// The output function is only needed for this test file so we can print the contents of the IntervalMap
typedef std::pair<int, long> Pair;
std::ostream& operator<<(std::ostream &o, const Pair &p) {
    o <<"(" <<p.first <<"," <<p.second <<")";
    return o;
}

template<typename T>
std::ostream& operator<<(std::ostream &o, const Sawyer::Container::Interval<T> &interval) {
    o <<"[" <<interval.least() <<"," <<interval.greatest() <<"]";
    return o;
}

template<class Interval, class T, class Policy>
static void show(const Sawyer::Container::IntervalMap<Interval, T, Policy> &imap) {
    typedef typename Sawyer::Container::IntervalMap<Interval, T, Policy> Map;
    std::cerr <<"  size = " <<(boost::uint64_t)imap.size() <<" in " <<imap.nIntervals() <<"\n";
    std::cerr <<"  nodes = {";
    for (typename Map::ConstNodeIterator iter=imap.nodes().begin(); iter!=imap.nodes().end(); ++iter) {
        const Interval &interval = iter->key();
        const T &value = iter->value();
        std::cerr <<" [" <<interval.least() <<"," <<interval.greatest() <<"]=" <<value;
    }
    std::cerr <<" }\n";
}

template<class Interval>
static void show(const Sawyer::Container::IntervalSet<Interval> &iset) {
    std::cerr <<"  size = " <<(boost::uint64_t)iset.size() <<" in " <<iset.nIntervals() <<"\n";
    std::cerr <<"  nodes = {";
    typedef typename Sawyer::Container::IntervalSet<Interval>::ConstIntervalIterator Iter;
    for (Iter iter=iset.intervals().begin(); iter!=iset.intervals().end(); ++iter) {
        const Interval &interval = *iter;
        std::cerr <<" [" <<interval.least() <<"," <<interval.greatest() <<"]";
    }
    std::cerr <<" }\n";
}

// Some basic interval tests
template<class Interval>
static void interval_tests() {

    // Unlike boost::numeric::interval, our default constructor creates an empty interval
    Interval e1;
    ASSERT_always_require(e1.isEmpty());
    ASSERT_always_require(e1.size()==0);

    // boost::numeric::interval_lib::width() is weird: it is as if greatest() is not included in the interval.
    // Our version of size() is better, but it can overflow to zero.
    Interval e2(1);
    ASSERT_always_forbid(e2.isEmpty());
    ASSERT_always_require(e2.size()==1);

    Interval e3 = Interval::hull(0, boost::integer_traits<typename Interval::Value>::const_max);
    ASSERT_always_forbid(e3.isEmpty());
    ASSERT_always_require(e1.size()==0);                       // because of overflow

    Interval e4 = Interval::whole();
    ASSERT_always_forbid(e4.isEmpty());
    ASSERT_always_require(e4.isWhole());
}

template<class Interval, class Value>
static void imap_tests(const Value &v1, const Value &v2) {
    typedef Sawyer::Container::IntervalMap<Interval, Value> Map;
    typedef typename Interval::Value Scalar;
    Map imap;
    Sawyer::Optional<Scalar> opt;

    std::cerr <<"insert([100,119], v1)\n";
    imap.insert(Interval::hull(100, 119), v1);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==1);

    // Test lowerBound against one node
    ASSERT_always_require(imap.lowerBound(99)==imap.nodes().begin());
    ASSERT_always_require(imap.lowerBound(100)==imap.nodes().begin());
    ASSERT_always_require(imap.lowerBound(118)==imap.nodes().begin());
    ASSERT_always_require(imap.lowerBound(119)==imap.nodes().begin());
    ASSERT_always_require(imap.lowerBound(120)==imap.nodes().end());

    // Test upperBound against one node
    ASSERT_always_require(imap.upperBound(99)==imap.nodes().begin());
    ASSERT_always_require(imap.upperBound(100)==imap.nodes().end());
    ASSERT_always_require(imap.upperBound(118)==imap.nodes().end());
    ASSERT_always_require(imap.upperBound(119)==imap.nodes().end());
    ASSERT_always_require(imap.upperBound(120)==imap.nodes().end());

    // Test findPrior against one node
    ASSERT_always_require(imap.findPrior(99)==imap.nodes().end());
    ASSERT_always_require(imap.findPrior(100)==imap.nodes().begin());
    ASSERT_always_require(imap.findPrior(119)==imap.nodes().begin());
    ASSERT_always_require(imap.findPrior(120)==imap.nodes().begin());

    // Test findAll against one node
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(99, 99)))==0);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(99, 100)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(99, 119)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(99, 120)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 100)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 119)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 120)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(119, 119)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(119, 120)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(120, 120)))==0);

    // Test least() against one node
    ASSERT_always_require(imap.least()==100);
    opt = imap.least(Scalar(99));
    ASSERT_always_require(opt && *opt==100);
    opt = imap.least(Scalar(100));
    ASSERT_always_require(opt && *opt==100);
    opt = imap.least(Scalar(101));
    ASSERT_always_require(opt && *opt==101);
    opt = imap.least(Scalar(119));
    ASSERT_always_require(opt && *opt==119);
    opt = imap.least(Scalar(120));
    ASSERT_always_require(!opt);

    // Test greatest() against one node
    ASSERT_always_require(imap.greatest()==119);
    opt = imap.greatest(Scalar(120));
    ASSERT_always_require(opt && *opt==119);
    opt = imap.greatest(Scalar(119));
    ASSERT_always_require(opt && *opt==119);
    opt = imap.greatest(Scalar(118));
    ASSERT_always_require(opt && *opt==118);
    opt = imap.greatest(Scalar(100));
    ASSERT_always_require(opt && *opt==100);
    opt = imap.greatest(Scalar(99));
    ASSERT_always_require(!opt);

    // Erase the right part
    std::cerr <<"erase([110,119])\n";
    imap.erase(Interval::hull(110, 119));
    show(imap);
    ASSERT_always_require(imap.size()==10);
    ASSERT_always_require(imap.nIntervals()==1);

    // Re-insert the right part
    std::cerr <<"insert([110,119], v1)\n";
    imap.insert(Interval::hull(110, 119), v1);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==1);

    // Erase the left part
    std::cerr <<"erase([100,109])\n";
    imap.erase(Interval::hull(100, 109));
    show(imap);
    ASSERT_always_require(imap.size()==10);
    ASSERT_always_require(imap.nIntervals()==1);

    // Re-insert the left part
    std::cerr <<"insert([100,109], v1)\n";
    imap.insert(Interval::hull(100, 109), v1);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==1);

    // Erase the middle part
    std::cerr <<"erase([105,114])\n";
    imap.erase(Interval::hull(105, 114));
    show(imap);
    ASSERT_always_require(imap.size()==10);
    ASSERT_always_require(imap.nIntervals()==2);

    // Test lowerBound against multiple nodes
    typename Map::NodeIterator secondIter = imap.nodes().begin(); ++secondIter;
    ASSERT_always_require(imap.lowerBound(99)==imap.nodes().begin());
    ASSERT_always_require(imap.lowerBound(100)==imap.nodes().begin());
    ASSERT_always_require(imap.lowerBound(104)==imap.nodes().begin());
    ASSERT_always_require(imap.lowerBound(105)==secondIter);
    ASSERT_always_require(imap.lowerBound(114)==secondIter);
    ASSERT_always_require(imap.lowerBound(115)==secondIter);
    ASSERT_always_require(imap.lowerBound(119)==secondIter);
    ASSERT_always_require(imap.lowerBound(120)==imap.nodes().end());

    // Test upperBound against multiple nodes
    ASSERT_always_require(imap.upperBound(99)==imap.nodes().begin());
    ASSERT_always_require(imap.upperBound(100)==secondIter);
    ASSERT_always_require(imap.upperBound(104)==secondIter);
    ASSERT_always_require(imap.upperBound(105)==secondIter);
    ASSERT_always_require(imap.upperBound(114)==secondIter);
    ASSERT_always_require(imap.upperBound(115)==imap.nodes().end());
    ASSERT_always_require(imap.upperBound(119)==imap.nodes().end());
    ASSERT_always_require(imap.upperBound(120)==imap.nodes().end());

    // Test findPrior against multiple nodes
    ASSERT_always_require(imap.findPrior(99)==imap.nodes().end());
    ASSERT_always_require(imap.findPrior(100)==imap.nodes().begin());
    ASSERT_always_require(imap.findPrior(104)==imap.nodes().begin());
    ASSERT_always_require(imap.findPrior(105)==imap.nodes().begin());
    ASSERT_always_require(imap.findPrior(114)==imap.nodes().begin());
    ASSERT_always_require(imap.findPrior(115)==secondIter);
    ASSERT_always_require(imap.findPrior(119)==secondIter);
    ASSERT_always_require(imap.findPrior(120)==secondIter);

    // Test find against multiple nodes
    ASSERT_always_require(imap.find(99)==imap.nodes().end());
    ASSERT_always_require(imap.find(100)==imap.nodes().begin());
    ASSERT_always_require(imap.find(104)==imap.nodes().begin());
    ASSERT_always_require(imap.find(105)==imap.nodes().end());
    ASSERT_always_require(imap.find(114)==imap.nodes().end());
    ASSERT_always_require(imap.find(115)==secondIter);
    ASSERT_always_require(imap.find(119)==secondIter);
    ASSERT_always_require(imap.find(120)==imap.nodes().end());

    // Test findAll against multiple nodes
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull( 99,  99)))==0);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull( 99, 100)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull( 99, 104)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull( 99, 105)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull( 99, 114)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull( 99, 115)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull( 99, 119)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull( 99, 120)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 100)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 104)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 105)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 114)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 115)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 119)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(100, 120)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(104, 104)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(104, 105)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(104, 114)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(104, 115)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(104, 119)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(104, 120)))==2);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(105, 105)))==0);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(105, 114)))==0);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(105, 115)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(105, 119)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(105, 120)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(114, 114)))==0);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(114, 115)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(114, 119)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(114, 120)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(115, 115)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(115, 119)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(115, 120)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(119, 119)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(119, 120)))==1);
    ASSERT_always_require(boost::distance(imap.findAll(Interval::hull(120, 120)))==0);
    
    // Overwrite the right end with a different value
    std::cerr <<"insert(119, v2)\n";
    imap.insert(119, v2);
    show(imap);
    ASSERT_always_require(imap.size()==10);
    ASSERT_always_require(imap.nIntervals()==3);

    // Overwrite the left end with a different value
    std::cerr <<"insert(100, v2)\n";
    imap.insert(100, v2);
    show(imap);
    ASSERT_always_require(imap.size()==10);
    ASSERT_always_require(imap.nIntervals()==4);

    // Test least against multiple nodes
    ASSERT_always_require(imap.least()==100);
    opt = imap.least(Scalar(99));  ASSERT_always_require(opt && *opt==100);
    opt = imap.least(Scalar(100)); ASSERT_always_require(opt && *opt==100);
    opt = imap.least(Scalar(101)); ASSERT_always_require(opt && *opt==101);
    opt = imap.least(Scalar(102)); ASSERT_always_require(opt && *opt==102);
    opt = imap.least(Scalar(103)); ASSERT_always_require(opt && *opt==103);
    opt = imap.least(Scalar(104)); ASSERT_always_require(opt && *opt==104);
    opt = imap.least(Scalar(105)); ASSERT_always_require(opt && *opt==115);
    opt = imap.least(Scalar(114)); ASSERT_always_require(opt && *opt==115);
    opt = imap.least(Scalar(115)); ASSERT_always_require(opt && *opt==115);
    opt = imap.least(Scalar(116)); ASSERT_always_require(opt && *opt==116);
    opt = imap.least(Scalar(117)); ASSERT_always_require(opt && *opt==117);
    opt = imap.least(Scalar(118)); ASSERT_always_require(opt && *opt==118);
    opt = imap.least(Scalar(119)); ASSERT_always_require(opt && *opt==119);
    opt = imap.least(Scalar(120)); ASSERT_always_require(!opt);

    // Test greatest() against multiple nodes
    ASSERT_always_require(imap.greatest()==119);
    opt = imap.greatest(Scalar(99));  ASSERT_always_require(!opt);
    opt = imap.greatest(Scalar(100)); ASSERT_always_require(opt && *opt==100);
    opt = imap.greatest(Scalar(101)); ASSERT_always_require(opt && *opt==101);
    opt = imap.greatest(Scalar(102)); ASSERT_always_require(opt && *opt==102);
    opt = imap.greatest(Scalar(103)); ASSERT_always_require(opt && *opt==103);
    opt = imap.greatest(Scalar(104)); ASSERT_always_require(opt && *opt==104);
    opt = imap.greatest(Scalar(105)); ASSERT_always_require(opt && *opt==104);
    opt = imap.greatest(Scalar(114)); ASSERT_always_require(opt && *opt==104);
    opt = imap.greatest(Scalar(115)); ASSERT_always_require(opt && *opt==115);
    opt = imap.greatest(Scalar(116)); ASSERT_always_require(opt && *opt==116);
    opt = imap.greatest(Scalar(117)); ASSERT_always_require(opt && *opt==117);
    opt = imap.greatest(Scalar(118)); ASSERT_always_require(opt && *opt==118);
    opt = imap.greatest(Scalar(119)); ASSERT_always_require(opt && *opt==119);
    opt = imap.greatest(Scalar(120)); ASSERT_always_require(opt && *opt==119);

    // Test leastUnmapped against multiple nodes
    opt = imap.leastUnmapped(Scalar(0  )); ASSERT_always_require(opt && *opt==0);
    opt = imap.leastUnmapped(Scalar(99 )); ASSERT_always_require(opt && *opt==99);
    opt = imap.leastUnmapped(Scalar(100)); ASSERT_always_require(opt && *opt==105);
    opt = imap.leastUnmapped(Scalar(101)); ASSERT_always_require(opt && *opt==105);
    opt = imap.leastUnmapped(Scalar(102)); ASSERT_always_require(opt && *opt==105);
    opt = imap.leastUnmapped(Scalar(103)); ASSERT_always_require(opt && *opt==105);
    opt = imap.leastUnmapped(Scalar(104)); ASSERT_always_require(opt && *opt==105);
    opt = imap.leastUnmapped(Scalar(105)); ASSERT_always_require(opt && *opt==105);
    opt = imap.leastUnmapped(Scalar(114)); ASSERT_always_require(opt && *opt==114);
    opt = imap.leastUnmapped(Scalar(115)); ASSERT_always_require(opt && *opt==120);
    opt = imap.leastUnmapped(Scalar(116)); ASSERT_always_require(opt && *opt==120);
    opt = imap.leastUnmapped(Scalar(117)); ASSERT_always_require(opt && *opt==120);
    opt = imap.leastUnmapped(Scalar(118)); ASSERT_always_require(opt && *opt==120);
    opt = imap.leastUnmapped(Scalar(119)); ASSERT_always_require(opt && *opt==120);
    opt = imap.leastUnmapped(Scalar(120)); ASSERT_always_require(opt && *opt==120);
    opt = imap.leastUnmapped(Scalar(121)); ASSERT_always_require(opt && *opt==121);

    // Test greatestUnmapped against multiple nodes
    opt = imap.greatestUnmapped(Scalar(0  )); ASSERT_always_require(opt && *opt==0);
    opt = imap.greatestUnmapped(Scalar(99 )); ASSERT_always_require(opt && *opt==99);
    opt = imap.greatestUnmapped(Scalar(100)); ASSERT_always_require(opt && *opt==99);
    opt = imap.greatestUnmapped(Scalar(101)); ASSERT_always_require(opt && *opt==99);
    opt = imap.greatestUnmapped(Scalar(102)); ASSERT_always_require(opt && *opt==99);
    opt = imap.greatestUnmapped(Scalar(103)); ASSERT_always_require(opt && *opt==99);
    opt = imap.greatestUnmapped(Scalar(104)); ASSERT_always_require(opt && *opt==99);
    opt = imap.greatestUnmapped(Scalar(105)); ASSERT_always_require(opt && *opt==105);
    opt = imap.greatestUnmapped(Scalar(114)); ASSERT_always_require(opt && *opt==114);
    opt = imap.greatestUnmapped(Scalar(115)); ASSERT_always_require(opt && *opt==114);
    opt = imap.greatestUnmapped(Scalar(116)); ASSERT_always_require(opt && *opt==114);
    opt = imap.greatestUnmapped(Scalar(117)); ASSERT_always_require(opt && *opt==114);
    opt = imap.greatestUnmapped(Scalar(118)); ASSERT_always_require(opt && *opt==114);
    opt = imap.greatestUnmapped(Scalar(119)); ASSERT_always_require(opt && *opt==114);
    opt = imap.greatestUnmapped(Scalar(120)); ASSERT_always_require(opt && *opt==120);
    opt = imap.greatestUnmapped(Scalar(121)); ASSERT_always_require(opt && *opt==121);

    // Re-insert the middle part
    std::cerr <<"insert([105,114], v1)\n";
    imap.insert(Interval::hull(105, 114), v1);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==3);

    // Overwrite in the middle
    std::cerr <<"insert(109, v2)\n";
    imap.insert(109, v2);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==5);

    // Overwrite joining to the left
    std::cerr <<"insert(110, v2)\n";
    imap.insert(110, v2);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==5);

    // Overwrite joining to the right
    std::cerr <<"insert(108, v2)\n";
    imap.insert(108, v2);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==5);

    // Overwrite joining on both sides
    std::cerr <<"insert([101,107], v2)\n";
    imap.insert(Interval::hull(101, 107), v2);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==3);

    // Overwrite and delete joining on both sides
    std::cerr <<"insert([109,118], v2)\n";
    imap.insert(Interval::hull(109, 118), v2);
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==1);
}

// Test splitting and joining in more complex ways.  We'll store values that are the same as the intervals where they're
// stored.
template<class I>
class IntervalPolicy {
public:
    typedef I Interval;
    typedef I Value;

    bool merge(const Interval &leftInterval, Value &leftValue, const Interval &rightInterval, Value &rightValue) {
        ASSERT_always_forbid(leftInterval.isEmpty());
        ASSERT_always_forbid(rightInterval.isEmpty());
        ASSERT_always_require(leftInterval == leftValue);
        ASSERT_always_require(rightInterval == rightValue);
        ASSERT_always_require(leftInterval.greatest() + 1 == rightInterval.least());
        leftValue = Interval::hull(leftValue.least(), rightValue.greatest());
        return true;
    }

    Value split(const Interval &interval, Value &value, const typename Interval::Value &splitPoint) {
        ASSERT_always_forbid(interval.isEmpty());
        ASSERT_always_require(interval == value);
        ASSERT_always_require(splitPoint > interval.least() && splitPoint <= interval.greatest());
        Value right = Value::hull(splitPoint, value.greatest());
        value = Value::hull(value.least(), splitPoint-1);
        return right;
    }

    void truncate(const Interval &interval, Value &value, const typename Interval::Value &splitPoint) {
        split(interval, value, splitPoint);
    }
};

template<class Interval>
static void imap_policy_tests() {
    typedef Sawyer::Container::IntervalMap<Interval, Interval, IntervalPolicy<Interval> > Map;
    Map imap;

    std::cerr <<"insert([100,119], [100,119])\n";
    imap.insert(Interval::hull(100, 119), Interval::hull(100, 119));
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==1);

    // Erase the right part
    std::cerr <<"erase([110,119])\n";
    imap.erase(Interval::hull(110, 119));
    show(imap);
    ASSERT_always_require(imap.size()==10);
    ASSERT_always_require(imap.nIntervals()==1);

    // Re-insert the right part
    std::cerr <<"insert([110,119], [110,119])\n";
    imap.insert(Interval::hull(110, 119), Interval::hull(110, 119));
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==1);

    // Erase the left part
    std::cerr <<"erase([100,109])\n";
    imap.erase(Interval::hull(100, 109));
    show(imap);
    ASSERT_always_require(imap.size()==10);
    ASSERT_always_require(imap.nIntervals()==1);

    // Re-insert the left part
    std::cerr <<"insert([100,109], [100,109])\n";
    imap.insert(Interval::hull(100, 109), Interval::hull(100, 109));
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==1);

    // Erase the middle part
    std::cerr <<"erase([105,114])\n";
    imap.erase(Interval::hull(105, 114));
    show(imap);
    ASSERT_always_require(imap.size()==10);
    ASSERT_always_require(imap.nIntervals()==2);

    // Re-insert the middle part
    std::cerr <<"insert([105,114], [105,114])\n";
    imap.insert(Interval::hull(105, 114), Interval::hull(105, 114));
    show(imap);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==1);
}

static void search_tests() {
    typedef Sawyer::Container::Interval<int> Interval;
    typedef Sawyer::Container::IntervalMap<Interval, int> IMap;
    typedef Sawyer::Container::IntervalMap<Interval, float> IMap2;
    IMap imap;
    IMap2 map2;

    imap.insert(Interval::hull(100, 109), 0);
    imap.insert(Interval::hull(120, 129), 0);
    ASSERT_always_require(imap.size()==20);
    ASSERT_always_require(imap.nIntervals()==2);

    IMap::NodeIterator first = imap.nodes().begin();
    IMap::NodeIterator second = first; ++second;
    IMap::NodeIterator none = imap.nodes().end();

    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(-1, 10))==none);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(98, 99))==none);

    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(99, 100))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(99, 109))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(99, 110))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(99, 119))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(99, 120))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(99, 129))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(99, 130))==first);
    
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(100, 100))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(100, 109))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(100, 110))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(100, 119))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(100, 120))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(100, 129))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(100, 130))==first);
    
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(109, 109))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(109, 110))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(109, 119))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(109, 120))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(109, 129))==first);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(109, 130))==first);
    
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(110, 110))==none);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(110, 119))==none);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(110, 120))==second);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(110, 129))==second);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(110, 130))==second);
    
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(119, 119))==none);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(119, 120))==second);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(119, 129))==second);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(119, 130))==second);
    
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(120, 120))==second);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(120, 129))==second);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(120, 130))==second);

    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(129, 129))==second);
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(129, 130))==second);
    
    ASSERT_always_require(imap.findFirstOverlap(Interval::hull(130, 130))==none);

    map2.clear();
    map2.insert(Interval::hull(10, 99), 1.0);
    map2.insert(Interval::hull(110, 119), 2.0);
    map2.insert(Interval::hull(130, 139), 3.0);
    ASSERT_always_require(imap.findFirstOverlap(imap.nodes().begin(), map2, map2.nodes().begin()).first==none);

    map2.clear();
    map2.insert(Interval::hull(10, 99), 1.0);
    map2.insert(105, 1.5);
    map2.insert(Interval::hull(110, 119), 2.0);
    map2.insert(Interval::hull(130, 139), 3.0);
    ASSERT_always_require(imap.findFirstOverlap(imap.nodes().begin(), map2, map2.nodes().begin()).first==first);

    map2.clear();
    map2.insert(Interval::hull(10, 99), 1.0);
    map2.insert(Interval::hull(110, 139), 2.0);
    ASSERT_always_require(imap.findFirstOverlap(imap.nodes().begin(), map2, map2.nodes().begin()).first==second);
}

// All we need for storing a value in an IntervalMap is that it has a copy constructor, an assignment operator,
// and an equality operator
class MinimalApi {
    int id;
    MinimalApi();
public:
    MinimalApi(int id): id(id) {}                                       // only for testing
    int getId() const { return id; }                                    // only for testing

    MinimalApi(const MinimalApi &other) { id=other.id; }
    MinimalApi& operator=(const MinimalApi &other) { id=other.id; return *this; }
    bool operator==(const MinimalApi &other) {
        return id==other.id;
    }
};
std::ostream& operator<<(std::ostream &o, const MinimalApi &v) {        // only for testing
    o <<"MinimalApi(" <<v.getId() <<")";
    return o;
}

template<class Interval>
static void basic_set_tests() {
    typedef Sawyer::Container::IntervalSet<Interval> Set;
    Set set;

    Interval all = Interval::whole();

    std::cerr <<"invert() empty set\n";
    typename Interval::Value allSize = (all.greatest()-all.least()) + 1;// must cast because width(all) returns wrong type
    set.invert(all);
    show(set);
    ASSERT_always_require(!set.isEmpty());
    ASSERT_always_require(set.size()==allSize);
    ASSERT_always_require(set.nIntervals()==1);

    std::cerr <<"invert() all set\n";
    set.invert(all);
    show(set);
    ASSERT_always_require(set.isEmpty());
    ASSERT_always_require(set.size()==0);
    ASSERT_always_require(set.nIntervals()==0);

    std::cerr <<"insert([100,119])\n";
    set.insert(Interval::hull(100, 119));
    show(set);
    ASSERT_always_require(set.size()==20);
    ASSERT_always_require(set.nIntervals()==1);

    // Erase the right part
    std::cerr <<"erase([110,119])\n";
    set.erase(Interval::hull(110, 119));
    show(set);
    ASSERT_always_require(set.size()==10);
    ASSERT_always_require(set.nIntervals()==1);

    // Re-insert the right part
    std::cerr <<"insert([110,119])\n";
    set.insert(Interval::hull(110, 119));
    show(set);
    ASSERT_always_require(set.size()==20);
    ASSERT_always_require(set.nIntervals()==1);

    // Erase the left part
    std::cerr <<"erase([100,109])\n";
    set.erase(Interval::hull(100, 109));
    show(set);
    ASSERT_always_require(set.size()==10);
    ASSERT_always_require(set.nIntervals()==1);

    // Re-insert the left part
    std::cerr <<"insert([100,109])\n";
    set.insert(Interval::hull(100, 109));
    show(set);
    ASSERT_always_require(set.size()==20);
    ASSERT_always_require(set.nIntervals()==1);

    // Erase the middle part
    std::cerr <<"erase([105,114])\n";
    set.erase(Interval::hull(105, 114));
    show(set);
    ASSERT_always_require(set.size()==10);
    ASSERT_always_require(set.nIntervals()==2);

    // Invert selection
    std::cerr <<"invert()\n";
    set.invert(all);
    show(set);
    ASSERT_always_require(set.size()==typename Interval::Value(allSize-10));
    ASSERT_always_require(set.nIntervals()==3);

    // Invert again
    std::cerr <<"invert()\n";
    set.invert(all);
    show(set);
    ASSERT_always_require(set.size()==10);
    ASSERT_always_require(set.nIntervals()==2);
    
    // Test exist against multiple nodes
    ASSERT_always_require(set.contains(99)==false);
    ASSERT_always_require(set.contains(100)==true);
    ASSERT_always_require(set.contains(104)==true);
    ASSERT_always_require(set.contains(105)==false);
    ASSERT_always_require(set.contains(114)==false);
    ASSERT_always_require(set.contains(115)==true);
    ASSERT_always_require(set.contains(119)==true);
    ASSERT_always_require(set.contains(120)==false);

    // Re-insert the middle part
    std::cerr <<"insert([105,114])\n";
    set.insert(Interval::hull(105, 114));
    show(set);
    ASSERT_always_require(set.size()==20);
    ASSERT_always_require(set.nIntervals()==1);
}

template<class Interval>
static void set_ctor_tests() {
    typedef Sawyer::Container::IntervalMap<Interval, float> Map;
    Map map1;
    map1.insert(Interval::hull(70, 79), 3.141f);
    map1.insert(Interval::hull(100, 109), 1.414f);

    Map map2;
    map2.insert(65, 6.28f);

    std::cerr <<"construct set from map\n";
    typedef Sawyer::Container::IntervalSet<Interval> Set;
    Set set1(map1);
    show(set1);
    ASSERT_always_require(set1.size()==20);
    ASSERT_always_require(set1.nIntervals()==2);

    std::cerr <<"assign set from map\n";
    typedef Sawyer::Container::IntervalSet<Interval> Set;
    set1 = map2;
    show(set1);
    ASSERT_always_require(set1.size()==1);
    ASSERT_always_require(set1.nIntervals()==1);
}

template<class Interval>
static void set_iterators_tests() {
    std::cerr <<"value iterator\n";

    typedef Sawyer::Container::IntervalSet<Interval> Set;
    Set set;

    set.insert(1);
    set.insert(Interval::hull(3, 5));
    set.insert(Interval::hull(7, 9));
    show(set);

    std::cerr <<"  scalar value iterator {";
    BOOST_FOREACH (const typename Set::Scalar &scalar, set.scalars())
        std::cerr <<" " <<scalar;
    std::cerr <<" }\n";
}

template<class Interval>
static void set_find_tests() {
    std::cerr <<"set_find\n";

    typedef Sawyer::Container::IntervalSet<Interval> Set;
    typedef boost::iterator_range<typename Set::ConstIntervalIterator> Range;

    Set set;
    set.insert(1);                                      // node0
    set.insert(Interval::hull(3, 4));                   // node1
    set.insert(Interval::hull(6, 8));                   // node2
    set.insert(Interval::hull(10, 13));                 // node3
    set.insert(Interval::hull(15, 19));                 // node4
    show(set);

    ASSERT_always_require(set.nIntervals()==5);
    typename Set::ConstIntervalIterator iter = set.intervals().begin();
    typename Set::ConstIntervalIterator node0 = iter; ++iter;
    typename Set::ConstIntervalIterator node1 = iter; ++iter;
    typename Set::ConstIntervalIterator node2 = iter; ++iter;
    /*typename Set::ConstIntervalIterator node3 = iter;*/ ++iter;
    typename Set::ConstIntervalIterator node4 = iter; ++iter;
    ASSERT_always_require(iter==set.intervals().end());

    std::cerr <<"  findAll([])\n";
    Range r0 = set.findAll(Interval());
    ASSERT_always_require(r0.begin()==set.intervals().end());
    ASSERT_always_require(r0.end()==set.intervals().end());

    std::cerr <<"  findAll([0])\n";
    Range r1 = set.findAll(Interval(0));
    ASSERT_always_require(r1.begin()==set.intervals().end());
    ASSERT_always_require(r1.end()==set.intervals().end());

    std::cerr <<"  findAll([20,29])\n";
    Range r2 = set.findAll(Interval::hull(20, 29));
    ASSERT_always_require(r2.begin()==set.intervals().end());
    ASSERT_always_require(r2.end()==set.intervals().end());

    std::cerr <<"  findAll([1])\n";
    Range r3 = set.findAll(Interval(1));
    ASSERT_always_require(r3.begin()==node0);
    ASSERT_always_require(r3.end()==node1);

    std::cerr <<"  findAll([1,3])\n";
    Range r4 = set.findAll(Interval::hull(1, 3));
    ASSERT_always_require(r4.begin()==node0);
    ASSERT_always_require(r4.end()==node2);

    std::cerr <<"  findAll([6,15])\n";
    Range r5 = set.findAll(Interval::hull(6, 15));
    ASSERT_always_require(r5.begin()==node2);
    ASSERT_always_require(r5.end()==set.intervals().end());

    std::cerr <<"  findAll([16,20])\n";
    Range r6 = set.findAll(Interval::hull(16, 20));
    ASSERT_always_require(r6.begin()==node4);
    ASSERT_always_require(r6.end()==set.intervals().end());

    std::cerr <<"  findAll([0,25])\n";
    Range r7 = set.findAll(Interval::hull(0, 25));
    ASSERT_always_require(r7.begin()==node0);
    ASSERT_always_require(r7.end()==set.intervals().end());
}

template<class Interval>
static void set_overlap_tests() {
    std::cerr <<"set overlap\n";

    typedef Sawyer::Container::IntervalSet<Interval> Set;
    typedef typename Set::ConstIntervalIterator Iter;
    typedef std::pair<typename Set::ConstIntervalIterator, typename Set::ConstIntervalIterator> IterPair;

    Set a;
    a.insert(Interval::hull(3, 6));
    a.insert(Interval::hull(9, 12));
    a.insert(Interval::hull(15, 18));
    show(a);

    Set b;
    b.insert(Interval::hull(4, 4));
    b.insert(Interval::hull(6, 12));
    b.insert(Interval::hull(14, 19));
    show(b);

    Iter ai = a.intervals().begin();
    Iter a0 = ai; ++ai;
    Iter a1 = ai; ++ai;
    Iter a2 = ai; ++ai;
    ASSERT_always_require(ai==a.intervals().end());
    
    Iter bi = b.intervals().begin();
    Iter b0 = bi; ++bi;
    Iter b1 = bi; ++bi;
    Iter b2 = bi; ++bi;
    ASSERT_always_require(bi==b.intervals().end());

    // Finding first overlap with an interval

    Iter i1 = a.findFirstOverlap(Interval());
    ASSERT_always_require(i1==ai);

    Iter i2 = a.findFirstOverlap(Interval(0));
    ASSERT_always_require(i2==ai);

    Iter i3 = a.findFirstOverlap(Interval(3));
    ASSERT_always_require(i3==a0);

    Iter i4 = a.findFirstOverlap(Interval::hull(2, 6));
    ASSERT_always_require(i4==a0);

    Iter i5 = a.findFirstOverlap(Interval::hull(3, 10));
    ASSERT_always_require(i5==a0);

    Iter i6 = a.findFirstOverlap(Interval::hull(6, 15));
    ASSERT_always_require(i6==a0);

    Iter i7 = a.findFirstOverlap(Interval::hull(19, 29));
    ASSERT_always_require(i7==ai);

    // Finding first overlap among two sets

    IterPair ip1 = a.findFirstOverlap(a0, b, b0);
    ASSERT_always_require(ip1.first==a0);
    ASSERT_always_require(ip1.second==b0);

    IterPair ip2 = a.findFirstOverlap(a1, b, b0);
    ASSERT_always_require(ip2.first==a1);
    ASSERT_always_require(ip2.second==b1);

    IterPair ip3 = a.findFirstOverlap(a0, b, b1);
    ASSERT_always_require(ip3.first==a0);
    ASSERT_always_require(ip3.second==b1);

    IterPair ip4 = a.findFirstOverlap(a0, b, b2);
    ASSERT_always_require(ip4.first==a2);
    ASSERT_always_require(ip4.second==b2);
    
    IterPair ip5 = a.findFirstOverlap(ai, b, bi);
    ASSERT_always_require(ip5.first==ai);
    ASSERT_always_require(ip5.second==bi);
}

template<class Interval>
static void set_union_tests() {
    std::cerr <<"set union\n";

    typedef Sawyer::Container::IntervalSet<Interval> Set;
    //typedef typename Set::ConstIntervalIterator Iter;

    Set a;
    a.insert(Interval::hull(3, 6));
    a.insert(Interval::hull(9, 12));
    a.insert(Interval::hull(15, 18));
    show(a);

    Set b;
    b.insert(Interval::hull(4, 4));
    b.insert(Interval::hull(6, 12));
    b.insert(Interval::hull(14, 19));
    show(b);

    Set c = a | b;
    show(c);
    ASSERT_always_require(c.nIntervals()==2);
    ASSERT_always_require(c.size()==16);
}

template<class Interval>
static void set_intersection_tests() {
    std::cerr <<"set intersection\n";
    
    typedef Sawyer::Container::IntervalSet<Interval> Set;
    typedef typename Set::ConstIntervalIterator Iter;

    Set a;
    a.insert(Interval::hull(3, 6));
    a.insert(Interval::hull(9, 12));
    a.insert(Interval::hull(15, 18));
    show(a);

    Set b;
    b.insert(Interval::hull(4, 4));
    b.insert(Interval::hull(6, 12));
    b.insert(Interval::hull(14, 19));
    show(b);

    // Intersection depends on inverse, so test inverse first
    Set c = ~a;
    ASSERT_always_require(c.nIntervals()==4);
    show(c);

    Set d = ~c;
    show(d);
    ASSERT_always_require(d==a);

    // Intersection depends on subtraction, so test subtraction first
    Set diff1 = a - b;
    show(diff1);
    ASSERT_always_require(diff1.nIntervals()==2);
    ASSERT_always_require(diff1.size()==2);

    Set diff2 = b - a;
    show(diff2);
    ASSERT_always_require(diff2.nIntervals()==3);
    ASSERT_always_require(diff2.size()==4);

    // Test intersection operator
    Set e = a & b;
    show(e);
    ASSERT_always_require(e.nIntervals()==4);
    ASSERT_always_require(e.size()==10);

    a.intersect(b);
    show(a);
    ASSERT_always_require(a.nIntervals()==4);
    Iter ai = a.intervals().begin();
    ASSERT_always_require(*ai==Interval(4)); ++ai;
    ASSERT_always_require(*ai==Interval(6)); ++ai;
    ASSERT_always_require(*ai==Interval::hull(9, 12)); ++ai;
    ASSERT_always_require(*ai==Interval::hull(15, 18));
}

template<class T>
struct CheckOverflow {
    void operator()(T x) {
        ASSERT_always_require2(T(x+T(2)) > x, "overflow");
    }
};

template<>
struct CheckOverflow<int> {
    void operator()(int) {
        // signed integer overflow is undefined behavior and results in
        // warnings from good compilers.
    }
};

template<class Interval>
static void iterator_tests(typename Interval::Value low) {
    CheckOverflow<typename Interval::Value>()(low);
    std::cerr <<"  empty interval iterators\n";

    Interval empty;
    ASSERT_always_require(empty.begin() == empty.begin());
    ASSERT_always_require(empty.end() == empty.end());
    ASSERT_always_require(empty.begin() == empty.end());

    std::cerr <<"  non-empty interval iterators: hull(" <<low <<", " <<low+3 <<")\n";
    Interval a = Interval::hull(low, low+2);
    typename Interval::ConstIterator ai = a.begin();
    ASSERT_always_require(ai == a.begin());
    ASSERT_always_require(ai != a.end());
    ASSERT_always_require(*ai == low+0);
    ++ai;
    ASSERT_always_require(ai != a.begin());
    ASSERT_always_require(ai != a.end());
    ASSERT_always_require(*ai == low+1);
    ++ai;
    ASSERT_always_require(ai != a.begin());
    ASSERT_always_require(ai != a.end());
    ASSERT_always_require(*ai == low+2);
    ++ai;
    ASSERT_always_require(ai != a.begin());
    ASSERT_always_require(ai == a.end());
    --ai;
    ASSERT_always_require(ai != a.begin());
    ASSERT_always_require(ai != a.end());
    ASSERT_always_require(*ai == low+2);
    --ai;
    ASSERT_always_require(ai != a.begin());
    ASSERT_always_require(ai != a.end());
    ASSERT_always_require(*ai == low+1);
    --ai;
    ASSERT_always_require(ai == a.begin());
    ASSERT_always_require(ai != a.end());
    ASSERT_always_require(*ai == low+0);
    --ai;
    ASSERT_always_require(ai != a.begin());
    ASSERT_always_require(ai == a.end());
    ++ai;
    ASSERT_always_require(ai == a.begin());
    ASSERT_always_require(ai != a.end());
    ASSERT_always_require(*ai == low+0);

    std::cerr <<"  BOOST_FOREACH over non-const intervals\n";
    typename Interval::Value i = low;
    BOOST_FOREACH (typename Interval::Value j, a.values()) {
        ASSERT_always_require(i == j);
        ++i;
    }
    i = low;
    BOOST_FOREACH (typename Interval::Value j, a) {
        ASSERT_always_require(i == j);
        ++i;
    }

    std::cerr <<"  BOOST_FOREACH over const intervals\n";
    const Interval b = a;
    i = low;
    BOOST_FOREACH (typename Interval::Value j, b.values()) {
        ASSERT_always_require(i == j);
        ++i;
    }
    i = low;
    BOOST_FOREACH (typename Interval::Value j, b) {
        ASSERT_always_require(i == j);
        ++i;
    }

    std::cerr <<"  BOOST_REVERSE_FOREACH over non-const intervals\n";
    i = low + 2;
    BOOST_REVERSE_FOREACH (typename Interval::Value j, a.values()) {
        ASSERT_always_require(i == j);
        --i;
    }
    i = low + 2;
    BOOST_REVERSE_FOREACH (typename Interval::Value j, a) {
        ASSERT_always_require(i == j);
        --i;
    }

    std::cerr <<"  BOOST_REVERSE_FOREACH over const intervals\n";
    i = low + 2;
    BOOST_REVERSE_FOREACH (typename Interval::Value j, b.values()) {
        ASSERT_always_require(i == j);
        --i;
    }
    i = low + 2;
    BOOST_REVERSE_FOREACH (typename Interval::Value j, b) {
        ASSERT_always_require(i == j);
        --i;
    }
}

unsigned do_something(unsigned i) {
    static volatile unsigned total = 1;
    if (1 == i) {
        total = 1;
    } else if (i != 0) {
        total += i;
    }
    return total;
}

#if 0 // [Robb Matzke 2016-12-08]
static void
iterator_performance_test() {
    Sawyer::Container::Interval<unsigned> interval = Sawyer::Container::Interval<unsigned>::baseSize(0, 10000000000);
    //Sawyer::Container::Interval<unsigned> interval = Sawyer::Container::Interval<unsigned>::baseSize(0, 1000);

    Sawyer::Stopwatch timer;
    for (unsigned i=interval.least(); i<=interval.greatest(); ++i) {
        do_something(i);
        if (i == interval.greatest())
            break;
    }
    timer.stop();
    std::cout <<"old-style overflow-safe interval loop: total=" <<do_something(0) <<" in " <<timer <<" seconds\n";

    do_something(0);
    timer.restart();
    BOOST_FOREACH (unsigned i, interval)
        do_something(i);
    timer.stop();
    std::cout <<"new-style overflow-safe interval loop: total=" <<do_something(0) <<" in " <<timer <<" seconds\n";
}
#endif

int main() {
    Sawyer::initializeLibrary();

#if 0 // DEBUGGING [Robb Matzke 2016-12-08]
    iterator_performance_test();
    exit(1);
#endif

    // Basic interval tests
    std::cerr <<"=== Basic interval tests for 'unsigned' ===\n";
    interval_tests<Sawyer::Container::Interval<unsigned> >();
    std::cerr <<"=== Basic interval tests for 'unsigned long' ===\n";
    interval_tests<Sawyer::Container::Interval<unsigned long> >();
    std::cerr <<"=== Basic interval tests for 'boost::uint64_t' ===\n";
    interval_tests<Sawyer::Container::Interval<boost::uint64_t> >();
    std::cerr <<"=== Basic interval tests for 'unsigned short' ===\n";
    interval_tests<Sawyer::Container::Interval<unsigned short> >();
    std::cerr <<"=== Basic interval tests for 'boost::uint8_t' ===\n";
    interval_tests<Sawyer::Container::Interval<boost::uint8_t> >();

    // Interval Iterators
    std::cerr <<"=== Interval iterator tests for 'unsigned' ===\n";
    iterator_tests<Sawyer::Container::Interval<unsigned> >(3);
    iterator_tests<Sawyer::Container::Interval<unsigned> >(0); // tests decrement overflow
    std::cerr <<"=== Interval iterator tests for 'boost::uint8_t' ===\n";
    iterator_tests<Sawyer::Container::Interval<boost::uint8_t> >(3);
    iterator_tests<Sawyer::Container::Interval<boost::uint8_t> >(0); // tests decrement overflow
    iterator_tests<Sawyer::Container::Interval<boost::uint8_t> >(255-3); // tests increment overflow
    std::cerr <<"=== Interval iterator tests for 'int' ===\n";
    iterator_tests<Sawyer::Container::Interval<int> >(1);
    iterator_tests<Sawyer::Container::Interval<int> >(0);
    iterator_tests<Sawyer::Container::Interval<int> >(-1);
    iterator_tests<Sawyer::Container::Interval<int> >(-3);
    iterator_tests<Sawyer::Container::Interval<int> >(-4);
    
    // Test that Interval can be used in an IntervalMap
    std::cerr <<"=== Interval map tests for 'unsigned' ===\n";
    imap_tests<Sawyer::Container::Interval<unsigned>, int>(1, 2);
    std::cerr <<"=== Interval map tests for 'unsigned long' ===\n";
    imap_tests<Sawyer::Container::Interval<unsigned long>, int>(1, 2);
    std::cerr <<"=== Interval map tests for 'boost::uint64_t' ===\n";
    imap_tests<Sawyer::Container::Interval<boost::uint64_t>, int>(1, 2);
    std::cerr <<"=== Interval map tests for 'unsigned short' ===\n";
    imap_tests<Sawyer::Container::Interval<unsigned short>, int>(1, 2);
    std::cerr <<"=== Interval map tests for 'boost::uint8_t' ===\n";
    imap_tests<Sawyer::Container::Interval<boost::uint8_t>, int>(1, 2);

    // Test some non-unsigned stuff
    std::cerr <<"=== Interval map tests for 'int' ===\n";
    imap_tests<Sawyer::Container::Interval<int>, int>(1, 2);
    std::cerr <<"=== Interval map tests for 'double' ===\n";
    imap_tests<Sawyer::Container::Interval<double>, int>(1, 2);

    // Test whether other types can be used as the values in an IntervalMap
    std::cerr <<"=== Interval map tests for 'unsigned' and 'short' ===\n";
    imap_tests<Sawyer::Container::Interval<unsigned>, short>(1, 2);
    std::cerr <<"=== Interval map tests for 'unsigned' and 'double' ===\n";
    imap_tests<Sawyer::Container::Interval<unsigned>, double>(1.0, 2.0);
    std::cerr <<"=== Interval map tests for 'unsigned' and 'bool' ===\n";
    imap_tests<Sawyer::Container::Interval<unsigned>, bool>(false, true);
    std::cerr <<"=== Interval map tests for 'unsigned' and 'Pair' ===\n";
    imap_tests<Sawyer::Container::Interval<unsigned>, Pair>(Pair(1, 2), Pair(3, 4));
    std::cerr <<"=== Interval map tests for 'unsigned' and 'MinimalApi' ===\n";
    imap_tests<Sawyer::Container::Interval<unsigned>, MinimalApi>(MinimalApi(0), MinimalApi(1));

    // Check that merging and splitting of values works correctly.
    std::cerr <<"=== Policy tests for 'unsigned' ===\n";
    imap_policy_tests<Sawyer::Container::Interval<unsigned> >();
    std::cerr <<"=== Policy tests for 'boost::uint64_t' ===\n";
    imap_policy_tests<Sawyer::Container::Interval<boost::uint64_t> >();
    std::cerr <<"=== Policy tests for 'double' ===\n";
    imap_policy_tests<Sawyer::Container::Interval<double> >();

    // others
    std::cerr <<"=== Search tests ===\n";
    search_tests();

    // Basic IntervalSet tests
    std::cerr <<"=== basic set tests for 'unsigned' ===\n";
    basic_set_tests<Sawyer::Container::Interval<unsigned> >();
    std::cerr <<"=== basic set tests for 'unsigned long' ===\n";
    basic_set_tests<Sawyer::Container::Interval<unsigned long> >();
    std::cerr <<"=== basic set tests for 'boost::uint64_t' ===\n";
    basic_set_tests<Sawyer::Container::Interval<boost::uint64_t> >();
    std::cerr <<"=== basic set tests for 'unsigned short' ===\n";
    basic_set_tests<Sawyer::Container::Interval<unsigned short> >();
    std::cerr <<"=== basic set tests for 'boost::uint8_t' ===\n";
    basic_set_tests<Sawyer::Container::Interval<boost::uint8_t> >();
#if 0 // FIXME[Robb Matzke 2015-03-11]: does not work for g++-4.8.3-12ubuntu3 with optimizations: invert operation fails
    std::cerr <<"=== basic set tests for 'int' ===\n";
    basic_set_tests<Sawyer::Container::Interval<int> >();
#endif

    // IntervalSet constructors
    std::cerr <<"=== constructor tests for 'unsigned' ===\n";
    set_ctor_tests<Sawyer::Container::Interval<unsigned> >();
    std::cerr <<"=== constructor tests for 'unsigned long' ===\n";
    set_ctor_tests<Sawyer::Container::Interval<unsigned long> >();
    std::cerr <<"=== constructor tests for 'boost::uint64_t' ===\n";
    set_ctor_tests<Sawyer::Container::Interval<boost::uint64_t> >();
    std::cerr <<"=== constructor tests for 'unsigned short' ===\n";
    set_ctor_tests<Sawyer::Container::Interval<unsigned short> >();
    std::cerr <<"=== constructor tests for 'boost::uint8_t' ===\n";
    set_ctor_tests<Sawyer::Container::Interval<boost::uint8_t> >();
    std::cerr <<"=== constructor tests for 'int' ===\n";
    set_ctor_tests<Sawyer::Container::Interval<int> >();

    // IntervalSet iterators
    std::cerr <<"=== iterator tests for 'unsigned' ===\n";
    set_iterators_tests<Sawyer::Container::Interval<unsigned> >();

    // IntervalSet searching
    std::cerr <<"=== search tests for 'unsigned' ===\n";
    set_find_tests<Sawyer::Container::Interval<unsigned> >();
    std::cerr <<"=== search tests for 'int' ===\n";
    set_find_tests<Sawyer::Container::Interval<int> >();
    std::cerr <<"=== overlap tests for 'unsigned' ===\n";
    set_overlap_tests<Sawyer::Container::Interval<unsigned> >();
    std::cerr <<"=== union tests for 'unsigned' ===\n";
    set_union_tests<Sawyer::Container::Interval<unsigned> >();
    std::cerr <<"=== intersection tests for 'unsigned' ===\n";
    set_intersection_tests<Sawyer::Container::Interval<unsigned> >();

    return 0;
}
