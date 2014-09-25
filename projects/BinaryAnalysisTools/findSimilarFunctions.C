#include <rose.h>

#include <Disassembler.h>
#include <EditDistance/TreeEditDistance.h>
#include <EditDistance/LinearEditDistance.h>
#include <Partitioner2/Engine.h>
#include <SqlDatabase.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <sawyer/CommandLine.h>
#include <sawyer/Message.h>
#include <sawyer/ProgressBar.h>
#include <sawyer/Stopwatch.h>

#include <dlib/matrix.h>
#include <dlib/optimization.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command line

static Sawyer::Message::Facility mlog;

// Configuration information from the command-line
enum EditDistanceMetric { METRIC_TREE, METRIC_LINEAR };

static std::string
metricName(EditDistanceMetric m) {
    switch (m) {
        case METRIC_TREE:       return "tree";
        case METRIC_LINEAR:     return "linear";
    }
    ASSERT_not_reachable("invalid metric");
}

struct Settings {
    EditDistanceMetric metric;
    std::string isaName;
    Settings(): metric(METRIC_LINEAR) {}
};

// Parse command-line and apply to settings.
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup generic = CommandlineProcessing::genericSwitches();
    SwitchGroup tool("Switches for this tool");

    tool.insert(Switch("isa")
                .argument("architecture", anyParser(settings.isaName))
                .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs. An ISA must be "
                     "specified if either of the specimens is a raw file, otherwise the ISA will be determined from "
                     "the binary container."));

    tool.insert(Switch("metric")
                .argument("name", enumParser(settings.metric)
                          ->with("tree", METRIC_TREE)
                          ->with("linear", METRIC_LINEAR))
                .doc("Metric to use when comparing two functions.  The following metrics are implemented:"
                     "@named{linear}{The \"linear\" method creates a list consisting of AST node types and, in the case "
                     "of SgAsmInstruction nodes, the instruction kind (e.g., \"x86_pop\", \"x86_mov\", etc) for each function. "
                     "It then computes an edit distance for any pair of lists by using the Levenshtein algorithm and normalizes "
                     "the edit cost according to the size of the lists that were compared.}"
                     "@named{tree}{The \"tree\" method is similar to the linear method but restricts edit operations "
                     "according to the depth of the nodes in the functions' ASTs.  This method is orders of magnitude slower "
                     "than the \"linear\" method and doesn't seem to give better results.}"
                     "The default metric is \"" + metricName(settings.metric) + "\"."));

    Parser parser;
    parser
        .purpose("finds similar functions")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] [--] @v{specimen1} @v{specimen2}")
        .doc("description",
             "This tool attempts to correlate functions in one binary specimen with related functions in the other specimen. "
             "It does so by parsing, loading, disassembling, and partitioning each specimen to obtain a list of functions. "
             "Then it computes a syntactic distance between all pairs of functions using a specified distance metric "
             "(see @s{metric}) to create an edge-weighted, bipartite graph.  Finally, a minimum weight perfect matching is "
             "found using the Kuhn-Munkres algorithm.  The answer is output as a list of function correlations and their "
             "distance from each other.  The specimens need not have the same number of functions, in which case one of "
             "the specimens will have null functions inserted to make them the same size.  The distance between a null "
             "function and some other function is always zero regardless of metric.")
        .doc("Specimens",
             "The binary specimens can be constructed from files in two ways."
             "@bullet{If the specimen name is a simple file name then the specimen is passed to the \"buildBinaryComposite\" "
             "so its container format (ELF, PE, etc) can be parsed and its segments loaded into virtual memory.}"
             "@bullet{If the specimen name begins with the string \"map:\" then it is treated as a memory map resource "
             "string. " + MemoryMap::insertFileDocumentation() + "}");

    return parser.with(generic).with(tool).parse(argc, argv).apply();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SubstitutionPredicate: public EditDistance::TreeEditDistance::SubstitutionPredicate {
public:
    virtual bool operator()(SgNode *source, SgNode *target) /*override*/ {
        if (source->variantT() != target->variantT())
            return false;
        if (SgAsmInstruction *si = isSgAsmInstruction(source)) {
            SgAsmInstruction *ti = isSgAsmInstruction(target);
            return si->get_anyKind() == ti->get_anyKind();
        }
        return true;
    }
};

