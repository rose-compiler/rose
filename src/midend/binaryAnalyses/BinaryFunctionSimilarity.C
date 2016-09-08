#include <sage3basic.h>

#include <BinaryFunctionSimilarity.h>
#include <EditDistance/LinearEditDistance.h>
#include <Partitioner2/Partitioner.h>

#ifdef ROSE_HAVE_DLIB
    #include <dlib/matrix.h>
    #include <dlib/optimization.h>
#endif

namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {

#ifdef ROSE_HAVE_DLIB
typedef dlib::matrix<double> DistanceMatrix;
#else
struct DistanceMatrixDummy {
    DistanceMatrixDummy(long, long) {}
    long nr() const { ASSERT_not_reachable("no dlib support"); }
    long nc() const { ASSERT_not_reachable("no dlib support"); }
    double& operator()(long, long) { ASSERT_not_reachable("no dlib support"); }
    double operator()(long, long) const { ASSERT_not_reachable("no dlib support"); }
};
typedef DistanceMatrixDummy DistanceMatrix;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream &out, const FunctionSimilarity &x) {
    x.printCharacteristicValues(out);
    return out;
}

double
cartesianDistance(const FunctionSimilarity::CartesianPoint &a, const FunctionSimilarity::CartesianPoint &b) {
    ASSERT_require(a.size() == b.size());
    double sum = 0.0;
    for (size_t i=0; i<a.size(); ++i)
        sum += (a[i]-b[i]) * (a[i]-b[i]);
    return sqrt(sum);
}

// Find a 1:1 mapping from rows to columns of the specified square matrix such that the total cost is minimized. Returns a
// vector V such that V[i] = j maps rows i to columns j.
static std::vector<long>
findMinimumAssignment(const DistanceMatrix &matrix) {
#ifdef ROSE_HAVE_DLIB
    ASSERT_forbid(matrix.size() == 0);
    ASSERT_require(matrix.nr() == matrix.nc());

    // We can avoid the O(n^3) Kuhn-Munkres algorithm if all values of the matrix are the same.
    double minValue, maxValue;
    dlib::find_min_and_max(matrix, minValue /*out*/, maxValue /*out*/);
    if (minValue == maxValue) {
        std::vector<long> ident;
        ident.reserve(matrix.nr());
        for (long i=0; i<matrix.nr(); ++i)
            ident.push_back(i);
        return ident;
    }

    // Dlib's Kuhn-Munkres finds the *maximum* mapping over *integers*, so we negate everything to find the minumum, and we map
    // the doubles onto a reasonably large interval of integers. The interval should be large enough to have some precision,
    // but not so large that things might overflow.
    const int iGreatest = 1000000;                      // arbitrary upper bound for integer interval
    dlib::matrix<long> intMatrix(matrix.nr(), matrix.nc());
    for (long i=0; i<matrix.nr(); ++i) {
        for (long j=0; j<matrix.nc(); ++j)
            intMatrix(i, j) = round(-iGreatest * (matrix(i, j) - minValue) / (maxValue - minValue));
    }
    return dlib::max_cost_assignment(intMatrix);
#else
    throw FunctionSimilarity::Exception("dlib support is necessary for FunctionSimilarity analysis"
                                        "; see ROSE installation instructions");
#endif
}

// Given a square matrix and a 1:1 mapping from rows to columns, return the total cost of the mapping.
static double
totalAssignmentCost(const DistanceMatrix &matrix, const std::vector<long> assignment) {
    double sum = 0.0;
    ASSERT_require(matrix.nr() == matrix.nc());
    ASSERT_require((size_t)matrix.nr() == assignment.size());
    for (long i=0; i<matrix.nr(); ++i) {
        ASSERT_require(assignment[i] < matrix.nc());
        sum += matrix(i, assignment[i]);
    }
    return sum;
}

