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

template<typename Instruction>
static int kind(Instruction *insn) {
    if (Instruction *i = dynamic_cast<Instruction*>(insn))
        return i->get_kind();
    return -1;
}

// When can AST nodes be substituted?
static struct: TreeEditDistance::SubstitutionPredicate {
    virtual bool operator()(SgNode *a, SgNode *b) /*override*/ {
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

static Sawyer::Message::Facility mlog;

int
main(int argc, char *argv[]) {
    // Initialize
    rose::Diagnostics::initialize();                    // because librose doesn't initialize itself until frontend();
    mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insert(mlog);

    // Parse command-line (see --help for usage)
    Settings settings;
    std::vector<std::string> positionalArgs = parseCommandLine(argc, argv, settings).unreachedArgs();
    ASSERT_always_require2(positionalArgs.size()==2, "see --help, except do not supply librose switches");

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

    // Disassemble and partition code into functions.  Engine::partition is the top-level, do-everything function.
    mlog[INFO] <<"disassembling and partitioning specimens...\n";
    Sawyer::Stopwatch partitionTime;
    SgAsmBlock *gblock1 = Partitioner2::Engine().partition(interp1);
    SgAsmBlock *gblock2 = Partitioner2::Engine().partition(interp2);
    mlog[INFO] <<"disassembled and partitioned in " <<partitionTime <<" seconds\n";

#ifdef USE_OLD_TREE_EDIT_DISTANCE
    // Original (local) implementation
    mlog[INFO] <<"Computing edit distance using old method... (watch out -- lots of output coming!)\n";
    Sawyer::Stopwatch oldTime;
    tree_edit_distance(gblock1, gblock2);
    mlog[INFO] <<"Old method took " <<oldTime <<" seconds\n";
#endif

    // Edit distance
    mlog[INFO] <<"Computing edit distance over instructions...\n";
    Sawyer::Stopwatch editDistanceTime;
    TreeEditDistance editDistance;
    editDistance.substitutionCost(0);
    editDistance.substitutionPredicate(&isSameType);
    editDistance.compute(gblock1, gblock2);
    mlog[INFO] <<"Edit distance computed in " <<editDistanceTime <<" seconds\n";
    TreeEditDistance::Edits edits = editDistance.edits();
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