// A 2d region that needs to be initialized in a matrix.
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
    MyWorkList(size_t matrixSize, size_t nItems): progress_(mlog[INFO]) {
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
    dlib::matrix<size_t> &matrix;                       // matrix(i,j) must be thread safe
    const std::vector<SgAsmFunction*> &functions1, &functions2;
    Metric metric;
public:
    MatrixInitializer(MyWorkList &workList,
                      dlib::matrix<size_t> &matrix,
                      const std::vector<SgAsmFunction*> &functions1,
                      const std::vector<SgAsmFunction*> &functions2,
                      Metric &metric)
        : workList(workList), matrix(matrix), functions1(functions1), functions2(functions2), metric(metric) {}

    void operator()() {
        WorkItem toDo;
        while (workList.next().assignTo(toDo)) {
            for (size_t i=toDo.iBegin; i<toDo.iEnd; ++i) {
                if (i<functions1.size()) {
                    metric.setTree1(functions1[i]);
                    for (size_t j=toDo.jBegin; j<toDo.jEnd; ++j)
                        matrix(i, j) = j<functions2.size() ? metric.compute(functions2[j]).cost() : 0;
                } else {
                    for (size_t j=toDo.jBegin; j<toDo.jEnd; ++j) {
                        matrix(i, j) = 0;
                    }
                }
            }
        }
    }
};

template<class Metric>
void
initializeMatrix(dlib::matrix<size_t> &matrix, Metric &metric,
                 const std::vector<SgAsmFunction*> &functions1, const std::vector<SgAsmFunction*> &functions2) {
    const size_t matrixSize = std::max(functions1.size(), functions2.size());
    const size_t nThreads = 10;                         // number of threads to initialize matrix
    const size_t nItems = 1000 * nThreads;              // since not all work items take an equal time

    MyWorkList workList(matrixSize, nItems);
    
    MatrixInitializer<Metric> m0(workList, matrix, functions1, functions2, metric);
    boost::thread t0(m0);
    MatrixInitializer<Metric> m1(workList, matrix, functions1, functions2, metric);
    boost::thread t1(m1);
    MatrixInitializer<Metric> m2(workList, matrix, functions1, functions2, metric);
    boost::thread t2(m2);
    MatrixInitializer<Metric> m3(workList, matrix, functions1, functions2, metric);
    boost::thread t3(m3);
    MatrixInitializer<Metric> m4(workList, matrix, functions1, functions2, metric);
    boost::thread t4(m4);
    MatrixInitializer<Metric> m5(workList, matrix, functions1, functions2, metric);
    boost::thread t5(m5);
    MatrixInitializer<Metric> m6(workList, matrix, functions1, functions2, metric);
    boost::thread t6(m6);
    MatrixInitializer<Metric> m7(workList, matrix, functions1, functions2, metric);
    boost::thread t7(m7);
    MatrixInitializer<Metric> m8(workList, matrix, functions1, functions2, metric);
    boost::thread t8(m8);
    MatrixInitializer<Metric> m9(workList, matrix, functions1, functions2, metric);
    boost::thread t9(m9);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
}

// output(i,j) = max(input) - input(i,j) for all i,j
// returns max(input)
// matrix serves as both input and output
template<typename T>
static T
flipCosts(dlib::matrix<T> &matrix) {
    T maxValue = matrix(0, 0);
    for (long i=0; i<matrix.nr(); ++i) {
        for (long j=0; j<matrix.nc(); ++j) {
            maxValue = std::max(maxValue, matrix(i, j));
        }
    }
    for (long i=0; i<matrix.nr(); ++i) {
        for (long j=0; j<matrix.nc(); ++j) {
            matrix(i, j) = maxValue - matrix(i, j);
        }
    }
    return maxValue;
}

static std::vector<SgAsmFunction*>
loadFunctions(const std::string &fileName, Disassembler *disassembler) {
    Stream info(mlog[INFO]);
    P2::Engine engine;

    // Load the specimen into a memory map
    SgAsmInterpretation *interp = NULL;
    MemoryMap map;
    if (!boost::starts_with(fileName, "map:")) {
        info <<"parsing binary container";
        Sawyer::Stopwatch parseTime;
        SgBinaryComposite *file = SageBuilderAsm::buildBinaryComposite(fileName);
        ASSERT_not_null(file);
        interp = file->get_interpretations()->get_interpretations().back();
        ASSERT_not_null(interp);
        engine.loadSpecimen(interp);
        ASSERT_not_null(interp->get_map());
        map = *interp->get_map();
        if (!disassembler) {
            disassembler = engine.allocateDisassembler(interp);
            ASSERT_not_null(disassembler);
        }
        info <<"; completed in " <<parseTime <<" seconds\n";
    } else {
        map.insertFile(fileName.substr(3));
        if (!disassembler)
            throw std::runtime_error("an instruction set architecture must be specified with the \"--isa\" switch");
    }

    // Partition instructions into functions
    info <<"disassembling and partitioning";
    P2::Modules::deExecuteZeros(map, 256);
    Sawyer::Stopwatch partitionTime;
    P2::Partitioner partitioner = engine.createTunedPartitioner(disassembler, map);
    engine.partition(partitioner, interp);
    SgAsmBlock *gblock = partitioner.buildGlobalBlockAst();
    info <<"; completed in " <<partitionTime <<" seconds\n";

    return SageInterface::querySubTree<SgAsmFunction>(gblock);
}

