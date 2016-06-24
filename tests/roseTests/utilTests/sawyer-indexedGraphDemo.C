// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Demonstrate some ways to use a Sawyer::Container::Graph with vertex and edge indexes

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <Sawyer/Graph.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demo 1: A graph whose vertices are unique std::string city names and whose edges are the time in hours it takes to travel
// from the source to the target city by train, including layovers.
//
// We want the vertices (cities) to be indexed so we don't have to keep track of them ourselves. I.e., we want to be able to
// find the vertex that represents a city when all we know is the city name.  We also want to get an error if we try to insert
// the same city twice.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [demo1]

// First, we need data types for what we'll be storing at each vertex and each edge. Sawyer's graph implementation divides
// concerns between the Sawyer library and the user in a manner very similar to STL containers. Take std::list for example: the
// STL is reponsible for managing the notions of vertices and the linear connectivity between vertices, while the user is
// responsible for his data stored at each node. Similarly, Sawyer's Graph type is responsible for managing the vertices and
// the connectivity between vertices (i.e., edges), while the user is responsible only for his data stored at each vertex
// and/or edge.
typedef std::string TrainCityName;                      // User's data stored at each vertex
typedef double TrainTravelTime;                         // User's data stored at each edge

// An indexed Graph needs to know what part of the user's node and/or edge data to use as the lookup keys. In this example, the
// lookup keys for the vertices are exactly the strings we stored there, and we don't need any index for edges.  Subsequent
// examples will expand on this.
typedef TrainCityName TrainVertexKey;

// Create the indexed graph. This is done by providing the 3rd and/or 4th template arguments: the types for the vertex and edge
// lookup keys. The defaults for these arguments are to not have an index; having an index might substantially slow down vertex
// and edge inserting and erasing.
typedef Sawyer::Container::GraphEdgeNoKey<TrainTravelTime> EdgeKey;
typedef Sawyer::Container::Graph<TrainCityName, TrainTravelTime, TrainVertexKey, EdgeKey> TrainGraph;

