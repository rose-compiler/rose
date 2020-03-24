#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>

#include <rose_isnan.h>

#include <BinaryFunctionSimilarity.h>
#include <Combinatorics.h>
#include <CommandLine.h>
#include <Diagnostics.h>
#include <EditDistance/LinearEditDistance.h>
#include <Partitioner2/Partitioner.h>

#include <Sawyer/ProgressBar.h>
#include <Sawyer/Stopwatch.h>
#include <Sawyer/ThreadWorkers.h>

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {


Sawyer::Message::Facility FunctionSimilarity::mlog;

// Approx number of tasks to create for each worker thread. The finest granularity of work (a single comparison between two
// functions) is often not the most efficient way to schedule worker threads because if the comparisons are cheap then the
// scheduling overhead accounts for a higher percentage of the total time. The opposite exteme of one task per thread, is also
// not always efficient because the length of the tasks can be quite different from one another. A compromise is to create some
// fixed number of tasks per worker thread so the individual tasks are larger, but there's still enough of them to balance the
// work load.
static const size_t tasksPerWorker = 100;               // arbitrary

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
FunctionSimilarity::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::FunctionSimilarity");
        mlog.comment("matching function pairs based on similarity");
    }
}

std::ostream&
operator<<(std::ostream &out, const FunctionSimilarity &x) {
    x.printCharacteristicValues(out);
    return out;
}

// class method
double
FunctionSimilarity::cartesianDistance(const FunctionSimilarity::CartesianPoint &a,
                                      const FunctionSimilarity::CartesianPoint &b) {
    ASSERT_require(a.size() == b.size());
    double sum = 0.0;
    for (size_t i=0; i<a.size(); ++i)
        sum += (a[i]-b[i]) * (a[i]-b[i]);
    return sqrt(sum);
}

// class method
std::vector<size_t>
FunctionSimilarity::findMinimumAssignment(const DistanceMatrix &matrix) {
#ifdef ROSE_HAVE_DLIB
    ASSERT_forbid(matrix.size() == 0);
    ASSERT_require(matrix.nr() == matrix.nc());

    // We can avoid the O(n^3) Kuhn-Munkres algorithm if all values of the matrix are the same.
    double minValue, maxValue;
    dlib::find_min_and_max(matrix.dlib(), minValue /*out*/, maxValue /*out*/);
    if (minValue == maxValue) {
        std::vector<size_t> ident;
        ident.reserve(matrix.nr());
        for (size_t i=0; i<matrix.nr(); ++i)
            ident.push_back(i);
        return ident;
    }

    // Dlib's Kuhn-Munkres finds the *maximum* mapping over *integers*, so we negate everything to find the minumum, and we map
    // the doubles onto a reasonably large interval of integers. The interval should be large enough to have some precision,
    // but not so large that things might overflow.
    const int iGreatest = 1000000;                      // arbitrary upper bound for integer interval
    dlib::matrix<long> intMatrix(matrix.nr(), matrix.nc());
    for (size_t i=0; i<matrix.nr(); ++i) {
        for (size_t j=0; j<matrix.nc(); ++j)
            intMatrix(i, j) = round(-iGreatest * (matrix(i, j) - minValue) / (maxValue - minValue));
    }

    // Return a proper type -- indexes and sizes should never be signed types since it makes no sense for them to be negative.
    std::vector<size_t> retval;
    retval.reserve(matrix.nr());
    BOOST_FOREACH (double d, dlib::max_cost_assignment(intMatrix))
        retval.push_back(d);
    return retval;
#else
    throw FunctionSimilarity::Exception("dlib support is necessary for FunctionSimilarity analysis"
                                        "; see ROSE installation instructions");
#endif
}

