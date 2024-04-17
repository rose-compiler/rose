#include <rose.h>
#ifdef ROSE_HAVE_DLIB

static const char *purpose = "finds similar functions";
static const char *description =
    "This tool attempts to correlate functions in one binary specimen with related functions in the other specimen. "
    "The two specimens are specified as ROSE binary analysis (RBA) files. It computes a syntactic distance between all "
    "pairs of functions using a specified distance metric (see @s{metric}) to create an edge-weighted, bipartite graph. "
    "Finally, a minimum weight perfect matching is found using the Kuhn-Munkres algorithm.  The answer is output as a "
    "list of function correlations and their distance from each other.  The specimens need not have the same number of "
    "functions, in which case one of the specimens will have null functions inserted to make them the same size.  The "
    "distance between a null function and some other function is always zero regardless of metric.\n\n";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>

#include <EditDistance/TreeEditDistance.h>
#include <EditDistance/LinearEditDistance.h>

#include <boost/filesystem.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Stopwatch.h>

#include <dlib/matrix.h>
#include <dlib/optimization.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
using namespace StringUtility;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command line

static Sawyer::Message::Facility mlog;

// Configuration information from the command-line
enum EditDistanceMetric { METRIC_TREE, METRIC_LINEAR, METRIC_INSN, METRIC_SIZE, METRIC_SIZE_ADDR };

static std::string
metricName(EditDistanceMetric m) {
    switch (m) {
        case METRIC_TREE:       return "tree";
        case METRIC_INSN:       return "insn";
        case METRIC_LINEAR:     return "linear";
        case METRIC_SIZE:       return "size";
        case METRIC_SIZE_ADDR:  return "sizeaddr";
    }
    ASSERT_not_reachable("invalid metric");
}

struct Settings {
    EditDistanceMetric metric = METRIC_INSN;
    bool listPairings = true;
    SerialIo::Format stateFormat = SerialIo::BINARY;
};

