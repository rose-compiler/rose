// Functionality for determining whether a function is a no-op. This is all part of Partitioner2::Partitioner, just separated
// from the main Partitioner.C file so that file isn't so big.

#include <sage3basic.h>

#include <BinaryNoOperation.h>
#include <Diagnostics.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/ProgressBar.h>

using namespace rose::Diagnostics;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

bool
Partitioner::functionIsNoop(const Function::Ptr &function) const {
    Sawyer::Message::Stream debug(mlog[DEBUG] <<"Partitioner::functionIsNoop(" <<function->printableName() <<")\n");
    ASSERT_not_null(function);
    bool retval = false;

    // If an analysis has already been run then return its value.
    if (function->isNoop().getOptional().assignTo(retval)) {
        SAWYER_MESG(debug) <<"  " <<function->printableName() <<" is " <<(retval?"":"not ") <<"no-op (cached)\n";
        return retval;
    }

    // No-op analyser. We use an arbitrary concrete initial stack pointer because it allows the analysis to assume stack
    // semantics. That is, items popped from the stack are no longer considered to be valid memory.  Using an odd value for the
    // initial stack reduces the number of false positives for stack-aligning instructions being no-ops (e.g., "AND SP,
    // 0xfffffff0" will not be a no-op).
    NoOperation noOpAnalyzer(newDispatcher(newOperators()));
    noOpAnalyzer.initialStackPointer(0xdddd0001);
    
    // If any vertex of this function (other than its return vertices) has successors that are in some other function and that
    // other function is not a proven no-op, then assume that this function is not a no-op either.  We do this separately from
    // the data-flow because this loop is much faster than that one.
    BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexIterator bbVertex = findPlaceholder(bbVa);
        ASSERT_require(cfg().isValidVertex(bbVertex));
        BasicBlock::Ptr bb = bbVertex->value().bblock();
        ASSERT_not_null(bb);
        if (!basicBlockIsFunctionReturn(bb)) {
            BOOST_FOREACH (const ControlFlowGraph::Edge &edge, bbVertex->outEdges()) {
                BOOST_FOREACH (const Function::Ptr &targetFunc, edge.target()->value().owningFunctions().values()) {
                    if (targetFunc != function && !targetFunc->isNoop().isCached()) {
                        // target function is not a no-op, or no-op analysis has not been run on it yet.
                        SAWYER_MESG(debug) <<"  " <<function->printableName() <<" calls " <<targetFunc->printableName()
                                           <<" which is not a no-op"
                                           <<(targetFunc->isNoop().isCached()?" (proven)":" (untested)") <<"\n";
                        retval = false;
                        goto done;
                    }
                }
            }
        }
    }
    
    // Now use data-flow to look at each basic block. If any basic block of this function is not a noop then assume the
    // function as a whole is not a no-op.
    BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
        if (BasicBlock::Ptr bb = basicBlockExists(bbVa)) {
            // Get the instructions for this block, excluding the final instruction if this is a function return.
            std::vector<SgAsmInstruction*> insns = bb->instructions();
            if (basicBlockIsFunctionReturn(bb)) {
                insns.pop_back();
            } else {
                // If the basic block is not a function return and its CFG successor is undiscovered or indeterminate then we
                // must assume that its successor imparts some effect to this function.
                ControlFlowGraph::ConstVertexIterator placeholder = findPlaceholder(bbVa);
                ASSERT_require(cfg().isValidVertex(placeholder));
                BOOST_FOREACH (const ControlFlowGraph::Edge &edge, placeholder->outEdges()) {
                    if (edge.target()->value().type() != V_BASIC_BLOCK) {
                        SAWYER_MESG(debug) <<"  " <<function->printableName() <<" is not a no-op because the successor of "
                                           <<bb->printableName() <<" is missing: " <<vertexName(edge.target()) <<"\n";
                        retval = false;
                        goto done;
                    }
                }
            }
            if (!noOpAnalyzer.isNoop(insns)) {
                SAWYER_MESG(debug) <<"  " <<function->printableName() <<" is not a no-op because "
                                   <<bb->printableName() <<" is not a no-op\n";
                retval = false;
                goto done;
            }
        } else {
            SAWYER_MESG(debug) <<"  " <<function->printableName() <<" is not a no-op because it is missing a basic block at "
                               <<StringUtility::addrToString(bbVa);
            retval = false;
            goto done;
        }
    }

    // Must be a no-op since all basic blocks are no-ops.
    SAWYER_MESG(debug) <<"  " <<function->printableName() <<" is a no-op\n";
    retval = true;

done:
    function->isNoop() = retval;
    return retval;
}

void
Partitioner::allFunctionIsNoop() const {
    // Function call graph
    FunctionCallGraph cgAnalyzer = functionCallGraph();
    const FunctionCallGraph::Graph &cg = cgAnalyzer.graph();

    // FIXME[Robb Matzke 2016-02-26]: For now this is serial, but it could be parallel.
    // Use a function call graph and process functions using a reverse depth-first traversal. We do this because if function A
    // calls or transfers control to function B we need to know whether function B is a no-op before we can determine if
    // function A is a no-op.  Actually, that's not precisely true -- it is necessary but not sufficient for B to be a no-op in
    // order for A to be a no-op.
    Sawyer::ProgressBar<size_t> progress(cg.nVertices(), mlog[MARCH], "no-op function analysis");
    typedef Sawyer::Container::Algorithm::DepthFirstForwardGraphTraversal<const FunctionCallGraph::Graph> Traversal;
    std::vector<bool> processed(cg.nVertices(), false);
    for (size_t i=0; i<cg.nVertices(); ++i) {
        if (processed[i])
            continue;
        for (Traversal t(cg, cg.findVertex(i), Sawyer::Container::Algorithm::LEAVE_VERTEX); t; ++t) {
            if (processed[t.vertex()->id()])
                continue;
            ++progress;
            Function::Ptr function = t.vertex()->value();
            (void) functionIsNoop(function);            // result is cached in the function
            processed[t.vertex()->id()] = true;
        }
    }
}

void
Partitioner::forgetFunctionIsNoop() const {
    BOOST_FOREACH (const Function::Ptr &function, functions())
        forgetFunctionIsNoop(function);
}

void
Partitioner::forgetFunctionIsNoop(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    function->isNoop().clear();
}

} // namespace
} // namespace
} // namespace