// Combine some values into a single value
double
combine(FunctionSimilarity::Statistic s, const std::vector<double> &values) {
    ASSERT_forbid(values.empty());
    switch (s) {
        case FunctionSimilarity::AVERAGE: {
            double sum = 0.0;
            BOOST_FOREACH (double v, values)
                sum += v;
            return sum / values.size();
        }
        case FunctionSimilarity::MAXIMUM: {
            return *std::max_element(values.begin(), values.end());
        }
        case FunctionSimilarity::MEDIAN: {
            std::vector<double> tmp = values;
            std::nth_element(tmp.begin(), tmp.begin() + tmp.size()/2, tmp.end());
            double retval = tmp[tmp.size()/2];
            if (0 == values.size() % 2) {
                std::nth_element(tmp.begin(), tmp.begin() + tmp.size()/2 + 1, tmp.end());
                retval = (retval + tmp[tmp.size()/2 + 1]) / 2.0;
            }
            return retval;
        }
    }
    ASSERT_not_reachable("invalid statistic");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionSimilarity::CategoryId
FunctionSimilarity::declarePointCategory(const std::string &name, size_t dimensionality, bool allowExisting) {
    ASSERT_forbid(name.empty());
    ASSERT_require(dimensionality > 0);
    CategoryId id = categoryNames_.insertMaybe(name, categories_.size());
    if (id == categories_.size()) {
        categories_.push_back(Category(name, CARTESIAN_POINT));
        categories_.back().dimensionality = dimensionality;
    } else if (!allowExisting) {
        throw Exception("category \"" + StringUtility::cEscape(name) + "\" already exists");
    }
    return id;
}

FunctionSimilarity::CategoryId
FunctionSimilarity::declareListCategory(const std::string &name, bool allowExisting) {
    ASSERT_forbid(name.empty());
    CategoryId id = categoryNames_.insertMaybe(name, categories_.size());
    if (id == categories_.size()) {
        categories_.push_back(Category(name, ORDERED_LIST));
    } else if (!allowExisting) {
        throw Exception("category \"" + StringUtility::cEscape(name) + "\" already exists");
    }
    return id;
}

FunctionSimilarity::CValKind
FunctionSimilarity::categoryKind(CategoryId id) const {
    ASSERT_require(id < categories_.size());
    return categories_[id].kind;
}

size_t
FunctionSimilarity::categoryDimensionality(CategoryId id) const {
    ASSERT_require(id < categories_.size());
    return categories_[id].dimensionality;
}

double
FunctionSimilarity::categoryWeight(CategoryId id) const {
    ASSERT_require(id < categories_.size());
    return categories_[id].weight;
}

void
FunctionSimilarity::categoryWeight(CategoryId id, double wt) {
    ASSERT_require(id < categories_.size());
    categories_[id].weight = wt;
}

void
FunctionSimilarity::insertPoint(const P2::Function::Ptr &function, CategoryId id, const CartesianPoint &point) {
    ASSERT_not_null(function);
    ASSERT_require(id < categories_.size());
    ASSERT_require(categories_[id].kind == CARTESIAN_POINT);
    ASSERT_require(categories_[id].dimensionality == point.size());
    FunctionInfo &finfo = functions_.insertMaybeDefault(function);
    if (id >= finfo.categories.size())
        finfo.categories.resize(id+1);
    finfo.categories[id].pointCloud.push_back(point);
}

void
FunctionSimilarity::insertList(const P2::FunctionPtr &function, CategoryId id, const OrderedList &list) {
    ASSERT_not_null(function);
    ASSERT_require(id < categories_.size());
    ASSERT_require(categories_[id].kind == ORDERED_LIST);
    FunctionInfo &finfo = functions_.insertMaybeDefault(function);
    if (id >= finfo.categories.size())
        finfo.categories.resize(id+1);
    finfo.categories[id].orderedLists.push_back(list);
}

size_t
FunctionSimilarity::size(const P2::Function::Ptr &function, CategoryId id) const {
    if (!function || id>=categories_.size() || !functions_.exists(function))
        return 0;
    const FunctionInfo &finfo = functions_[function];
    if (id >= finfo.categories.size())
        return 0;
    return std::max(finfo.categories[id].pointCloud.size(), finfo.categories[id].orderedLists.size());
}

const FunctionSimilarity::PointCloud&
FunctionSimilarity::points(const P2::Function::Ptr &function, CategoryId id) const {
    static const PointCloud none;
    if (0 == size(function, id))
        return none;
    return functions_[function].categories[id].pointCloud;
}

const FunctionSimilarity::OrderedLists&
FunctionSimilarity::lists(const P2::Function::Ptr &function, CategoryId id) const {
    static const OrderedLists none;
    if (0 == size(function, id))
        return none;
    return functions_[function].categories[id].orderedLists;
}

double
FunctionSimilarity::compare(const P2::Function::Ptr &f1, const P2::Function::Ptr &f2) const {
    ASSERT_not_null(f1);
    ASSERT_not_null(f2);

    if (!functions_.exists(f1))
        throw Exception(f1->printableName() + " is not analyzed");
    if (!functions_.exists(f2))
        throw Exception(f2->printableName() + " is not analyzed");
    const FunctionInfo &finfo1 = functions_[f1];
    const FunctionInfo &finfo2 = functions_[f2];

    std::vector<double> categoryDistances;
    for (CategoryId id=0; id<categories_.size(); ++id) {
        double d = NAN;
        switch (categories_[id].kind) {
            case CARTESIAN_POINT:
                d = comparePointClouds(finfo1.categories[id].pointCloud, finfo2.categories[id].pointCloud);
                break;
            case ORDERED_LIST:
                d = compareOrderedLists(finfo1.categories[id].orderedLists, finfo2.categories[id].orderedLists);
                break;
        }
        ASSERT_require(!isnan(d));
        categoryDistances.push_back(d * categories_[id].weight);
    }
    return combine(categoryAccumulatorType_, categoryDistances);
}

std::vector<FunctionSimilarity::FunctionDistancePair>
FunctionSimilarity::compareOneToAll(const P2::Function::Ptr &needle) const {
    return compareOneToMany(needle, functions_.keys());
}

std::vector<FunctionSimilarity::FunctionPair>
FunctionSimilarity::minimumCostMapping(const std::vector<P2::Function::Ptr> &list1,
                                       const std::vector<P2::Function::Ptr> &list2) const {
    size_t n = std::max(list1.size(), list2.size());
    DistanceMatrix dm(n, n);
    for (size_t i=0; i<n; ++i) {
        P2::Function::Ptr f1 = i < list1.size() ? list1[i] : P2::Function::Ptr();
        for (size_t j=0; j<n; ++j) {
            P2::Function::Ptr f2 = j < list2.size() ? list2[j] : P2::Function::Ptr();
            dm(i, j) = compare(f1, f2);
        }
    }

    std::vector<long> assignment = findMinimumAssignment(dm);
    ASSERT_require(assignment.size() == n);
    std::vector<FunctionPair> retval;
    retval.reserve(n);
    for (size_t i=0; i<n; ++i) {
        size_t j = assignment[i];
        retval.push_back(FunctionPair(i < list1.size() ? list1[i] : P2::Function::Ptr(),
                                      j < list2.size() ? list2[j] : P2::Function::Ptr()));
    }
    return retval;
}

// class method
double
FunctionSimilarity::comparePointClouds(const PointCloud &points1, const PointCloud &points2) {
    size_t size = std::max(points1.size(), points2.size());
    if (0 == size)
        return 0.0;
    size_t dimensionality = points1.empty() ? points2[0].size() : points1[0].size();
    const CartesianPoint origin(dimensionality, 0.0);
    DistanceMatrix dm(size, size);
    for (size_t i=0; i<size; ++i) {
        const CartesianPoint &p1 = i < points1.size() ? points1[i] : origin;
        for (size_t j=0; j<size; ++j) {
            const CartesianPoint &p2 = j < points2.size() ? points2[j] : origin;
            dm(i, j) = cartesianDistance(p1, p2);
        }
    }
    return totalAssignmentCost(dm, findMinimumAssignment(dm)) / size;
}

// class method
double
FunctionSimilarity::compareOrderedLists(const OrderedLists &lists1, const OrderedLists &lists2) {
    static const OrderedList empty;
    size_t nLists = std::max(lists1.size(), lists2.size());
    if (0 == nLists)
        return 0.0;
    double sum = 0.0;
    for (size_t i=0; i<nLists; ++i) {
        const OrderedList &list1 = i < lists1.size() ? lists1[i] : empty;
        const OrderedList &list2 = i < lists2.size() ? lists2[i] : empty;
        if (!list1.empty() || !list2.empty())
            sum += (double)EditDistance::levenshteinDistance(list1, list2) / std::max(list1.size(), list2.size());
    }
    return sum / nLists;
}

typedef P2::ControlFlowGraph::ConstVertexIterator (*LinkageDirection)(const P2::ControlFlowGraph::Edge&);

static P2::ControlFlowGraph::ConstVertexIterator
forward(const P2::ControlFlowGraph::Edge &e) { return e.target(); }

static P2::ControlFlowGraph::ConstVertexIterator
reverse(const P2::ControlFlowGraph::Edge &e) { return e.source(); }

// Convert an edge list (e.g., predecessors or successors) to a value in [0.0, 1.0].
static double
normalizedNumberOfNeighbors(const boost::iterator_range<P2::ControlFlowGraph::ConstEdgeIterator> &edgeList,
                            LinkageDirection direction) {
    size_t n = 0;
    BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, edgeList) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = direction(edge);
        if (vertex->value().type() == P2::V_INDETERMINATE) {
            return 1.0;
        } else if (++n >= 3) {
            return 1.0;
        }
    }
    switch (n) {
        case 0: return 0.0;
        case 1: return 1.0/3;
        case 2: return 2.0/3;
    }
    ASSERT_not_reachable("logic error");
}