// Parse command-line and apply to settings. Return the two RBA file names.
static std::pair<boost::filesystem::path, boost::filesystem::path>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup tool("Switches for this tool");
    tool.name("tool");

    tool.insert(Switch("metric")
                .argument("name", enumParser(settings.metric)
                          ->with("tree", METRIC_TREE)
                          ->with("linear", METRIC_LINEAR)
                          ->with("insn", METRIC_INSN)
                          ->with("size", METRIC_SIZE)
                          ->with("sizeaddr", METRIC_SIZE_ADDR))
                .doc("Metric to use when comparing two functions.  The following metrics are implemented:"

                     "@named{linear}{The \"linear\" method creates a list consisting of AST node types and, in the case "
                     "of SgAsmInstruction nodes, the instruction kind (e.g., \"x86_pop\", \"x86_mov\", etc) for each function. "
                     "It then computes an edit distance for any pair of lists by using the Levenshtein algorithm and normalizes "
                     "the edit cost according to the size of the lists that were compared.}"

                     "@named{insn}{This is the same as the \"linear\" method but it computes the edit distance for only "
                     "the instruction types without considering their operands.}"

                     "@named{tree}{The \"tree\" method is similar to the \"linear\" method but restricts edit operations "
                     "according to the depth of the nodes in the functions' ASTs.  This method is orders of magnitude slower "
                     "than the \"linear\" method and doesn't seem to give better results.}"

                     "@named{size}{Uses difference in AST size as the distance metric.  The difference between two functions "
                     "is the absolute value of the difference in the size of their ASTs. This is easily the fastest metric.}"

                     "@named{sizeaddr}{Uses difference in AST size and difference in entry address as the distance metric. "
                     "Functions are sorted into a vector according to their entry address and the difference in vector index "
                     "contributes to the distance between two functions.}"

                     "The default metric is \"" + metricName(settings.metric) + "\"."));

    Rose::CommandLine::insertBooleanSwitch(tool, "list", settings.listPairings,
                                           "Produce a listing that indicates how functions in the first specimen map into "
                                           "functions into the second specimen.  Regardless of whether the pairings are "
                                           "listed, the output will contain summary information.");

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{RBA-1} @v{RBA-2}");
    parser.with(tool).with(gen);
    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
    if (args.size() != 2) {
        mlog[FATAL] <<"inccorect usage; see --help\n";
        exit(1);
    }

    return std::make_pair(args[0], args[1]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t treeSize(SgNode *ast) {
    struct T1: AstSimpleProcessing {
        size_t n;
        T1(): n(0) {}
        void visit(SgNode*) { ++n; }
    } t1;
    t1.traverse(ast, preorder);
    return t1.n;
}

size_t treeSize(const std::vector<SgAsmFunction*> &functions) {
    size_t n = 0;
    for (SgAsmFunction *function: functions)
        n += treeSize(function);
    return n;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// An edit distance metric that runs Levenshtein over a list of instruction types but does not include any of the
// instruction operands.

class InsnEditDistance {
    std::vector<unsigned> list1_, list2_;
    size_t cost_;
public:
    InsnEditDistance(): cost_(0) {}
    void setTree1(SgNode *ast) { setTree(ast, list1_); }
    void setTree2(SgNode *ast) { setTree(ast, list2_); }
    InsnEditDistance& compute(SgNode *ast) {
        setTree2(ast);
        return compute();
    }
    InsnEditDistance& compute() {
        cost_ = EditDistance::levenshteinDistance(list1_, list2_);
        return *this;
    }
    size_t cost() const { return cost_; }
    double relativeCost() const { return (double)cost_ / std::max(list1_.size(), list2_.size()); }
private:
    void setTree(SgNode *ast, std::vector<unsigned> &list) {
        struct T1: AstSimpleProcessing {
            std::vector<unsigned> &list;
            T1(std::vector<unsigned> &list): list(list) {}
            void visit(SgNode *node) {
                if (SgAsmInstruction *insn = isSgAsmInstruction(node))
                    list.push_back(insn->get_anyKind());
            }
        } t1(list);
        list.clear();
        t1.traverse(ast, preorder);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A metric that just compares the size of two ASTs

class SizeDistance {
    size_t size1_, size2_;
public:
    SizeDistance(): size1_(0), size2_(0) {}
    void setTree1(SgNode *ast) { size1_ = treeSize(ast); }
    void setTree2(SgNode *ast) { size2_ = treeSize(ast); }
    SizeDistance& compute(SgNode *ast) { setTree2(ast); return *this; }
    SizeDistance& compute() { return *this; }
    size_t cost() const { return size1_<size2_ ? size2_-size1_ : size1_-size2_; }
    double relativeCost() const { return (double)cost() / std::max(size1_, size2_); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A metric that compares size and address of two functions

class SizeAddrDistance: public SizeDistance {
    size_t pos1_, pos2_;                                // positions of functions being compared
    typedef Sawyer::Container::Map<SgAsmFunction*, size_t> FMap;
    FMap fmap1_, fmap2_; // maps function to position in executable
public:
    SizeAddrDistance(const std::vector<SgAsmFunction*> &functions1, const std::vector<SgAsmFunction*> &functions2)
        : pos1_(0), pos2_(0) {
        ASSERT_require(P2::isSorted(functions1, P2::sortFunctionNodesByAddress));
        for (SgAsmFunction *function: functions1)
            fmap1_.insert(function, function->get_entryVa());
        ASSERT_require(P2::isSorted(functions1, P2::sortFunctionNodesByAddress));
        for (SgAsmFunction *function: functions2)
            fmap2_.insert(function, function->get_entryVa());
    }
    void setTree1(SgNode *ast) { SizeDistance::setTree1(ast); setTree(ast, fmap1_, pos1_/*out*/); }
    void setTree2(SgNode *ast) { SizeDistance::setTree2(ast); setTree(ast, fmap2_, pos2_/*out*/); }
    SizeDistance& compute(SgNode *ast) { setTree2(ast); return *this; }
    SizeDistance& compute() {return *this; }
    size_t cost() const { return SizeDistance::cost() + diffAbs(); }
    double relativeCost() const { return (SizeDistance::relativeCost() + 7.0*diffRel()) / 8.0; }
private:
    size_t diffAbs() const { return pos1_ < pos2_ ? pos2_-pos1_ : pos1_-pos2_; }
    size_t diffRel() const { return (double)diffAbs() / std::max(fmap1_.size(), fmap2_.size()); }
    void setTree(SgNode *ast, const FMap &functions, size_t &pos) {
        SgAsmFunction *function = isSgAsmFunction(ast);
        ASSERT_not_null(function);
        ASSERT_require(functions.exists(function));
        pos = functions[function];
    }
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tree edit distance metric

class SubstitutionPredicate: public EditDistance::TreeEditDistance::SubstitutionPredicate {
public:
    virtual bool operator()(SgNode *source, SgNode *target) override {
        if (source->variantT() != target->variantT())
            return false;
        if (SgAsmInstruction *si = isSgAsmInstruction(source)) {
            SgAsmInstruction *ti = isSgAsmInstruction(target);
            return si->get_anyKind() == ti->get_anyKind();
        }
        return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stuff for initializing the distance matrix

struct WorkItem {
    size_t iBegin, jBegin, iEnd, jEnd;
    WorkItem(size_t iBegin, size_t jBegin, size_t iEnd, size_t jEnd)
        : iBegin(iBegin), jBegin(jBegin), iEnd(iEnd), jEnd(jEnd) {}
    WorkItem(): iBegin(0), jBegin(0), iEnd(0), jEnd(0) {}
};

// A work list for initializing a matrix.  The worklist will contain approx nItems instances of WorkItem that partition
// the matrix.
class MyWorkList {
    std::vector<WorkItem> toDo_;
    boost::mutex mutex_;
    Sawyer::ProgressBar<size_t> progress_;
    size_t next_;
public:
    MyWorkList(size_t matrixSize, size_t nItems): progress_(mlog[MARCH]) {
        size_t itemSize = (size_t)ceil(sqrt((double)matrixSize*matrixSize/nItems));
        ASSERT_require(itemSize>0);
        for (size_t i=0; i<matrixSize; i+=itemSize) {
            size_t di = std::min(itemSize, matrixSize-i);
            for (size_t j=0; j<matrixSize; j+=itemSize) {
                size_t dj = std::min(itemSize, matrixSize-i);
                toDo_.push_back(WorkItem(i, j, i+di, j+dj));
            }
        }
        next_ = 0;
        progress_.value(0, toDo_.size());
    }

    size_t size() const {
        return toDo_.size();
    }

    Sawyer::Optional<WorkItem> next() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        if (next_ >= toDo_.size())
            return Sawyer::Nothing();
        WorkItem work = toDo_[next_++];
        ++progress_;
        return work;
    }
};

template<class Metric>
class MatrixInitializer {
    MyWorkList &workList;
    dlib::matrix<double> &distance;                     // distance(i,j) must be thread safe
    const std::vector<SgAsmFunction*> &functions1, &functions2;
    Metric metric;
    boost::thread thread;
public:
    MatrixInitializer(MyWorkList &workList,
                      dlib::matrix<double> &distance,
                      const std::vector<SgAsmFunction*> &functions1,
                      const std::vector<SgAsmFunction*> &functions2,
                      Metric &metric)
        : workList(workList), distance(distance), functions1(functions1), functions2(functions2), metric(metric) {}

    void start() {
        thread = boost::thread(&MatrixInitializer::init, this);
    }

    void wait() {
        thread.join();
    }

    // You'd think that work items that span entire rows rather than squarish 2d regions would be faster because they'd
    // have fewer calls to metric.setTree1, but it turns out they're about 5% slower. [Robb P. Matzke 2014-09-27]
    void init() {
        WorkItem toDo;
        while (workList.next().assignTo(toDo)) {
            for (size_t i=toDo.iBegin; i<toDo.iEnd; ++i) {
                if (i<functions1.size()) {
                    metric.setTree1(functions1[i]);
                    for (size_t j=toDo.jBegin; j<toDo.jEnd; ++j) {
                        if (j<functions2.size()) {
                            distance(i, j) = metric.compute(functions2[j]).relativeCost();
                        } else {
                            distance(i, j) = 0.0;
                        }
                    }
                } else {
                    for (size_t j=toDo.jBegin; j<toDo.jEnd; ++j) {
                        distance(i, j) = 0.0;
                    }
                }
            }
        }
    }
};

template<class Metric>
void
initializeMatrix(dlib::matrix<double> &distance, Metric &metric, size_t nThreads,
                 const std::vector<SgAsmFunction*> &functions1, const std::vector<SgAsmFunction*> &functions2) {
    const size_t matrixSize = std::max(functions1.size(), functions2.size());
    nThreads = std::max((size_t)1, nThreads);
    MyWorkList workList(matrixSize, 1000*nThreads);

    std::vector<MatrixInitializer<Metric>*> workers;

    for (size_t i=0; i<nThreads; ++i) {
        workers.push_back(new MatrixInitializer<Metric>(workList, distance, functions1, functions2, metric));
        workers.back()->start();
    }
    for (size_t i=0; i<workers.size(); ++i) {
        workers[i]->wait();
        delete workers[i];
    }
}

template<typename T>
static std::pair<T, T>
minmax(const dlib::matrix<T> &matrix) {
    T maxValue = matrix(0, 0);
    T minValue = matrix(0, 0);
    for (long i=0; i<matrix.nr(); ++i) {
        for (long j=0; j<matrix.nc(); ++j) {
            maxValue = std::max(maxValue, matrix(i, j));
            minValue = std::min(minValue, matrix(i, j));
        }
    }
    return std::make_pair(minValue, maxValue);
}

// Convert floating point matrix to integer and flip all the values so that dlib::max_cost_assignment is actually
// computing a minimum instead of a maximum.
template<typename T>
static void
munkresCost(const dlib::matrix<double> &src, T scale, dlib::matrix<T> &dst /*out*/) {
    ASSERT_require(dst.nr()==src.nr() && dst.nc()==src.nc());
    std::pair<double, double> range = minmax(src);
    if (range.first==range.second) {
        for (long i=0; i<src.nr(); ++i) {
            for (long j=0; j<dst.nc(); ++j)
                dst(i, j) = 0;
        }
    } else {
        for (long i=0; i<src.nr(); ++i) {
            for (long j=0; j<dst.nc(); ++j)
                dst(i, j) = round(((range.second-src(i, j)) / (range.second-range.first)) * scale);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::vector<SgAsmFunction*>
loadFunctions(const boost::filesystem::path rbaName, const P2::Engine::Ptr &engine) {
    ASSERT_not_null(engine);
    engine->reset();                                           // clear all but config properties
    engine->settings().partitioner.doingPostAnalysis = false;  // not needed for this tool
    SgAsmBlock *gblock = engine->buildAst(rbaName.string());   // parse, load, link, disassemble, partition, build AST
    return SageInterface::querySubTree<SgAsmFunction>(gblock); // return just the functions
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[]) {
    // Initialization
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("comparing functions");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();
    Stream info(mlog[INFO]);

    // Parse command-line
    P2::Engine::Ptr engine = P2::EngineBinary::instance();
    Settings settings;
    std::pair<boost::filesystem::path, boost::filesystem::path> rbaFiles = parseCommandLine(argc, argv, settings);
    size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    if (0 == nThreads)
        nThreads = boost::thread::hardware_concurrency();

    std::vector<SgAsmFunction*> functions1 = loadFunctions(rbaFiles.first, engine);
    std::vector<SgAsmFunction*> functions2 = loadFunctions(rbaFiles.second, engine);
    info <<"specimen1 has " <<plural(functions1.size(), "functions") <<" containing " <<treeSize(functions1) <<" AST nodes.\n";
    info <<"specimen2 has " <<plural(functions2.size(), "functions")<<" containing " <<treeSize(functions2) <<" AST nodes.\n";
    if (functions1.empty() || functions2.empty())
        return 0;

    // Build the matrix of distances {functions1} X {functions2}
    Sawyer::Stopwatch matrixInitTime;
    size_t matrixSize = std::max(functions1.size(), functions2.size());
    info <<"distance matrix has " <<plural(matrixSize*matrixSize, "elements") <<"\n";
    info <<"initializing \""+metricName(settings.metric)+"\" distance matrix with " <<plural(nThreads, "threads");
    dlib::matrix<double> distance(matrixSize, matrixSize);
    switch (settings.metric) {
        case METRIC_TREE: {
            EditDistance::TreeEditDistance::Analysis metric;
            SubstitutionPredicate canSubst;
            metric.substitutionCost(0);
            metric.substitutionPredicate(&canSubst);
            initializeMatrix(distance /*out*/, metric, nThreads, functions1, functions2);
            break;
        }
        case METRIC_LINEAR: {
            EditDistance::LinearEditDistance::Analysis<> metric;
            initializeMatrix(distance /*out*/, metric, nThreads, functions1, functions2);
            break;
        }
        case METRIC_INSN: {
            InsnEditDistance metric;
            initializeMatrix(distance /*out*/, metric, nThreads, functions1, functions2);
            break;
        }
        case METRIC_SIZE: {
            SizeDistance metric;
            initializeMatrix(distance /*out*/, metric, nThreads, functions1, functions2);
            break;
        }
        case METRIC_SIZE_ADDR: {
            SizeAddrDistance metric(functions1, functions2);
            initializeMatrix(distance /*out*/, metric, nThreads, functions1, functions2);
            break;
        }
    }
    info <<"; completed in " <<matrixInitTime <<"\n";

    // Use the Kuhn-Munkres algorithm to find a minimum weight perfect matching for the bipartite graph (represented by the
    // matrix) in O(n^3) time.  Also called the "Hungarian method".
    info <<"Running Kuhn-Munkres";
    Sawyer::Stopwatch munkresTime;
    dlib::matrix<unsigned long> cost(distance.nr(), distance.nc());
    munkresCost(distance, 1000000ul, cost /*out*/);
    std::vector<long> assignments = dlib::max_cost_assignment(cost);
    info <<"; completed in " <<munkresTime <<"\n";

    double totalDistance = 0.0;
    for (size_t i=0; i<assignments.size(); ++i)
        totalDistance += distance(i, assignments[i]);
    std::cout <<"Total cost:    " <<totalDistance <<"\n";
    std::cout <<"Relative cost: " <<(totalDistance / std::max(functions1.size(), functions2.size())) <<"\n";

    // Show the results
    FormattedTable results;
    results.columnHeader(0, 0, "Distance");
    results.columnHeader(0, 1, "SourceVa");
    results.columnHeader(0, 2, "SourceSize");
    results.columnHeader(0, 3, "SourceName");
    results.columnHeader(0, 4, "TargetVa");
    results.columnHeader(0, 5, "TargetSize");
    results.columnHeader(0, 6, "TargetName");
    results.columnHeader(0, 7, "NameClash");
    size_t nClashes = 0;
    for (size_t i=0; i<assignments.size(); ++i) {
        using namespace StringUtility;
        size_t j=assignments[i];
        if (i<functions1.size() && j<functions2.size()) {
            bool nameClash = functions1[i]->get_name() != functions2[j]->get_name();
            results.insert(i, 0, distance(i, j));
            results.insert(i, 1, StringUtility::addrToString(functions1[i]->get_entryVa()));
            results.insert(i, 2, treeSize(functions1[i]));
            results.insert(i, 3, cEscape(functions1[i]->get_name()));
            results.insert(i, 4, StringUtility::addrToString(functions2[j]->get_entryVa()));
            results.insert(i, 5, treeSize(functions2[j]));
            results.insert(i, 6, cEscape(functions2[j]->get_name()));
            results.insert(i, 7, nameClash?"clash":"");
            if (nameClash)
                ++nClashes;
        } else if (i<functions1.size()) {
            results.insert(i, 0, 0.0);
            results.insert(i, 1, StringUtility::addrToString(functions1[i]->get_entryVa()));
            results.insert(i, 2, treeSize(functions1[i]));
            results.insert(i, 3, cEscape(functions1[i]->get_name()));
            results.insert(i, 4, 0);
            results.insert(i, 5, 0);
            results.insert(i, 6, "");
            results.insert(i, 7, "");
        } else {
            results.insert(i, 0, 0.0);
            results.insert(i, 1, 0);
            results.insert(i, 2, 0);
            results.insert(i, 3, "");
            results.insert(i, 4, StringUtility::addrToString(functions2[j]->get_entryVa()));
            results.insert(i, 5, treeSize(functions2[j]));
            results.insert(i, 6, cEscape(functions2[j]->get_name()));
            results.insert(i, 7, "");
        }
    }
    if (settings.listPairings)
        std::cout <<results;

    if (nClashes>0) {
        mlog[WARN] <<nClashes <<" of " <<StringUtility::plural(assignments.size(), "parings")
                   <<" (" <<(100.0*nClashes/assignments.size()) <<" percent)"
                   <<(1==nClashes?" was":" were") <<" between functions with different names\n";
    }
}

#else

#include <rose.h>
#include <Rose/Diagnostics.h>

#include <iostream>
#include <cstring>

int main(int, char *argv[]) {
    ROSE_INITIALIZE;
    Sawyer::Message::Facility mlog;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog[Rose::Diagnostics::FATAL] <<argv[0] <<": this tool is not available in this ROSE configuration\n";

    for (char **arg = argv+1; *arg; ++arg) {
        if (!strcmp(*arg, "--no-error-if-disabled"))
            return 0;
    }
    return 1;
}

#endif