static void
demo1() {
    TrainGraph g;

    // Insert a few cities. Sawyer uses iterators also for pointers, just like an "int*" can point into a C array of integers
    // and also be used to iterate over that array. Incrementing these particular three iterators probably isn't too useful,
    // just as incrementing an "int*" to an array representation of a lattice isn't too useful.
    TrainGraph::VertexIterator albuquerque = g.insertVertex("Albuquerque");
    TrainGraph::VertexIterator boston = g.insertVertex("Boston");
    TrainGraph::VertexIterator chicago = g.insertVertex("Chicago");
    ASSERT_always_require(g.nVertices() == 3);

    // Since the vertices are indexed they must be unique. Trying to add another vertex with the same label will throw an
    // exception.
    try {
        g.insertVertex("Boston");
        ASSERT_not_reachable("insertion should have failed");
    } catch (Sawyer::Exception::AlreadyExists &e) {
    }

    // Due to the index, we can find a vertex given its value.  This is O(log|V|).
    TrainGraph::VertexIterator c2 = g.findVertexValue("Chicago");
    ASSERT_always_require(c2 == chicago);

    // We can also insert a vertex conditionally since we have an index.
    TrainGraph::VertexIterator denver = g.insertVertexMaybe("Denver"); // inserted since it doesn't exist yet
    TrainGraph::VertexIterator d2 = g.insertVertexMaybe("Denver");     // returns an existing vertex
    ASSERT_always_require(d2 == denver);

    // Each vertex stores its label which you can get in constant time.
    ASSERT_always_require(chicago->value() == "Chicago");

    // Lets add some edges
    TrainGraph::EdgeIterator ab = g.insertEdge(albuquerque, boston, 58); // 58 hours, etc.
    ASSERT_always_require(ab != g.edges().end());
    TrainGraph::EdgeIterator ba = g.insertEdge(boston, albuquerque, 54);
    ASSERT_always_require(ba != g.edges().end());
    TrainGraph::EdgeIterator ac = g.insertEdge(albuquerque, chicago, 27);
    ASSERT_always_require(ac != g.edges().end());
    TrainGraph::EdgeIterator ca = g.insertEdge(chicago, albuquerque, 24);
    ASSERT_always_require(ca != g.edges().end());
    TrainGraph::EdgeIterator bc = g.insertEdge(boston, chicago, 23);
    ASSERT_always_require(bc != g.edges().end());
    TrainGraph::EdgeIterator cb = g.insertEdge(chicago, boston, 25);
    ASSERT_always_require(cb != g.edges().end());
    ASSERT_always_require(g.nEdges() == 6);

    // We can add vertices at the same time if they don't exist.
    TrainGraph::EdgeIterator ce = g.insertEdgeWithVertices("Chicago", "Effingham", 6 /*hours*/);
    ASSERT_always_require(g.nVertices() == 5);
    ASSERT_always_require(g.nEdges() == 7);
    TrainGraph::VertexIterator effingham = ce->target();
    ASSERT_always_require(effingham->value() == "Effingham");

    // Similarly, we can have the graph automatically erase a vertex when we erase that vertex's last edge. Unfortunately this
    // leaves the effingham pointer dangling, which might result in a segfault if we try to dereference it.
    g.eraseEdgeWithVertices(ce);
    ASSERT_always_require(g.nVertices() == 4);
    ASSERT_always_require(g.nEdges() == 6);
    ASSERT_always_forbid(g.isValidVertex(g.findVertexValue("Effingham")));

    // Since edges are not indexed in our graph, adding the same edge twice results in two parallel edges with the same value.
    TrainGraph::EdgeIterator ab2 = g.insertEdge(albuquerque, boston, 58);
    ASSERT_always_require(g.nEdges() == 7);
    ASSERT_always_require(ab != ab2);
    g.eraseEdge(ab2);                                   // let's erase it
    ab2 = g.edges().end();                              // good programming practice to avoid dangling pointers
    ASSERT_always_require(g.nEdges() == 6);

    // We can erase a vertex, which also erases the incident edges.
    g.eraseVertex(chicago);
    effingham = g.vertices().end();
    ASSERT_always_require(g.nVertices() == 3);          // Albuquerque, Boston, Denver
    ASSERT_always_require(g.nEdges() == 2);             // Albuquerque->Boston and Boston->Albuquerque
}

//! [demo1]

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demo 2: This demo builds on the previous demo by defining a graph that stores multiple things at each vertex, two of which
// are used as the key. The edges represent layovers at airports.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [demo2]

// Each vertex stores a Flight
struct Flight {
    std::string airline;
    int number;
    double cost;
    // pretend there's lots more...

    Flight(const std::string airline, int number, double cost = 0.0)
        : airline(airline), number(number), cost(cost) {}
};

// The FlightKey is used to index the vertices. It needs the following functionality:
//    1. A copy constructor
//    2. Construction from a vertex value (Flight)
//    3. Comparison (operator <)
//
// In the previous demo, where the user's vertex data and key were both std::string, it happens that std::string satisfies all
// these requirements. In this Flight demo, we could have done something similar by making Flight both the vertex type and the
// key type and adding an operator<. The reason that's not a good idea is that it would make the key much heavier than it needs
// to be -- we'd end up storing two copies of each vertex's data: one in the vertex and another in the vertex index, when all
// we really need to store in the index is a short string. This also demonstrates that the key can be constructed on the fly
// from the vertex's value (as long as the same key is always generated for the same value).
class FlightKey {
    std::string key_;
public:
    // Extract the key from the flight
    explicit FlightKey(const Flight &flight) {
        key_ = flight.airline + " " + boost::lexical_cast<std::string>(flight.number);
    }

    bool operator<(const FlightKey &other) const {
        return key_ < other.key_;
    }
};

