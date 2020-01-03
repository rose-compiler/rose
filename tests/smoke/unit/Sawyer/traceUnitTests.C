// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Unit tests for Sawyer::Container::Trace
#include <Sawyer/Assert.h>
#include <Sawyer/Trace.h>

using namespace Sawyer::Container;

#define require(X) ASSERT_always_require(X)

template<class T, class IndexTag>
static void
print(const Trace<T, IndexTag> &trace) {
#if 1
    (void) trace; // to avoid warning about unused trace
#elif 0
    trace.dump(std::cerr);
    std::cerr <<"\n";
#else
    std::cout <<"trace = [" <<trace <<"]\n";
#endif
}

template<class Trace>
struct Visitor {
    typename Trace::ConstIterator iter;

    Visitor(const typename Trace::ConstIterator &iter)
        : iter(iter) {}

    bool operator()(const typename Trace::Label &label) {
        require(label == *iter);
        ++iter;
        return true;
    }
};

// Test iteration and traversal by comparing them.
template<class Trace>
static void testIterationTraversal(const Trace &trace) {
    Visitor<Trace> visitor(trace.begin());
    trace.traverse(visitor);
    require(visitor.iter == trace.end());
}

template<class T>
static void
test_empty() {
    Trace<T> trace;
    require(trace.isEmpty());
    require(trace.size() == 0);
    require(trace.exists(T()) == false);
    require(trace.nLabels() == 0);
    require(trace.burstiness() == 0.0);
    std::vector<T> vector = trace.toVector();
    require(vector.empty());
    testIterationTraversal(trace);
}

template<class IndexType, class IndexTag>
static void
test_append_integers() {
    Trace<IndexType, IndexTag> trace;
    trace.reserve(6);                                   // only a hint, but we can test that it compiles

    trace.append(5);                                    // [5]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 1);
    require(trace.front() == 5);
    require(trace.back() == 5);
    require(trace.exists(5));
    require(!trace.exists(4));
    require(trace.nLabels() == 1);
    std::vector<IndexType> vector = trace.toVector();
    require(vector.size() == 1);
    require(vector[0] == 5);
    require(trace.burstiness(5) == 0.0);
    require(trace.burstiness(4) == 0.0);
    require(trace.burstiness() == 0.0);

    trace.append(5);                                    // [5, 5]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 2);
    require(trace.front() == 5);
    require(trace.back() == 5);
    require(trace.exists(5));
    require(!trace.exists(4));
    require(trace.nLabels() == 1);
    vector = trace.toVector();
    require(vector.size() == 2);
    require(vector[0] == 5);
    require(vector[1] == 5);
    require(trace.burstiness(5) == 1.0);
    require(trace.burstiness() == 1.0);

    trace.append(4);                                    // [5, 5, 4]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 3);
    require(trace.front() == 5);
    require(trace.back() == 4);
    require(trace.exists(5));
    require(trace.exists(4));
    require(trace.nLabels() == 2);
    vector = trace.toVector();
    require(vector.size() == 3);
    require(vector[0] == 5);
    require(vector[1] == 5);
    require(vector[2] == 4);
    require(trace.burstiness(5) == 1.0);
    require(trace.burstiness(4) == 0.0);
    require(trace.burstiness() == 1.0);

    trace.append(6);                                    // [5, 5, 4, 6]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 4);
    require(trace.front() == 5);
    require(trace.back() == 6);
    require(trace.exists(5));
    require(trace.exists(4));
    require(trace.exists(6));
    require(trace.nLabels() == 3);
    vector = trace.toVector();
    require(vector.size() == 4);
    require(vector[0] == 5);
    require(vector[1] == 5);
    require(vector[2] == 4);
    require(vector[3] == 6);
    require(trace.burstiness(5) == 1.0);
    require(trace.burstiness(4) == 1.0);
    require(trace.burstiness() == 1.0);

    trace.append(5);                                    // [5, 5, 4, 6, 5]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 5);
    require(trace.front() == 5);
    require(trace.back() == 5);
    require(trace.exists(5));
    require(trace.exists(4));
    require(trace.exists(6));
    require(trace.nLabels() == 3);
    vector = trace.toVector();
    require(vector.size() == 5);
    require(vector[0] == 5);
    require(vector[1] == 5);
    require(vector[2] == 4);
    require(vector[3] == 6);
    require(vector[4] == 5);
    require(trace.burstiness(5) == 1.0);
    require(trace.burstiness(4) == 1.0);
    require(trace.burstiness(6) == 1.0);
    require(trace.burstiness() == 1.0);

    trace.append(5);                                    // [5, 5, 4, 6, 5, 5]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 6);
    require(trace.front() == 5);
    require(trace.back() == 5);
    require(trace.exists(5));
    require(trace.exists(4));
    require(trace.exists(6));
    require(trace.nLabels() == 3);
    vector = trace.toVector();
    require(vector.size() == 6);
    require(vector[0] == 5);
    require(vector[1] == 5);
    require(vector[2] == 4);
    require(vector[3] == 6);
    require(vector[4] == 5);
    require(vector[5] == 5);
    require(trace.burstiness(5) == 2.0/3);
    require(trace.burstiness(4) == 1.0);
    require(trace.burstiness(6) == 1.0);
    require(trace.burstiness() == 8.0/9);

    testIterationTraversal(trace);
}

