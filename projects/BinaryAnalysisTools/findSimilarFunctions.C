#include <rose.h>

#include <EditDistance/TreeEditDistance.h>
#include <EditDistance/LinearEditDistance.h>
#include <Partitioner2/Engine.h>
#include <sawyer/CommandLine.h>
#include <sawyer/Message.h>
#include <sawyer/ProgressBar.h>
#include <sawyer/Stopwatch.h>

#include "munkres.h"

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

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
    Settings(): metric(METRIC_LINEAR) {}
};

// Parse command-line and apply to settings.
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup generic = CommandlineProcessing::genericSwitches();
    SwitchGroup tool("Switches for this tool");

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
             "function and some other function is always zero regardless of metric.");

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

template<class Metric>
void
initializeMatrix(Matrix<double> &matrix, Metric &metric,
                 const std::vector<SgAsmFunction*> &functions1, const std::vector<SgAsmFunction*> &functions2) {
    const size_t matrixSize = std::max(functions1.size(), functions2.size());
    Sawyer::ProgressBar<size_t> progress(matrixSize*matrixSize, mlog[INFO]);
    for (size_t i=0; i<matrixSize; ++i) {
        if (i<functions1.size()) {
            metric.setTree1(functions1[i]);
            for (size_t j=0; j<matrixSize; ++j) {
                matrix(i, j) = j<functions2.size() ? metric.compute(functions2[j]).relativeCost() : 0.0;
                ++progress;
            }
        } else {
            for (size_t j=0; j<matrixSize; ++j) {
                matrix(i, j) = 0.0;
                ++progress;
            }
        }
    }
}

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
    ASSERT_always_require2(positionalArgs.size()==2, "see --help");
    
    // Parse the ELF/PE containers for the two specimens
    mlog[INFO] <<"parsing and loading specimens";
    Sawyer::Stopwatch loadTime;
    SgBinaryComposite *file1 = SageBuilderAsm::buildBinaryComposite(positionalArgs[0]); // also creates the SgProject
    SgBinaryComposite *file2 = SageBuilderAsm::buildBinaryComposite(positionalArgs[1]); // re-uses existing SgProject
    ASSERT_not_null(file1);
    ASSERT_not_null(file2);
    SgAsmInterpretation *interp1 = file1->get_interpretations()->get_interpretations().back(); // Best interp is usually the
    SgAsmInterpretation *interp2 = file2->get_interpretations()->get_interpretations().back(); // last one (PE is after DOS)
    ASSERT_not_null(interp1);
    ASSERT_not_null(interp2);
    mlog[INFO] <<"; completed in " <<loadTime <<" seconds\n";

    // Disassemble and partition code into functions.  Engine::partition is the top-level, do-everything function.
    mlog[INFO] <<"disassembling and partitioning specimens";
    Sawyer::Stopwatch partitionTime;
    SgAsmBlock *gblock1 = Partitioner2::Engine().partition(interp1);
    SgAsmBlock *gblock2 = Partitioner2::Engine().partition(interp2);
    mlog[INFO] <<"; completed in " <<partitionTime <<" seconds\n";
    std::vector<SgAsmFunction*> functions1 = SageInterface::querySubTree<SgAsmFunction>(gblock1);
    std::vector<SgAsmFunction*> functions2 = SageInterface::querySubTree<SgAsmFunction>(gblock2);
    mlog[INFO] <<"specimen1 has " <<StringUtility::plural(functions1.size(), "functions") <<"\n";
    mlog[INFO] <<"specimen2 has " <<StringUtility::plural(functions2.size(), "functions") <<"\n";

    // Build the matrix of costs {functions1} X {functions2}
    mlog[INFO] <<"initializing cost matrix";
    Sawyer::Stopwatch matrixInitTime;
    size_t matrixSize = std::max(functions1.size(), functions2.size());
    Matrix<double> matrix(matrixSize, matrixSize);
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
    Matrix<double> cost = matrix;
    mlog[INFO] <<"; completed in " <<matrixInitTime <<" seconds\n";

    // Use the Kuhn-Munkres algorithm to find a minimum weight perfect matching for the bipartite graph (represented by the
    // matrix) in O(n^3) time.  Also called the "Hungarian method".  The resulting matrix will have one zero per row and one
    // zero per column to represent the perfect-match edges in the bipartite graph.
    mlog[INFO] <<"Running Kuhn-Munkres";
    Sawyer::Stopwatch munkresTime;
    Munkres().solve(matrix);
    mlog[INFO] <<"; completed in " <<munkresTime <<" seconds\n";

    // Show the results
    for (size_t i=0; i<matrixSize; ++i) {
        for (size_t j=0; j<matrixSize; ++j) {
            if (0==matrix(i, j)) {
                if (i<functions1.size() && j<functions2.size()) {
                    std::cout <<"function " <<StringUtility::addrToString(functions1[i]->get_entry_va())
                              <<" \"" <<StringUtility::cEscape(functions1[i]->get_name()) <<"\""
                              <<" resolves to function " <<StringUtility::addrToString(functions2[j]->get_entry_va())
                              <<" \"" <<StringUtility::cEscape(functions2[j]->get_name()) <<"\""
                              <<" costing " <<cost(i, j) <<"\n";
#if 1 // DEBUGGING [Robb P. Matzke 2014-09-20]
                    if (functions1[i]->get_name() != functions2[j]->get_name()) {
                        mlog[WARN] <<"mismatch \"" <<StringUtility::cEscape(functions1[i]->get_name()) <<"\""
                                   <<" versus \"" <<StringUtility::cEscape(functions2[j]->get_name()) <<"\""
                                   <<" costing " <<cost(i, j) <<"\n";
                    }
#endif
                } else if (i<functions1.size()) {
                    std::cout <<"function " <<StringUtility::addrToString(functions1[i]->get_entry_va())
                              <<" \"" <<StringUtility::cEscape(functions1[i]->get_name()) <<"\""
                              <<" has no counterpart in specimen2\n";
                } else if (j<functions1.size()) {
                    std::cout <<"no function in specimen1 resolves to function "
                              <<StringUtility::addrToString(functions2[j]->get_entry_va())
                              <<" \"" <<StringUtility::cEscape(functions2[j]->get_name()) <<"\"\n";
                }
            }
        }
    }
}