// The graph edges store the time in minutes for a layover between two flights. We don't index the edges.
struct Layover {
    std::string airportCode;
    unsigned minutes;

    Layover(const std::string code, unsigned minutes)
        : airportCode(code), minutes(minutes) {}
};

// Create the graph with vertex indexing.
typedef Sawyer::Container::Graph<Flight, Layover, FlightKey> FlightGraph;

static void
demo2() {
    FlightGraph g;

    // Insert a couple flights as vertices
    FlightGraph::VertexIterator ua9934 = g.insertVertex(Flight("United", 9934, 187.00));
    FlightGraph::VertexIterator d1540 = g.insertVertex(Flight("Delta", 1540, 218.00));
    ASSERT_always_require(g.nVertices() == 2);

    // We can look up a vertex with given flight information. Since the cost isn't part of the lookup key, we can provide
    // anything we want.
    FlightGraph::VertexIterator v1 = g.findVertexValue(Flight("United", 9934));
    ASSERT_always_require(v1 == ua9934);

    // We can add a layover between two flights
    FlightGraph::EdgeIterator lay1 = g.insertEdge(ua9934, d1540, Layover("ABQ", 120));
    ASSERT_always_require(g.nEdges() == 1);

    // We can add a layover while populating the flights at the same time.
    FlightGraph::EdgeIterator lay2 = g.insertEdgeWithVertices(Flight("Alaska", 123, 99.00),
                                                              Flight("Alaska", 456, 99.00),
                                                              Layover("DIA", 60));
    ASSERT_always_require(g.nVertices() == 4);
    ASSERT_always_require(g.nEdges() == 2);

    // We're prevented from adding the same flight twice even if it has a different cost.
    try {
        g.insertVertex(Flight("United", 9934, 340.52));
        ASSERT_not_reachable("duplicate vertex insertion should have failed");
    } catch (const Sawyer::Exception::AlreadyExists&) {
        ASSERT_always_require(g.nVertices() == 4);
    }

    // We can remove a layover along with removing flights that no longer have edges.
    g.eraseEdgeWithVertices(lay2);
    ASSERT_always_require(g.nVertices() == 2);
    ASSERT_always_require(g.nEdges() == 1);

    // We can remove an edge without removing vertices
    g.eraseEdge(lay1);
    ASSERT_always_require(g.nVertices() == 2);
    ASSERT_always_require(g.nEdges() == 0);
}

//! [demo2]

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demo 3: The default graph index is based on std::map and has guaranteed O(log N) lookup time.  If you want to use a
// hash-based index you can quite easily do that.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [demo3]

// Define the type of values we'll store at each vertex.
struct Person {
    std::string lastName, firstName;
    int birthYear;
    std::string stateIssuedId;

    Person(const std::string &lastName, const std::string &firstName, int birthYear, const std::string &id)
        : lastName(lastName), firstName(firstName), birthYear(birthYear), stateIssuedId(id) {}
};

// Use the state-issued ID number as the unique key to identify each person.
struct PersonKey {
    std::string id;

    // Needed by Sawyer::Container::Graph
    explicit PersonKey(const Person &p): id(p.stateIssuedId) {}

    // Needed by boost::unordered_map
    bool operator==(const PersonKey &other) const {
        return id == other.id;
    }
};

// Needed by boost::unordered_map
size_t
hash_value(const PersonKey &key) {
    size_t seed = 0;
    BOOST_FOREACH (char ch, key.id)
        boost::hash_combine(seed, ch);
    return seed;
}

// Partly specialize the Sawyer::Container::GraphIndexTraits for our key type.  You could do this by hand, but this macro is
// more convenient. It must be at global scope. The "2" means the GraphHashIndex class template takes two arguments: PersonKey
// and a graph iterator.
SAWYER_GRAPH_INDEXING_SCHEME_2(PersonKey, Sawyer::Container::GraphHashIndex);