struct AddressRenderer: SqlDatabase::Renderer<rose_addr_t> {
    std::string operator()(const rose_addr_t &value, size_t width) const /*override*/ {
        return StringUtility::addrToString(value);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[]) {
    // Initialization
    rose::Diagnostics::initialize();
    mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insert(mlog);

    // Parse command-line
    Settings settings;
    std::vector<std::string> positionalArgs = parseCommandLine(argc, argv, settings).unreachedArgs();
    Disassembler *disassembler = NULL;
    if (!settings.isaName.empty())
        disassembler = Disassembler::lookup(settings.isaName);
    disassembler->set_progress_reporting(-1.0);         // turn it off
    ASSERT_always_require2(positionalArgs.size()==2, "see --help");
    
    // Parse the ELF/PE containers for the two specimens
    std::vector<SgAsmFunction*> functions1 = loadFunctions(positionalArgs[0], disassembler);
    std::vector<SgAsmFunction*> functions2 = loadFunctions(positionalArgs[1], disassembler);
    mlog[INFO] <<"specimen1 has " <<StringUtility::plural(functions1.size(), "functions") <<"\n";
    mlog[INFO] <<"specimen2 has " <<StringUtility::plural(functions2.size(), "functions") <<"\n";
    if (functions1.empty() || functions2.empty())
        return 0;

    // Build the matrix of costs {functions1} X {functions2}
    mlog[INFO] <<"initializing cost matrix";
    Sawyer::Stopwatch matrixInitTime;
    size_t matrixSize = std::max(functions1.size(), functions2.size());
    dlib::matrix<size_t> matrix(matrixSize, matrixSize);
    switch (settings.metric) {
        case METRIC_TREE: {
            EditDistance::TreeEditDistance::Analysis metric;
            SubstitutionPredicate canSubst;
            metric.substitutionCost(0);
            metric.substitutionPredicate(&canSubst);
            initializeMatrix(matrix /*out*/, metric, functions1, functions2);
            break;
        }
        case METRIC_LINEAR: {
            EditDistance::LinearEditDistance::Analysis<> metric;
            initializeMatrix(matrix /*out*/, metric, functions1, functions2);
            break;
        }
    }
    dlib::matrix<size_t> cost = matrix;                 // save actual costs before we adjust the matrix
    flipCosts(matrix);                                  // flip values so we can minimize by calling max_cost_assignment
    mlog[INFO] <<"; completed in " <<matrixInitTime <<" seconds\n";

    // Use the Kuhn-Munkres algorithm to find a minimum weight perfect matching for the bipartite graph (represented by the
    // matrix) in O(n^3) time.  Also called the "Hungarian method".  The resulting matrix will have one zero per row and one
    // zero per column to represent the perfect-match edges in the bipartite graph.
    mlog[INFO] <<"Running Kuhn-Munkres";
    Sawyer::Stopwatch munkresTime;
    std::vector<long> assignments = dlib::max_cost_assignment(matrix);
    mlog[INFO] <<"; completed in " <<munkresTime <<" seconds\n";

    // Show the results
    AddressRenderer renderAddress;
    SqlDatabase::Table<size_t, rose_addr_t, std::string, rose_addr_t, std::string> results;
    results.headers("Distance", "SourceVa", "SourceName", "TargetVa", "TargetName");
    results.renderers().r1 = &renderAddress;
    results.renderers().r3 = &renderAddress;
    for (size_t i=0; i<assignments.size(); ++i) {
        size_t j=assignments[i];
        if (i<functions1.size() && j<functions2.size()) {
            results.insert(cost(i, j),
                           functions1[i]->get_entry_va(), StringUtility::cEscape(functions1[i]->get_name()), 
                           functions2[j]->get_entry_va(), StringUtility::cEscape(functions2[j]->get_name()));
#if 1 // DEBUGGING [Robb P. Matzke 2014-09-20]
            if (functions1[i]->get_name() != functions2[j]->get_name()) {
                mlog[WARN] <<"mismatch \"" <<StringUtility::cEscape(functions1[i]->get_name()) <<"\""
                           <<" versus \"" <<StringUtility::cEscape(functions2[j]->get_name()) <<"\""
                           <<" costing " <<cost(i, j) <<"\n";
            }
#endif
        } else if (i<functions1.size()) {
            results.insert(0,
                           functions1[i]->get_entry_va(), StringUtility::cEscape(functions1[i]->get_name()), 
                           0, "");
        } else {
            results.insert(0,
                           0, "",
                           functions2[j]->get_entry_va(), StringUtility::cEscape(functions2[j]->get_name()));
        }
    }
    results.print(std::cout);
}