FunctionSimilarity::CategoryId
FunctionSimilarity::declareCfgConnectivity(const std::string &categoryName) {
    return declarePointCategory(categoryName, 4, false /*error if exists*/);
}

void
FunctionSimilarity::measureCfgConnectivity(CategoryId id, const P2::Partitioner &partitioner,
                                           const P2::Function::Ptr &function) {
    BOOST_FOREACH (rose_addr_t bbva, function->basicBlockAddresses()) {
        CartesianPoint point;
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(bbva);
        if (partitioner.cfg().isValidVertex(vertex)) {
            // Direct neighbors
            point.push_back(normalizedNumberOfNeighbors(vertex->outEdges(), forward));
            point.push_back(normalizedNumberOfNeighbors(vertex->inEdges(), reverse));

            // Second-level neighbors
            double totalForward = 0.0, totalReverse = 0.0;
            BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, vertex->outEdges())
                totalForward = normalizedNumberOfNeighbors(edge.target()->outEdges(), forward);
            BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, vertex->inEdges())
                totalReverse = normalizedNumberOfNeighbors(edge.source()->inEdges(), reverse);
            point.push_back(vertex->nOutEdges() ? totalForward/vertex->nOutEdges() : 0.0);
            point.push_back(vertex->nInEdges() ? totalReverse/vertex->nInEdges() : 0.0);

            insertPoint(function, id, point);
        }
    }
}