static void
demo3() {
    typedef Sawyer::Container::Graph<Person, Sawyer::Nothing, PersonKey> Graph;
    Graph g;

    g.insertVertex(Person("Bear", "Smokey", 1944, "123-45-6789"));
    try {
        g.insertVertex(Person("Bear", "Yogi", 1958, "123-45-6789")); // Evil Yogi stealing Smokey's ID
        ASSERT_not_reachable("inserting the same ID twice should have failed");
    } catch (const Sawyer::Exception::AlreadyExists&) {
    }
};

//! [demo3]

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demo 4: Using your own index type.  Sawyer provides a map-based index with O(log N) time and a hash-based index with O(N)
// lookup time (but nominally constant).  Users can also create their own index if neither of these are sufficient.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [demo4]

// Declare our vertex values and keys.  We'll keep this example simple by having each vertex store only an integer label which
// is also used as its key.
typedef size_t VertexLabel;

// Declare the index type and define its required operations.  The vertex labels are assumed to be small integers, thus we can
// store them in a vector to acheive constant lookup time. However, we need to be able to keep track of which vector elements
// are occupied and which aren't (in order to represent holes in our label space created perhaps by erasing vertices), so we'll
// use Sawyer::Optional.
template<class ConstVertexIterator>
class VertexLabelIndex {
    typedef std::vector<Sawyer::Optional<ConstVertexIterator> > Vector;
    Vector vector_;

public:
    void clear() {
        vector_.clear();
    }

    void insert(const VertexLabel &label, const ConstVertexIterator &iter) {
        if (label >= vector_.size())
            vector_.resize(label+1);
        vector_[label] = iter;
    }

    void erase(const VertexLabel &label) {
        if (label < vector_.size())
            vector_[label] = Sawyer::Nothing();
    }

    Sawyer::Optional<ConstVertexIterator> lookup(const VertexLabel &label) const {
        return label < vector_.size() ? vector_[label] : Sawyer::Optional<ConstVertexIterator>();
    }
};

// Use this convenience macro to partly specialize Sawyer::Container::GraphIndexTraits. This has to be at global scope and the
// index is expected to take one template argument: a graph iterator type. If this doesn't meet your needs then take a look at
// the macro definition -- there's nothing magical about it.
SAWYER_GRAPH_INDEXING_SCHEME_1(VertexLabel, VertexLabelIndex);

// Now do some things with this graph
static void
demo4() {
    typedef Sawyer::Container::Graph<VertexLabel, Sawyer::Nothing, VertexLabel> Graph;
    Graph g;

    g.insertVertex(1);
    g.insertVertex(2);
    g.insertVertex(3);
    g.insertVertex(8);
    g.insertEdge(g.findVertexValue(3), g.findVertexValue(8));

    try {
        g.insertVertex(2);
        ASSERT_not_reachable("inserting the same vertex label twice should have failed");
    } catch (const Sawyer::Exception::AlreadyExists&) {
    }

    // Erasing a vertex doesn't change any of our vertex labels--it just leaves a hole. This is in contrast to Sawyer's vertex
    // ID numbers which are always consecutive.
    g.eraseVertex(g.findVertexValue(2));
    Graph::VertexIterator v3 = g.findVertexValue(3);
    ASSERT_always_require(g.isValidVertex(v3));         // i.e, v3 != g.vertices().end()
    ASSERT_always_require(v3->nOutEdges() == 1);
    Graph::EdgeIterator e38 = v3->outEdges().begin();
    Graph::VertexIterator v8 = g.findVertexValue(8);
    ASSERT_always_require(g.isValidVertex(v8));
    ASSERT_always_require(e38->target() == v8);

    // And we should be able to re-insert vertex with label 2 again.
    g.insertVertex(2);
}

//! [demo4]

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main() {
    demo1();
    demo2();
    demo3();
    demo4();
}