template<class LabelType>
static void
test_append_strings() {
    Trace<LabelType> trace;
    trace.reserve(6);                                   // only a hint, but we can test that it compiles

    trace.append("e");                                  // [e]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 1);
    require(trace.front() == "e");
    require(trace.back() == "e");
    require(trace.exists("e"));
    require(!trace.exists("d"));
    require(trace.nLabels() == 1);
    std::vector<LabelType> vector = trace.toVector();
    require(vector.size() == 1);
    require(vector[0] == "e");
    require(trace.burstiness("e") == 0.0);
    require(trace.burstiness("d") == 0.0);
    require(trace.burstiness() == 0.0);

    trace.append("e");                                  // [e, e]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 2);
    require(trace.front() == "e");
    require(trace.back() == "e");
    require(trace.exists("e"));
    require(!trace.exists("d"));
    require(trace.nLabels() == 1);
    vector = trace.toVector();
    require(vector.size() == 2);
    require(vector[0] == "e");
    require(vector[1] == "e");
    require(trace.burstiness("e") == 1.0);
    require(trace.burstiness() == 1.0);

    trace.append("d");                                  // [e, e, d]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 3);
    require(trace.front() == "e");
    require(trace.back() == "d");
    require(trace.exists("e"));
    require(trace.exists("d"));
    require(trace.nLabels() == 2);
    vector = trace.toVector();
    require(vector.size() == 3);
    require(vector[0] == "e");
    require(vector[1] == "e");
    require(vector[2] == "d");
    require(trace.burstiness("e") == 1.0);
    require(trace.burstiness("d") == 0.0);
    require(trace.burstiness() == 1.0);

    trace.append("f");                                  // [e, e, d, f]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 4);
    require(trace.front() == "e");
    require(trace.back() == "f");
    require(trace.exists("e"));
    require(trace.exists("d"));
    require(trace.exists("f"));
    require(trace.nLabels() == 3);
    vector = trace.toVector();
    require(vector.size() == 4);
    require(vector[0] == "e");
    require(vector[1] == "e");
    require(vector[2] == "d");
    require(vector[3] == "f");
    require(trace.burstiness("e") == 1.0);
    require(trace.burstiness("d") == 1.0);
    require(trace.burstiness() == 1.0);

    trace.append("e");                                  // [e, e, d, f, e]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 5);
    require(trace.front() == "e");
    require(trace.back() == "e");
    require(trace.exists("e"));
    require(trace.exists("d"));
    require(trace.exists("f"));
    require(trace.nLabels() == 3);
    vector = trace.toVector();
    require(vector.size() == 5);
    require(vector[0] == "e");
    require(vector[1] == "e");
    require(vector[2] == "d");
    require(vector[3] == "f");
    require(vector[4] == "e");
    require(trace.burstiness("e") == 1.0);
    require(trace.burstiness("d") == 1.0);
    require(trace.burstiness("f") == 1.0);
    require(trace.burstiness() == 1.0);

    trace.append("e");                                  // [e, e, d, f, e, e]
    print(trace);
    require(!trace.isEmpty());
    require(trace.size() == 6);
    require(trace.front() == "e");
    require(trace.back() == "e");
    require(trace.exists("e"));
    require(trace.exists("d"));
    require(trace.exists("f"));
    require(trace.nLabels() == 3);
    vector = trace.toVector();
    require(vector.size() == 6);
    require(vector[0] == "e");
    require(vector[1] == "e");
    require(vector[2] == "d");
    require(vector[3] == "f");
    require(vector[4] == "e");
    require(vector[5] == "e");
    require(trace.burstiness("e") == 2.0/3);
    require(trace.burstiness("d") == 1.0);
    require(trace.burstiness("f") == 1.0);
    require(trace.burstiness() == 8.0/9);

    testIterationTraversal(trace);
}