FunctionSimilarity::CategoryId
FunctionSimilarity::declareMnemonicStream(const std::string &categoryName) {
    return declareListCategory(categoryName, false /*error if exists*/);
}

void
FunctionSimilarity::measureMnemonicStream(CategoryId id, const P2::Partitioner &partitioner,
                                          const P2::Function::Ptr &function) {
    BOOST_FOREACH (rose_addr_t bbva, function->basicBlockAddresses()) {
        if (P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(bbva)) {
            OrderedList list;
            BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                list.push_back(insn->get_anyKind());
            insertList(function, id, list);
        }
    }
}

void
FunctionSimilarity::printCharacteristicValues(std::ostream &out) const {
    out <<"FunctionSimilarity characteristic values for all functions:\n";
    BOOST_FOREACH (const Functions::Node &fnode, functions_.nodes()) {
        out <<"  " <<fnode.key()->printableName() <<":\n";
        for (size_t id=0; id<categories_.size(); ++id) {
            out <<"    category \"" <<StringUtility::cEscape(categories_[id].name) <<"\""
                <<", weight=" <<categories_[id].weight;
            const FunctionInfo &finfo = fnode.value();
            if (id < finfo.categories.size()) {
                switch (categories_[id].kind) {
                    case CARTESIAN_POINT:
                        out <<", npoints=" <<finfo.categories[id].pointCloud.size() <<":\n";
                        ASSERT_require(finfo.categories[id].orderedLists.empty());
                        BOOST_FOREACH (const CartesianPoint &pt, finfo.categories[id].pointCloud) {
                            ASSERT_require(pt.size() == categories_[id].dimensionality);
                            out <<"      (";
                            for (size_t i=0; i<pt.size(); ++i)
                                out <<(0==i?"":", ") <<pt[i];
                            out <<")\n";
                        }
                        break;
                    case ORDERED_LIST:
                        out <<", nlists=" <<finfo.categories[id].orderedLists.size() <<":\n";
                        ASSERT_require(finfo.categories[id].pointCloud.empty());
                        BOOST_FOREACH (const OrderedList &list, finfo.categories[id].orderedLists) {
                            out <<"      [";
                            for (size_t i=0; i<list.size(); ++i)
                                out <<(0==i?"":", ") <<list[i];
                            out <<"]\n";
                        }
                        break;
                }
            } else {
                out <<", empty\n";
            }
        }
    }
}

} // namespace
} // namespace