// class method
double
FunctionSimilarity::totalAssignmentCost(const DistanceMatrix &matrix, const std::vector<size_t> &assignment) {
    double sum = 0.0;
    ASSERT_require(matrix.nr() == matrix.nc());
    ASSERT_require((size_t)matrix.nr() == assignment.size());
    for (size_t i=0; i<matrix.nr(); ++i) {
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
FunctionSimilarity::compare(const P2::Function::Ptr &f1, const P2::Function::Ptr &f2, double dflt) const {
    ASSERT_require(f1 != NULL || f2 != NULL);
    std::vector<double> categoryDistances;

    // If only one function is supplied, then compare it against no-function (i.e., use the distance of the non-null function
    // from the origin).
    const FunctionInfo empty;
    const FunctionInfo &finfo1 = f1 ? functions_[f1] : empty;
    const FunctionInfo &finfo2 = f2 ? functions_[f2] : empty;

    for (CategoryId id=0; id<categories_.size(); ++id) {
        double d = NAN;
        switch (categories_[id].kind) {
            case CARTESIAN_POINT: {
                static const PointCloud emptyCloud;
                const PointCloud &cd1 = id < finfo1.categories.size() ? finfo1.categories[id].pointCloud : emptyCloud;
                const PointCloud &cd2 = id < finfo2.categories.size() ? finfo2.categories[id].pointCloud : emptyCloud;
                d = comparePointClouds(cd1, cd2);
                break;
            }
            case ORDERED_LIST: {
                static const OrderedLists emptyLists;
                const OrderedLists &cd1 = id < finfo1.categories.size() ? finfo1.categories[id].orderedLists : emptyLists;
                const OrderedLists &cd2 = id < finfo2.categories.size() ? finfo2.categories[id].orderedLists : emptyLists;
                d = compareOrderedLists(cd1, cd2);
                break;
            }
        }
        ASSERT_require(!rose_isnan(d));
        categoryDistances.push_back(d * categories_[id].weight);
    }
    return combine(categoryAccumulatorType_, categoryDistances);
}

std::vector<FunctionSimilarity::FunctionDistancePair>
FunctionSimilarity::compareOneToAll(const P2::Function::Ptr &needle) const {
    return compareOneToMany(needle, functions_.keys());
}

// Represents a single task in a multi-threaded collection of tasks. The task is to fill in part of a row-major matrix of
// distances between pairs of functions, beginning at the specified row and column of the matrix and comparing N pairs.
struct ComparisonTask {
    size_t startRow, startCol, nComparisons;
    double *results;

    ComparisonTask()
        : startRow(0), startCol(0), nComparisons(0), results(0) {}

    ComparisonTask(size_t startRow, size_t startCol, size_t nComparisons, double *results)
        : startRow(startRow), startCol(startCol), nComparisons(nComparisons), results(results) {}
};

// Collection of tasks which the worker threads process
typedef Sawyer::Container::Graph<ComparisonTask> ComparisonTasks;

// How a worker thread processes one task.
struct ComparisonFunctor {
    const FunctionSimilarity *self;
    const std::vector<P2::Function::Ptr> &rowFunctions; // functions for each row of the matrix
    const std::vector<P2::Function::Ptr> &colFunctions; // functions for each column of the matrix
    const size_t matrixSize;                            // number of rows and columns in square matrix
    Progress::Ptr progress;
    Sawyer::ProgressBar<size_t> &progressBar;
    const double dfltCompare;                           // distance between functions when either has no data

    ComparisonFunctor(const FunctionSimilarity *self,
                      const std::vector<P2::Function::Ptr> &rowFunctions,
                      const std::vector<P2::Function::Ptr> &colFunctions,
                      const Progress::Ptr &progress, Sawyer::ProgressBar<size_t> &progressBar,
                      double dfltCompare)
        : self(self), rowFunctions(rowFunctions), colFunctions(colFunctions),
          matrixSize(std::max(rowFunctions.size(), colFunctions.size())),
          progress(progress), progressBar(progressBar), dfltCompare(dfltCompare) {}

    void operator()(size_t taskId, const ComparisonTask &task) {
        ASSERT_require(task.nComparisons > 0);
        ASSERT_not_null(task.results);
        ASSERT_require(task.startRow < matrixSize);
        ASSERT_require(task.startCol < matrixSize);
        ASSERT_require(task.startRow * matrixSize + task.startCol + task.nComparisons <= matrixSize * matrixSize);

        size_t i = task.startRow;
        size_t j = task.startCol;
        for (size_t k=0; k<task.nComparisons; ++k) {
            P2::Function::Ptr a = i < rowFunctions.size() ? rowFunctions[i] : P2::Function::Ptr();
            P2::Function::Ptr b = j < colFunctions.size() ? colFunctions[j] : P2::Function::Ptr();
            task.results[k] = self->compare(a, b, dfltCompare);
            if (++j == matrixSize) {
                ++i;
                j = 0;
            }
        }
        progressBar.increment(task.nComparisons);
        progress->update(progressBar.ratio());
    }
};

// Monitor progress of comparison for debugging
struct ComparisonMonitor {
    const FunctionSimilarity *analyzer;
    const std::vector<P2::Function::Ptr> &rowFunctions; // functions for each row of the matrix
    const std::vector<P2::Function::Ptr> &colFunctions; // functions for each column of the matrix
    Sawyer::Message::Stream &stream;

    explicit ComparisonMonitor(const FunctionSimilarity *analyzer,
                               const std::vector<P2::Function::Ptr> &rowFunctions,
                               const std::vector<P2::Function::Ptr> &colFunctions,
                               Sawyer::Message::Stream &stream)
        : analyzer(analyzer), rowFunctions(rowFunctions), colFunctions(colFunctions), stream(stream) {}
    
    void operator()(const ComparisonTasks &tasks, size_t nThreads, const std::set<size_t> &running) {
        if (stream) {
            stream <<StringUtility::plural(running.size(), "tasks")
                   <<" running on " <<StringUtility::plural(nThreads, "workers") <<"\n";
            BOOST_FOREACH (size_t vertexId, running) {
                ComparisonTasks::ConstVertexIterator vertex = tasks.findVertex(vertexId);
                const ComparisonTask &task = vertex->value();
                stream <<"  task " <<vertexId <<": start=(" <<task.startRow <<", " <<task.startCol <<") "
                       <<StringUtility::plural(task.nComparisons, "pairs");

                size_t maxPoints = 0;
                size_t i = task.startRow;
                size_t j = task.startCol;
                for (size_t k=0; k<task.nComparisons; ++k) {
                    P2::Function::Ptr a = i < rowFunctions.size() ? rowFunctions[i] : P2::Function::Ptr();
                    P2::Function::Ptr b = j < colFunctions.size() ? colFunctions[j] : P2::Function::Ptr();
                    for (size_t catId = 0; catId < analyzer->nCategories(); ++catId) {
                        if (analyzer->categoryKind(catId) == FunctionSimilarity::CARTESIAN_POINT) {
                            maxPoints = std::max(maxPoints, analyzer->points(a, catId).size());
                            maxPoints = std::max(maxPoints, analyzer->points(b, catId).size());
                        }
                    }
                }
                stream <<" maxPoints=" <<maxPoints <<"\n";
            }
        }
    }
};

// Divide a large problem into parallelizable sub-problems.  The large problem is to compare all functions in rowFunctions with
// all functions in colFunctions and return the distance between the functions of each pair. The problem is divided among the
// specified number of threads, which must be positive. For load balancing purposes, the number of tasks will be larger than
// the number of threads. The return value is the vector into which the tasks will write their results, and can be treated as a
// row-major vector. In other words, the distance between rowFunction[i] and colFunction[j] will be written to the return
// vector at index i*colFunction.size()+j. */
static std::vector<double>
buildTasks(const std::vector<P2::Function::Ptr> &rowFunctions, const std::vector<P2::Function::Ptr> &colFunctions,
           size_t nThreads, ComparisonTasks &tasks /*in,out*/) {
    ASSERT_require(nThreads > 0);                       // caller must already know number of threads

    // Allocate the results to be populated by the tasks
    const size_t totalComparisons = rowFunctions.size() * colFunctions.size();
    std::vector<double> results(totalComparisons, NAN);

    // Decide how many tasks and how much work per task.
    const size_t nTasks = nThreads > 1 ? nThreads * tasksPerWorker : (size_t)1;
    const size_t comparisonsPerTask = (totalComparisons + nTasks - 1) / nTasks;

    // Divvy up the matrix among the tasks.
    for (size_t i = 0; i < totalComparisons; i += comparisonsPerTask) {
        size_t startRow = i / colFunctions.size();
        size_t startCol = i % colFunctions.size();
        size_t n = std::min(comparisonsPerTask, totalComparisons-i);
        ComparisonTask task(startRow, startCol, n, &(results[i]));
        tasks.insertVertex(task);
    }

    return results;
}

std::vector<double>
FunctionSimilarity::computeDistances(const std::vector<P2::Function::Ptr> &rowFunctions,
                                     const std::vector<P2::Function::Ptr> &colFunctions,
                                     size_t nThreads) const {
    ComparisonTasks tasks;
    std::vector<double> distances = buildTasks(rowFunctions, colFunctions, nThreads, tasks /*out*/);
    Sawyer::ProgressBar<size_t> progressBar(rowFunctions.size()*colFunctions.size(), mlog[MARCH], "dist matrix");
    progressBar.suffix(" elements");
    ComparisonFunctor f(this, rowFunctions, colFunctions, progress_, progressBar, 1.0);
    if (mlog[WHERE]) {
        ComparisonMonitor monitor(this, rowFunctions, colFunctions, mlog[WHERE]);
        Sawyer::workInParallel(tasks, nThreads, f, monitor, boost::chrono::seconds(10));
    } else {
        Sawyer::workInParallel(tasks, nThreads, f);
    }
    return distances;
}

std::vector<FunctionSimilarity::FunctionDistancePair>
FunctionSimilarity::compareOneToMany(const P2::Function::Ptr &needle, const std::vector<P2::Function::Ptr> &others) const {
    ASSERT_not_null(needle);
    Sawyer::Message::Stream where = mlog[WHERE];
    size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    if (0 == nThreads)
        nThreads = boost::thread::hardware_concurrency();
    SAWYER_MESG(where) <<"comparing " <<needle->printableName()
                       <<" to " <<StringUtility::plural(others.size(), "others")
                       <<" with " <<StringUtility::plural(nThreads, "threads");

    // Compute the distances between the needle function and the other functions in parallel
    std::vector<P2::Function::Ptr> needleVector(1, needle);
    Sawyer::Stopwatch stopwatch;
    std::vector<double> distances = computeDistances(needleVector, others, nThreads);
    SAWYER_MESG(where) <<"; took " <<stopwatch <<" seconds\n";
    return Combinatorics::zip(others, distances);
}

std::vector<std::vector<double> >
FunctionSimilarity::compareManyToMany(const std::vector<P2::Function::Ptr> &list1,
                                      const std::vector<P2::Function::Ptr> &list2) const {
    Sawyer::Message::Stream where = mlog[WHERE];
    size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    if (0 == nThreads)
        nThreads = boost::thread::hardware_concurrency();
    SAWYER_MESG(where) <<"comparing " <<StringUtility::plural(list1.size(), "functions")
                       <<" to " <<StringUtility::plural(list2.size(), "functions")
                       <<" with " <<StringUtility::plural(nThreads, "threads");

    // Compute the distances between all pairs of functions in parallel
    Sawyer::Stopwatch stopwatch;
    std::vector<double> distances = computeDistances(list1, list2, nThreads);
    SAWYER_MESG(where) <<"; took " <<stopwatch <<" seconds\n";

    // Convert the distances to the return type.
    std::vector<std::vector<double> > retval;
    retval.reserve(list1.size());
    for (size_t i=0, k=0; i<list1.size(); ++i) {
        retval.push_back(std::vector<double>());
        retval.back().reserve(list2.size());
        for (size_t j=0; j<list2.size(); ++j)
            retval.back().push_back(distances[k++]);
    }
    return retval;
}

FunctionSimilarity::DistanceMatrix
FunctionSimilarity::compareManyToManyMatrix(std::vector<P2::Function::Ptr> list1,
                                            std::vector<P2::Function::Ptr> list2) const {
    Sawyer::Message::Stream where = mlog[WHERE];
    size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    if (0 == nThreads)
        nThreads = boost::thread::hardware_concurrency();
    SAWYER_MESG(where) <<"comparing " <<StringUtility::plural(list1.size(), "functions")
                       <<" to " <<StringUtility::plural(list2.size(), "functions")
                       <<" with " <<StringUtility::plural(nThreads, "threads");

    // Pad the shorter vector with null function because the matrix must be square.
    if (list1.size() < list2.size()) {
        list1.resize(list2.size());
    } else if (list2.size() < list1.size()) {
        list2.resize(list1.size());
    }
    
    // Compute the distances between all pairs of functions in parallel
    Sawyer::Stopwatch stopwatch;
    std::vector<double> distances = computeDistances(list1, list2, nThreads);
    SAWYER_MESG(where) <<"; took " <<stopwatch <<" seconds\n";

    // Convert the distances to the return type
    DistanceMatrix dm(list1.size());
    for (size_t i=0, k=0; i<dm.nr(); ++i) {
        for (size_t j=0; j<dm.nc(); ++j)
            dm(i, j) = distances[k++];
    }
    return dm;
}

std::vector<FunctionSimilarity::FunctionPair>
FunctionSimilarity::findMinimumCostMapping(const std::vector<P2::Function::Ptr> &list1,
                                           const std::vector<P2::Function::Ptr> &list2) const {
    Sawyer::Message::Stream where = mlog[WHERE], debug = mlog[DEBUG];
    SAWYER_MESG(where) <<"minimum mapping between " <<StringUtility::plural(list1.size(), "functions")
                       <<" and " <<StringUtility::plural(list2.size(), "functions") <<"\n";
    Sawyer::Stopwatch stopwatch;

    DistanceMatrix dm = compareManyToManyMatrix(list1, list2);
    ASSERT_require(dm.nr() == dm.nc());
    size_t n = dm.nr();

    // Find the minimum total cost 1:1 assignment of list1 functions (rows) to list2 functions (cols)
    debug <<"starting Kuhn-Munkres";
    std::vector<size_t> assignment = findMinimumAssignment(dm);
    ASSERT_require(assignment.size() == n);
    debug <<"; done\n";
    
    // Convert mapping to a return type that doesn't depend on dlib.  This function won't run if dlib isn't available, but at
    // least user code compiles without having to do anything special.
    std::vector<FunctionPair> retval;
    retval.reserve(n);
    for (size_t i=0; i<n; ++i) {
        size_t j = assignment[i];
        retval.push_back(FunctionPair(i < list1.size() ? list1[i] : P2::Function::Ptr(),
                                      j < list2.size() ? list2[j] : P2::Function::Ptr()));
    }

    SAWYER_MESG(where) <<"; completed in " <<stopwatch <<" seconds\n";
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
    DistanceMatrix dm(size);
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
                                           const P2::Function::Ptr &function, size_t maxPoints) {
    size_t nPoints = 0;
    BOOST_FOREACH (rose_addr_t bbva, function->basicBlockAddresses()) {
        CartesianPoint point;
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(bbva);
        if (partitioner.cfg().isValidVertex(vertex)) {
            if (++nPoints > maxPoints) {
                mlog[WARN] <<"cfg connectivity truncated at " <<StringUtility::plural(maxPoints, "vertices")
                           <<" for " <<function->printableName() <<"\n";
                break;
            }
            
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
FunctionSimilarity::declareCallGraphConnectivity(const std::string &categoryName) {
    return declarePointCategory(categoryName, 1, false  /*error if exists*/);
}

void
FunctionSimilarity::measureCallGraphConnectivity(CategoryId id, const P2::Partitioner &partitioner,
                                                 const P2::Function::Ptr &function) {
    ASSERT_require(partitioner.nFunctions() > 0);
    BOOST_FOREACH (rose_addr_t bbva, function->basicBlockAddresses()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(bbva);
        if (partitioner.cfg().isValidVertex(vertex) && vertex->value().type() == P2::V_BASIC_BLOCK &&
            partitioner.basicBlockIsFunctionCall(vertex->value().bblock())) {
            BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, vertex->outEdges()) {
                P2::ControlFlowGraph::ConstVertexIterator callee = edge.target();
                CartesianPoint point;
                if (callee->value().type() == P2::V_INDETERMINATE) {
                    point.push_back(1.0);
                } else if (callee->nInEdges() <= partitioner.nFunctions()) {
                    double x = log(callee->nInEdges()) / log(partitioner.nFunctions());
                    point.push_back(x);
                } else {
                    point.push_back(1.0);
                }
                insertPoint(function, id, point);
            }
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

// class method
double
FunctionSimilarity::maximumDistance(const DistanceMatrix &dm) {
    double retval = NAN;
    for (size_t i=0; i<dm.nr(); ++i) {
        for (size_t j=0; j<dm.nc(); ++j) {
            double d = dm(i, j);
            if (!rose_isnan(d)) {
                if (rose_isnan(retval)) {
                    retval = d;
                } else {
                    retval = std::max(retval, d);
                }
            }
        }
    }
    return retval;
}

// class method
double
FunctionSimilarity::averageDistance(const DistanceMatrix &dm) {
    double sum = 0.0;
    size_t n = 0;
    for (size_t i=0; i<dm.nr(); ++i) {
        for (size_t j=0; j<dm.nc(); ++j) {
            double d = dm(i, j);
            if (!rose_isnan(d)) {
                sum += d;
                ++n;
            }
        }
    }
    return n > 0 ? sum / n : NAN;
}

// class method
double
FunctionSimilarity::medianDistance(const DistanceMatrix &dm) {
    std::vector<double> list;
    list.reserve(dm.nr() * dm.nc());
    for (size_t i=0; i<dm.nr(); ++i) {
        for (size_t j=0; j<dm.nc(); ++j) {
            double d = dm(i, j);
            if (!rose_isnan(d))
                list.push_back(d);
        }
    }
    if (list.empty())
        return NAN;

    size_t half = list.size() / 2;
    std::nth_element(list.begin(), list.begin()+half, list.end());
    if (list.size() % 2 == 1) {
        return list[half];
    } else {
        double m = *std::max_element(list.begin(), list.begin()+half);
        return (m + list[half]) / 2.0;
    }
}

} // namespace
} // namespace

#endif