// User-defined class
class UserLabel {
    std::string label;
public:
    UserLabel() {}

    // This constructor is only for our own test harness, so we can say: x == "e" where x is a UserClass instance
    /*implicit*/ UserLabel(const char *s): label(s) {}

    bool operator==(const UserLabel &other) const { return label == other.label; }
    bool operator<(const UserLabel &other) const { return label < other.label; }
};

// User-defined index (cheating here by using class derivation)
static size_t nUserIndexConstructed = 0;
template<class Label, class Value>
class UserIndex: public Sawyer::Container::TraceDetail::MapIndex<Label, Value> {
public:
    UserIndex() { ++nUserIndexConstructed; }
};

// Tag and specialization for user-defined index
class UserIndexTag {};

namespace Sawyer { namespace Container {
template<class Label, class Value>
struct TraceIndexTraits<Label, Value, UserIndexTag> {
    typedef UserIndex<Label, Value> Index;
};
}}

int
main() {
    test_empty<size_t>();
    test_empty<std::string>();
    test_empty<int>();

    // Integer types using a vector index
    test_append_integers<int, TraceVectorIndexTag>();
    test_append_integers<unsigned, TraceVectorIndexTag>();
    test_append_integers<short, TraceVectorIndexTag>();
    test_append_integers<unsigned short, TraceVectorIndexTag>();
    test_append_integers<long, TraceVectorIndexTag>();
    test_append_integers<unsigned long, TraceVectorIndexTag>();

    // Integer types using a map index (the TraceMapIndex tag is optional, and we tested that in test_empty() already)
    test_append_integers<int, TraceMapIndexTag>();
    test_append_integers<unsigned, TraceMapIndexTag>();
    test_append_integers<short, TraceMapIndexTag>();
    test_append_integers<unsigned short, TraceMapIndexTag>();
    test_append_integers<long, TraceMapIndexTag>();
    test_append_integers<unsigned long, TraceMapIndexTag>();

    // Integer types using a user-defined index
    nUserIndexConstructed = 0;
    test_append_integers<int, UserIndexTag>();
    test_append_integers<unsigned, UserIndexTag>();
    test_append_integers<short, UserIndexTag>();
    test_append_integers<unsigned short, UserIndexTag>();
    test_append_integers<long, UserIndexTag>();
    test_append_integers<unsigned long, UserIndexTag>();
    require(nUserIndexConstructed > 0);

    // String-like label types (it doesn't make sense to use a vector-based index here)
    test_append_strings<std::string>();
    test_append_strings<UserLabel>();
}
