#include <rose.h>
#include <BinaryLoader.h>
#include <Partitioner2/Engine.h>
#include <EditDistance/TreeEditDistance.h>
#include <sawyer/Stopwatch.h>

#include "commandLine.h"

// Optional: to use old implementation
#define USE_OLD_TREE_EDIT_DISTANCE
#ifdef USE_OLD_TREE_EDIT_DISTANCE
# include "treeEditDistance.h"
#endif


using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup gen = CommandlineProcessing::genericSwitches();
    SwitchGroup tool = toolCommandLineSwitches(settings);

    Parser parser;
    parser
        .purpose("demonstrates tree edit distance")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen1} @v{specimen2}")
        .doc("description",
             "This tool performs tree edit distance between two binary files a couple different ways in order to compare "
             "the old, local implementation and the new librose implementation. See @s{use-old} for more details.  The "
             "output of the old implementation and the new implementation are not usually identical because there are "
             "usually multiple edit paths with the same total cost.");

    return parser.with(gen).with(tool).parse(argc, argv).apply();
}

template<typename Instruction>
static int kind(Instruction *insn) {
    if (Instruction *i = dynamic_cast<Instruction*>(insn))
        return i->get_kind();
    return -1;
}

// When can AST nodes be substituted?
static struct: EditDistance::TreeEditDistance::SubstitutionPredicate {
    virtual bool operator()(SgNode *a, SgNode *b) ROSE_OVERRIDE {
        if (a->variantT()==b->variantT()) {
            if (SgAsmInstruction *ai = isSgAsmInstruction(a)) {
                SgAsmInstruction *bi = isSgAsmInstruction(b);
                return ai->get_anyKind() == bi->get_anyKind();
            }
            return true;
        }
        return false;
    }
} isSameType;

// Count nodes in subtree.
struct NodeCounter: AstSimpleProcessing {
    size_t n;
    NodeCounter(): n(0) {}
    void visit(SgNode*) { ++n; }
};

static Sawyer::Message::Facility mlog;

int
main(int argc, char *argv[]) {
    // Initialize
    rose::Diagnostics::initialize();                    // because librose doesn't initialize itself until frontend();
    mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse command-line (see --help for usage)
    Settings settings;
    std::vector<std::string> positionalArgs = parseCommandLine(argc, argv, settings).unreachedArgs();
    ASSERT_always_require2(positionalArgs.size()==2, "see --help");

    // Parse the ELF/PE containers for the two specimens
    mlog[INFO] <<"parsing and loading specimens...\n";
    Sawyer::Stopwatch loadTime;
    SgBinaryComposite *file1 = SageBuilderAsm::buildBinaryComposite(positionalArgs[0]); // also creates the SgProject
    SgBinaryComposite *file2 = SageBuilderAsm::buildBinaryComposite(positionalArgs[1]); // re-uses existing SgProject
    ASSERT_not_null(file1);
    ASSERT_not_null(file2);
    SgAsmInterpretation *interp1 = file1->get_interpretations()->get_interpretations().back(); // Best interp is usually the
    SgAsmInterpretation *interp2 = file2->get_interpretations()->get_interpretations().back(); // last one (PE is after DOS)
    ASSERT_not_null(interp1);
    ASSERT_not_null(interp2);
    mlog[INFO] <<"parsed and loaded in " <<loadTime <<" seconds\n";

    // Disassemble and partition code into functions.
    mlog[INFO] <<"disassembling and partitioning specimens...\n";
    Sawyer::Stopwatch partitionTime;
    SgAsmBlock *gblock1 = Partitioner2::Engine().partition(interp1).buildAst(interp1);
    SgAsmBlock *gblock2 = Partitioner2::Engine().partition(interp2).buildAst(interp2);
    mlog[INFO] <<"disassembled and partitioned in " <<partitionTime <<" seconds\n";

    // Some stats before we start.
    NodeCounter nNodes1, nNodes2;
    nNodes1.traverse(gblock1, preorder);
    nNodes2.traverse(gblock2, preorder);
    mlog[INFO] <<"specimen 1 has " <<StringUtility::plural(nNodes1.n, "nodes") <<"\n";
    mlog[INFO] <<"specimen 2 has " <<StringUtility::plural(nNodes2.n, "nodes") <<"\n";

    // Original (local) implementation
    if (settings.useOldImplementation) {
        mlog[INFO] <<"Computing edit distance using old method... (watch out -- lots of output coming!)\n";
        Sawyer::Stopwatch oldTime;
        tree_edit_distance(gblock1, gblock2);
        mlog[INFO] <<"Old method took " <<oldTime <<" seconds\n";
    }

    // Edit distance
    mlog[INFO] <<"Computing edit distance over instructions...\n";
    Sawyer::Stopwatch editDistanceTime;
    EditDistance::TreeEditDistance::Analysis editDistance;
    editDistance.substitutionCost(0);
    editDistance.substitutionPredicate(&isSameType);
    editDistance.compute(gblock1, gblock2);
    mlog[INFO] <<"Edit distance computed in " <<editDistanceTime <<" seconds\n";
    EditDistance::TreeEditDistance::Edits edits = editDistance.edits();
    std::cout <<"  Nodes in source tree: " <<editDistance.sourceTreeNodes().size() <<"\n"
              <<"  Nodes in target tree: " <<editDistance.targetTreeNodes().size() <<"\n"
              <<"  Graph vertices:       " <<editDistance.graphSize().first <<"\n"
              <<"  Graph edges:          " <<editDistance.graphSize().second <<"\n"
              <<"  Number of edits:      " <<edits.size() <<"\n"
              <<"  Total cost of edits:  " <<editDistance.cost() <<"\n"
              <<"  Relative cost:        " <<editDistance.relativeCost() <<"\n";
#if 0 // [Robb P. Matzke 2014-09-18]
    std::cout <<"  Individual edits:\n";
    BOOST_FOREACH (const TreeEditDistance::Edit &edit, edits)
        std::cout <<"    " <<edit <<"\n";
#endif

    return 0;
}
